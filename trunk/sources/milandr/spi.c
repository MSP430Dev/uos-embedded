#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <kernel/internal.h>

/*
 * Номера прерываний от интерфейсов SSP.
 */
#define IRQ_SSP1	8
#define IRQ_SSP2	20

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
#if ! defined (SSP1_ON_PORTB) && \
    ! defined (SSP1_ON_PORTD) && \
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
#if ! defined (SSP2_ON_PORTB) && \
    ! defined (SSP2_ON_PORTC) && \
    ! defined (SSP2_ON_PORTD) && \
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
void spi_queue_put (spi_queue_t *q, unsigned word)
{
	unsigned short *head;

	/*debug_printf ("spi_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < SPI_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += SPI_QUEUE_SIZE;

	/* Put the packet in. */
	*head = word;
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Get a packet from queue.
 * When empty, returns 0.
 */
static inline __attribute__((always_inline))
unsigned spi_queue_get (spi_queue_t *q)
{
	unsigned word = 0;
	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + SPI_QUEUE_SIZE);
	if (q->count > 0) {
		/* Get the first packet from queue. */
		word = *q->tail;

		/* Advance head pointer. */
		if (--q->tail < q->queue)
			q->tail += SPI_QUEUE_SIZE;
		--q->count;
	}
	return word;
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
 * Инициализация внешних сигналов SSP1.
 */
static void spi_setup_ssp1 ()
{
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

//#elif defined (SSP1_ON_PORTB)
	/* Альтернативная функция:
	 * PB12	- SSP1_FSS
	 * PB13	- SSP1_CLK
	 * PB14	- SSP1_RXD
	 * PB15	- SSP1_TXD */
	/* TODO */

//#elif defined (SSP1_ON_PORTF)
	/* Альтернативная функция:
	 * PF0 - SSP1_TXD
	 * PF1 - SSP1_CLK
	 * PF2 - SSP1_FSS
	 * PF3 - SSP1_RXD */
	/* TODO */
#else
#   error "SSP1_ON_PORTx not defined"
#endif
	/* Разрешение тактовой частоты на SSP1, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
		ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
}

/*
 * Инициализация внешних сигналов SSP2.
 */
static void spi_setup_ssp2 ()
{
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

//#elif defined (SSP2_ON_PORTB)
	/* Переопределённая функция:
	 * PB12	- SSP2_FSS
	 * PB13	- SSP2_CLK
	 * PB14	- SSP2_RXD
	 * PB15	- SSP2_TXD */
	/* TODO */

//#elif defined (SSP2_ON_PORTC)
	/* Переопределённая функция:
	 * PC0 - SSP2_FSS
	 * PC1 - SSP2_CLK
	 * PC2 - SSP2_RXD
	 * PC3 - SSP2_TXD */
	/* TODO */

//#elif defined (SSP2_ON_PORTF)
	/* Переопределённая функция:
	 * PF12	- SSP2_FSS
	 * PF13	- SSP2_CLK
	 * PF14	- SSP2_RXD
	 * PF15	- SSP2_TXD */
	/* TODO */
#else
#   error "SSP2_ON_PORTx not defined"
#endif
	/* Разрешение тактовой частоты на SSP2, источник HCLK. */
	ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
		ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
}

/*
 * Transmit the word.
 */
void spi_output (spi_t *c, unsigned word)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;

	mutex_lock (&c->lock);
	while (! (reg->SR & ARM_SSP_SR_TNF)) {
		/* Ждём появления места в FIFO передатчика. */
		mutex_wait (&c->lock);
	}
	reg->DR = word;
	c->out_packets++;
	mutex_unlock (&c->lock);
}

/*
 * Извлекаем данные из приёмного FIFO.
 */
static void receive_data (spi_t *c)
{
	SSP_t *reg = (c->port == 0) ? ARM_SSP1 : ARM_SSP2;
	unsigned sr = reg->SR;

	while (sr & ARM_SSP_SR_RNE) {
		unsigned word = reg->DR;
//debug_printf ("<%04x> ", word);
		sr = reg->SR;

		if (spi_queue_is_full (&c->inq)) {
			c->in_discards++;
			continue;
		}
		/* Пакет успешно принят. */
		c->in_packets++;
		spi_queue_put (&c->inq, word);
	}
}

/*
 * Fetch received word.
 * Returns 0 when no data is avaiable.
 */
int spi_input (spi_t *c, unsigned *word)
{
	int reply = 0;

	mutex_lock (&c->lock);
	receive_data (c);
	if (! spi_queue_is_empty (&c->inq)) {
		*word = spi_queue_get (&c->inq);
		reply = 1;
	}
	mutex_unlock (&c->lock);
	return reply;
}

/*
 * Wait for word received.
 */
void spi_input_wait (spi_t *c, unsigned *word)
{
	mutex_lock (&c->lock);
	receive_data (c);
	while (spi_queue_is_empty (&c->inq)) {
		/* Ждём приёма пакета. */
		mutex_wait (&c->lock);
	}
	*word = spi_queue_get (&c->inq);
	mutex_unlock (&c->lock);
}

/*
 * Fast interrupt handler.
 */
static bool_t spi_handle_interrupt (void *arg)
{
	spi_t *c = (spi_t*) arg;

	c->intr++;
	receive_data (c);
	arch_intr_allow (c->port);
	return 0;
}

/*
 * Set up the SPI driver.
 */
void spi_init (spi_t *c, int port, int bits_per_word, unsigned nsec_per_bit)
{
	/* Инициализация структуры данных драйвера. */
	c->port = port;
	c->master = (nsec_per_bit > 0);
	spi_queue_init (&c->inq);

	/* Выбор соответствующего интерфейса SSP и
	 * установка внешних сигналов. */
	SSP_t *reg;
	int irq;
	if (c->port == 0) {
		spi_setup_ssp1 ();
		reg = ARM_SSP1;
		irq = IRQ_SSP1;
	} else {
		spi_setup_ssp2 ();
		reg = ARM_SSP2;
		irq = IRQ_SSP2;
	}

	/* Инициализация всех регистров данного интерфейса SSP.
	 * Ловим прерывания от приёмника. */
	reg->CR0 = ARM_SSP_CR0_FRF_SPI | ARM_SSP_CR0_DSS (bits_per_word);
	if (c->master) {
		/* Режим master. */
		reg->CR0 |= ARM_SSP_CR0_SCR ((KHZ * nsec_per_bit + 1000000) / 2000000);
		reg->CR1 = 0;
		reg->CPSR = 2;
	} else {
		/* Режим slave. */
		reg->CR1 = ARM_SSP_CR1_MS;
		reg->CPSR = 12;
	}
	reg->DMACR = 0;
	reg->IM = ARM_SSP_IM_RX | ARM_SSP_IM_RT;
	reg->CR1 |= ARM_SSP_CR1_SSE;

	/* Подключение к нужному номеру прерывания. */
	mutex_lock_irq (&c->lock, irq, spi_handle_interrupt, c);
	mutex_unlock (&c->lock);
}
