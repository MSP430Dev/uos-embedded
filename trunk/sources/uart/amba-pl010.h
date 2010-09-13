#include "kernel/internal.h"

/*
 * ARM PrimeCell UART (PL010).
 */
#define RECEIVE_IRQ(p)	((p==ARM_UART1_BASE) ? 6 : 7)	/* UARTINTR */

/*
 * Setup baudrate generator.
 */
static void inline setup_baud_rate (unsigned base, unsigned khz, unsigned long baud)
{
	UART_t *u = (UART_t*) base;

	if (base == ARM_UART1_BASE) {
		ARM_RSTCLK->UART_CLOCK &= ~(ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(7));

		/* Set pins and clock for UART1. */
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOA;	// вкл. тактирования PORTA
		ARM_GPIOA->FUNC |= ARM_FUNC_REDEF(6) |		// переопределенная функция для
				   ARM_FUNC_REDEF(7);		// PA6(UART1_RXD) и PA7(UART1_TXD)
		ARM_GPIOA->ANALOG |= 0xC0;			// цифровые выводы
		ARM_GPIOA->PWR &= ~(ARM_PWR_MASK(6) || ARM_PWR_MASK(7));
		ARM_GPIOA->PWR |= ARM_PWR_SLOW(6) | ARM_PWR_SLOW(7);
		ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_UART1;	// вкл. тактирования UART1
	} else {
		/* Pins and clock for UART2 already initialized by _init_(). */
		ARM_RSTCLK->UART_CLOCK &= ~(ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(7));
	}
#ifdef SETUP_HCLK_HSI
	// HCLK (8 МГц)
	if (base == ARM_UART1_BASE) {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(0);
	} else {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(0);
	}
	u->IBRD = ARM_UART_IBRD (8000000, baud);
	u->FBRD = ARM_UART_FBRD (8000000, baud);
#else
	// HCLK/4 (KHZ/4)
	if (base == ARM_UART1_BASE) {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(2);
	} else {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(2);
	}
	u->IBRD = ARM_UART_IBRD (khz*1000/4, baud);
	u->FBRD = ARM_UART_FBRD (khz*1000/4, baud);
#endif
}

static void inline enable_receiver (unsigned base)
{
	UART_t *u = (UART_t*) base;

	/* Управление линией */
	u->LCR_H = ARM_UART_LCRH_WLEN8 |	// длина слова 8 бит
		   ARM_UART_LCRH_FEN;		// FIFO

	/* Пороги FIFO */
	u->IFLS = ARM_UART_IFLS_RX_1_8 |	// приём: 1/8 буфера
		  ARM_UART_IFLS_TX_1_8;		// передача: 1/8 буфера

	/* Управление */
	u->CR = ARM_UART_CR_RXE |		// приём разрешен
		ARM_UART_CR_TXE;		// передача разрешена

//	u->ICR = ~0;				// сброс прерывания
	u->DMACR = 0;				// управление DMA

	u->CR |= ARM_UART_CR_UARTEN;		// пуск приемопередатчика
}

static void inline enable_receive_interrupt (unsigned base)
{
	UART_t *u = (UART_t*) base;

	/* Маска прерывания */
	u->IMSC = ARM_UART_RIS_RX;		// от приемника
}

static void inline enable_transmit_interrupt (unsigned base)
{
	UART_t *u = (UART_t*) base;

	/* Маска прерывания */
	u->IMSC |= ARM_UART_RIS_TX;		// от передатчика
}

static void inline disable_transmit_interrupt (unsigned base)
{
	UART_t *u = (UART_t*) base;

	/* Маска прерывания */
//u->ICR = ARM_UART_RIS_TX;			// сброс прерывания
	u->IMSC &= ~ARM_UART_RIS_TX;		// от передатчика
}

static void inline transmit_byte (unsigned base, unsigned c)
{
	UART_t *u = (UART_t*) base;

//u->ICR = ARM_UART_RIS_TX;			// сброс прерывания
	u->DR = c;				// данные
}

static unsigned inline get_received_byte (unsigned base)
{
	UART_t *u = (UART_t*) base;

	return u->DR;				// данные
}

static unsigned inline test_transmitter_enabled (unsigned base)
{
	UART_t *u = (UART_t*) base;

	return u->CR & ARM_UART_CR_TXE;
}

static unsigned inline test_transmitter_empty (unsigned base)
{
	UART_t *u = (UART_t*) base;

	if (base == ARM_UART1_BASE)
		ARM_NVIC_ISER0 = 1 << 6;
	else
		ARM_NVIC_ISER0 = 1 << 7;

	return ! (u->FR & ARM_UART_FR_TXFF);	// FIFO передатчика неполно
}

static unsigned inline test_get_receive_data (unsigned base, unsigned char *d)
{
	UART_t *u = (UART_t*) base;

	if (u->FR & ARM_UART_FR_RXFE) 		// FIFO приемника пусто
		return 0;
	*d = u->DR;				// данные
	return 1;
}

#define test_frame_error(p)		0
#define test_parity_error(p)		0
#define test_overrun_error(p)		0
#define clear_receive_errors(p)		/*empty*/
