/*
 * Testing debug output.
 */
#include <runtime/lib.h>

#define LED1	8
#define LED2	4
#define LED3	2
#define LED4	1

int main (void)
{
	LATE = 0xFFF0;
	TRISE = 0xFFF0;
	for (;;) {
		LATESET = LED1; mdelay (200);
		LATESET = LED2; mdelay (200);
		LATESET = LED3; mdelay (200);
		LATESET = LED4; mdelay (200);
		LATECLR = LED1; mdelay (200);
		LATECLR = LED2; mdelay (200);
		LATECLR = LED3; mdelay (200);
		LATECLR = LED4; mdelay (200);
	}
}
