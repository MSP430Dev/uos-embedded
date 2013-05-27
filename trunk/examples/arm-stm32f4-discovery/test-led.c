/*
 * Проверка светодиодов на отладочной плате
 */
#include <runtime/lib.h>

#define LED_ON(n) (GPIOD->BSRR = GPIO_SET(12 + n))
#define LED_OFF(n) (GPIOD->BSRR = GPIO_RESET(12 + n))

void init_leds ()
{
    RCC->AHB1ENR |= RCC_GPIODEN;
    GPIOD->MODER = GPIO_OUT(12) | GPIO_OUT(13) | GPIO_OUT(14) | GPIO_OUT(15);
}

int main (void)
{
	init_leds ();

	int i = 0;	
	for (;;) {
		LED_OFF(i);
		i = (i + 1) % 4;
		LED_ON(i);
		mdelay (500);
	}
}
