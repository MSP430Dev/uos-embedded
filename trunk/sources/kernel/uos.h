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
#ifndef __KERNEL_UOS_H_
#define	__KERNEL_UOS_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RECURSIVE_LOCKS
#	define RECURSIVE_LOCKS	1
#endif

/* System data structures. */
typedef struct _opacity_t opacity_t;
typedef struct _task_t task_t;
typedef struct _lock_t lock_t;
typedef struct _lock_irq_t lock_irq_t;

/* Lock group data structures. */
typedef struct _lock_group_t lock_group_t;
typedef struct _lock_slot_t lock_slot_t;

/* Task management. */
task_t *task_create (void (*func)(void*), void *arg, const char *name, int_t priority,
	opacity_t *stack, unsigned stacksz);
void task_exit (void *status);
void task_delete (task_t *task, void *status);
void *task_wait (task_t *task);
int task_stack_avail (task_t *task);
const char *task_name (task_t *task);
int_t task_priority (task_t *task);
void task_set_priority (task_t *task, int_t priority);
void *task_private (task_t *task);
void task_set_private (task_t *task, void *privatep);
void task_yield ();

/* Lock management. */
void lock_take (lock_t *lock);
void lock_release (lock_t *lock);
bool_t lock_try (lock_t *lock);
void lock_signal (lock_t *lock, void *message);
void *lock_wait (lock_t *lock);

/* Fast irq handler. */
typedef bool_t (*handler_t) (void*);

/* Interrupt management. */
void lock_take_irq (lock_t*, int_t irq, handler_t func, void *arg);
void lock_release_irq (lock_t*);

/* Utility calls. */
void uos_halt (void);

/* User-supplied startup routine. */
extern void uos_init (void);

/* Group management. */
lock_group_t *lock_group_init (opacity_t *buf, unsigned buf_size);
bool_t lock_group_add (lock_group_t*, lock_t*);
void lock_group_listen (lock_group_t*);
void lock_group_unlisten (lock_group_t*);
void lock_group_wait (lock_group_t *g, lock_t **lock_ptr, void **msg_ptr);

#if not_implemented_yet
void lock_group_take (lock_t*);
bool_t lock_group_try (lock_t*);
void lock_group_release (lock_t*);
#endif

/*
 * ----------
 * | Lock   |
 * | ~~~~   |
 * | master --> T
 * |	    |
 * | waiters--> T -> T...
 * |	    |
 * | slaves --> T -> T -> T...
 * ---------- <-/----/----/
 */
struct _lock_t {
	list_t		entry;		/* double linked list pointers */
	task_t *	master;		/* task, acquired the lock */
#if RECURSIVE_LOCKS
	int_t		deep;		/* LY: recursive locking deep */
#endif
	list_t		waiters;	/* tasks, stopped on `wait' */
	list_t		slaves;		/* tasks, waiting for lock */
	list_t		groups;		/* group slots, waiting for signal */
	lock_irq_t *	irq;		/* irq, associated with the lock */
	int_t		prio;		/* current lock priority */
};

inline extern void lock_init (lock_t *lock) {
	// LY: пока оформлена как заглушка, __lock_init() вызывается автоматически.
}

/*
 * Slot: a group element.
 */
struct _lock_slot_t {
	list_t		entry;		/* double linked list pointers */
	lock_group_t *	group;		/* parent group */
	lock_t *	lock;		/* link to the related lock */
	void *		message;	/* return value for lock_group_wait() */
	bool_t		active;		/* 1 when lock was signal()led  */
};

/*
 * Group: an array of slots.
 */
struct _lock_group_t {
	lock_t		lock;		/* lock to group_wait() on it */
	task_t *	waiter;		/* the waiting task pointer */
	uint_t		size;		/* size of slot[] array */
	uint_t		num;		/* number of elements in slot[] */
	lock_slot_t	slot [1];	/* array of slots is placed here */
};

struct _opacity_t {
	void* opacity[1];
};

#ifndef OPACITY_ALIGN
#	define OPACITY_ALIGN sizeof(void*)
#endif

#define OPACITY(name, bytes) \
	opacity_t name [((bytes) + sizeof (opacity_t) - 1) / sizeof (opacity_t)] \
		__attribute__((aligned(OPACITY_ALIGN)))

extern task_t *task_broken;
extern void *task_broken_stack;
extern void *broken_stack;

#ifdef __cplusplus
}
#endif

#endif /* !__SYS_UOS_H_ */
