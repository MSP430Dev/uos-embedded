#include <runtime/lib.h>

/*
 * Microsecond delay.
 */

#ifdef ELVEES_MC24

/* Specialization for Elvees MC-24 */

#include <runtime/mips32/io.h>

#define TIME_BEFORE(unknown, known) ((long)(unknown) - (long)(known) < 0)

void udelay (unsigned usec)
{
	unsigned timeout = mips32_read_c0_register(C0_COUNT) + usec * (KHZ / 1000);
	while ( TIME_BEFORE( mips32_read_c0_register(C0_COUNT), timeout ) ) {
		asm volatile ("nop");
	};
}

#else

/* Generic delay function (has to be calibrated for each CPU type) */

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

#endif
