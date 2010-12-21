/*
 * Testing debug output.
 */
#include <runtime/lib.h>

#define LEDUSB	8
#define LED1	4
#define LED2	2
#define LED3	1

int main (void)
{
	PORTE = 0xFFF0;
	TRISE = 0xFFF0;
	for (;;) {
		PORTESET = LEDUSB; mdelay (100);
		PORTESET = LED1;   mdelay (100);
		PORTESET = LED2;   mdelay (100);
		PORTESET = LED3;   mdelay (100);
		PORTECLR = LEDUSB; mdelay (100);
		PORTECLR = LED1;   mdelay (100);
		PORTECLR = LED2;   mdelay (100);
		PORTECLR = LED3;   mdelay (100);
	}
}
