#include <runtime/lib.h>

/*
 * Microsecond delay.
 * Time is correct for KHZ from 4000 to 18000.
 * Warning: max value is 65535 microseconds.
 */
void udelay (small_uint_t usec)
{
	do {
		/* Two ticks for decrement and two for branch */
#if KHZ > 4000
		asm volatile ("nop");
#endif
#if KHZ > 5000
		asm volatile ("nop");
#endif
#if KHZ > 6000
		asm volatile ("nop");
#endif
#if KHZ > 7000
		asm volatile ("nop");
#endif
#if KHZ > 8000
		asm volatile ("nop");
#endif
#if KHZ > 9000
		asm volatile ("nop");
#endif
#if KHZ > 10000
		asm volatile ("nop");
#endif
#if KHZ > 11000
		asm volatile ("nop");
#endif
#if KHZ > 12000
		asm volatile ("nop");
#endif
#if KHZ > 13000
		asm volatile ("nop");
#endif
#if KHZ > 14000
		asm volatile ("nop");
#endif
#if KHZ > 15000
		asm volatile ("nop");
#endif
#if KHZ > 16000
		asm volatile ("nop");
#endif
#if KHZ > 17000
		asm volatile ("nop");
#endif
	} while (--usec);
}
