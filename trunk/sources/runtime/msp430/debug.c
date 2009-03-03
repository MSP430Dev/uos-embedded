#include <runtime/lib.h>

static int debug_char = -1;

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	msp430_intr_disable (&x);

	/* Wait for transmitter holding register empty. */
	while (! (UTCTL0 & TXEPT) && ! (U0IFG & UTXIFG0))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	TXBUF0 = c;

	/* Wait for transmitter holding register empty. */
	while (! (UTCTL0 & TXEPT) && ! (U0IFG & UTXIFG0))
		continue;

/*	watchdog_alive ();*/
	if (c == '\n') {
		c = '\r';
		goto again;
	}
	msp430_intr_restore (x);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
/*debug_printf ("getchar -> 0x%02x\n", c);*/
		return c;
	}
	msp430_intr_disable (&x);
	for (;;) {
		/* Check for errors. */
		if (URCTL0 & RXERR)
			URCTL0 &= ~(FE + PE + OE + BRK + RXERR);

		/* Wait until receive data available. */
		if (! (U0IFG & URXIFG0)) {
/*			watchdog_alive ();*/
			msp430_intr_restore (x);
			msp430_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = RXBUF0;
		break;
	}
	msp430_intr_restore (x);
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0)
		return debug_char;

	msp430_intr_disable (&x);

	/* Check for errors. */
	if (URCTL0 & RXERR)
		URCTL0 &= ~(FE + PE + OE + BRK + RXERR);

	/* Wait until receive data available. */
	if (! (U0IFG & URXIFG0)) {
		msp430_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = RXBUF0;
	msp430_intr_restore (x);
	debug_char = c;
	return c;
}

void
debug_puts (const char *p)
{
	for (; *p; ++p)
		debug_putchar (0, *p);
}
