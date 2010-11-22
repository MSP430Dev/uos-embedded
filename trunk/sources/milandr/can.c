#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/can.h>
#include <kernel/internal.h>

#define NRBUF		16		/* number of receive buffers */
#define NTBUF		1		/* number of transmit buffers */

#define ALL_RBUFS	((1 << NRBUF) - 1)
#define ALL_TBUFS	(((1 << NTBUF) - 1) << NRBUF)

/*
 * Initialize queue.
 */
static inline __attribute__((always_inline))
void can_queue_init (can_queue_t *q)
{
	q->tail = q->queue;
	q->count = 0;
}

/*
 * Add a packet to queue.
 * Before call, a user should check that the queue is not full.
 */
static inline __attribute__((always_inline))
void can_queue_put (can_queue_t *q, can_frame_t *fr)
{
	can_frame_t *head;

	/*debug_printf ("can_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < CAN_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += CAN_QUEUE_SIZE;

	/* Put the packet in. */
	*head = *fr;
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Get a packet from queue.
 * When empty, returns {0,0,0,0};
 */
static inline __attribute__((always_inline))
void can_queue_get (can_queue_t *q, can_frame_t *fr)
{
	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + CAN_QUEUE_SIZE);
	if (q->count > 0) {
		/* Get the first packet from queue. */
		*fr = *q->tail;

		/* Advance head pointer. */
		if (--q->tail < q->queue)
			q->tail += CAN_QUEUE_SIZE;
		--q->count;
	}
}

/*
 * Check that queue is full.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_full (can_queue_t *q)
{
	return (q->count == CAN_QUEUE_SIZE);
}

/*
 * Check that queue is empty.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_empty (can_queue_t *q)
{
	return (q->count == 0);
}

/*
 * Задание скорости работы.
 */
static void can_setup_timing (can_t *c, int kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	/* Изначально разбиваем битовый интервал на 10 квантов:
	 * 1) Первый квант - всегда - сегмент синхронизации.
	 * 2) Второй квант - сегмент компенсации задержки распространения (PSEG).
	 * 3) Четыре кванта - сегмент фазы 1 (SEG1).
	 * 4) Четыре кванта - сегмент фазы 2 (SEG2).
	 * Такое разбиение совместно с SJW = 4 обеспечивает наибольшее возможное расхождение часов устройств
	 * (так написано в рекомендациях BOSCH)
	 */
	/* Рассчитываем значение делителя частоты исходя из того, что битовый интервал разбит на 10 квантов */
	unsigned brp = (KHZ / kbitsec / 10) - 1;
	/* Из-за дискретности brp, его значение может быть слишком грубым. 
	 * Посчитаем, сколько реально квантов получится при таком brp?
         */
	unsigned nq = KHZ / (brp + 1) / kbitsec;
	/* Если квантов больше, чем 10, то за счет разницы увеличим сегмент компенсации
	 * задержки распространения. Тем самым "подстроимся" под необходимую частоту
	 */
	unsigned pseg = 1 + (nq - 10);
	c->kbitsec = kbitsec;

	reg->BITTMNG = CAN_BITTMNG_SB | CAN_BITTMNG_SJW (4) |
		CAN_BITTMNG_SEG2 (4) | CAN_BITTMNG_SEG1 (4) |
		CAN_BITTMNG_PSEG (pseg) | CAN_BITTMNG_BRP (brp);

debug_printf ("can: %u (%u) kbit/sec, brp=%u, BITTMNG = %08x\n", kbitsec, KHZ / (brp + 1) / nq, brp, reg->BITTMNG);
}

/*
 * Инициализация всех регистров контроллера CAN.
 */
static void can_setup (can_t *c, int kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	if (c->port == 0) {
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
	reg->CONTROL = 0;
	if (c->loop)
		reg->CONTROL |= CAN_CONTROL_STM |
			CAN_CONTROL_SAP | CAN_CONTROL_ROP;
	can_setup_timing (c, kbitsec);

	unsigned i;
	for (i=0; i<32; i++) {
		reg->BUF_CON[i] = CAN_BUF_CON_EN;
		reg->BUF[i].ID = 0;
		reg->BUF[i].DLC = CAN_DLC_LEN(8) | CAN_DLC_R1 | CAN_DLC_SSR;
		reg->BUF[i].DATAL = 0;
		reg->BUF[i].DATAH = 0;
		reg->MASK[i].MASK = 0;
		reg->MASK[i].FILTER = 0;

		/* Первые 16 буферов используются для приёма. */
		if (i < NRBUF) {
			reg->BUF_CON[i] |= CAN_BUF_CON_RX_ON;
			if (i == NRBUF-1) {
				/* 16-й буфер служит для обнаружения потерь. */
				reg->BUF_CON[i] |= CAN_BUF_CON_OVER_EN;
			}
		}
	}

	/* Максимальное значение счётчика ошибок. */
	reg->OVER = 255;

	/* Разрешение прерываний от приёмных буферов. */
	reg->INT_RX = ALL_RBUFS;
	reg->INT_TX = 0;
	reg->INT_EN = CAN_INT_EN_RX | CAN_INT_EN_TX | CAN_INT_EN_GLB;
	reg->STATUS = 0;
	reg->CONTROL |= CAN_CONTROL_EN;
}

/*
 * Запись пакета в передающий буфер.
 */
static int transmit_enqueue (can_t *c, const can_frame_t *fr)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	/* Проверяем, что есть свободный буфер для передачи. */
	int i = 31 - arm_count_leading_zeroes (reg->TX & ALL_TBUFS);
	if (i < NRBUF)
		return 0;

	/* Нашли свободный буфер. */
	if (reg->BUF_CON[i] & CAN_BUF_CON_TX_REQ) {
		/* Такого не может быть. */
		debug_printf ("can_output: tx buffer busy\n");
		return 0;
	}

	CAN_BUF_t *buf = &reg->BUF[i];
	if (fr->dlc & CAN_DLC_IDE) {
		/* Расширенный формат кадра */
		buf->ID = fr->id & (CAN_ID_EID_MASK | CAN_ID_SID_MASK);
		buf->DLC = CAN_DLC_LEN (fr->dlc) |
			CAN_DLC_IDE | CAN_DLC_SSR | CAN_DLC_R1;
	} else {
		/* Стандартный формат кадра */
		buf->ID = fr->id & CAN_ID_SID_MASK;
		buf->DLC = CAN_DLC_LEN (fr->dlc);
	}
	buf->DATAL = fr->data[0];
	buf->DATAH = fr->data[1];
	reg->BUF_CON[i] |= CAN_BUF_CON_TX_REQ;
	c->out_packets++;

	/* Разрешение прерывания от передающего буфера. */
	reg->INT_TX |= 1 << i;
	return 1;
}

/*
 * Установка скорости интерфейса CAN.
 */
void can_set_speed (can_t *c, unsigned kbitsec)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	mutex_lock (&c->lock);
	reg->CONTROL &= ~CAN_CONTROL_EN;
	can_setup_timing (c, kbitsec);
	reg->CONTROL |= CAN_CONTROL_EN;
	mutex_unlock (&c->lock);
}

/*
 * Управление режимом шлейфа.
 */
void can_set_loop (can_t *c, int on)
{
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;

	mutex_lock (&c->lock);
	c->loop = on;
	reg->CONTROL = 0;
	if (c->loop)
		reg->CONTROL |= CAN_CONTROL_STM |
			CAN_CONTROL_SAP | CAN_CONTROL_ROP;
	reg->CONTROL |= CAN_CONTROL_EN;
	mutex_unlock (&c->lock);
}

/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
void can_output (can_t *c, const can_frame_t *fr)
{
	mutex_lock (&c->lock);
	while (! transmit_enqueue (c, fr)) {
		/* Ждём появления места в буфере устройства. */
		mutex_wait (&c->lock);
	}
	mutex_unlock (&c->lock);
}

/*
 * Fetch received frame.
 */
void can_input (can_t *c, can_frame_t *fr)
{
	mutex_lock (&c->lock);
	while (can_queue_is_empty (&c->inq)) {
		/* Ждём приёма пакета. */
		mutex_wait (&c->lock);
	}
	can_queue_get (&c->inq, fr);
	mutex_unlock (&c->lock);
}

static bool_t can_handle_interrupt (void *arg)
{
	can_t *c = (can_t *)arg;
	CAN_t *reg = (c->port == 0) ? ARM_CAN1 : ARM_CAN2;
	bool_t ok = 0;

	unsigned status = reg->STATUS;
	reg->STATUS = 0;
//debug_printf ("can interrupt: STATUS = %08x\n", status);
#if 0
	c->rec = CAN_STATUS_REC (status);
	if (status & CAN_STATUS_REC8)
		c->rec += 256;

	c->tec = CAN_STATUS_TEC (status);
	if (status & CAN_STATUS_TEC8)
		c->tec += 256;

	if (status & CAN_STATUS_BUS_OFF) {
		/* Шина CAN отвалилась, требуется перезапуск. */
		debug_printf ("can interrupt: BUS OFF\n");
		ok = 1;
	}
	if (status & CAN_STATUS_ID_LOWER) {
		/* При передаче был проигран арбитраж */
		c->out_collisions++;
		ok = 1;
	}
	if (status & CAN_STATUS_ERR_ACK) {
		/* Ошибка подтверждения приема */
		c->out_nacks++;
	}
	if (status & CAN_STATUS_ERR_FRAME) {
		/* Ошибка формата принимаемого пакета */
		c->in_frame_errors++;
		ok = 1;
	}
	if (status & CAN_STATUS_ERR_CRC) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_crc_errors++;
		ok = 1;
	}
	if (status & CAN_STATUS_ERR_BITSTUFF) {
		/* Ошибка контрольной суммы принимаемого пакета */
		c->in_bitstuff_errors++;
		ok = 1;
	}
	if (status & CAN_STATUS_ERR_BIT) {
		/* Ошибка передаваемых битов пакета */
		c->out_bit_errors++;
		ok = 1;
	}
#endif
	/* Есть ли буферы с принятым сообщением. */
	if (status & CAN_STATUS_RX_READY) {
		unsigned i;
		for (i=0; i<NRBUF; i++) {
			unsigned bufcon = reg->BUF_CON[i];
			if (! (bufcon & CAN_BUF_CON_RX_FULL))
				continue;
			if (bufcon & CAN_BUF_CON_OVER_WR) {
				/* Сообщение перезаписано */
//debug_printf ("OVERWRITE!\n");
				c->in_discards++;
				reg->BUF_CON[i] = bufcon &
					~(CAN_BUF_CON_RX_FULL |
					CAN_BUF_CON_OVER_WR);
				ok = 0;
				continue;
			}
			CAN_BUF_t *buf = &reg->BUF[i];
			can_frame_t fr;
			fr.id = buf->ID;
			fr.dlc = buf->DLC;
			fr.data[0] = buf->DATAL;
			fr.data[1] = buf->DATAH;
/*debug_printf ("can rx: %08x-%08x-%08x-%08x\n", fr.id, fr.dlc, fr.data[0], fr.data[1]);*/
			reg->BUF_CON[i] = bufcon & ~CAN_BUF_CON_RX_FULL;

			if (CAN_DLC_LEN (fr.dlc) > 8) {
				c->in_errors++;
				ok = 1;
				continue;
			}
			if (can_queue_is_full (&c->inq)) {
				c->in_discards++;
				ok = 1;
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
		reg->INT_TX = ~reg->TX & ALL_TBUFS;
	}

	arch_intr_allow (c->port);

	return ok;
}

/*
 * Set up the CAN driver.
 */
void can_init (can_t *c, int port, int prio, unsigned kbitsec)
{
	c->port = port;
	can_queue_init (&c->inq);
	can_setup (c, kbitsec);

	int irq = (port == 0) ? 0 : 1;
	mutex_lock_irq (&c->lock, irq, can_handle_interrupt, c);
	mutex_unlock (&c->lock);
}
