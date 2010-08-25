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
 * Perform the task switch.
 */
void arch_task_switch (task_t *target)
{
	/* Save all registers in stack. */
	asm volatile (
	"mrs	r12, apsr \n"
"	push	{r12} \n"		/* psr (from r12) */
"	push	{lr} \n"		/* pc */
"	push	{r0-r3,r12,lr} \n"	/* save r0-r3,r12,lr */
"	push	{r4-r11}"		/* save r4-r11 */
	);

	/* Save current task stack. */
	task_current->stack_context = arm_get_stack_pointer ();

	task_current = target;

	/* Switch to the new task. */
	arm_set_stack_pointer (task_current->stack_context);

	if (arm_get_ipsr() != 0) {
		/* Return from exception. */
		asm volatile (
	"	pop	{r4-r11} \n"		/* load r4-r11 */
	"	mov	lr, #0xFFFFFFF9 \n"	/* thread mode, main stack */
	"	bx	lr"			/* return from exception */
		);
	} else {
		/* Enter task. */
		asm volatile (
	"	pop	{r4-r11} \n"		/* load r4-r11 */
	"	pop	{r0-r3,r12,lr} \n"	/* load r0-r3,r12,lr */
	"	pop	{r0-r1} \n"		/* load pc, psr */
	"	bx	r0"			/* return from function */
		);
	}
}

/*
 * The common part of the interrupt handler,
 * to minimize the code size.
 * Attribute "naked" skips function prologue.
 */
void __attribute__ ((naked))
_irq_handler_ (void)
{
	mutex_irq_t *h;
	int irq;

	for (;;) {
		/* Get the current irq number */
//		irq = ???;
		if (irq >= ARCH_INTERRUPTS)
			break;

		/* Disable the irq, to avoid loops */
//		ARM_INTPND = 1 << irq;		/* clear pending irq */

/*debug_printf ("<%d> ", irq);*/
		h = &mutex_irq [irq];
		if (! h->lock)
			continue;

		if (h->handler) {
			/* If the lock is free -- call fast handler. */
			if (h->lock->master) {
				/* Lock is busy -- remember pending irq.
				 * Call fast handler later, in mutex_unlock(). */
				h->pending = 1;
				continue;
			}
			if ((h->handler) (h->arg) != 0) {
				/* The fast handler returns 1 when it fully
				 * serviced an interrupt. In this case
				 * there is no need to wake up the interrupt
				 * servicing task, stopped on mutex_wait.
				 * Task switching is not performed. */
#ifdef ARM_1986BE9
//				if (irq == 4 || irq == 6) {
//					/* Enable UART transmit irq. */
//					ARM_INTMSK &= ~(1 << irq);
//				}
#endif
				continue;
			}
		}

		/* Signal the interrupt handler, if any. */
		mutex_activate (h->lock, 0);
	}

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

	/* Restore registers. */
	asm volatile (
	"pop	{r4-r11} \n"		/* load r4-r11 */
"	mov	lr, #0xFFFFFFF9 \n"	/* thread mode, main stack */
"	bx	lr");			/* return from exception */
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void arch_intr_allow (int irq)
{
#ifdef ARM_1986BE9
//	*AT91C_AIC_IECR = 1 << irq;
/*debug_printf ("<IECR:=%x> ", 1 << irq);*/
#endif
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

	*--sp = 0;			/* psr */
	*--sp = (unsigned) func;	/* pc - callee address */
	*--sp = 0;			/* lr */
	*--sp = 0;			/* r12 */
	*--sp = 0;			/* r3 */
	*--sp = 0;			/* r2 */
	*--sp = 0;			/* r1 */
	*--sp = (unsigned) arg;		/* r0 - task argument */
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

/*
  Integrity checks are provided to check the following conditions on an exception return:

  * The Exception Number being returned from (as held in the IPSR at
    the start of the return) must be listed in the SCB as being active.

  * If no exceptions other than the returning exception are active,
    the mode being returned to must be Thread mode. This checks for
    a mismatch of the number of exception returns.

  * If at least one exception other than the returning exception is active,
    under normal circumstances the mode being returned to must be Handler mode.
    This checks for a mismatch of the number of exception returns.
    This check can be disabled using the NONBASETHRDENA control bit in the SCB.

  * On return to Thread mode, the Exception Number restored into the IPSR must be 0.

  * On return to Handler mode, the Exception Number restored into the IPSR must not be 0.

  * The EXC_RETURN[3:0] must not be listed as reserved in Table B1-8 on page B1-26.

  An exception return error causes an INVPC UsageFault, with the illegal
  EXC_RETURN value in the link register (LR).
 */
