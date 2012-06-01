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
	AD1PCFG = 0xFFFF;
	PORTB = (1 << 15) | (1 << 12);
	TRISB &= ~((1 << 15) | (1 << 12));
	for (;;) {
		PORTBCLR = (1 << 12); mdelay (500);
		PORTBCLR = (1 << 15); mdelay (500);
		PORTBSET = (1 << 12); mdelay (500);	
		PORTBSET = (1 << 15); mdelay (500);	
	}
}
