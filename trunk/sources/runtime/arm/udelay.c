#include <runtime/lib.h>

/*
 * Microsecond delay.
 */
void udelay (unsigned usec)
{
	unsigned nloops;

	if (! usec)
		return;

	nloops = (usec * (KHZ / 1000) + 7) >> 3;
	do {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
	} while (--nloops);
}
