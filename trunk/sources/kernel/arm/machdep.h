/*
 * Machine-dependent uOS declarations for: ARM7TDMI (Samsung S3C4530A), GCC.
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
#define MACHDEP_BUILD_STACK_FRAME(t,f,a,sz) arm_build_stack_frame (t, \
	(unsigned long) f, (unsigned long) a, \
	(unsigned long*) ((unsigned long)t + sz))
void arm_build_stack_frame (task_t *t, unsigned long func,
	unsigned long arg, unsigned long *stack);

/*
 * Perform the task switch.
 */
#define MACHDEP_TASK_SWITCH(target) arm_task_switch (target)
void arm_task_switch (task_t *target);

/*
 * The total number of different hardware interrupts.
 */
#define MACHDEP_INTERRUPTS	21

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
#define MACHDEP_INTR_DISABLE(x)	arm_intr_disable (x)
#define MACHDEP_INTR_RESTORE(x)	arm_intr_restore (x)

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 *
 * WARNING! MACHDEP_INTR_ALLOW(n) MUST be called when interrupt disabled
 */
#define MACHDEP_INTR_ALLOW(n)	arm_intr_allow (n)
void arm_intr_allow (uint_t irq);

/*
 * Idle system activity.
 */
#define MACHDEP_IDLE()					\
	do {						\
		arm_intr_enable ();			\
		for (;;) 				\
			arm_bus_yield ();		\
	} while (0)

/*
 * Uncomment these lines for remote debugging.
 */
#ifndef NDEBUG
#	define MACHDEP_HALT()		breakpoint()
#endif
