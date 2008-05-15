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
 * Set task priority.
 */
void
task_set_priority (task_t *t, int_t new_prio)
{
	int_t x;

	MACHDEP_INTR_DISABLE (&x);
	assert (STACK_GUARD (task_current));
	t->base_prio = new_prio;

	/* Recalculate the value of task priority.
	 * It must be the maximum of base priority,
	 * and all slave lock priorities. */
	task_recalculate_prio (t);

	task_schedule ();
	MACHDEP_INTR_RESTORE (x);
}
