#include <runtime/lib.h>

/*
 * Millisecond delay.
 * Warning: max value is 255 milliseconds.
 */
void mdelay (small_uint_t usec)
{
	while (usec-- > 0) {
		udelay (250);
		udelay (250);
		udelay (250);
		udelay (250);
	}
}
