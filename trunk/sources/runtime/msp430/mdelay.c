#include <runtime/lib.h>

/*
 * Millisecond delay.
 * Warning: max value is 255 milliseconds.
 */
void mdelay (small_uint_t msec)
{
	while (msec-- > 0) {
		udelay (1000);
	}
}
