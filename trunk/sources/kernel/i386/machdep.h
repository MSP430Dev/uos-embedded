/*
 * Machine-dependent uOS declarations for: i386, GCC.
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

/*
 * Type for saving task stack context.
 */
#define MACHDEP_SAVED_STATE_T	unsigned long

/*
 * Build the initial task's stack frame.
 * Arguments:
 *	t  - the task object, with the stack space appended
 *	f  - the task function to call
 *	a  - the function argument
 *	sz - stack size in bytes
 */
#define MACHDEP_BUILD_STACK_FRAME(t,f,a,sz) i386_build_stack_frame (t, \
	(unsigned long) f, (unsigned long) a, \
	(unsigned long*) ((unsigned long)t + sz))
void i386_build_stack_frame (task_t *t, unsigned long func,
	unsigned long arg, unsigned long *stack);

/*
 * Perform the task switch.
 */
#define MACHDEP_TASK_SWITCH()	{ asm ("pushf\npush %cs"); i386_task_switch(); }
void i386_task_switch (void);

/*
 * The total number of different hardware interrupts.
 */
#define MACHDEP_INTERRUPTS	16

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
#define MACHDEP_INTR_DISABLE(x)	I386_INTR_DISABLE (x)
#define MACHDEP_INTR_RESTORE(x)	I386_INTR_RESTORE (x)

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
#define MACHDEP_INTR_ALLOW(n)	i386_intr_allow (n)
void i386_intr_allow (int irq);

/*
 * Idle system activity.
 * Enable interrupts and enter sleep mode.
 * (optional feature)
 */
#define MACHDEP_IDLE()		{			\
				I386_INTR_ENABLE ();	\
				for (;;)		\
					asm ("hlt");	\
				}

/*
 * Uncomment these lines for remote debugging.
 */
#ifndef NDEBUG
#define MACHDEP_HALT()		breakpoint()
#endif
