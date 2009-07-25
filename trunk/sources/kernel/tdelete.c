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
	arch_state_t x;

	arch_intr_disable (&x);

	list_unlink (&t->item);
	if (t == task_current)
		task_need_schedule = 1;

	if (t->lock) {

		/* Recalculate the value of lock priority.
		 * It must be the maximum of all slave task priorities. */
		if (t->lock->prio <= t->prio)
			mutex_recalculate_prio (t->lock);

		t->lock = 0;
	}

	t->wait = 0;
	while (! list_is_empty (&t->slaves)) {
		mutex_t *m = (mutex_t*) list_first (&t->slaves);
		assert (t == m->master);
		mutex_unlock (m);
	}

	/* When task is destroyed, base_prio becomes 0. */
	t->base_prio = 0;
	mutex_signal (&t->finish, message);

	if (task_need_schedule)
		task_schedule ();
	arch_intr_restore (x);
}
