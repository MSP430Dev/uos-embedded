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
 * Send the signal to the lock. All tasks waiting for the signal
 * on the lock are unblocked, possibly causing task switch.
 */
void
lock_signal (lock_t *m, void *message)
{
	arch_state_t x;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	__lock_check (m);

	if (! list_is_empty (&m->waiters) || ! list_is_empty (&m->groups)) {
		lock_activate (m, message);
		if (task_need_schedule)
			task_schedule ();
	}
	arch_intr_restore (x);
}

/*
 * Wait for the signal on the lock. The calling task is blocked.
 * In the case the lock has associated IRQ number,
 * it is unmasked (permitted to happen).
 */
void *
lock_wait (lock_t *m)
{
	arch_state_t x;
#if RECURSIVE_LOCKS
	small_int_t deep;
#endif

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	assert (task_current->wait == 0);
	__lock_check (m);

	/* On pending irq, we must call fast handler. */
	if (m->irq) {
		if (m->irq->pending) {
			m->irq->pending = 0;
			if ((m->irq->handler) (m->irq->arg) == 0) {
				/* Unblock all tasks, waiting for irq. */
				lock_activate (m, 0);
				if (task_need_schedule)
					task_schedule ();
				arch_intr_restore (x);
				return 0;
			}
		}
 		arch_intr_allow (m->irq->irq);
	}

	task_current->wait = m;
	task_move (&m->waiters, task_current);
 	if (m->master != task_current) {
		/* LY: мы не удерживаем lock, поэтому просто ждем сигнала. */
		task_schedule ();
 		arch_intr_restore (x);
 		return task_current->message;
 	}

 	/* The lock is hold by the current task - release it. */
#if RECURSIVE_LOCKS
	assert (m->deep > 0);
	deep = m->deep;
	m->deep = 0;
#endif
	lock_dequeue (m);

	/* Recalculate the value of task priority.
	 * It must be the maximum of base priority,
	 * and all slave lock priorities. */
	if (task_current->prio <= m->prio && task_current->base_prio < m->prio)
		task_recalculate_prio (task_current);

	m->master = 0;
	while (! list_is_empty (&m->slaves)) {
		task_t *t = list_first_entry (&m->slaves, task_t, entry);
		assert (t->lock == m);
		t->lock = 0;
		task_activate (t);
	}
	m->prio = 0;
	task_schedule ();

	/* Acquire the lock again. */
	while (m->master) {
		/* Monitor is locked, block the task. */
		assert (task_current->lock == 0);
#if RECURSIVE_LOCKS
		assert (m->deep > 0);
#endif
		task_current->lock = m;

		/* Put this task into the list of lock slaves. */
		task_move (&m->slaves, task_current);

		/* Update the value of lock priority.
		 * It must be the maximum of all slave task priorities. */
		if (m->prio < task_current->prio) {
			m->prio = task_current->prio;

			/* Increase the priority of master task. */
			if (m->master->prio < m->prio)
				m->master->prio = m->prio;
		}

		task_schedule ();
	}

	/* Put this lock into the list of task slaves. */
	m->master = task_current;
#if RECURSIVE_LOCKS
	assert (m->deep == 0);
	m->deep = deep;
#endif
 	lock_enqueue (&task_current->slaves, m);

	/* Update the value of task priority.
	 * It must be the maximum of base priority,
	 * and all slave lock priorities. */
	if (task_current->prio < m->prio)
		task_current->prio = m->prio;

	arch_intr_restore (x);
	return task_current->message;
}
