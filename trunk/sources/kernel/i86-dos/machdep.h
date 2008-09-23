/*
 * Machine-dependent uOS declarations for: Intel i86, DOS, Bruce CC.
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
 * We need 4 bytes for SS:SP.
 */
typedef short arch_state_t [2];

/*
 * Build the initial task's stack frame.
 * Arguments:
 *	t  - the task object, with the stack space appended
 *	f  - the task function to call
 *	a  - the function argument
 *	sz - stack size in bytes
 */
#define MACHDEP_BUILD_STACK_FRAME(t,f,a,sz) \
	dos_build_stack_frame (t, (void*) f, a, (unsigned short*)t + (sz)/2)
void dos_build_stack_frame (task_t *t, void *func, void *arg,
	unsigned short *sp);

/*
 * Perform the task switch.
 */
#define MACHDEP_TASK_SWITCH()	{ asm ("pushf\npush CS"); dos_task_switch(); }
void dos_task_switch (void);

/*
 * The total number of different hardware interrupts.
 */
#define MACHDEP_INTERRUPTS	16

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
#define MACHDEP_INTR_DISABLE(x)	{ asm ("pushf\npop AX\ncli"); *(x) = _AX; }
#define MACHDEP_INTR_RESTORE(x)	{ _AX = (x); asm ("push AX\npopf"); }

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
#define MACHDEP_INTR_ALLOW(n)	dos_intr_allow (n)
void dos_intr_allow (int irq);

/*
 * Bind the handler to the given hardware interrupt.
 * (optional feature)
 */
#define MACHDEP_INTR_BIND(n)	dos_intr_bind (n)
void dos_intr_bind (unsigned char irq);

/*
 * Unbind the interrupt handler.
 * (optional feature)
 */
#define MACHDEP_INTR_UNBIND(n)	dos_intr_unbind (n)
void dos_intr_unbind (unsigned char irq);

/*
 * Idle system activity.
 * Enable interrupts and enter sleep mode.
 * (optional feature)
 */
#define MACHDEP_IDLE()		{			\
				asm ("sti");		\
				for (;;)		\
					asm ("hlt");	\
				}

/*
 * Halt the system: unbind all interrupts and exit to DOS.
 * (optional feature)
 */
#define MACHDEP_HALT()		dos_halt()
