#include <runtime/lib.h>

/*
 * Millisecond delay.
 */
void mdelay (unsigned msec)
{
	while (msec-- > 0) {
		udelay (1000);
	}
}
