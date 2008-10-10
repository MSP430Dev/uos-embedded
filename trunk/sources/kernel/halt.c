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
 * Halt uOS, return to the parent operating system (if any).
 * Optionally print debugging information about the system state.
 */
void
uos_halt (int dump_flag)
{
#if LINUX386
	exit (0);
#else
	unsigned char n;
	task_t *t;

	if (dump_flag) {
		task_print_debug (0);
		n = 0;
		list_iterate (t, &task_active) {
			if (t != task_idle && t != task_current)
				task_print_debug (t);
			if (! uos_valid_memory_address (t))
				break;
			if (++n > 32 || list_is_empty (&t->item)) {
				debug_puts ("...\n");
				break;
			}
		}
		if (task_current && task_current != task_idle)
			task_print_debug (task_current);

		debug_dump_stack (task_name (task_current), __builtin_alloca (0),
			(void*) task_current->stack_context, __builtin_return_address (0));
		debug_printf ("\n*** Please report this information");
	}

	/* Halt CPU. */
	debug_printf ("\n*** System halted.\n");
	for (;;)
		continue;
#endif
}
