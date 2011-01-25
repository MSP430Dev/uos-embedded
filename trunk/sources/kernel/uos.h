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
#   define RECURSIVE_LOCKS	1
#endif

/* System data structures. */
typedef struct _array_t array_t;
typedef struct _task_t task_t;
typedef struct _mutex_t mutex_t;
typedef struct _mutex_irq_t mutex_irq_t;

/* Lock group data structures. */
typedef struct _mutex_group_t mutex_group_t;
typedef struct _mutex_slot_t mutex_slot_t;

/* Fast irq handler. */
typedef bool_t (*handler_t) (void*);

/* Task management. */
task_t *task_create (void (*func)(void*), void *arg, const char *name, int priority,
	array_t *stack, unsigned stacksz);
void task_exit (void *status);
void task_delete (task_t *task, void *status);
void *task_wait (task_t *task);
int task_stack_avail (task_t *task);
const char *task_name (task_t *task);
int task_priority (task_t *task);
void task_set_priority (task_t *task, int priority);
void *task_private (task_t *task);
void task_set_private (task_t *task, void *privatep);
void task_yield (void);

struct _stream_t;
void task_print (struct _stream_t *stream, task_t *t);

#ifndef ARCH_HAVE_FPU
//__attribute__ ((error ("no float point coprocessor: ARCH_HAVE_FPU required")))
#endif
unsigned int task_fpu_control (task_t *t, unsigned int mode, unsigned int mask);

/* Lock management. */
void mutex_lock (mutex_t *lock);
void mutex_unlock (mutex_t *lock);
bool_t mutex_trylock (mutex_t *lock);
void mutex_signal (mutex_t *lock, void *message);
void *mutex_wait (mutex_t *lock);

/* Interrupt management. */
void mutex_lock_irq (mutex_t*, int irq, handler_t func, void *arg);
void mutex_unlock_irq (mutex_t*);
void mutex_attach_irq (mutex_t *m, int irq, handler_t func, void *arg);

/* Group management. */
mutex_group_t *mutex_group_init (array_t *buf, unsigned buf_size);
bool_t mutex_group_add (mutex_group_t*, mutex_t*);
void mutex_group_listen (mutex_group_t*);
void mutex_group_unlisten (mutex_group_t*);
void mutex_group_wait (mutex_group_t *g, mutex_t **lock_ptr, void **msg_ptr);

/* User-supplied startup routine. */
extern void uos_init (void);

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
struct _mutex_t {
	list_t		item;		/* double linked list pointers */
	task_t *	master;		/* task, acquired the lock */
	list_t		waiters;	/* tasks, stopped on `wait' */
	list_t		slaves;		/* tasks, waiting for lock */
	list_t		groups;		/* group slots, waiting for signal */
	mutex_irq_t *	irq;		/* irq, associated with the lock */
	int		prio;		/* current lock priority */
#if RECURSIVE_LOCKS
	small_int_t	deep;		/* recursive locking deep */
#endif
};

/*
 * Slot: a group element.
 */
struct _mutex_slot_t {
	list_t		item;		/* double linked list pointers */
	mutex_group_t *	group;		/* parent group */
	mutex_t *	lock;		/* link to the related lock */
	void *		message;	/* return value for mutex_group_wait() */
	bool_t		active;		/* 1 when lock was signal()led  */
};

/*
 * Group: an array of slots.
 */
struct _mutex_group_t {
	mutex_t		lock;		/* lock to group_wait() on it */
	task_t *	waiter;		/* the waiting task pointer */
	small_uint_t	size;		/* size of slot[] array */
	small_uint_t	num;		/* number of elements in slot[] */
	mutex_slot_t	slot [1];	/* array of slots is placed here */
};

/*
 * Opaque array, well aligned.
 * Used for allocating task stacks and lock groups.
 */
struct _array_t {
	void *data;
};

#define ARRAY(name, bytes) \
	array_t name [((bytes) + sizeof (array_t) - 1) / sizeof (array_t)]

#ifdef __cplusplus
}
#endif

#endif /* !__SYS_UOS_H_ */
