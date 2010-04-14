/*
 * UART driver for external 3-channel controller.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/uartx.h>

/*
 * External UART registers.
 */
#define UARTX_R(n,r)	*(volatile unsigned*)(0xB0001000 + ((n)<<12) + (r))

#define UARTX_RBR(n)	UARTX_R (n, 0x00)	/* Приемный буфер */
#define UARTX_THR(n)	UARTX_R (n, 0x00)	/* Передающий буфер */
#define UARTX_IER(n)	UARTX_R (n, 0x04)	/* Разрешение прерываний */
#define UARTX_IIR(n)	UARTX_R (n, 0x08)	/* Идентификация прерывания */
#define UARTX_FCR(n)	UARTX_R (n, 0x08)	/* Управление FIFO */
#define UARTX_LCR(n)	UARTX_R (n, 0x0C)	/* Управление линией */
#define UARTX_MCR(n)	UARTX_R (n, 0x10)	/* Управление модемом */
#define UARTX_LSR(n)	UARTX_R (n, 0x14)	/* Состояние линии */
#define UARTX_MSR(n)	UARTX_R (n, 0x18)	/* Состояние модема */
#define UARTX_SPR(n)	UARTX_R (n, 0x1C)	/* Scratch Pad */
#define UARTX_DLL(n)	UARTX_R (n, 0x00)	/* Делитель младший */
#define UARTX_DLM(n)	UARTX_R (n, 0x04)	/* Делитель старший */
#define UARTX_SCLR(n)	UARTX_R (n, 0x14)	/* Предделитель (scaler) */

#define RECEIVE_IRQ	36			/* external interrupt /IRQ2 */

mutex_t uartx_lock;

/**\def TIMER_STACKSZ
 * \~english
 * Size of stack for UART task in bytes.
 *
 * \~russian
 * Размер стека для задачи драйвера UART, в байтах.
 */
#ifndef UART_STACKSZ
#   if __AVR__
#      define UART_STACKSZ	0x100		/* 100 enough for AVR */
#   endif
#   if defined (__arm__) || defined (__thumb__)
#      define UART_STACKSZ	0x200
#   endif
#   if MIPS32
#      define UART_STACKSZ	0x400
#   endif
#   if MSP430
#      define UART_STACKSZ	0x100
#   endif
#   if LINUX386
#      define UART_STACKSZ	4000
#   endif
#endif

ARRAY (uartx_rstack, UART_STACKSZ);		/* task receive stack */

/*
 * Start transmitting a byte.
 * Assume the transmitter is stopped, and the transmit queue is not empty.
 * Return 1 when we are expecting the hardware interrupt.
 */
static bool_t
uartx_transmit_start (uartx_t *u)
{
	if (u->out_first == u->out_last)
		mutex_signal (&u->transmitter, 0);

	/* Check that transmitter buffer is busy. */
	if (! (UARTX_LSR(u->port) & MC_LSR_TXRDY)) {
		return 1;
	}

	/* Nothing to transmit or no CTS - stop transmitting. */
	if (u->out_first == u->out_last) {
		/* Disable `transmitter empty' interrupt. */
		UARTX_IER (u->port) &= ~MC_IER_ETXRDY;
		return 0;
	}

	/* Send byte. */
	UARTX_THR (u->port) = *u->out_first;

	++u->out_first;
	if (u->out_first >= u->out_buf + UART_OUTBUFSZ)
		u->out_first = u->out_buf;

	/* Enable `transmitter empty' interrupt. */
	UARTX_IER (u->port) |= MC_IER_ETXRDY;
	return 1;
}

/*
 * Wait for transmitter to finish.
 */
static void
uartx_fflush (uartx_t *u)
{
	mutex_lock (&u->transmitter);

	while (u->out_first != u->out_last)
		mutex_wait (&u->transmitter);

	mutex_unlock (&u->transmitter);
}

/*
 * Send a byte to the UART transmitter.
 */
static void
uartx_putchar (uartx_t *u, short c)
{
	unsigned char *newlast;

	mutex_lock (&u->transmitter);

	newlast = u->out_last + 1;
	if (newlast >= u->out_buf + UART_OUTBUFSZ)
		newlast = u->out_buf;
	while (u->out_first == newlast)
		mutex_wait (&u->transmitter);

	/* TODO: unicode to utf8 conversion. */
	*u->out_last = c;
	u->out_last = newlast;
	uartx_transmit_start (u);

	mutex_unlock (&u->transmitter);
}

/*
 * Wait for the byte to be received and return it.
 */
static unsigned short
uartx_getchar (uartx_t *u)
{
	unsigned char c;

	mutex_lock (&u->receiver);

	/* Wait until receive data available. */
	while (u->in_first == u->in_last)
		mutex_wait (&u->receiver);
	/* TODO: utf8 to unicode conversion. */
	c = *u->in_first++;
	if (u->in_first >= u->in_buf + UART_INBUFSZ)
		u->in_first = u->in_buf;

	mutex_unlock (&u->receiver);
	return c;
}

static int
uartx_peekchar (uartx_t *u)
{
	int c;

	mutex_lock (&u->receiver);
	/* TODO: utf8 to unicode conversion. */
	c = (u->in_first == u->in_last) ? -1 : *u->in_first;
	mutex_unlock (&u->receiver);
	return c;
}

static void
uartx_interrupt (uartx_t *u)
{
	u->lsr = UARTX_LSR (u->port);
	/*debug_printf ("lsr%d=%02x; ", u->port, u->lsr);*/

	if (u->lsr & MC_LSR_FE) {
		u->frame_errors++;
	}
	if (u->lsr & MC_LSR_PE) {
		u->parity_errors++;
	}
	if (u->lsr & MC_LSR_OE) {
		u->overruns++;
	}
	if (u->lsr & MC_LSR_BI) {
		/*debug_printf ("BREAK DETECTED\n");*/
	}
	uartx_transmit_start (u);

	/* Check that receive data is available,
	 * and get the received byte. */
	if (u->lsr & MC_LSR_RXRDY) {
		unsigned c = UARTX_RBR (u->port);

		unsigned char *newlast = u->in_last + 1;
		if (newlast >= u->in_buf + UART_INBUFSZ)
			newlast = u->in_buf;

		/* Ignore input on buffer overflow. */
		if (u->in_first != newlast) {
			*u->in_last = c;
			u->in_last = newlast;
		} else
			u->overruns++;
		mutex_signal (&u->receiver, 0);
	}
}

/*
 * Receive interrupt task.
 */
static void
uartx_receiver (void *arg)
{
	uartx_t *u = arg;
	unsigned port;

	/*
	 * Enable receiver.
	 */
	mutex_lock_irq (&uartx_lock, RECEIVE_IRQ, 0, 0);
	UARTX_IER (0) |= MC_IER_ERXRDY | MC_IER_ERLS;
	UARTX_IER (1) |= MC_IER_ERXRDY | MC_IER_ERLS;
	UARTX_IER (2) |= MC_IER_ERXRDY | MC_IER_ERLS;

	for (;;) {
		mutex_wait (&uartx_lock);
/*debug_putchar (0, '!');*/
		for (port=0; port<3; port++)
			uartx_interrupt (u + port);
	}
}

static mutex_t *
uartx_receive_lock (uartx_t *u)
{
	return &u->receiver;
}

static stream_interface_t uartx_interface = {
	.putc = (void (*) (stream_t*, short))		uartx_putchar,
	.getc = (unsigned short (*) (stream_t*))	uartx_getchar,
	.peekc = (int (*) (stream_t*))			uartx_peekchar,
	.flush = (void (*) (stream_t*))			uartx_fflush,
	.receiver = (mutex_t *(*) (stream_t*))		uartx_receive_lock,
};

/*
 * Проверка регистров UART.
 */
void
uartx_test_registers (int port)
{
	unsigned i, msr, spr, errors = 0;

	/* Проверяем регистр модемного статуса.
	 * Он не может быть FF. */
	(void) UARTX_MSR (port);
	for (i=0; i<10; i++) {
		msr = UARTX_MSR (port);
		if (msr == 0xff) {
			debug_printf ("uartx %d error: MSR=%02x\n",
				port, msr);
			++errors;
		}
	}

	/* Проверяем, что регистр Scratchpad доступен на запись. */
	for (i=0; i<0x100; i++) {
		UARTX_SPR (port) = i;
		spr = UARTX_SPR (port);
		if (spr != i) {
			debug_printf ("uartx %d error: SPR written %02x read %02x\n",
				port, i, spr);
			++errors;
		}
	}
	UARTX_SPR (port) = 0;
	if (! errors) {
		debug_printf ("Testing UART%d registers: OK\n", port);
	}
}

/*
 * Проверка прерывания UART.
 */
void
uartx_test_irq ()
{
	unsigned cause, errors = 0;

	UARTX_IER (0) = 0;
	UARTX_IER (1) = 0;
	UARTX_IER (2) = 0;
	udelay (10);
	cause = mips32_read_c0_register (C0_CAUSE);
	if (cause & CA_IP_IRQ2) {
		debug_printf ("uartx error: incorrect /IRQ2, cause=%08x\n",
			cause);
		++errors;
	}

	UARTX_IER (0) = ~0;
	UARTX_IER (1) = ~0;
	UARTX_IER (2) = ~0;
	udelay (10);
	cause = mips32_read_c0_register (C0_CAUSE);
	if (! (cause & CA_IP_IRQ2)) {
		debug_printf ("uartx error: no /IRQ2, cause=%08x\n",
			cause);
		++errors;
	}

	UARTX_IER (0) = 0;
	UARTX_IER (1) = 0;
	UARTX_IER (2) = 0;
	udelay (10);
	cause = mips32_read_c0_register (C0_CAUSE);
	if (cause & CA_IP_IRQ2) {
		debug_printf ("uartx error: unexpected /IRQ2, cause=%08x\n",
			cause);
		++errors;
	}
	if (! errors) {
		debug_printf ("Testing UART interrupts: OK\n");
	}
}

void
uartx_init (uartx_t *u, int prio, unsigned int khz, unsigned long baud)
{
	unsigned port, divisor;

	divisor = MC_DL_BAUD (khz * 1000, baud);
	for (port=0; port<3; port++) {
		u[port].interface = &uartx_interface;
		u[port].port = port;
		u[port].khz = khz;
		u[port].in_first = u[port].in_last = u[port].in_buf;
		u[port].out_first = u[port].out_last = u[port].out_buf;

		/* Setup baud rate generator. */
		UARTX_LCR (port) = MC_LCR_8BITS | MC_LCR_DLAB;
		UARTX_DLM (port) = divisor >> 8;
		UARTX_DLL (port) = divisor;
		UARTX_LCR (port) = MC_LCR_8BITS;
		UARTX_SCLR (port) = 0;
		UARTX_SPR (port) = 0;
		UARTX_IER (port) = 0;
		UARTX_MSR (port) = 0;
		UARTX_MCR (port) = MC_MCR_DTR | MC_MCR_RTS | MC_MCR_OUT2;
		UARTX_FCR (port) = MC_FCR_RCV_RST | MC_FCR_XMT_RST | MC_FCR_ENABLE;

		/* Clear pending status, data and irq. */
		(void) UARTX_LSR (port);
		(void) UARTX_MSR (port);
		(void) UARTX_RBR (port);
		(void) UARTX_IIR (port);

		uartx_test_registers (port);
	}

	uartx_test_irq ();

	/* Create uart receive task. */
	task_create (uartx_receiver, u, "uartx", prio,
		uartx_rstack, sizeof (uartx_rstack));
}
