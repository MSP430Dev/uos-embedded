/*
 * The internal uOS definitions.
 * Not for the end user.
 *
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
#ifndef __KERNEL_INTERNAL_H_
#define	__KERNEL_INTERNAL_H_ 1

#if __AVR__
#	include <kernel/avr/machdep.h>
#endif
#if defined (__arm__) || defined (__thumb__)
#	include <kernel/arm/machdep.h>
#endif
#if I386
#	include <kernel/i386/machdep.h>
#endif
#if MIPS32
#	include <kernel/mips32/machdep.h>
#endif
#if MSP430
#	include <kernel/msp430/machdep.h>
#endif
#if LINUX386
#	include <kernel/linux386/machdep.h>
#endif

/*
 * ----------
 * | Task   |
 * | ~~~~   |
 * | lock ----> M
 * |	    |
 * | wait ----> M
 * |	    |
 * | slaves --> M -> M -> M...
 * ---------- <-/----/----/
 */
struct _task_t {
	list_t		item;		/* double linked list pointers */
	lock_t *	lock;		/* lock, blocking the task */
	lock_t *	wait;		/* lock, the task is waiting for */
	list_t		slaves;		/* locks, acquired by task */
	void *		message;	/* return value for lock_wait() */
	void *		privatep;	/* private task data pointer */
	const char *	name;		/* printable task name */
	small_int_t	base_prio;	/* initial task priority */
	small_int_t	prio;		/* current task priority */
	arch_stack_t	stack_context;	/* saved sp when not running */
	lock_t		finish;		/* lock to wait on for task finished */
	unsigned long	ticks;		/* a number of switches to the task */
	unsigned char	stack [1]	/* stack area is placed here */
		__attribute__((aligned(sizeof(void*))));
};

struct _lock_irq_t {
	lock_t *	lock;		/* lock, associated with this irq */
	handler_t	handler;	/* fast interrupt handler */
	void *		arg;		/* argument for fast handler */
	small_int_t	irq;		/* irq number */
	bool_t		pending;	/* interrupt is pending */
};

/* The table of interrupt handlers. */
extern lock_irq_t lock_irq [];

/* Current running task. */
extern task_t *task_current;

/* Global flag set to 1 when update task_active */
extern bool_t task_need_schedule;

/* Special `idle' task. */
extern task_t *task_idle;

/* List of tasks ready to run. */
extern list_t task_active;

/* Switch to most priority task. */
void task_schedule (void);

/* Initialize a data structure of the lock. */
void lock_init (lock_t *);

/* Activate all waiters of the lock. */
void lock_activate (lock_t *m, void *message);

/* Recalculate task priority, based on priorities of acquired locks. */
void task_recalculate_prio (task_t *t);

/* Recalculate lock priority, based on priorities of waiting tasks. */
void lock_recalculate_prio (lock_t *m);

/* Utility functions. */
inline static bool_t task_is_waiting (task_t *task) {
	return (task->lock || task->wait);
}

inline static void task_activate (task_t *task) {
	assert (! task_is_waiting (task));
	list_append (&task_active, &task->item);
	if (task_current->prio < task->prio)
		task_need_schedule = 1;
}

/* Task policy, e.g. the scheduler.
 * Task_active contains a list of all tasks, which are ready to run.
 * Find a task with the biggest priority. */
	__attribute__ ((always_inline))
inline static task_t *task_policy (void)
{
	task_t *t, *r;

	r = task_idle;
	list_iterate (t, &task_active) {
		if (t->prio > r->prio)
			r = t;
	}
	return r;
}

#define STACK_MAGIC		0xaau

#define STACK_GUARD(x)		((x)->stack[0] == STACK_MAGIC)

#endif /* !__KERNEL_INTERNAL_H_ */
