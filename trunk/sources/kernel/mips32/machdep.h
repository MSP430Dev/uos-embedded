/*
 * Machine-dependent uOS declarations for MIPS32.
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
#   error "Don't include directly, use <kernel/internal.h> instead."
#endif

/*
 * The total number of different hardware interrupts.
 */
#define ARCH_INTERRUPTS		32

/*
 * Type for saving task stack context.
 */
typedef void *arch_stack_t;

static inline arch_stack_t
arch_get_stack_pointer ()
{
	return mips32_get_stack_pointer ();
}

static inline void
arch_set_stack_pointer (arch_stack_t sp)
{
	mips32_set_stack_pointer (sp);
}

/*
 * Type for saving task interrupt mask.
 */
typedef int arch_state_t;

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
void arch_task_switch (task_t *target);

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static inline void
arch_intr_disable (arch_state_t *x)
{
	mips32_intr_disable (x);
}

static inline void
arch_intr_restore (arch_state_t x)
{
	mips32_intr_restore (x);
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
	mips32_intr_enable ();
	for (;;) {
		asm volatile (".set mips4 \n	wait");
	}
}

/*
 * Halt the system: unbind all interrupts and exit.
 * (optional feature)
 */
static inline void
arch_halt ()
{
}
