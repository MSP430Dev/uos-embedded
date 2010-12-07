#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/can.h>
#include <kernel/internal.h>

/*
 * Таблица выводов интерфейса SSP1.
 * ---- pin --- alt ----------- redef ---
 *	PB12	SSP1_FSS (X33.1/8)
 *	PB13	SSP1_CLK (X33.1/6)
 *	PB14	SSP1_RXD (X33.1/4)
 *	PB15	SSP1_TXD (X33.1/3)
 *	PD9			SSP1_FSS (X32.2/9)
 *	PD10			SSP1_CLK (X32.1/29, X32)
 *	PD11			SSP1_RXD (X32.1/30, X34)
 *	PD12			SSP1_TXD (X32.1/31, X36)
 *	PE12	SSP1_RXD (X32.1/25)
 *	PE13	SSP1_FSS (X32.1/26)
 *	PF0	SSP1_TXD (X32.2/13)
 *	PF1	SSP1_CLK (X32.2/14)
 *	PF2	SSP1_FSS (X32.1/5)
 *	PF3	SSP1_RXD (X32.1/6)
 *
 * Требуемое расположение выводов SPI1 можно задавать макросом
 * CFLAGS в файле 'target.cfg'. По умолчанию используются PD9-PD12.
 */
#if ! defined (SSP1_ON_PORTB) &&
    ! defined (SSP1_ON_PORTD) &&
    ! defined (SSP1_ON_PORTF)
#	define SSP1_ON_PORTD
#endif

/*
 * Таблица выводов интерфейса SSP2.
 * ---- pin --- alt ----------- redef ---
 *	PB12			SSP2_FSS (X33.1/8)
 *	PB13			SSP2_CLK (X33.1/6)
 *	PB14			SSP2_RXD (X33.1/4)
 *	PB15			SSP2_TXD (X33.1/3)
 *	PC0			SSP2_FSS (X33.2/10)
 *	PC1			SSP2_CLK (X33.2/11)
 *	PC2			SSP2_RXD (X33.2/12)
 *	PC3			SSP2_TXD (X33.2/5)
 *	PC14	SSP2_FSS (X33.2/28)
 *	PC15	SSP2_RXD (X33.2/24)
 *	PD2	SSP2_RXD (X33.2/13, microSD_7)
 *	PD3	SSP2_FSS (X33.2/15, microSD_2)
 *	PD5	SSP2_CLK (X33.2/16, microSD_5)
 *	PD6	SSP2_TXD (X33.2/14, microSD_3)
 *	PF12			SSP2_FSS (X32.1/15)
 *	PF13			SSP2_CLK (X32.1/16)
 *	PF14			SSP2_RXD (X32.1/19)
 *	PF15			SSP2_TXD (X32.1/20)
 *
 * Требуемое расположение выводов SPI2 можно задавать макросом
 * CFLAGS в файле 'target.cfg'. По умолчанию используются PD2-PD6.
 */
#if ! defined (SSP2_ON_PORTB) &&
    ! defined (SSP2_ON_PORTC) &&
    ! defined (SSP2_ON_PORTD) &&
    ! defined (SSP2_ON_PORTF)
#	define SSP2_ON_PORTD
#endif

/*
 * Initialize queue.
 */
static inline __attribute__((always_inline))
void spi_queue_init (spi_queue_t *q)
{
	q->tail = q->queue;
	q->count = 0;
}

/*
 * Add a packet to queue.
 * Before call, a user should check that the queue is not full.
 */
static inline __attribute__((always_inline))
void spi_queue_put (spi_queue_t *q, spi_frame_t *fr)
{
	spi_frame_t *head;

	/*debug_printf ("spi_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < SPI_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += SPI_QUEUE_SIZE;

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
void spi_queue_get (spi_queue_t *q, spi_frame_t *fr)
{
	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + SPI_QUEUE_SIZE);
	if (q->count > 0) {
		/* Get the first packet from queue. */
		*fr = *q->tail;

		/* Advance head pointer. */
		if (--q->tail < q->queue)
			q->tail += SPI_QUEUE_SIZE;
		--q->count;
	}
}

/*
 * Check that queue is full.
 */
static inline __attribute__((always_inline))
bool_t spi_queue_is_full (spi_queue_t *q)
{
	return (q->count == SPI_QUEUE_SIZE);
}

/*
 * Check that queue is empty.
 */
static inline __attribute__((always_inline))
bool_t spi_queue_is_empty (spi_queue_t *q)
{
	return (q->count == 0);
}

/*
 * Инициализация всех регистров контроллера SSP.
 */
static void spi_setup (spi_t *c, int kbitsec)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	if (c->port == 0) {
		/* Включаем тактирование порта SSP1. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP1;

#ifdef SSP1_ON_PORTD
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;

		/* Переопределённая функция:
		 * PD9	- SSP1_FSS
		 * PD10	- SSP1_CLK
		 * PD11	- SSP1_RXD
		 * PD12	- SSP1_TXD */
		ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC &
			~(ARM_FUNC_MASK(9) | ARM_FUNC_MASK(10) |
			  ARM_FUNC_MASK(11) | ARM_FUNC_MASK(12))) |
			ARM_FUNC_REDEF(9) | ARM_FUNC_REDEF(10) |
			ARM_FUNC_REDEF(11) | ARM_FUNC_REDEF(12);

		/* Цифровые выводы. */
		ARM_GPIOD->ANALOG |= (1 << 9) | (1 << 10) |
				     (1 << 11) | (1 << 12);

		/* Быстрый фронт. */
		ARM_GPIOD->PWR = (ARM_GPIOD->PWR &
			~(ARM_PWR_MASK(9) | ARM_PWR_MASK(10) |
			  ARM_PWR_MASK(11) | ARM_PWR_MASK(12))) |
			ARM_PWR_FAST(9) | ARM_PWR_FAST(10) |
			ARM_PWR_FAST(11) | ARM_PWR_FAST(12);
#else
#   error "SSP1_ON_PORTx not defined"
#endif
		/* Разрешение тактовой частоты на SSP1, источник HCLK. */
		ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
			ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
	} else {
		/* Включаем тактирование порта SSP2. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP2;

#ifdef SSP2_ON_PORTD
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;

		/* Альтернативная функция:
		 * PD2 - SSP2_RXD
		 * PD3 - SSP2_FSS
		 * PD5 - SSP2_CLK
		 * PD6 - SSP2_TXD */
		ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC &
			~(ARM_FUNC_MASK(2) | ARM_FUNC_MASK(3) |
			  ARM_FUNC_MASK(5) | ARM_FUNC_MASK(6))) |
			ARM_FUNC_ALT(2) | ARM_FUNC_ALT(3) |
			ARM_FUNC_ALT(5) | ARM_FUNC_ALT(6);

		/* Цифровые выводы. */
		ARM_GPIOD->ANALOG |= (1 << 2) | (1 << 3) |
				     (1 << 5) | (1 << 6);

		/* Быстрый фронт. */
		ARM_GPIOD->PWR = (ARM_GPIOD->PWR &
			~(ARM_PWR_MASK(2) | ARM_PWR_MASK(3) |
			  ARM_PWR_MASK(5) | ARM_PWR_MASK(6))) |
			ARM_PWR_FAST(2) | ARM_PWR_FAST(3) |
			ARM_PWR_FAST(5) | ARM_PWR_FAST(6);
#else
#   error "SSP2_ON_PORTx not defined"
#endif
		/* Разрешение тактовой частоты на SSP2, источник HCLK. */
		ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
			ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
	}
	reg->CR0 = ARM_SSP_CR0_FRF_SPI | ARM_SSP_CR0_DSS(16) |
		ARM_SSP_CR0_SCR ((KHZ / kbitsec + 1) / 2);
	reg->CR1 = c->master ? 0 | ARM_SSP_CR1_MS;
	reg->CPSR = 2;
	reg->DMACR = 0;
	reg->IM = ARM_SSP_IM_TX | ARM_SSP_IM_RX | ARM_SSP_IM_RT;
	reg->CR1 |= ARM_SSP_CR1_SSE;
}

/*
 * Запись пакета в передающий буфер.
 */
static int transmit_enqueue (spi_t *c, const spi_frame_t *fr)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	/* Проверяем, что есть свободный буфер для передачи. */
	int i = 32 - arm_count_leading_zeroes (~(reg->TX & ALL_TBUFS));
	if (i < NRBUF || i >= NRBUF + NTBUF)
		return 0;

	/* Нашли свободный буфер. */
	if (reg->BUF_CON[i] & SPI_BUF_CON_TX_REQ) {
		/* Такого не может быть. */
		debug_printf ("spi_output: tx buffer busy\n");
		return 0;
	}

	SPI_BUF_t *buf = &reg->BUF[i];
	if (fr->dlc & SPI_DLC_IDE) {
		/* Расширенный формат кадра */
		buf->ID = fr->id & (SPI_ID_EID_MASK | SPI_ID_SID_MASK);
		buf->DLC = SPI_DLC_LEN (fr->dlc) |
			SPI_DLC_IDE | SPI_DLC_SSR | SPI_DLC_R1;
	} else {
		/* Стандартный формат кадра */
		buf->ID = fr->id & SPI_ID_SID_MASK;
		buf->DLC = SPI_DLC_LEN (fr->dlc);
	}
	buf->DATAL = fr->data[0];
	buf->DATAH = fr->data[1];
	reg->BUF_CON[i] |= SPI_BUF_CON_TX_REQ;
	c->out_packets++;

	/* Разрешение прерывания от передающего буфера. */
	reg->INT_TX |= 1 << i;

	return 1;
}

/*
 * Transmit the frame.
 * Return 0 when there is no space in queue.
 */
void spi_output (spi_t *c, const spi_frame_t *fr)
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
void spi_input (spi_t *c, spi_frame_t *fr)
{
	mutex_lock (&c->lock);
	while (spi_queue_is_empty (&c->inq)) {
		/* Ждём приёма пакета. */
		mutex_wait (&c->lock);
	}
	spi_queue_get (&c->inq, fr);
	mutex_unlock (&c->lock);
}

static bool_t spi_handle_interrupt (void *arg)
{
	spi_t *c = (spi_t *)arg;
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	unsigned status = reg->STATUS;
	reg->STATUS = 0;
//debug_printf ("can interrupt: STATUS = %08x\n", status);

	/* Есть ли буферы с принятым сообщением. */
	if (status & SPI_STATUS_RX_READY) {
		unsigned i;
		for (i=0; i<NRBUF; i++) {
			unsigned bufcon = reg->BUF_CON[i];
			if (! (bufcon & SPI_BUF_CON_RX_FULL))
				continue;
			if (bufcon & SPI_BUF_CON_OVER_WR) {
				/* Сообщение перезаписано */
//debug_printf ("OVERWRITE!\n");
				c->in_discards++;
				reg->BUF_CON[i] = bufcon &
					~(SPI_BUF_CON_RX_FULL |
					SPI_BUF_CON_OVER_WR);
				continue;
			}
			SPI_BUF_t *buf = &reg->BUF[i];
			spi_frame_t fr;
			fr.id = buf->ID;
			if (fr.id == 0)
				fr.id = buf->ID;
			if (fr.id == 0)
				fr.id = buf->ID;
			fr.dlc = buf->DLC;
			if (fr.dlc == 0)
				fr.dlc = buf->DLC;
			if (fr.dlc == 0)
				fr.dlc = buf->DLC;
			fr.data[0] = buf->DATAL;
			if (fr.data[0] == 0)
				fr.data[0] = buf->DATAL;
			if (fr.data[0] == 0)
				fr.data[0] = buf->DATAL;
			fr.data[1] = buf->DATAH;
			if (fr.data[1] == 0)
				fr.data[1] = buf->DATAH;
			if (fr.data[1] == 0)
				fr.data[1] = buf->DATAH;
/*debug_printf ("can rx: %08x-%08x-%08x-%08x\n", fr.id, fr.dlc, fr.data[0], fr.data[1]);*/
			reg->BUF_CON[i] = bufcon & ~SPI_BUF_CON_RX_FULL;

			if (SPI_DLC_LEN (fr.dlc) > 8) {
				c->in_errors++;
				continue;
			}
			if (spi_queue_is_full (&c->inq)) {
				c->in_discards++;
				continue;
			}
			/* Пакет успешно принят. */
			c->in_packets++;
			spi_queue_put (&c->inq, &fr);
		}
	}

	/* Есть ли закончившие передающие буферы. */
	if (status & SPI_STATUS_TX_READY) {
		/* Оставляем маску только для активных буферов. */
		reg->INT_TX = ~reg->TX & ALL_TBUFS;
	}

	arch_intr_allow (c->port);
	return 0;
}

/*
 * Set up the SPI driver.
 */
void spi_init (spi_t *c, int port, int master, unsigned kbitsec)
{
	c->port = port;
	c->master = master;
	spi_queue_init (&c->inq);
	spi_setup (c, kbitsec);

	int irq = (port == 0) ? 8 : 20;
	mutex_lock_irq (&c->lock, irq, spi_handle_interrupt, c);
	mutex_unlock (&c->lock);
}
