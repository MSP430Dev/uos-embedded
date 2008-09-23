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
 * Halt uOS, return to the parent operating system.
 */
void
uos_halt (int dump_flag)
{
	uint_t n;
	task_t *t;
	void *callee = __builtin_return_address (0);
	void *sp = arch_get_stack_pointer ();

	if (dump_flag) {
		debug_task_print (0);
		n = 0;
		list_iterate_entry (t, &task_active, entry) {
			if (t != task_idle && t != task_current)
				debug_task_print (t);
			if (! uos_valid_memory_address (t))
				break;
			if (++n > 32 || t == list_entry (t->entry.f, task_t, entry)) {
				debug_puts ("...\n");
				break;
			}
		}
		if (task_current && task_current != task_idle)
			debug_task_print (task_current);

		debug_dump_stack (__debug_task_name (task_current), sp,
			(void*) task_current->stack_context, callee);
		debug_printf ("\n*** Please report this information\n");
	}
	arch_halt ();
}
