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
 * Initialize the group data structure.
 * Buffer must have at least sizeof(lock_group_t) bytes.
 * It must be zeroed before call to lock_group_init().
 */
lock_group_t *
lock_group_init (array_t *buf, unsigned buf_size)
{
	lock_group_t *g;

	assert (buf_size >= sizeof(lock_group_t));
	g = (lock_group_t*) buf;
	g->size = 1 + (buf_size - sizeof(lock_group_t)) / sizeof(lock_slot_t);
	return g;
}

/*
 * Add a lock to the group.
 * Return 1 on success, 0 on failure.
 */
bool_t
lock_group_add (lock_group_t *g, lock_t *m)
{
	lock_slot_t *s;

	__lock_check (m);
	if (g->num >= g->size)
		return 0;
	s = g->slot + g->num;
	list_init (&s->entry);
	s->group = g;
	s->lock = m;
	s->message = 0;
	++g->num;
	return 1;
}

/*
 * Start listening on all locks in the group.
 * Attach slots to the lock->groups linked list of every lock.
 * Use lock_group_unlisten() to stop listening.
 * Beware, multiple calls to lock_group_listen() will crash the system!
 */
void
lock_group_listen (lock_group_t *g)
{
	arch_state_t x;
	lock_slot_t *s;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	for (s = g->slot + g->num; --s >= g->slot; ) {
		assert (! list_is_linked (&s->entry));
		s->message = 0;
		list_ahead (&s->lock->groups, &s->entry);
	}
	arch_intr_restore (x);
}

/*
 * Stop listening on the group.
 * Detach slots from the lock->groups linked lists.
 * Use lock_group_listen() to start listening.
 * Beware, multiple calls to lock_group_unlisten() will crash the system!
 */
void
lock_group_unlisten (lock_group_t *g)
{
	arch_state_t x;
	lock_slot_t *s;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	for (s = g->slot + g->num; --s >= g->slot; ) {
		assert (list_is_linked (&s->entry));
		s->message = 0;
		list_del (&s->entry);
	}
	arch_intr_restore (x);
}

/*
 * Wait for the signal on any lock in the group.
 * The calling task is blocked until the lock_signal().
 * Returns the lock and the signalled message.
 */
void
lock_group_wait (lock_group_t *g, lock_t **lock_ptr, void **msg_ptr)
{
	arch_state_t x;
	lock_slot_t *s;

	arch_intr_disable (&x);
	assert (STACK_GUARD (task_current));
	assert (task_current->wait == 0);
	assert (g->num > 0);

	for (;;) {
		/* Find an active slot. */
		for (s = g->slot + g->num; --s >= g->slot; ) {
			if (s->active) {
				if (lock_ptr)
					*lock_ptr = s->lock;
				if (msg_ptr)
					*msg_ptr = s->message;
				s->active = 0;
				arch_intr_restore (x);
				return;
			}
		}

		/* Suspend the task. */
		task_dequeue (task_current);
		g->waiter = task_current;
		task_schedule ();
	}
}
