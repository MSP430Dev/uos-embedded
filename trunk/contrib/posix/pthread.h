#ifndef __PTHREAD_H__
#define __PTHREAD_H__

/*
 * Implement Posix pthread functions using uOS primitives.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

#define pthread_t				task_t*
#define pthread_self()				task_current

#define pthread_mutex_t				mutex_t

#define pthread_mutex_lock(lock)		({ mutex_lock (lock); 0; })
#define pthread_mutex_trylock(lock)		( mutex_trylock (lock) > 0 ? 0 : -1 )
#define pthread_mutex_unlock(lock)		({ mutex_unlock (lock); 0; })
static inline int pthread_mutex_destroy (pthread_mutex_t *mutex)
	{ return 0; }

#define pthread_mutexattr_t			int

#define pthread_mutexattr_init(a)		( *a = 0 )
#define pthread_mutexattr_destroy(a)		/* empty */
#define pthread_mutexattr_settype(a,t)		/* empty */
#define pthread_mutex_init(m,a)			0

/* TODO - no recursive mutexs in uOS. */
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP	{ 0 }

#define pthread_key_t				int
#define pthread_key_create(kp,x)		{ *kp = 0; }
#define pthread_getspecific(key)		task_current->privatep
#define pthread_setspecific(key,val)		task_set_private (task_current, val)

#define PTHREAD_STACK_SIZE			0x4000	/* 16 kilobytes */
#define PTHREAD_PRIO				0x4000	/* 16 kilobytes */

#define pthread_create(taskp,attr,func,arg)	{ \
	int stacksz = PTHREAD_STACK_SIZE;	  \
	char *stack = calloc (1, stacksz);	  \
	if (stack) *taskp = task_create ((void(*)(void*)) func, \
		arg, "task", PTHREAD_PRIO, stack, stacksz); }

#define pthread_cancel(t)			{ \
	if (t->base_prio) task_delete (t, 0); 	  \
	t->base_prio = 0;			}

#define pthread_join(t,statusp)			{ \
	if (t->base_prio) task_wait (t);	  \
	t->base_prio = 0;			}

#define pthread_kill(t,msg)			{ \
	if (t->base_prio) task_delete (t, 0); 	  \
	t->base_prio = 0;			  \
	free (t);				}

#define pthread_testcancel(t)			/* empty */
#define pthread_equal(t1, t2)			((t1) == (t2))

#define sched_yield()				task_yield()
#define setpriority(a,b,c)			/* empty */

#define pthread_cond_t				mutex_t*

#define pthread_cond_init(cond,x)		{ *cond = 0; }

#define pthread_cond_wait(cond,lock)		{ \
	*cond = lock;				  \
	mutex_wait (lock);			}

#define pthread_cond_signal(cond)		mutex_signal (*cond, 0)

#define pthread_cond_broadcast(cond)		mutex_signal (*cond, 0)

#define pthread_cond_destroy(cond)		{ *cond = 0; }

/* TODO - return ETIMEDOUT on timeout */
#define pthread_cond_timedwait(cond,lock,timo)	({ \
	*cond = lock;				  \
	mutex_wait (lock);			  \
	0;					})

#endif
