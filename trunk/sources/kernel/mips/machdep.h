/*
 * Machine-dependent uOS declarations for MIPS32.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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
#ifdef PIC32MX
/*
 * Total number of interrupt vectors.
 */
#define ARCH_INTERRUPTS		46
#endif

#ifdef ELVEES_MC24
/*
 * 32 interrupts linked to MASKR[31:0] register.
 * 8 interrupts linked to IP[7:0] bits of CP0_CAUSE register.
 */
#define ARCH_INTERRUPTS		(32+8)
#endif

#ifdef ELVEES_MC24R2
/*
 * 23 interrupts linked to MASKR0[22:0] register.
 * 4 interrupts linked to MASKR1[3:0] register.
 * 32 interrupts linked to MASKR2[31:0] register.
 * 5 interrupts linked to MASKR3[4:0] register.
 * COMPARE interrupt linked to irq 19.
 */
#define ARCH_INTERRUPTS		(23+4+32+5)
#endif

#ifdef ELVEES_NVCOM01
/*
 * 32 interrupts linked to MASKR0[31:0] register.
 * 4 interrupts linked to MASKR1[3:0] register.
 * 16 interrupts linked to MASKR2[15:0] register.
 */
#define ARCH_INTERRUPTS		(32+4+16)
#endif

#ifdef ELVEES_NVCOM02
/*
 * TODO
 */
#define ARCH_INTERRUPTS		(32+4+16)
#endif

#ifdef ELVEES_MCT02
/*
 * 23 interrupts linked to MASKR0[22:0] register.
 * 8 interrupts linked to MASKR1[7:0] register.
 * 32 interrupts linked to MASKR2[31:0] register.
 * 5 interrupts linked to MASKR3[4:0] register.
 * 30 interrupts linked to MASKR4[29:0] register.
 * COMPARE interrupt linked to irq 19.
 */
#define ARCH_INTERRUPTS		(23+8+32+5+30)
#endif

/*
 * Type for saving task stack context.
 */
typedef void *arch_stack_t;

static inline arch_stack_t
arch_get_stack_pointer ()
{
	return mips_get_stack_pointer ();
}

static inline void
arch_set_stack_pointer (arch_stack_t sp)
{
	mips_set_stack_pointer (sp);
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
	mips_intr_disable (x);
}

static inline void
arch_intr_restore (arch_state_t x)
{
	mips_intr_restore (x);
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
	mips_intr_enable ();
	for (;;) {
		asm volatile (
			".set mips4 \n"
		"	wait \n"
		"	.set mips1");
	}
}

/*
 * State of float-point coprocessor.
 */
typedef unsigned arch_fpu_t;

#define ARCH_FPU_STATE	(~0)		/* FPU disabled by default */

/*
 * Change/query a mode of float-point coprocessor.
 */
unsigned int arch_fpu_control (task_t *t, unsigned int mode, unsigned int mask);
