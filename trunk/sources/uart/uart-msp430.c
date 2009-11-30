#include <runtime/lib.h>
#include "msp430.h"

#ifdef __MSP430_HAS_UART0__
void
msp430_set_baud (int port, unsigned khz, unsigned long baud)
{
	unsigned div, mod, i;
	unsigned long hz, a, b;

	/* Compute baud divisor. */
	hz = khz * 500L;
	div = hz / baud;

	/* Compute modulation byte. */
	a = baud * div;
	b = hz;
	mod = 0;
	for (i=0; i<8; ++i) {
		if (b+b-a-a >= baud) {
			mod |= 1 << i;
			a += baud;
		}
		a += baud * div;
		b += hz;
	}
/*debug_printf ("port %d baud %ld: div %04x mod %02x\n", port, baud, div, mod);*/

#ifdef __MSP430_HAS_UART1__
	if (port) {
		UBR01 = div;
		UBR11 = div >> 8;
		UMCTL1 = mod;
	} else
#endif
	{
		UBR00 = div;
		UBR10 = div >> 8;
		UMCTL0 = mod;
	}
}
#endif


#ifdef __MSP430_HAS_USCI__
static unsigned long
compute_error (unsigned long hz, unsigned long baud, int mod)
{
	unsigned long a, b, err, maxerr;
	unsigned int n, i;

	n = hz / baud;
	a = baud * n;
	b = hz;
	maxerr = 0;
	for (i=0; i<8; ++i) {
		if (mod & 0x80)
			a += baud;
		err = (b > a) ? b-a : a-b;
		if (err > maxerr)
			maxerr = err;
		a += baud * n;
		b += hz;
		mod <<= 1;
	}
	return maxerr;
}

void
msp430_set_baud (int port, unsigned khz, unsigned long baud)
{
	static const unsigned char modtab[8] =
		{ 0, 0x40, 0x44, 0x54, 0x55, 0x75, 0x77, 0x7f };
	unsigned int div, best_s, s;
	unsigned long hz, min_err, err;

	/* Compute baud divisor. */
	hz = khz * 1000L;
	div = hz / baud;

	/* Compute modulation value. */
	best_s = 0;
	min_err = hz;
	for (s=0; s<8; ++s) {
		err = compute_error (hz, baud, modtab[s]);
		if (err < min_err) {
			min_err = err;
			best_s = s;
		}
	}
/*debug_printf ("port %d hz %ld baud %ld: div %04x s %d\n", port, hz, baud, div, best_s);*/

	switch (port) {
	case 0:
		UCA0BR0 = div;
		UCA0BR1 = div >> 8;
		UCA0MCTL = best_s << 1;
		break;
#ifdef __MSP430_HAS_USCI_AB1__
	case 1:
		UCA1BR0 = div;
		UCA1BR1 = div >> 8;
		UCA1MCTL = best_s << 1;
		break;
#endif
#ifdef __MSP430_HAS_USCI_AB2__
	case 2:
		UCA2BR0 = div;
		UCA2BR1 = div >> 8;
		UCA2MCTL = best_s << 1;
		break;
#endif
#ifdef __MSP430_HAS_USCI_AB3__
	case 3:
		UCA3BR0 = div;
		UCA3BR1 = div >> 8;
		UCA3MCTL = best_s << 1;
		break;
#endif
	}
}
#endif
