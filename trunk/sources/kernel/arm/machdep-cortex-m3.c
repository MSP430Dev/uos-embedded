/*
 * Machine-dependent part of uOS for: ARM Cortex-M3, GCC.
 *
 * Copyright (C) 2010 Serge Vakulenko, <vak@cronyx.ru>
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

/*
 * Supervisor call exception handler: do the task switch.
 */
void __attribute__ ((naked))
_svc_ (task_t *target)
{
	/* Save registers R4-R11 and BASEPRI in stack. */
	asm volatile (
	"mrs	r12, basepri \n\t"
	"push	{r4-r12}"
	);

	/* Save current task stack. */
	task_current->stack_context = arm_get_stack_pointer ();

	task_current = target;

	/* Switch to the new task. */
	arm_set_stack_pointer (task_current->stack_context);

	/* Load registers R4-R11 and BASEPRI. */
	asm volatile (
	"pop	{r4-r12} \n\t"
	"msr	basepri, r12"
	);

	/* Return from exception. */
	asm volatile ("bx	lr");
}

/*
 * The common part of the interrupt handler,
 * to minimize the code size.
 * Attribute "naked" skips function prologue.
 */
void __attribute__ ((naked))
_irq_handler_ (void)
{
	/* Save registers R4-R11 and BASEPRI in stack. */
	asm volatile (
	"mrs	r12, basepri \n\t"
	"push	{r4-r12}"
	);

	/* Save return address. */
	unsigned lr = arm_get_register (14);

	/* Get the current irq number */
	int irq;
	unsigned ipsr = arm_get_ipsr ();
	if (ipsr == 15) {
		/* Systick interrupt. */
		irq = 32;

	} else if (ipsr >= 16 && ipsr < 48) {
		irq = ipsr - 16;
		ARM_NVIC_ICER0 = 1 << irq;	/* clear pending irq */
	} else {
		/* Cannot happen. */
debug_printf ("<interrupt with ipsr==0> ");
		goto done;
	}

//debug_printf ("<%d> ", irq);
	mutex_irq_t *h = &mutex_irq [irq];
	if (! h->lock) {
		/* Cannot happen. */
debug_printf ("<unexpected interrupt> ");
		goto done;
	}

	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Lock is busy -- remember pending irq.
			 * Call fast handler later, in mutex_unlock(). */
			h->pending = 1;
			goto done;
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully
			 * serviced an interrupt. In this case
			 * there is no need to wake up the interrupt
			 * servicing task, stopped on mutex_wait.
			 * Task switching is not performed. */
#ifdef ARM_1986BE9
//				if (irq == 6 || irq == 7) {
//					/* Enable UART transmit irq. */
//					ARM_NVIC_ISER0 = 1 << irq;
//				}
#endif
			goto done;
		}
	}

	/* Signal the interrupt handler, if any. */
	mutex_activate (h->lock, 0);

	/* LY: copy few lines of code from task_schedule() here. */
	if (task_need_schedule)	{
		task_t *new;

		task_need_schedule = 0;
		new = task_policy ();
		if (new != task_current) {
			task_current->stack_context = arm_get_stack_pointer ();
			task_current = new;
			new->ticks++;
			arm_set_stack_pointer (task_current->stack_context);
		}
	}
done:
	/* Restore return address. */
	arm_set_register (14, lr);

	/* Load registers R4-R11 and BASEPRI. */
	asm volatile (
	"pop	{r4-r12} \n\t"
	"msr	basepri, r12"
	);

	/* Return from exception. */
	asm volatile ("bx	lr");
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void arch_intr_allow (int irq)
{
	if (irq == 32) {
		/* Systick interrupt: nothing to do. */
	} else {
		ARM_NVIC_ISER0 = 1 << irq;
debug_printf ("<ISER0:=%x> ", 1 << irq);
	}
}

/*
 * Build the initial task's stack frame.
 * Arguments:
 * t	- the task object
 * func	- the task function to call
 * arg	- the function argument
 * sp	- the pointer to (end of) stack space
 */
void
arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg,
	unsigned stacksz)
{
	unsigned *sp = (unsigned*) ((char*) t + stacksz);

	*--sp = 0x01000000;		/* xpsr - must set Thumb bit */
	*--sp = (unsigned) func;	/* pc - callee address */
	*--sp = 0;			/* lr */
	*--sp = 0;			/* r12 */
	*--sp = 0;			/* r3 */
	*--sp = 0;			/* r2 */
	*--sp = 0;			/* r1 */
	*--sp = (unsigned) arg;		/* r0 - task argument */
	*--sp = 0;			/* basepri */
	*--sp = 0;			/* r11 */
	*--sp = 0;			/* r10 */
	*--sp = 0;			/* r9 */
	*--sp = 0;			/* r8 */
	*--sp = 0;			/* r7 */
	*--sp = 0;			/* r6 */
	*--sp = 0;			/* r5 */
	*--sp = 0;			/* r4 */

	t->stack_context = (void*) sp;
}
