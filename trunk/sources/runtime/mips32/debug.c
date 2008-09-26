#include <runtime/lib.h>

static int debug_char = -1;

#ifdef ELVEES_MC24
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	mips32_intr_disable (&x);

	/* Wait for transmitter holding register empty. */
	while (! (MC_LSR & MC_LSR_TXRDY))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	MC_THR = c;

	/* Wait for transmitter holding register empty. */
	while (! (MC_LSR & MC_LSR_TXRDY))
		continue;

/*	watchdog_alive ();*/
	if (c == '\n') {
		c = '\r';
		goto again;
	}
	mips32_intr_restore (x);
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
	mips32_intr_disable (&x);
	for (;;) {
		/* Wait until receive data available. */
		if (! (MC_LSR & MC_LSR_RXRDY)) {
/*			watchdog_alive ();*/
			mips32_intr_restore (x);
			mips32_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = MC_RBR;
		break;
	}
	mips32_intr_restore (x);
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

	mips32_intr_disable (&x);

	/* Wait until receive data available. */
	if (! (MC_LSR & MC_LSR_RXRDY)) {
		mips32_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = MC_RBR;
	mips32_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ELVEES_MC24 */

void
debug_puts (const char *p)
{
	for (; *p; ++p)
		debug_putchar (0, *p);
}
