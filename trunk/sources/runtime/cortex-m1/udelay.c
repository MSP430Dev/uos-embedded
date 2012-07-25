/*
 * Precise microsecond delay using SysTick timer.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>

void udelay (unsigned usec)
{
	if (! usec)
		return;

	unsigned ctrl = ARM_SYSTICK->CTRL;
	if (! (ctrl & ARM_SYSTICK_CTRL_ENABLE)) {
		/* Start timer using HCLK clock, no interrupts. */
		ARM_SYSTICK->LOAD = 0xFFFFFF;
		ARM_SYSTICK->CTRL = ARM_SYSTICK_CTRL_HCLK |
			ARM_SYSTICK_CTRL_ENABLE;
	}
	unsigned load = ARM_SYSTICK->LOAD & 0xFFFFFF;
	unsigned now = ARM_SYSTICK->VAL & 0xFFFFFF;
#ifdef SETUP_HCLK_HSI
	unsigned final = now - usec * 8;
#else
	unsigned final = now - usec * (KHZ / 1000);
#endif
	for (;;) {
		ctrl = ARM_SYSTICK->CTRL;
		if (ctrl & ARM_SYSTICK_CTRL_COUNTFLAG) {
			final += load;
		}

		/* This comparison is valid only when using a signed type. */
		now = ARM_SYSTICK->VAL & 0xFFFFFF;
		if ((int) ((now - final) << 8) < 0)
			break;
	}
}
