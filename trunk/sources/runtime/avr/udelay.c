#include <runtime/lib.h>

/*
 * Microsecond delay.
 * Time is correct for KHZ up to 20000.
 * Warning: max value is 255 microseconds.
 */
 
#if 0
void udelay (volatile small_uint_t usec)
{
    small_uint_t portion = 12;
    while (usec >= portion)
        usec -= portion;
}
 
#else

void udelay (volatile small_uint_t usec)
{
	do {
		/* Two ticks for decrement and branch */
#if KHZ > 2000
		asm volatile ("nop");
#endif
#if KHZ > 3000
		asm volatile ("nop");
#endif
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
#if KHZ > 18000
		asm volatile ("nop");
#endif
#if KHZ > 19000
		asm volatile ("nop");
#endif
	} while (--usec);
}
#endif
