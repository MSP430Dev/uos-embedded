/*
 * Testing LED.
 */
#include <runtime/lib.h>

#define LED     (1 << 3)    /* PA3: green */

int main (void)
{
	/* Configure LED pins as output high. */
	PORTASET = LED;
	TRISACLR = LED;

	for (;;) {
		/* Continuously turn all LEDS on and off. */
		PORTACLR = LED; mdelay (100);
		PORTASET = LED; mdelay (100);
	}
}
