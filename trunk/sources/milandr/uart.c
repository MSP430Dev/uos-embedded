/*
 * ARM PrimeCell UART (PL010).
 *
 * TODO: implement fast interrupt handling.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>

/*
 * Setup baudrate generator.
 */
static void inline
setup_baud_rate (unsigned base, unsigned khz, unsigned long baud)
{
	UART_t *reg = (UART_t*) base;

	reg->CTL &= ~ARM_UART_CTL_UARTEN;		// останов приемопередатчика
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
	reg->IBRD = ARM_UART_IBRD (8000000, baud);
	reg->FBRD = ARM_UART_FBRD (8000000, baud);
#else
	// HCLK/4 (KHZ/4)
	if (base == ARM_UART1_BASE) {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN1 | ARM_UART_CLOCK_BRG1(2);
	} else {
		ARM_RSTCLK->UART_CLOCK |= ARM_UART_CLOCK_EN2 | ARM_UART_CLOCK_BRG2(2);
	}
	reg->IBRD = ARM_UART_IBRD (khz*1000/4, baud);
	reg->FBRD = ARM_UART_FBRD (khz*1000/4, baud);
#endif
	reg->CTL |= ARM_UART_CTL_UARTEN;		// пуск приемопередатчика
}

/*
 * Start transmitting a byte.
 * Assume the transmitter is stopped, and the transmit queue is not empty.
 * Return 1 when we are expecting the hardware interrupt.
 */
static void
uart_transmit_start (uart_t *u)
{
	UART_t *reg = (UART_t*) u->port;

	if (u->out_first == u->out_last)
		mutex_signal (&u->transmitter, 0);

	/* Check that transmitter buffer is busy. */
	if (reg->FR & ARM_UART_FR_TXFF) {	// FIFO передатчика полно
		reg->ICR = ARM_UART_RIS_TX;	// сброс прерывания
		return;
	}

	/* Nothing to transmit - stop transmitting. */
	if (u->out_first == u->out_last) {
		/* Disable `transmitter empty' interrupt. */
		reg->ICR = ARM_UART_RIS_TX;	// сброс прерывания
		return;
	}

	/* Send byte. */
	reg->DR = *u->out_first++;
	if (u->out_first >= u->out_buf + UART_OUTBUFSZ)
		u->out_first = u->out_buf;
}

/*
 * Wait for transmitter to finish.
 */
static void
uart_fflush (uart_t *u)
{
	UART_t *reg = (UART_t*) u->port;

	mutex_lock (&u->transmitter);

	/* Check that transmitter is enabled. */
	if (reg->CTL & ARM_UART_CTL_TXE)
		while (u->out_first != u->out_last)
			mutex_wait (&u->transmitter);

	mutex_unlock (&u->transmitter);
}

/*
 * Send a byte to the UART transmitter.
 */
void
uart_putchar (uart_t *u, short c)
{
	UART_t *reg = (UART_t*) u->port;
	unsigned char *newlast;

	mutex_lock (&u->transmitter);

	/* Check that transmitter is enabled. */
	if (reg->CTL & ARM_UART_CTL_TXE) {
again:		newlast = u->out_last + 1;
		if (newlast >= u->out_buf + UART_OUTBUFSZ)
			newlast = u->out_buf;
		while (u->out_first == newlast)
			mutex_wait (&u->transmitter);

		*u->out_last = c;
		u->out_last = newlast;
		uart_transmit_start (u);

		if (c == '\n') {
			c = '\r';
			goto again;
		}
	}
	mutex_unlock (&u->transmitter);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
uart_getchar (uart_t *u)
{
	unsigned char c;

	mutex_lock (&u->receiver);

	/* Wait until receive data available. */
	while (u->in_first == u->in_last)
		mutex_wait (&u->receiver);

	c = *u->in_first++;
	if (u->in_first >= u->in_buf + UART_INBUFSZ)
		u->in_first = u->in_buf;

	mutex_unlock (&u->receiver);
	return c;
}

int
uart_peekchar (uart_t *u)
{
	int c;

	mutex_lock (&u->receiver);
	c = (u->in_first == u->in_last) ? -1 : *u->in_first;
	mutex_unlock (&u->receiver);
	return c;
}

/*
 * Fast interrupt handler.
 * Assume the transmitter is stopped, and the transmit queue is not empty.
 * Return 1 when we are expecting the hardware interrupt.
 */
static bool_t
uart_interrupt (uart_t *u)
{
	UART_t *reg = (UART_t*) u->port;

//debug_printf ("[%08x] ", reg->FR);
	return 0;
}

/*
 * Interrupt task.
 */
static void
uart_task (void *arg)
{
	uart_t *u = arg;
	UART_t *reg = (UART_t*) u->port;
	unsigned char c = 0, *newlast;

	/*
	 * Enable UART.
	 */
	mutex_lock_irq (&u->receiver, (u->port==ARM_UART1_BASE) ? 6 : 7,
		uart_interrupt, arg);
	reg->CTL = 0;

	/* Управление линией */
	reg->LCRH = ARM_UART_LCRH_WLEN8;	// длина слова 8 бит
		   ARM_UART_LCRH_FEN;		// FIFO

	/* Пороги FIFO */
	reg->IFLS = ARM_UART_IFLS_RX_1_8 |	// приём: 1/8 буфера
		  ARM_UART_IFLS_TX_1_8;		// передача: 1/8 буфера

	/* Управление */
	reg->CTL = ARM_UART_CTL_RXE |		// приём разрешен
		ARM_UART_CTL_TXE;		// передача разрешена
	reg->ICR = ~0;				// сброс прерывания
	reg->DMACTL = 0;			// управление DMA
	reg->IM = ARM_UART_RIS_TX |		// прерывания от передатчика
		ARM_UART_RIS_RX;		// от приемника
	reg->CTL |= ARM_UART_CTL_UARTEN;	// пуск приемопередатчика

	for (;;) {
		unsigned ris = reg->RIS;
		if (! (ris & ARM_UART_RIS_RX)) {
			if (ris & ARM_UART_RIS_TX) {
				uart_transmit_start (u);
			}

			mutex_wait (&u->receiver);
			continue;
		}

		/* Check that receive data is available,
		 * and get the received byte. */
		if (reg->FR & ARM_UART_FR_RXFE) 	// FIFO приемника пусто
			continue;
		c = reg->DR;				// данные

		newlast = u->in_last + 1;
		if (newlast >= u->in_buf + UART_INBUFSZ)
			newlast = u->in_buf;

		/* Ignore input on buffer overflow. */
		if (u->in_first == newlast)
			continue;

		*u->in_last = c;
		u->in_last = newlast;
	}
}

mutex_t *
uart_receive_lock (uart_t *u)
{
	return &u->receiver;
}

static stream_interface_t uart_interface = {
	.putc = (void (*) (stream_t*, short))		uart_putchar,
	.getc = (unsigned short (*) (stream_t*))	uart_getchar,
	.peekc = (int (*) (stream_t*))			uart_peekchar,
	.flush = (void (*) (stream_t*))			uart_fflush,
	.receiver = (mutex_t *(*) (stream_t*))		uart_receive_lock,
};

void
uart_init (uart_t *u, small_uint_t port, int prio, unsigned int khz,
	unsigned long baud)
{
	u->interface = &uart_interface;
	u->in_first = u->in_last = u->in_buf;
	u->out_first = u->out_last = u->out_buf;
	u->khz = khz;
	u->port = (port == 0) ? ARM_UART1_BASE : ARM_UART2_BASE;

	/* Setup baud rate generator. */
	setup_baud_rate (u->port, u->khz, baud);

	/* Create uart task. */
	task_create (uart_task, u, "uart", prio,
		u->rstack, sizeof (u->rstack));
}
