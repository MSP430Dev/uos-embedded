/*
 * Testing LEDs.
 */
#include <runtime/lib.h>

#define LED1	(1 << 15)   /* RB15: green */
#define LED2	(1 << 10)   /* RA10: red */

int main (void)
{
	AD1PCFG = ~0;
	PORTBSET = LED1;
	TRISBCLR = LED1;
	PORTASET = LED2;
	TRISACLR = LED2;
	for (;;) {
		PORTBCLR = LED1; mdelay (100);
		PORTACLR = LED2; mdelay (100);
		PORTBSET = LED1; mdelay (100);
		PORTASET = LED2; mdelay (100);
	}
}
