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
 * Delete the task, return the given message to waiters.
 */
void
task_delete (task_t *t, void *message)
{
	int_t x;

	arch_intr_disable (&x);

	task_dequeue (t);
	if (t == task_current)
		task_need_schedule = 1;

	if (t->lock) {

		/* Recalculate the value of lock priority.
		 * It must be the maximum of all slave task priorities. */
		if (t->lock->prio <= t->prio)
			lock_recalculate_prio (t->lock);

		t->lock = 0;
	}

	t->wait = 0;
	while (! list_is_empty (&t->slaves)) {
		lock_t *m = list_first_entry (&t->slaves, lock_t, entry);
		assert (t == m->master);
		lock_release (m);
	}

	/* When task is destroyed, base_prio becomes 0. */
	t->base_prio = 0;
	lock_signal (&t->finish, message);

	if (task_need_schedule)
		task_schedule ();
	arch_intr_restore (x);
}
