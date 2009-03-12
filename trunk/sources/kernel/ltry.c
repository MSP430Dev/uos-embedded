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
 * Try to get the lock. Return 1 on success, 0 on failure.
 * The calling task does not block.
 * In the case the lock has associated IRQ number,
 * after acquiring the lock the IRQ will be disabled.
 */
bool_t
lock_try (lock_t *m)
{
	arch_state_t x;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	if (! m->item.next)
		lock_init (m);

 	if (m->master && m->master != task_current) {
 		/* Monitor is locked. */
 		arch_intr_restore (x);
 		return 0;
 	}
 	if (! m->master) {
 		assert (list_is_empty (&m->slaves));
#if RECURSIVE_LOCKS
		assert (m->deep == 0);
#endif
 		m->master = task_current;

		/* Put this lock into the list of task slaves. */
		list_append (&task_current->slaves, &m->item);

		/* Update the value of task priority.
		 * It must be the maximum of base priority,
		 * and all slave lock priorities. */
		if (task_current->prio < m->prio)
			task_current->prio = m->prio;
	}
#if RECURSIVE_LOCKS
	++m->deep;
#endif
	arch_intr_restore (x);
 	return 1;
}
