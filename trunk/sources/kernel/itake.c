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
 * Get the lock. The calling task would block
 * waiting for the lock to be released by somebody.
 * Associate IRQ with the lock, disable interrupts.
 * On interrupt, the signal message is sent to the lock.
 */
void
mutex_lock_irq (mutex_t *m, int irq, handler_t func, void *arg)
{
	arch_state_t x;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	if (! m->item.next)
		mutex_init (m);

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
			if (m->master->prio < m->prio)
				m->master->prio = m->prio;
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

	m->irq = &mutex_irq [irq];
	if (! m->irq->lock)
		arch_intr_bind (irq);
	m->irq->lock = m;
	m->irq->irq = irq;
	m->irq->handler = func;
	m->irq->arg = arg;
	m->irq->pending = 0;
	arch_intr_restore (x);
}
