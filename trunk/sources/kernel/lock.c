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

void __lock_init (lock_t *lock)
{
	list_init (&lock->item);
	list_init (&lock->waiters);
	list_init (&lock->slaves);
	list_init (&lock->groups);
	lock->irq = 0;
	lock->prio = 0;
	lock->master = 0;
#if RECURSIVE_LOCKS
	lock->deep = 0;
#endif
}

void __lock_alarm_init (lock_t *lock)
{
	debug_printf ("\nWARNING: lock %p is not initialized!\n", lock);
	__lock_init (lock);
}

/*
 * Get the lock. The calling task would block
 * waiting for the lock to be released by somebody.
 * In the case the lock has associated IRQ number,
 * after acquiring the lock the IRQ will be disabled.
 */
void
lock_take (lock_t *m)
{
	arch_state_t x;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	assert (task_current != m->master);
	__lock_check (m);

	while (m->master && m->master != task_current) {
		/* Monitor is locked, block the task. */
		assert (task_current->lock == 0);
#if RECURSIVE_LOCKS
		assert (m->deep > 0);
#endif
		task_current->lock = m;

		/* Put this task into the list of lock slaves. */
		list_append (&m->slaves, &task_current->item);

		/* Update the value of lock priority.
		 * It must be the maximum of all slave task priorities. */
		if (m->prio < task_current->prio) {
			m->prio = task_current->prio;

			/* Increase the priority of master task. */
			if (m->master->prio < m->prio) {
				m->master->prio = m->prio;
				/* No need to set task_need_schedule here. */
			}
		}

		task_schedule ();
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
}

/*
 * Recalculate the value of task priority.
 * It must be the maximum of base priority,
 * and all slave lock priorities.
 */
void
task_recalculate_prio (task_t *t)
{
	lock_t *m;
	small_int_t old_prio;

	old_prio = t->prio;
	t->prio = t->base_prio;
	list_iterate (m, &t->slaves)
		if (t->prio < m->prio)
			t->prio = m->prio;

	if (t->prio != old_prio) {
		if (t->lock) {
			if (t->prio > old_prio) {
				/* Priority increased. */
				if (t->lock->prio < t->prio)
					lock_recalculate_prio (t->lock);
			} else {
				/* Priority decreased. */
				if (t->lock->prio <= old_prio)
					lock_recalculate_prio (t->lock);
			}
		} else {
			if (t->prio > old_prio) {
				if (task_current->prio < t->prio && ! task_is_waiting (t)) {
					/* Active task increased priority - reschedule. */
					task_need_schedule = 1;
				}
			} else if (t == task_current) {
				/* Current task decreased priority - reschedule. */
				task_need_schedule = 1;
			}
		}
	}
}

/*
 * Recalculate the value of lock priority.
 * It must be the maximum of all slave task priorities.
 */
void
lock_recalculate_prio (lock_t *m)
{
	task_t *t;
	small_int_t old_prio;

	old_prio = m->prio;
	m->prio = 0;
	list_iterate (t, &m->slaves)
		if (m->prio < t->prio)
			m->prio = t->prio;

	if (m->prio != old_prio && m->master) {
		if (m->prio > old_prio) {
			/* Priority increased. */
			if (m->master->prio < m->prio)
				task_recalculate_prio (m->master);
		} else {
			/* Priority decreased. */
			if (m->master->prio <= old_prio &&
			    m->master->base_prio < old_prio)
				task_recalculate_prio (m->master);
		}
	}
}

/*
 * Release the lock. All tasks waiting for the lock
 * are unblocked, possibly causing task switch.
 * In the case the lock has associated IRQ number,
 * the IRQ will be enabled.
 */
void
lock_release (lock_t *m)
{
	arch_state_t x;

	assert (STACK_GUARD (task_current));
	arch_intr_disable (&x);
	assert (m->master != 0);

#if RECURSIVE_LOCKS
	if (--m->deep > 0) {
		arch_intr_restore (x);
		return;
	}
#endif

	/* Remove this lock from the list of task slaves. */
	list_unlink (&m->item);

	/* Recalculate the value of task priority.
	 * It must be the maximum of base priority,
	 * and all slave lock priorities. */
	if (m->master->prio <= m->prio && m->master->base_prio < m->prio)
		task_recalculate_prio (m->master);
	m->master = 0;

	/* On pending irq, we must call fast handler. */
	if (m->irq && m->irq->pending) {
		m->irq->pending = 0;

		/* Unblock all tasks, waiting for irq. */
		if ((m->irq->handler) (m->irq->arg) == 0)
			lock_activate (m, 0);
	}

	if (! list_is_empty (&m->slaves)) {
		do {
			task_t *t = (task_t*) list_first (&m->slaves);
			assert (t->lock == m);
			t->lock = 0;
			task_activate (t);
		} while (! list_is_empty (&m->slaves));
		m->prio = 0;
	}

	if (task_need_schedule)
		task_schedule ();
	arch_intr_restore (x);
}
