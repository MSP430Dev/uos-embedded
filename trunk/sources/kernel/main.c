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

#ifdef __AVR__
task_t *task_broken			/* LY: task_current value on reset/jmp0. */
	__attribute__((section(".ly")));
void *task_broken_stack			/* LY: stack_context value from task_current on reset/jmp0. */
	__attribute__((section(".ly")));
void *broken_stack			/* LY: SP value on reset/jmp0. */
	__attribute__((section(".ly")));
#endif

list_t task_active;			/* list of tasks ready to run */
task_t *task_current;			/* current running task */
task_t *task_idle;			/* background system task */
lock_irq_t lock_irq [ARCH_INTERRUPTS];	/* interrupt handlers */

static OPACITY (task_idle_data, sizeof(task_t) + sizeof(long));
bool_t task_need_schedule;

void task_force_schedule ()
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
		t = list_first_entry (&m->waiters, task_t, entry);
		assert (t->wait == m);
		t->wait = 0;
		t->message = message;
		task_activate (t);
	}

	/* Activate groups. */
	list_iterate_entry (s, &m->groups, entry) {
		assert (s->lock == m);
		s->message = message;
		s->active = 1;
		t = s->group->waiter;
/*debug_printf ("lock_activate: slot %p msg %s task %s\n", */
/*s, message, t ? t->name : "<null>");*/
		if (t) {
			assert (! list_is_linked (&t->entry));
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
#ifdef DEFINE_DEVICE_ADDR
	DEFINE_DEVICE_ADDR (task_stack_context_offset, offset_of (task_t, stack_context));
#endif
	/* Create the idle task. */
	task_idle = (task_t*) task_idle_data;
	task_idle->stack[0] = STACK_MAGIC;
	task_idle->name = "idle";
	list_init (&task_idle->slaves);
	lock_init (&task_idle->finish);

	list_init (&task_active);
	task_enqueue (&task_active, task_idle);
	task_current = task_idle;

	/* Create user tasks. */
	uos_init ();

	/* Switch to the most priority task. */
	task_force_schedule ();

	/* Idle task activity. */
	for (;;) {
		arch_idle ();
	}
}
