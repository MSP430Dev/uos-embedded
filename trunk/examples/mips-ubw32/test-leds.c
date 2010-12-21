/*
 * Testing debug output.
 */
#include <runtime/lib.h>

#define LEDUSB	8	/* PE3: green */
#define LED1	4	/* PE2: white */
#define LED2	2	/* PE1: red */
#define LED3	1	/* PE0: yellow */

int main (void)
{
	/* Configure LED pins as output high. */
	PORTE = LEDUSB | LED1 | LED2 | LED3;
	TRISE = ! (LEDUSB | LED1 | LED2 | LED3);

	for (;;) {
		/* Continuously turn all LEDS on and off. */
		PORTECLR = LEDUSB; mdelay (100);
		PORTECLR = LED1;   mdelay (100);
		PORTECLR = LED2;   mdelay (100);
		PORTECLR = LED3;   mdelay (100);
		PORTESET = LEDUSB; mdelay (100);
		PORTESET = LED1;   mdelay (100);
		PORTESET = LED2;   mdelay (100);
		PORTESET = LED3;   mdelay (100);
	}
}
