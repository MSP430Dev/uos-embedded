/*
 * Проверка светодиодов на отладочной плате
 */
#include <runtime/lib.h>

#define LED_ON(n) (ARM_GPIOD->DATA |= (1 << (10 + n)))
#define LED_OFF(n) (ARM_GPIOD->DATA &= ~(1 << (10 + n)))

void init_leds ()
{
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOD;
	ARM_GPIOD->FUNC = (ARM_GPIOD->FUNC & ~ARM_FUNC_MASK(10) & ~ARM_FUNC_MASK(11)
			& ~ARM_FUNC_MASK(12) & ~ARM_FUNC_MASK(13) & ~ARM_FUNC_MASK(14)) |
			ARM_FUNC_PORT(10) | ARM_FUNC_PORT(11) | ARM_FUNC_PORT(12) |
			ARM_FUNC_PORT(13) | ARM_FUNC_PORT(14);
	ARM_GPIOD->ANALOG |= (0x1F << 10);
	ARM_GPIOD->PWR = (ARM_GPIOD->PWR & ~ARM_PWR_MASK(10) & ~ARM_PWR_MASK(11)
			 & ~ARM_PWR_MASK(12) & ~ARM_PWR_MASK(13)  & ~ARM_PWR_MASK(14)) |
			ARM_PWR_FASTEST(10) | ARM_PWR_FASTEST(11) | ARM_PWR_FASTEST(12) |
			ARM_PWR_FASTEST(13) | ARM_PWR_FASTEST(14);
	ARM_GPIOD->OE |= (0x1F << 10);
}

int main (void)
{
	init_leds ();

	int i = 0;	
	for (;;) {
		LED_OFF(i);
		i = (i + 1) % 5;
		LED_ON(i);
		mdelay (500);
	}
}
