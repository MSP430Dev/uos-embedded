#include <runtime/lib.h>

/*
 * Microsecond delay.
 * TODO: calibrate for Elvees MC-24!
 */
void udelay (unsigned usec)
{
	unsigned nloops;

	if (! usec)
		return;
	nloops = (usec * (KHZ / 1000) + 7) >> 3;
#ifdef BOOT_FLASH_8BIT
	/* 8-bit flash memory is slower. */
	nloops = (nloops + 31) >> 5;
#endif
	do {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
	} while (--nloops);
}
