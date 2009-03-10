#include <runtime/lib.h>

/*
 * Millisecond delay.
 */
void mdelay (unsigned usec)
{
	while (usec-- > 0) {
		udelay (1000);
	}
}
