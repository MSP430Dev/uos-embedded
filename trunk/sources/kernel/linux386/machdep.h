/*
 * Machine-dependent uOS declarations for: Linux i386.
 * Using signals and pthreads to emulate interrupts.
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
#include <signal.h>
#include <setjmp.h>

/*
 * The total number of different hardware interrupts.
 * We are using signals for emulating interrupts.
 */
#define ARCH_INTERRUPTS		32

/*
 * Type for saving task stack context.
 */
typedef sigjmp_buf arch_stack_t;

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
	extern __sigset_t linux_sigall, linux_sigtmp;

	sigprocmask (SIG_BLOCK, &linux_sigall, &linux_sigtmp);
	*x = linux_sigtmp.__val [0];
}

static inline void
arch_intr_restore (arch_state_t x)
{
	extern __sigset_t linux_sigtmp;

	linux_sigtmp.__val[0] = x;
	sigprocmask (SIG_SETMASK, &linux_sigtmp, 0);
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 *
 * WARNING! MACHDEP_INTR_ALLOW(n) MUST be called when interrupt disabled
 */
static inline void
arch_intr_allow (int irq)
{
}

/*
 * Bind the handler to the given hardware interrupt.
 * (optional feature)
 */
void arch_intr_bind (int irq);

/*
 * Unbind the interrupt handler.
 * (optional feature)
 */
void arch_intr_unbind (int irq);

/*
 * Idle system activity.
 * Enable interrupts and enter sleep mode.
 * (optional feature)
 */
static inline void
arch_idle ()
{
	extern int pause (void);

	arch_intr_restore (0);
	for (;;) {
		pause ();
	}
}
