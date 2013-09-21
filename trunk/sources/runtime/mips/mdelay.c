/*
 * Millisecond delay routine.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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

void mdelay (unsigned msec)
{
//	PORTB = (1 << 15) | (1 << 12);
//	TRISB &= ~((1 << 15) | (1 << 12));

debug_printf ("msec = %d\n", msec);
	while (msec-- > 0) {
        //PORTBCLR = (1 << 15);
		udelay (1000);
        //PORTBSET = (1 << 15);
	}
}
