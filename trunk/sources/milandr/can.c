#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/can.h>

#define CAN_IRQ(nchan)	((nchan)==0 ? 1 : 0)	/* interrupt number */

#define NTXBUF		16		/* number of transmit buffers */
#define NRXBUF		16		/* number of receive buffers */

static void can_setup (can_t *c)
{
	if (c->reg == ARM_CAN0) {
		/* Включаем тактирование порта CAN1, PORTC. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN0;
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOC;

		/* Основная функция для PC8(CAN1_TX) и PC9(CAN1_RX). */
		ARM_GPIOC->FUNC = ARM_GPIOC->FUNC &
			~(ARM_FUNC_MASK(8) | ARM_FUNC_MASK(9)) |
			ARM_FUNC_MAIN(8) | ARM_FUNC_MAIN(9);

		/* Цифровые выводы. */
		ARM_GPIOC->ANALOG |= (1 << 8) | (1 << 9);

		/* Быстрый фронт. */
		ARM_GPIOC->PWR = ARM_GPIOC->PWR &
			~(ARM_PWR_MASK(8) || ARM_PWR_MASK(9)) |
			ARM_PWR_FAST(8) | ARM_PWR_FAST(9);

		/* Разрешение тактовой частоты на CAN1, источник HCLK. */
		RST_CLK->CAN_CLOCK = RST_CLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG1(7) |
			ARM_CAN_CLOCK_EN1 | ARM_CAN_CLOCK_BRG1(0);
	} else {
		/* Включаем тактирование порта CAN2, PORTD. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_CAN1;
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;

		/* Основная функция для PD9(CAN2_TX) и PD15(CAN1_RX). */
		ARM_GPIOD->FUNC = ARM_GPIOD->FUNC &
			~(ARM_FUNC_MASK(9) | ARM_FUNC_MASK(15)) |
			ARM_FUNC_MAIN(9) | ARM_FUNC_MAIN(15);

		/* Цифровые выводы. */
		ARM_GPIOD->ANALOG |= (1 << 9) | (1 << 15);

		/* Быстрый фронт. */
		ARM_GPIOD->PWR = ARM_GPIOD->PWR &
			~(ARM_PWR_MASK(9) || ARM_PWR_MASK(15)) |
			ARM_PWR_FAST(9) | ARM_PWR_FAST(15);

		/* Разрешение тактовой частоты на CAN2, источник HCLK. */
		RST_CLK->CAN_CLOCK = RST_CLK->CAN_CLOCK & ~ARM_CAN_CLOCK_BRG2(7) |
			ARM_CAN_CLOCK_EN2 | ARM_CAN_CLOCK_BRG2(0);
	}

	c->reg->BITTMNG = CAN_BITTMNG_SB |	/* Трехкратное семплирование */
		CAN_BITTMNG_SJW (4) |		/* Размер фазы SJW */
		CAN_BITTMNG_SEG2 (6) |		/* Размер фазы SEG2 */
		CAN_BITTMNG_SEG1 (7) |		/* Размер фазы SEG1 */
		CAN_BITTMNG_PSEG (2) |		/* Размер фазы PSEG */
		CAN_BITTMNG_BRP (4);		/* Предделитель системной частоты 1:5 */

	unsigned i;
	for (i=0; i<32; i++) {
		c->reg->BUF_CON[i] = CAN_BUF_CON_EN;
		c->reg->BUF[i].ID = (i << 2) << CAN_BUF_ID_SID_SHIFT;
		c->reg->BUF[i].DLC = CAN_BUF_DLC_LEN(8) |
			CAN_BUF_DLC_R1 | CAN_BUF_DLC_SSR;
		c->reg->BUF[i].DATAL = ((i + 1) << 24) | (i << 16) | (i << 8) | (i + 1);
		c->reg->BUF[i].DATAH = ((i + 1) << 24) | (i << 16) | (i << 8) | (i + 1);
		c->reg->MASK[i].MASK = 0;
		c->reg->MASK[i].FILTER = 0;
	}
	/* 0-й буфер используется для приёма. */
	c->reg->BUF_CON[0] |= CAN_BUF_CON_RX_ON;

	/* Максимальное значение счётчика ошибок. */
	c->reg->OVER = 255;

	/* Разрешение прерываний от приёмных буферов. */
	c->reg->INT_RX = 0x0000FFFF;
	c->reg->INT_EN = CAN_INT_EN_RX | CAN_INT_EN_GLB;
	c->reg->CONTROL = CAN_CONTROL_EN;
}

static int transmit_enqueue (can_t *c, unsigned *frame)
{
	int i = c->next_txbuf + NRXBUF;
	if (c->reg->BUF_CON[i] & CAN_BUF_CON_TX_REQ) {
		debug_printf ("can_output: tx buffer busy\n");
		return 0;
	}

	if (frame[1] & CAN_BUF_DLC_IDE) {
		/* Расширенный формат кадра */
		c->reg->BUF[i].ID = frame[0] &
			(CAN_BUF_ID_EID_MASK | CAN_BUF_ID_SID_MASK);
		c->reg->BUF[i].DLC = CAN_BUF_DLC_LEN (frame[1]) |
			CAN_BUF_DLC_IDE | CAN_BUF_DLC_SSR | CAN_BUF_DLC_R1;
	} else {
		/* Стандартный формат кадра */
		c->reg->BUF[i].ID = frame[0] & CAN_BUF_ID_SID_MASK;
		c->reg->BUF[i].DLC = CAN_BUF_DLC_LEN (frame[1]);
	}
	c->reg->BUF[i].DATAL = frame[2];
	c->reg->BUF[i].DATAH = frame[3];
	c->reg->BUF_CON[i] |= CAN_BUF_CON_TX_REQ;
	c->out_packets++;
	c->next_txbuf = (c->next_txbuf + 1) % NTXBUF;
	return 1;
}


/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
bool_t can_output (can_t *c, can_frame_t *fr)
{
	int status;

	mutex_lock (&c->lock);

	if (transmit_enqueue (c, fr)) {
		/* Есть место в буфере устройства. */
		mutex_unlock (&c->lock);
		return 1;
	}
	/* Сразу передать не удается, ставим в очередь. */
	if (can_queue_full (&c->outq)) {
		/* Нет места в очереди. */
		/*debug_printf ("can_output: no free space\n");*/
		++c->tx_qo;
		mutex_unlock (&c->lock);
		return 0;
	}
	buf_queue_put (&c->outq, fr);
	mutex_unlock (&c->lock);
	return 1;
}

/*
 * Fetch received frame.
 */
bool_t can_input (can_t *c, can_frame_t *fr)
{
	can_frame_t fr;
	bool_t result;

	mutex_lock (&c->lock);
	if (can_queue_empty (&c->inq))
		result = 0;
	else {
		*fr = can_queue_get (&c->inq);
		result = 1;
	}
	mutex_unlock (&c->lock);
	return result;
}

static void can_handle_interrupt (can_t *c)
{
	unsigned i;
	can_frame_t frame;

	if (c->reg->RX & 1) {
		for (i=0; i<1; i++) {
			if (c->reg->BUF_CON[i] & CAN_BUF_CON_RX_FULL) {
				c->reg->BUF_CON[i] &= ~CAN_BUF_CON_RX_FULL;
				memcpy (&frame, &c->reg->BUF[i], sizeof(can_frame_t));

				if (frame.DLC > 8) {
					c->in_errors++;
					continue;
				}
				if (can_queue_full (&c->inq)) {
					c->in_discards++;
					continue;
				}
				frame.reserve1 = 0;
				frame.reserve2 = 0;
				frame.reserve3 = 0;
				frame.reserve4 = 0;
				if (! frame.IDE)
					frame.EID = 0;
				memset (frame.data.data1 + frame.DLC, 0, 8 - frame.DLC);
				buf_queue_put (&c->inq, &frame);
			}
		}
	}

	unsigned status = c->reg->STATUS;
	if (status & 0x0000FC00)
		in_errors[c]++;
}

/*
 * Receive interrupt task.
 */
static void can_task (void *arg)
{
	can_t *c = arg;

	mutex_lock_irq (&c->lock, CAN_IRQ (c->port), 0, 0);
	can_setup (c);

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
void can_init (can_t *c, int port, int prio, int kbps)
{
	c->reg = (port == 0) ? ARM_CAN0 : ARM_CAN1;
	c->kbitsec = kbps;
	can_queue_init (&c->inq);
	can_queue_init (&c->outq);

	/* Create interrupt task. */
	task_create (can_task, c, "can", prio,
		c->stack, sizeof (c->stack));
}

#if 0
void Set_CAN_Speed(int c, int speed)
{
	switch (*PCAN_PARAM1(c)) {
	case CAN_SPEED_1000:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((6 - 1) << 22) | ((7 - 1) << 19) | ((2 - 1) << 16) | 4;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_800:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 4;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_500:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 7;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_250:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 15;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_125:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 31;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_50:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 79;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_20:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 199;
		c->reg->CONTROL = 1;
		break;
	case CAN_SPEED_10:
		c->reg->CONTROL = 0;
		c->reg->BITTMNG = (1 << 27) | ((4 - 1) << 25) | ((8 - 1) << 22) | ((8 - 1) << 19) | ((3 - 1) << 16) | 399;
		c->reg->CONTROL = 1;
		break;
	}
	kbitsec[c] = speed;
}

void CAN_Commands(int c)
{
	switch (Received_Command) {
	case CANCTL_INIT:
		CAN_Variables_Initialization(c);
		CAN_Initialized[c] = 1;
		break;
	case CANCTL_RESET:
		HWREG(0xE000E100) &= ~(1 << c);
		c->reg->CONTROL = 0;
		if (c == 0)
			Initialize_Port_CAN_0();
		else
			Initialize_Port_CAN_1();
		Generic_CAN_Default_Setup(c);
		CAN_Variables_Initialization(c);
		CAN_Initialized[c] = 1;
		break;
	case CANCTL_START:
		HWREG(0xE000E100) |= (1 << c);
		c->reg->CONTROL = 1;
		break;
	case CANCTL_STOP:
		HWREG(0xE000E100) &= ~(1 << c);
		c->reg->CONTROL = 0;
		break;
	}
}
#endif
