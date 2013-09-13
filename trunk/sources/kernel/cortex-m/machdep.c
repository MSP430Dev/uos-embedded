/*
 * Machine-dependent part of uOS for: ARM Cortex-M3, GCC.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *               2012-2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
 *               2013 Lyubimov Maxim <rosseltzong@yandex.ru>
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
 * Task switch.
 */
void __attribute__ ((naked))
_svc_ (task_t *target)
{
#ifdef ARM_CORTEX_M1
	/* Save registers R4-R11 and ISER0 in stack. */
	asm volatile (
	"push   {r4-r7} \n\t"
	"ldr    r6, =0xE000E100 \n\t"
	"ldr    r5, [r6, #0] \n\t"
	"mov    r1, r8 \n\t"
	"mov    r2, r9 \n\t"
	"mov    r3, r10 \n\t"
	"mov    r4, r11 \n\t"
	"push   {r1-r5} \n\t");
#else
	/* Save registers R4-R11 and BASEPRI in stack. */
	asm volatile (
	"mrs    r12, basepri \n\t"
	"push   {r4-r12}"
	);
#endif

	/* Save current task stack. */
	task_current->stack_context = arm_get_stack_pointer ();
	
	task_current = target;
	
	/* Switch to the new task. */
	arm_set_stack_pointer (task_current->stack_context);
	
#ifdef ARM_CORTEX_M1
	/* Load registers R4-R11 and ISER0.
	 * Return from exception. */
	asm volatile (
	"mov    r0, #6 \n\t"
	"mvn    r1, r0 \n\t"
	"mov    lr, r1 \n\t"
	"pop    {r1-r5} \n\t"
	"mov    r8, r1 \n\t"
	"mov    r9, r2 \n\t"
	"mov    r10, r3 \n\t"
	"mov    r11, r4 \n\t"
	"ldr    r6, =0xE000E180 \n\t"
	"ldr    r7, =0xFFFFFFFF \n\t"
	"str    r7, [r6, #0] \n\t"
	"ldr    r6, =0xE000E100 \n\t"
	"str    r5, [r6, #0] \n\t"
	"pop    {r4-r7} \n\t"
	"bx     lr \n\t"
	);
#else
	/* Load registers R4-R11 and BASEPRI.
	 * Return from exception. */
	asm volatile (
	"pop    {r4-r12} \n\t"
	"msr    basepri, r12 \n\t"
	"bx	lr"
	);
#endif
}

/*
 * The common part of the interrupt handler,
 * to minimize the code size.
 * Attribute "naked" skips function prologue.
 */
void __attribute__ ((naked))
_irq_handler_ (void)
{
#ifdef ARM_CORTEX_M1
	/* Save registers R4-R11 and ISER0 in stack.
	 * Save return address. */
	asm volatile (
	"push	{r4-r7} \n\t"
	"ldr	r6, =0xE000E100 \n\t"
	"ldr	r5, [r6, #0] \n\t"
	"mov    r1, r8 \n\t"
	"mov    r2, r9 \n\t"
	"mov    r3, r10 \n\t"
	"mov    r4, r11 \n\t"
	"push	{r1-r5} \n\t");
	//"push	{r1-r4} \n\t");

#else
	/* Save registers R4-R11 and BASEPRI in stack.
	 * Save return address. */
	asm volatile (
	"mrs	r12, basepri \n\t"
	"push	{r4-r12}"
	);
#endif

	/* Get the current irq number */
	int irq;
	unsigned ipsr = arm_get_ipsr ();
	if (ipsr == 15) {
		/* Systick interrupt. */
		irq = ARCH_TIMER_IRQ;
		ARM_SYSTICK->CTRL &= ~ARM_SYSTICK_CTRL_TICKINT;
    } else {
        irq = ipsr - 16;
        ARM_NVIC_ICER(irq >> 5) = 1 << (irq & 0x1F);
	}

//debug_printf ("<%d> ", irq);
	mutex_irq_t *h = &mutex_irq [irq];
	if (! h->lock) {
		/* Cannot happen. */
//debug_printf ("<unexpected interrupt> ");
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
#ifdef ARM_CORTEX_M1
	/* Load registers R4-R11. ISER0 saved value is thrown away.
	 * Return from exception. */
	asm volatile (
	"mov    r0, #6 \n\t"
	"mvn    r1, r0 \n\t"
	"mov    lr, r1 \n\t"
	"pop    {r1-r5} \n\t"
	"mov    r8, r1 \n\t"
	"mov    r9, r2 \n\t"
	"mov    r10, r3 \n\t"
	"mov    r11, r4 \n\t"
	"pop    {r4-r7} \n\t"
	"bx     lr \n\t"
	);
	
#else
	/* Load registers R4-R11 and BASEPRI.
	 * Return from exception. */
	asm volatile (
	"mvn    lr, #6 \n\t"		/* EXC_RETURN value */
	"pop    {r4-r12} \n\t"
	"msr    basepri, r12 \n\t"
	"bx     lr"
	);
#endif
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void arch_intr_allow (int irq)
{
	if (irq == ARCH_TIMER_IRQ) {
		/* Systick interrupt. */
		ARM_SYSTICK->CTRL |= ARM_SYSTICK_CTRL_TICKINT;
	} else {
		ARM_NVIC_ISER(irq >> 5) = 1 << (irq & 0x1F);
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
	*--sp = 0;			/* basepri (cortex-m1: r7) */
	*--sp = 0;			/* r11     (cortex-m1: r6) */
	*--sp = 0;			/* r10     (cortex-m1: r5) */
	*--sp = 0;			/* r9      (cortex-m1: r4) */
	*--sp = 0;			/* r8      (cortex-m1: ISER0) */
	*--sp = 0;			/* r7      (cortex-m1: r11) */
	*--sp = 0;			/* r6      (cortex-m1: r10) */
	*--sp = 0;			/* r5      (cortex-m1: r9) */
	*--sp = 0;			/* r4      (cortex-m1: r8) */

	t->stack_context = (void*) sp;
	
#ifdef ARM_CORTEX_M1
/* This is a hack due to ARM Cortex-M1 architecture.
 * It is impossible to use SVC to switch tasks, because
 * it induces Hard Fault when interrupts are masked with
 * PRIMASK register. PendSV exception could be used instead.
 * But it's masked by PRIMASK - no Hard Fault, but it is
 * still impossible to switch tasks with PendSV.
 * 
 * The solution is to mask interrupts with NVIC ICER0 register.
 * But then we have to manage masks between task carefully
 * (ISER0 content) in order not to lose current mask. It must 
 * be saved in task context.
 * Initial mask for each task (right after initialization)
 * shall be the same and corresponds to enabled interrupts.
 * We have to save task stack size somewhere to know where
 * to put this mask. In order not to increase size of task_t 
 * structure we temporarely use another field "ticks" for
 * that purpose (bad practice - I know!) and after initialization
 * we zero again this field.
 */
	t->ticks = stacksz;
#endif
}

/*
 * Additional machine-dependent initialization after
 * user initialization. Needed only for ARM Cortex-M1.
 * Save interrupt mask created by user initialization (uos_init)
 * into initial context of each task.
 */
#ifdef ARM_CORTEX_M1
void
uos_post_init ()
{
	task_t *t;
	unsigned *stack_iser0;
	list_iterate (t, &task_active) {
		unsigned stack_size = t->ticks;
		if (stack_size != 0) {
			stack_iser0 = (unsigned*) ((char*) t + stack_size - 13*4);
			*stack_iser0 = ARM_NVIC_ISER(0);
			t->ticks = 0;
		}
	}
}
#endif
