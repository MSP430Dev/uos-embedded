/*
 * Copyright (C) 2000-2005 Serge Vakulenko, <vak@cronyx.ru>
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
#include <kernel/uos.h>
#include <kernel/internal.h>

/*
 * Get the task name.
 * In case of system crash, the name could be damaged.
 * The correctness of the name is always verified.
 */
const char *
task_name (task_t *t)
{
	small_uint_t n, c;
	const unsigned char *p;

	/* Task name could have up to 16 chars. */
	p = (unsigned char*) t->name;
	for (n=0; n<16; n++) {
		/* On AVR, task name is stored in flash memory.
		 * On other architectures, there is no difference.  */
		c = flash_fetch (p++);
		if (! c) {
			if (n <= 2)
				break;
			return t->name;
		}
		if (c < ' ' || c > '~')
			break;
	}
	return "(damaged)";
}
