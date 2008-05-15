#include "runtime/lib.h"
#include <s3c4530/gpio.h>

unsigned long gpio_data;

/*
 * Set the mode of N-th general purpose pin.
 * Pnum must be in range 0..25.
 * Available modes:
 *	0 - input;
 *	1 - output;
 *	2 - alternate function, only for pins 2..7 and 18..25..
 */
void
gpio_config (int pnum, int mode)
{
	switch (mode) {
	case 0:				/* input pin */
		if ((pnum >= 2 && pnum <= 7) ||
		    (pnum >= 18 && pnum <= 25))
			ARM_IOPCON1 &= ~(1 << pnum);
		ARM_IOPMOD &= ~(1 << pnum);
		break;
	case 1:				/* output pin */
		if ((pnum >= 2 && pnum <= 7) ||
		    (pnum >= 18 && pnum <= 25))
			ARM_IOPCON1 &= ~(1 << pnum);
		ARM_IOPMOD |= 1 << pnum;
		break;
	default:			/* alternate function */
		if ((pnum >= 2 && pnum <= 7) ||
		    (pnum >= 18 && pnum <= 25))
			ARM_IOPCON1 |= 1 << pnum;
		break;
	}
}
