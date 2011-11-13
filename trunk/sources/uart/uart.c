#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>

#if __AVR__
#   include "avr.h"
#endif

#if ARM_S3C4530
#   include "samsung.h"
#endif

#if ARM_AT91SAM
#   include "at91sam.h"
#endif

#if PIC32MX
#   include "pic32.h"
#endif

#if defined (ELVEES)
#   include "elvees.h"
#endif

#if MSP430
#   include "msp430.h"
#endif

#if LINUX386
#   include "linux.h"
#endif

/*
 * Start transmitting a byte.
 * Assume the transmitter is stopped, and the transmit queue is not empty.
 * Return 1 when we are expecting the hardware interrupt.
 */
static bool_t
uart_transmit_start (uart_t *u)
{
	if (u->out_first == u->out_last)
		mutex_signal (&u->transmitter, 0);

	/* Check that transmitter buffer is busy. */
	if (! test_transmitter_empty (u->port)) {
		return 1;
	}

	/* Nothing to transmit or no CTS - stop transmitting. */
	if (u->out_first == u->out_last ||
	    (u->cts_query && u->cts_query (u) == 0)) {
		/* Disable `transmitter empty' interrupt. */
		disable_transmit_interrupt (u->port);
		return 0;
	}

	/* Send byte. */
	transmit_byte (u->port, *u->out_first);

	++u->out_first;
	if (u->out_first >= u->out_buf + UART_OUTBUFSZ)
		u->out_first = u->out_buf;

	/* Enable `transmitter empty' interrupt. */
	enable_transmit_interrupt (u->port);
	return 1;
}

/*
 * Wait for transmitter to finish.
 */
static void
uart_fflush (uart_t *u)
{
	mutex_lock (&u->transmitter);

	/* Check that transmitter is enabled. */
	if (test_transmitter_enabled (u->port))
		while (u->out_first != u->out_last)
			mutex_wait (&u->transmitter);

	mutex_unlock (&u->transmitter);
}

/*
 * CTS is active - wake up the transmitter.
 */
void
uart_cts_ready (uart_t *u)
{
	mutex_lock (&u->transmitter);
	uart_transmit_start (u);
	mutex_unlock (&u->transmitter);
}

/*
 * Register the CTS poller function.
 */
void
uart_set_cts_poller (uart_t *u, bool_t (*func) (uart_t*))
{
	mutex_lock (&u->transmitter);
	u->cts_query = func;
	mutex_unlock (&u->transmitter);
}

/*
 * Send a byte to the UART transmitter.
 */
void
uart_putchar (uart_t *u, short c)
{
	unsigned char *newlast;

	mutex_lock (&u->transmitter);

	/* Check that transmitter is enabled. */
	if (test_transmitter_enabled (u->port)) {
again:		newlast = u->out_last + 1;
		if (newlast >= u->out_buf + UART_OUTBUFSZ)
			newlast = u->out_buf;
		while (u->out_first == newlast)
			mutex_wait (&u->transmitter);

		/* TODO: unicode to utf8 conversion. */
		*u->out_last = c;
		u->out_last = newlast;
		uart_transmit_start (u);

		if (u->onlcr && c == '\n') {
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
	/* TODO: utf8 to unicode conversion. */
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
	/* TODO: utf8 to unicode conversion. */
	c = (u->in_first == u->in_last) ? -1 : *u->in_first;
	mutex_unlock (&u->receiver);
	return c;
}

/*
 * Receive interrupt task.
 */
static void
uart_receiver (void *arg)
{
	uart_t *u = arg;
	unsigned char c = 0, *newlast;

	/*
	 * Enable transmitter.
	 */
#ifdef TRANSMIT_IRQ
	mutex_lock_irq (&u->transmitter, TRANSMIT_IRQ (u->port),
		(handler_t) uart_transmit_start, u);
	enable_transmitter (u->port);
	mutex_unlock (&u->transmitter);
#endif
	/*
	 * Enable receiver.
	 */
	mutex_lock_irq (&u->receiver, RECEIVE_IRQ (u->port), 0, 0);
	enable_receiver (u->port);
	enable_receive_interrupt (u->port);

	for (;;) {
		mutex_wait (&u->receiver);

#ifdef clear_receive_errors
		clear_receive_errors (u->port);
#else
		if (test_frame_error (u->port)) {
			/*debug_printf ("FRAME ERROR\n");*/
			clear_frame_error (u->port);
		}
		if (test_parity_error (u->port)) {
			/*debug_printf ("PARITY ERROR\n");*/
			clear_parity_error (u->port);
		}
		if (test_overrun_error (u->port)) {
			/*debug_printf ("RECEIVE OVERRUN\n");*/
			clear_overrun_error (u->port);
		}
		if (test_break_error (u->port)) {
			/*debug_printf ("BREAK DETECTED\n");*/
			clear_break_error (u->port);
		}
#endif
#ifndef TRANSMIT_IRQ
		if (test_transmitter_enabled (u->port))
			uart_transmit_start (u);
#endif
		/* Check that receive data is available,
		 * and get the received byte. */
		if (! test_get_receive_data (u->port, &c))
			continue;
/*debug_printf ("%02x", c);*/

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
	u->onlcr = 1;
#if ARM_1986BE9
	u->port = (port == 0) ? ARM_UART1_BASE : ARM_UART2_BASE;
#else
	u->port = port;
#endif

	/* Setup baud rate generator. */
	setup_baud_rate (u->port, u->khz, baud);

	/* Create uart receive task. */
	task_create (uart_receiver, u, "uartr", prio,
		u->rstack, sizeof (u->rstack));
}
