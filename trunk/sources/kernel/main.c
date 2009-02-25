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

list_t task_active;			/* list of tasks ready to run */
task_t *task_current;			/* current running task */
task_t *task_idle;			/* background system task */
lock_irq_t lock_irq [ARCH_INTERRUPTS];	/* interrupt handlers */

static ARRAY (task_idle_data, sizeof(task_t) + sizeof(long));
bool_t task_need_schedule;

/*
 * Switch to most priority task if needed.
 */
void task_schedule ()
{
	task_t *new;

	task_need_schedule = 0;
	new = task_policy ();
	if (new != task_current) {
		new->ticks++;
		arch_task_switch (new);
	}
}

/*
 * Activate all waiters of the lock.
 */
void
lock_activate (lock_t *m, void *message)
{
	task_t *t;
	lock_slot_t *s;

	assert (m != 0);

	while (! list_is_empty (&m->waiters)) {
		t = (task_t*) list_first (&m->waiters);
		assert (t->wait == m);
		t->wait = 0;
		t->message = message;
		task_activate (t);
	}

	/* Activate groups. */
	list_iterate (s, &m->groups) {
		assert (s->lock == m);
		s->message = message;
		s->active = 1;
		t = s->group->waiter;
		if (t) {
			assert (list_is_empty (&t->item));
			s->group->waiter = 0;
			task_activate (t);
		}
	}
}

/*
 * Call user initialization routine uos_init(),
 * then create the idle task, and run the OS.
 * The idle task uses the default system stack.
 */
int
main (void)
{
	/* Create the idle task. */
	task_idle = (task_t*) task_idle_data;
	task_idle->stack[0] = STACK_MAGIC;
	task_idle->name = "idle";
	list_init (&task_idle->item);
	list_init (&task_idle->slaves);

	/* Make list of active tasks. */
	list_init (&task_active);
	task_current = task_idle;
	task_activate (task_idle);

	/* Create user tasks. */
	uos_init ();

	/* Switch to the most priority task. */
	assert (task_current == task_idle);
	task_schedule ();

	/* Idle task activity. */
	for (;;) {
		arch_idle ();
	}
}
