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

/*
 * Halt uOS, return to the parent operating system (if any).
 * Optionally print debugging information about the system state.
 */
void __attribute__((weak))
uos_halt (int dump_flag)
{
#if LINUX386
	exit (0);
#else
	if (dump_flag) {
		debug_dump_stack ("halt", __builtin_alloca (0),
			__builtin_frame_address (0), __builtin_return_address (0));
		debug_printf ("\n*** Please report this information");
	}

	/* Halt CPU. */
	debug_printf ("\n*** System halted.\n");
	for (;;)
		continue;
#endif
}
