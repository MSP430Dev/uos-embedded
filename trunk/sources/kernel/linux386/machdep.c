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

static void
interrupt_handler (int irq)
{
	lock_irq_t *h;

	/*printf ("IRQ %d\r\n", irq);*/
	h = &lock_irq [irq];

	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Lock is busy -- remember pending irq.
			 * Call fast handler later, in lock_release(). */
			h->pending = 1;
			return;
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully
			 * serviced an interrupt. In this case
			 * there is no need to wake up the interrupt
			 * servicing task, stopped on lock_wait.
			 * Task switching is not performed. */
			return;
		}
	}
	lock_activate (h->lock, 0);
	MACHDEP_TASK_SWITCH();
}

/*
 * Bind the handler to the given hardware interrupt.
 */
void
linux_intr_bind (unsigned char irq)
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
linux_intr_unbind (unsigned char irq)
{
	struct sigaction action;

	/*printf ("unbind irq %d\r\n", irq);*/
	memset (&action, 0, sizeof (action));
	action.sa_handler = SIG_DFL;
	sigaction (irq, &action, 0);
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
linux_build_stack_frame (task_t *t, void *func, void *arg, unsigned long *sp)
{
	*--sp = (unsigned long) arg;		/* task argument pointer */
	*--sp = (unsigned long) func;		/* return address */

	memset (&t->stack_context, 0, sizeof (t->stack_context));

	/* Calling environment. */
	t->stack_context->__jmpbuf[4] = (unsigned long) sp;	/* SP */
	t->stack_context->__jmpbuf[5] = (unsigned long) func;	/* PC */

	/* Saved the signal mask? */
	t->stack_context->__mask_was_saved = 1;
}
