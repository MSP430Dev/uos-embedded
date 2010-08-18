#include <runtime/lib.h>

/*
 * Microsecond delay.
 */
void udelay (unsigned usec)
{
	if (! usec)
		return;

#ifdef ARM_SYSTICK
	/* Precise delay using SysTick timer. */
	unsigned ctrl = ARM_SYSTICK->CTRL;
	if (! (ctrl & ARM_SYSTICK_CTRL_ENABLE)) {
		/* Start timer using HCLK clock, no interrupts. */
		ARM_SYSTICK->LOAD = 0xFFFFFF;
		ARM_SYSTICK->CTRL = ARM_SYSTICK_CTRL_HCLK |
			ARM_SYSTICK_CTRL_ENABLE;
	}
	unsigned load = ARM_SYSTICK->LOAD & 0xFFFFFF;
	unsigned now = ARM_SYSTICK->VAL & 0xFFFFFF;
	unsigned final = now - usec * (KHZ / 1000);

	for (;;) {
		ctrl = ARM_SYSTICK->CTRL;
		if (ctrl & ARM_SYSTICK_CTRL_COUNTFLAG) {
			final += load;
		}

		/* This comparison is valid only when using a signed type. */
		now = ARM_SYSTICK->VAL & 0xFFFFFF;
		if ((int) ((now - final) << 8) < 0)
			break;
	}
#else
	unsigned nloops = (usec * (KHZ / 1000) + 7) >> 3;
	do {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
	} while (--nloops);
#endif
}
