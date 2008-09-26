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
#include <runtime/list.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

/*
 * Release the lock. All tasks waiting for the lock
 * are unblocked, possibly causing task switch.
 * Disassociate IRQ with the lock, enable interrupts.
 */
void
lock_release_irq (lock_t *m)
{
	arch_state_t x;

	assert (STACK_GUARD (task_current));
	arch_intr_disable (&x);
	assert (m->master != 0);

#if RECURSIVE_LOCKS
	assert (m->deep > 0);
	if (--m->deep > 0) {
		arch_intr_restore (x);
		return;
	}
#endif

	/* Remove this lock from the list of task slaves. */
	lock_dequeue (m);

	/* Recalculate the value of task priority.
	 * It must be the maximum of base priority,
	 * and all slave lock priorities. */
	if (m->master->prio <= m->prio && m->master->base_prio < m->prio)
		task_recalculate_prio (m->master);
	m->master = 0;

	/* On pending irq, we must call fast handler. */
	if (m->irq) {
		if (m->irq->pending) {
			m->irq->pending = 0;

			/* Unblock all tasks, waiting for irq. */
			if ((m->irq->handler) (m->irq->arg) == 0)
				lock_activate (m, 0);
		}
		arch_intr_unbind (m->irq->irq);
		m->irq->lock = 0;
		m->irq = 0;
	}

	if (! list_is_empty (&m->slaves)) {
		do {
			task_t *t = list_first_entry (&m->slaves, task_t, entry);
			assert (t->lock == m);
			t->lock = 0;
			task_activate (t);
		} while (! list_is_empty (&m->slaves));
		m->prio = 0;
	}

	task_schedule ();
	arch_intr_restore (x);
}
