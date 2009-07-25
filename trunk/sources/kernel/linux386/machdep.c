/*
 * Machine-dependent part of uOS for: Linux i386.
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
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "kernel/internal.h"

/* Signal mask containing all signals in range 0..31. */
__sigset_t linux_sigall = { { 0xffffffff } };

/* Empty signal mask. */
__sigset_t linux_sigtmp;

/*
 * Perform the task switch.
 */
void
arch_task_switch (task_t *target)
{
	if (sigsetjmp (task_current->stack_context, 1) == 0) {
		task_current = target;
		siglongjmp (task_current->stack_context, 1);
	}
}

static void
interrupt_handler (int irq)
{
	mutex_irq_t *h;

	/*printf ("IRQ %d\r\n", irq);*/
	h = &mutex_irq [irq];

	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Lock is busy -- remember pending irq.
			 * Call fast handler later, in mutex_unlock(). */
			h->pending = 1;
			return;
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully
			 * serviced an interrupt. In this case
			 * there is no need to wake up the interrupt
			 * servicing task, stopped on mutex_wait.
			 * Task switching is not performed. */
			return;
		}
	}
	mutex_activate (h->lock, 0);
	arch_task_switch (task_policy ());
}

/*
 * Bind the handler to the given hardware interrupt.
 */
void
arch_intr_bind (int irq)
{
	struct sigaction action;

	/*printf ("bind irq %d\r\n", irq);*/
	memset (&action, 0, sizeof (action));
	action.sa_handler = interrupt_handler;
	action.sa_mask = linux_sigall;
	action.sa_flags = SA_RESTART;
	sigaction (irq, &action, 0);
}

/*
 * Unbind the interrupt handler.
 */
void
arch_intr_unbind (int irq)
{
	struct sigaction action;

	/*printf ("unbind irq %d\r\n", irq);*/
	memset (&action, 0, sizeof (action));
	action.sa_handler = SIG_DFL;
	sigaction (irq, &action, 0);
}

/*
 * Last version of LIBC implemented a pointer mangling:
 * encrypting of finction addresses, stored in memory.
 * So we need this stuff to use longjmp() without
 * corresponding setjmp().
 */
static unsigned inline __attribute__ ((always_inline))
mangle_pointer (unsigned val)
{
	int result;

	asm volatile (
	"xor	%%gs:0x18, %0 \n"
"	rol	$9, %0"
	: "=r" (result) : "0" (val));
	return result;
}

/*
 * Build the initial task's stack frame.
 * Arguments:
 * t	- the task object
 * func	- the task function to call
 * arg	- the function argument
 * sp	- pointer to the (end of) stack space
 */
void
arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg, unsigned stacksz)
{
	unsigned *sp = (unsigned*) ((char*) t + stacksz);

	*--sp = (unsigned) arg;				/* task argument pointer */
	*--sp = (unsigned) func;			/* return address */

	memset (&t->stack_context, 0, sizeof (t->stack_context));

	/* Calling environment. */
	t->stack_context->__jmpbuf[4] = mangle_pointer ((unsigned) sp);	/* SP */
	t->stack_context->__jmpbuf[5] = mangle_pointer ((unsigned) func); /* PC */

	/* Saved the signal mask? */
	t->stack_context->__mask_was_saved = 1;
}
