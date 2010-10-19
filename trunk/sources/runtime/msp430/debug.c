#include <runtime/lib.h>

#ifdef MSP430_DEBUG_USCIA3
    /* Use USCI_A3 for debug output. */
#   define TX_BUSY	(! (UCA3IFG & UCTXIFG))
#   define TX_DATA	UCA3TXBUF
#   define RX_CLRERR	{ UCA3STAT &= ~(UCFE + UCPE + UCBRK); }
#   define RX_AVAIL	(UCA3IFG & UCRXIFG)
#   define RX_DATA	UCA3RXBUF

#elif defined (__MSP430_HAS_USCI__)
    /* Use USCI_A0 for debug output. */
#   define TX_BUSY	(! (IFG2 & UCA0TXIFG))
#   define TX_DATA	UCA0TXBUF
#   define RX_CLRERR	{ UCA0STAT &= ~(UCFE + UCPE + UCBRK); }
#   define RX_AVAIL	(IFG2 & UCA0RXIFG)
#   define RX_DATA	UCA0RXBUF

#else
    /* Use USART0 for debug output. */
#   define TX_BUSY	(! (UTCTL0 & TXEPT) && ! (U0IFG & UTXIFG0))
#   define TX_DATA	TXBUF0
#   define RX_CLRERR	{ URCTL0 &= ~(FE + PE + BRK); }
#   define RX_AVAIL	(U0IFG & URXIFG0)
#   define RX_DATA	RXBUF0
#endif

bool_t debug_onlcr = 1;

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
	while (TX_BUSY)
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	TX_DATA = c;

	/* Wait for transmitter holding register empty. */
	while (TX_BUSY)
		continue;

/*	watchdog_alive ();*/
	if (debug_onlcr && c == '\n') {
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
		RX_CLRERR;

		/* Wait until receive data available. */
		if (! RX_AVAIL) {
/*			watchdog_alive ();*/
			msp430_intr_restore (x);
			msp430_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = RX_DATA;
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
	RX_CLRERR;

	/* Wait until receive data available. */
	if (! RX_AVAIL) {
		msp430_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = RX_DATA;
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
