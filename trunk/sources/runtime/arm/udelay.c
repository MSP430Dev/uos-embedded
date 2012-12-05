#include <runtime/lib.h>

/*
 * Microsecond delay.
 */
#ifdef ARM_OMAP44XX
void udelay (unsigned usec)
{
	if (! usec)
		return;

	if (! (ARM_PRT_CONTROL & ARM_PRT_TIMER_EN)) {
		ARM_PRT_LOAD = 0xFFFFFFFF;
		ARM_PRT_CONTROL = ARM_PRT_TIMER_EN | ARM_PRT_AUTO_RELOAD;
	}
	unsigned now = ARM_PRT_COUNTER;
	unsigned final = now - usec * (SYSTIMER_KHZ / 1000);
	for (;;) {
		if (ARM_PRT_INT_STATUS & ARM_PRT_EVENT) {
			ARM_PRT_INT_STATUS = ARM_PRT_EVENT; /* clear the flag */
			final += ARM_PRT_LOAD;
		}

		/* This comparison is valid only when using a signed type. */
		now = ARM_PRT_COUNTER;
		if ((int) (now - final) < 0)
			break;
	}
}
#else
void udelay (unsigned usec)
{
	if (! usec)
		return;

	unsigned nloops = (usec * (KHZ / 1000) + 7) >> 3;
	do {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
	} while (--nloops);
}
#endif
