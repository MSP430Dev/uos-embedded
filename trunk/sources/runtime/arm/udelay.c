#include <runtime/lib.h>

/*
 * Microsecond delay.
 */
void udelay (unsigned usec)
{
        unsigned i;

        do {
                for (i=0; i<KHZ/1000; ++i)
                        asm volatile ("nop");
        } while (--usec);
}
