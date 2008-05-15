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
#include <signal.h>
#include <setjmp.h>

/*
 * Type for saving task stack context.
 */
#define MACHDEP_SAVED_STATE_T	sigjmp_buf

/*
 * Build the initial task's stack frame.
 * Arguments:
 *	t  - the task object, with the stack space appended
 *	f  - the task function to call
 *	a  - the function argument
 *	sz - stack size in bytes
 */
#define MACHDEP_BUILD_STACK_FRAME(t,f,a,sz) \
	linux_build_stack_frame (t, (void*) f, a, (unsigned long*)t + (sz)/4)
void linux_build_stack_frame (task_t *t, void *func, void *arg,
	unsigned long *sp);

/*
 * Perform the task switch.
 */
#define MACHDEP_TASK_SWITCH()	{ \
	if (sigsetjmp (task_current->stack_context, 1) == 0) { \
		task_policy (); \
		siglongjmp (task_current->stack_context, 1); \
	} }

/*
 * The total number of different hardware interrupts.
 * We are using signals for emulating interrupts.
 */
#define MACHDEP_INTERRUPTS	32

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
#define MACHDEP_INTR_DISABLE(x)	{ \
		sigprocmask (SIG_BLOCK, &linux_sigall, &linux_sigtmp); \
		*(x) = linux_sigtmp.__val [0]; \
	}
#define MACHDEP_INTR_RESTORE(x)	{ \
		linux_sigtmp.__val[0] = (x); \
		sigprocmask (SIG_SETMASK, &linux_sigtmp, 0); \
	}
extern __sigset_t linux_sigall, linux_sigtmp;

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
#define MACHDEP_INTR_ALLOW(n)

/*
 * Bind the handler to the given hardware interrupt.
 * (optional feature)
 */
#define MACHDEP_INTR_BIND(n)	linux_intr_bind (n)
void linux_intr_bind (unsigned char irq);

/*
 * Unbind the interrupt handler.
 * (optional feature)
 */
#define MACHDEP_INTR_UNBIND(n)	linux_intr_unbind (n)
void linux_intr_unbind (unsigned char irq);

/*
 * Idle system activity.
 * Enable interrupts and enter sleep mode.
 * (optional feature)
 */
#define MACHDEP_IDLE()		{				\
				extern int pause (void);	\
				MACHDEP_INTR_RESTORE(0);	\
				for (;;)			\
					pause();		\
				}

/*
 * Halt the system: unbind all interrupts and exit.
 * (optional feature)
 */
#define MACHDEP_HALT()		exit(0)
