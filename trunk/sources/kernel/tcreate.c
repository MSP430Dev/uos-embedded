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
 * Create new task.
 */
task_t *
task_create (void (*func)(void*), void *arg, const char *name, int_t prio,
	opacity_t *stack, unsigned stacksz)
{
	task_t *t;
	int_t x;

	t = (task_t*) stack;
	memset (t, 0, sizeof(task_t));

	t->name = name;
	t->prio = t->base_prio = prio;
	list_init (&t->entry);
	list_init (&t->slaves);

	memset (t->stack, STACK_MAGIC, stacksz - sizeof(task_t));
	assert (STACK_GUARD (t));

	/* Build an interrupt frame on the task stack.
	 * Set instruction pointer to func, argument to arg. */
	arch_build_stack_frame (t, func, arg, stacksz);
	arch_intr_disable (&x);
	task_enqueue (&task_active, t);
	arch_intr_restore (x);
	return t;
}
