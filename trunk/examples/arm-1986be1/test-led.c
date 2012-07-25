/*
 * Проверка светодиодов на отладочной плате
 */
#include <runtime/lib.h>

#define LED_ON(n) (ARM_GPIOD->DATA |= (1 << (7 + n)))
#define LED_OFF(n) (ARM_GPIOD->DATA &= ~(1 << (7 + n)))

void init_leds ()
{
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;
	
	ARM_GPIOD->FUNC &= ~0x3FFFC000;
	ARM_GPIOD->DATA = 0x0000;
	ARM_GPIOD->OE = 0x7F80;
	ARM_GPIOD->ANALOG = 0x7F80;
	ARM_GPIOD->PWR = 0x3FFFC000;	
}

int main (void)
{
	init_leds ();

	int i = 7;	
	for (;;) {
		LED_OFF(i);
		i = (i + 1) % 8;
		LED_ON(i);
		mdelay (500);
	}	
}
