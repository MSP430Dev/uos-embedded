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
