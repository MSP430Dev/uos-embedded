/*
 * Testing LEDs.
 */
#include <runtime/lib.h>

#define LED1	(1 << 15)   /* RB15: green */
#define LED2	(1 << 12)   /* RB12: yellow */

int main (void)
{
	AD1PCFG = ~0;
	PORTBSET = LED1 | LED2;
	TRISBCLR = LED1 | LED2;
	for (;;) {
		PORTBCLR = LED1; mdelay (100);
		PORTBCLR = LED2; mdelay (100);
		PORTBSET = LED1; mdelay (100);
		PORTBSET = LED2; mdelay (100);
	}
}
