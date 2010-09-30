#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/can.h>

#define NRBUF		16		/* number of receive buffers */
#define NTBUF		16		/* number of transmit buffers */

#define ALL_RBUFS	((1 << NRBUF) - 1)
#define ALL_TBUFS	(((1 << NTBUF) - 1) << NRBUF)

/*
 * Задание скорости работы.
 */
static void can_setup_timing (can_t *c, int kbitsec)
{
	/* Разбиваем битовый интервал на 20 квантов:
	 * 1) Первый квант - всегда - сегмент синхронизации.
	 * 2) Три кванта - сегмент компенсации задержки распространения (PSEG).
	 * 3) Восемь квантов - сегмент фазы 1 (SEG1).
	 * 4) Восемь квантов - сегмент фазы 2 (SEG2).
	 */
	unsigned brp = ((KHZ / c->kbitsec + 10) / 20) - 1;
	c->kbitsec = (KHZ / (brp + 1) + 10) / 20;

	/* Максимальное отклонение фронта (SJW) - четыре кванта. */
	c->reg->BITTMNG = CAN_BITTMNG_SB | CAN_BITTMNG_SJW (4) |
		CAN_BITTMNG_SEG2 (8) | CAN_BITTMNG_SEG1 (8) |
		CAN_BITTMNG_PSEG (3) | CAN_BITTMNG_BRP (brp);
}

/*
 * Инициализация всех регистров контроллера CAN.
 */
static void can_setup (can_t *c, int kbitsec)
{
	if (c->reg == ARM_CAN1) {
		/* Включаем тактирование порта CAN1, PORTC. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN1;
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOC;

		/* Основная функция для PC8(CAN1_TX) и PC9(CAN1_RX). */
		ARM_GPIOC->FUNC = (ARM_GPIOC->FUNC &
			~(ARM_FUNC_MASK(8) | ARM_FUNC_MASK(9))) |
			ARM_FUNC_MAIN(8) | ARM_FUNC_MAIN(9);

		/* Цифровые выводы. */
		ARM_GPIOC->ANALOG |= (1 << 8) | (1 << 9);

		/* Быстрый фронт. */
		ARM_GPIOC->PWR = (ARM_GPIOC->PWR &
			~(ARM_PWR_MASK(8) || ARM_PWR_MASK(9))) |
			ARM_PWR_FAST(8) | ARM_PWR_FAST(9);

		/* Разрешение тактовой частоты на CAN1, источник HCLK. */
		ARM_RSTCLK->CAN_CLOCK = (ARM_RSTCLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG1(7)) |
			ARM_CAN_CLOCK_EN1 | ARM_CAN_CLOCK_BRG1(0);
	} else {
		/* Включаем тактирование порта CAN2, PORTD. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN2;
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;

		/* Основная функция для PD9(CAN2_TX) и PD15(CAN1_RX). */
		ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC &
			~(ARM_FUNC_MASK(9) | ARM_FUNC_MASK(15))) |
			ARM_FUNC_MAIN(9) | ARM_FUNC_MAIN(15);

		/* Цифровые выводы. */
		ARM_GPIOD->ANALOG |= (1 << 9) | (1 << 15);

		/* Быстрый фронт. */
		ARM_GPIOD->PWR = (ARM_GPIOD->PWR &
			~(ARM_PWR_MASK(9) || ARM_PWR_MASK(15))) |
			ARM_PWR_FAST(9) | ARM_PWR_FAST(15);

		/* Разрешение тактовой частоты на CAN2, источник HCLK. */
		ARM_RSTCLK->CAN_CLOCK = (ARM_RSTCLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG2(7)) |
			ARM_CAN_CLOCK_EN2 | ARM_CAN_CLOCK_BRG2(0);
	}
	c->reg->CONTROL = 0;
	can_setup_timing (c, kbitsec);

	unsigned i;
	for (i=0; i<32; i++) {
		c->reg->BUF_CON[i] = CAN_BUF_CON_EN;
		c->reg->BUF[i].ID = 0;
		c->reg->BUF[i].DLC = CAN_DLC_LEN(8) | CAN_DLC_R1 | CAN_DLC_SSR;
		c->reg->BUF[i].DATAL = 0;
		c->reg->BUF[i].DATAH = 0;
		c->reg->MASK[i].MASK = 0;
		c->reg->MASK[i].FILTER = 0;

		/* Первые 16 буферов используются для приёма. */
		if (i < NRBUF) {
			c->reg->BUF_CON[i] |= CAN_BUF_CON_RX_ON;
			if (i == NRBUF-1) {
				/* 16-й буфер служит для обнаружения потерь. */
				c->reg->BUF_CON[i] |= CAN_BUF_CON_OVER_EN;
			}
		}
	}

	/* Максимальное значение счётчика ошибок. */
	c->reg->OVER = 255;

	/* Разрешение прерываний от приёмных буферов. */
	c->reg->INT_RX = ALL_RBUFS;
	c->reg->INT_TX = 0;
	c->reg->INT_EN = CAN_INT_EN_RX | CAN_INT_EN_TX | CAN_INT_EN_GLB;
	c->reg->STATUS = 0;
	c->reg->CONTROL = CAN_CONTROL_EN;
}

/*
 * Запись пакета в передающий буфер.
 */
static int transmit_enqueue (can_t *c, const can_frame_t *fr)
{
	/* Проверяем, что есть свободный буфер для передачи. */
	int i = arm_count_leading_zeroes (c->reg->TX & ALL_TBUFS);
	if (i >= NTBUF)
		return 0;

	/* Нашли свободный буфер. */
	i = NRBUF + NTBUF - 1 - i;
	if (c->reg->BUF_CON[i] & CAN_BUF_CON_TX_REQ) {
		/* Такого не может быть. */
		debug_printf ("can_output: tx buffer busy\n");
		return 0;
	}

	if (fr->dlc & CAN_DLC_IDE) {
		/* Расширенный формат кадра */
		c->reg->BUF[i].ID = fr->id &
			(CAN_ID_EID_MASK | CAN_ID_SID_MASK);
		c->reg->BUF[i].DLC = CAN_DLC_LEN (fr->dlc) |
			CAN_DLC_IDE | CAN_DLC_SSR | CAN_DLC_R1;
	} else {
		/* Стандартный формат кадра */
		c->reg->BUF[i].ID = fr->id & CAN_ID_SID_MASK;
		c->reg->BUF[i].DLC = CAN_DLC_LEN (fr->dlc);
	}
	c->reg->BUF[i].DATAL = fr->data[0];
	c->reg->BUF[i].DATAH = fr->data[1];
	c->reg->BUF_CON[i] |= CAN_BUF_CON_TX_REQ;
	c->out_packets++;

	/* Разрешение прерывания от передающего буфера. */
	c->reg->INT_TX |= 1 << i;
	return 1;
}

/*
 * Установка скорости интерфейса CAN.
 */
void can_set_speed (can_t *c, unsigned kbitsec)
{
	mutex_lock (&c->lock);
	c->reg->CONTROL = 0;
	can_setup_timing (c, kbitsec);
	c->reg->CONTROL = CAN_CONTROL_EN;
	mutex_unlock (&c->lock);
}

/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
void can_output (can_t *c, const can_frame_t *fr)
{
	mutex_lock (&c->lock);
	for (;;) {
		if (transmit_enqueue (c, fr)) {
			/* Есть место в буфере устройства. */
			mutex_unlock (&c->lock);
			return;
		}
		mutex_wait (&c->lock);
	}
}

/*
 * Fetch received frame.
 */
void can_input (can_t *c, can_frame_t *fr)
{
	mutex_lock (&c->lock);
	for (;;) {
		if (! can_queue_is_empty (&c->inq)) {
			can_queue_get (&c->inq, fr);
			mutex_unlock (&c->lock);
			return;
		}
		mutex_wait (&c->lock);
	}
}

static void can_handle_interrupt (can_t *c)
{
	unsigned status = c->reg->STATUS;
	c->reg->STATUS = 0;

	c->rec = CAN_STATUS_REC (status);
	if (status & CAN_STATUS_REC8)
		c->rec += 256;

	c->tec = CAN_STATUS_TEC (status);
	if (status & CAN_STATUS_TEC8)
		c->tec += 256;

	if (status & CAN_STATUS_BUS_OFF) {
		/* Шина CAN отвалилась, требуется перезапуск. */
		debug_printf ("can interrupt: BUS OFF\n");
	}
	if (status & CAN_STATUS_ID_LOWER) {
		/* При передаче был проигран арбитраж */
		c->out_collisions++;
	}
	if (status & CAN_STATUS_ERR_ACK) {
		/* Ошибка подтверждения приема */
		c->out_nacks++;
	}
	if (status & CAN_STATUS_ERR_FRAME) {
		/* Ошибка формата принимаемого пакета */
		c->in_frame_errors++;
	}
	if (status & CAN_STATUS_ERR_CRC) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_crc_errors++;
	}
	if (status & CAN_STATUS_ERR_BITSTUFF) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_bitstuff_errors++;
	}
	if (status & CAN_STATUS_ERR_BIT) {
		/* Ошибка передаваемых битов пакета */
		c->out_bit_errors++;
	}

	/* Есть ли буферы с принятым сообщением. */
	if (status & CAN_STATUS_RX_READY) {
		unsigned i;
		for (i=0; i<NRBUF; i++) {
			unsigned bufcon = c->reg->BUF_CON[i];
			if (! (bufcon & CAN_BUF_CON_RX_FULL))
				continue;
			if (bufcon & CAN_BUF_CON_OVER_WR) {
				/* Сообщение перезаписано */
				c->in_discards++;
				c->reg->BUF_CON[i] = bufcon &
					~(CAN_BUF_CON_RX_FULL |
					CAN_BUF_CON_OVER_WR);
				continue;
			}
			can_frame_t fr;
			fr.id = c->reg->BUF[i].ID;
			fr.dlc = c->reg->BUF[i].DLC;
			fr.data[0] = c->reg->BUF[i].DATAL;
			fr.data[1] = c->reg->BUF[i].DATAH;
			c->reg->BUF_CON[i] = bufcon & ~CAN_BUF_CON_RX_FULL;

			if (CAN_DLC_LEN (fr.dlc) > 8) {
				c->in_errors++;
				continue;
			}
			if (can_queue_is_full (&c->inq)) {
				c->in_discards++;
				continue;
			}
			/* Пакет успешно принят. */
			c->in_packets++;
			can_queue_put (&c->inq, &fr);
		}
	}

	/* Есть ли закончившие передающие буферы. */
	if (status & CAN_STATUS_TX_READY) {
		/* Оставляем маску только для активных буферов. */
		c->reg->INT_TX = ~c->reg->TX & ALL_TBUFS;
	}
}

/*
 * Receive interrupt task.
 */
static void can_task (void *arg)
{
	can_t *c = arg;
	int irq = (c->reg == ARM_CAN1) ? 0 : 1;

	mutex_lock_irq (&c->lock, irq, 0, 0);

	for (;;) {
		/* Wait for interrupt. */
		mutex_wait (&c->lock);

		/* Process all interrupts. */
		++c->intr;
		can_handle_interrupt (c);
	}
}

/*
 * Set up the CAN driver.
 */
void can_init (can_t *c, int port, int prio, unsigned kbitsec)
{
	c->reg = (port == 0) ? ARM_CAN1 : ARM_CAN2;
	can_queue_init (&c->inq);
	can_setup (c, kbitsec);

	/* Create interrupt task. */
	task_create (can_task, c, "can", prio,
		c->stack, sizeof (c->stack));
}
