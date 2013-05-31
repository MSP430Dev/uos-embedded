/*
 * Machine-dependent uOS declarations for Cortex-M3, GCC.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *               2012-2013 Dmitry Podkhvatilin <vatilin@gmail.com>
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
#ifndef __KERNEL_INTERNAL_H_
#   error "Don't include directly, use <kernel/internal.h> instead."
#endif

/*
 * The total number of different hardware interrupts.
 */
#if defined (ARM_1986BE1) || defined (ARM_1986BE9)
#   define ARCH_INTERRUPTS		33
#   define ARCH_TIMER_IRQ       32
#elif defined (ARM_STM32F4)
#   define ARCH_INTERRUPTS      83
#   define ARCH_TIMER_IRQ       82
#endif

/*
 * Type for saving task stack context.
 */
typedef void *arch_stack_t;

/*
 * Type for saving task interrupt mask.
 */
typedef unsigned long arch_state_t;

/*
 * Build the initial task's stack frame.
 * Arguments:
 *	t  - the task object, with the stack space appended
 *	f  - the task function to call
 *	a  - the function argument
 *	sz - stack size in bytes
 */
void arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg,
	unsigned stacksz);

/*
 * Perform the task switch.
 */
static inline void
arch_task_switch (task_t *target)
{
#ifdef ARM_CORTEX_M1
	/* Use PendSV for task switching. */
	asm volatile (
	"mov    r0, %0 \n\t"
	: : "r" (target) : "r0", "memory", "cc");
	ARM_SCB->ICSR = ARM_ICSR_PENDSVSET;
#else
	/* Use supervisor call for task switching. */
	asm volatile (
	"mov    r0, %0 \n\t"
	"svc    #0"
	: : "r" (target) : "r0", "memory", "cc");
#endif
}

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static inline void
arch_intr_disable (arch_state_t *x)
{
	arm_intr_disable (x);
}

static inline void
arch_intr_restore (arch_state_t x)
{
	arm_intr_restore (x);
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 *
 * WARNING! MACHDEP_INTR_ALLOW(n) MUST be called when interrupt disabled
 */
void arch_intr_allow (int irq);

/*
 * Bind the handler to the given hardware interrupt.
 * (optional feature)
 */
static inline void
arch_intr_bind (int irq)
{
}

/*
 * Unbind the interrupt handler.
 * (optional feature)
 */
static inline void
arch_intr_unbind (int irq)
{
}

/*
 * Idle system activity.
 */
static inline void
arch_idle ()
{
	arm_intr_enable ();
	for (;;) {
#if defined (ARM_1986BE1) || defined (ARM_1986BE9)
		asm volatile ("nop;");
#else
		arm_bus_yield ();
#endif
	}
}
