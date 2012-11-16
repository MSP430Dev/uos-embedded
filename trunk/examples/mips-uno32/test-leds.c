/*
 * Testing debug output.
 */
#include <runtime/lib.h>

#define LED1	(1 << 6)    /* RG6: green */
#define LED2	(1 << 0)    /* RF6: green */

int main (void)
{
	/* Configure LED pins as output high. */
	PORTGSET = LED1;
	TRISGCLR = LED1;

	PORTFSET = LED2;
	TRISFCLR = LED2;

	for (;;) {
		/* Continuously turn all LEDS on and off. */
		PORTGCLR = LED1;   mdelay (100);
		PORTFCLR = LED2;   mdelay (100);
		PORTGSET = LED1;   mdelay (100);
		PORTFSET = LED2;   mdelay (100);
	}
}
