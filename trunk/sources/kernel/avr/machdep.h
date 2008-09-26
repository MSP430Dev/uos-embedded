/*
 * Machine-dependent uOS declarations for: Atmel AVR, GCC.
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
#ifndef __UOS_ARCH_H_
#	error "Don't include directly, use <kernel/arch.h> instead."
#endif

/*
 * The total number of different hardware interrupts.
 */
#define ARCH_INTERRUPTS		34

/*
 * Type for saving task stack context.
 */
typedef void *arch_stack_t;

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
	*x = inb (SREG);
	cli ();
}

static inline void
arch_intr_restore (arch_state_t x)
{
	outb (x, SREG);
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
 * Enable interrupts and enter sleep mode.
 * (optional feature)
 */
#ifdef  SMCR
#	define SLEEP_REG	SMCR
#else
#	define SLEEP_REG	MCUCR
#endif

static inline void
arch_idle ()
{
	setb (SE, SLEEP_REG);
	asm volatile ("sei");
	for (;;)
		asm volatile ("sleep");
}

/*
 * Halt the system: unbind all interrupts and exit.
 * (optional feature)
 */
void arch_halt (int dump_flag);

#if 0

#if FLASHEND > 0x1FFFF
#	define ASM_GOTO "jmp "
#elif FLASHEND > 0x2000
#	define ASM_GOTO "jmp "
#else
#	define ASM_GOTO "rjmp "
#endif

/* LY: temporary, until full commit ----------------------------------------- */

extern inline void* __arch_read_return (void *sp)
{
	unsigned ret = *(unsigned*) sp;
	return (void*) ((ret << 8) | (ret >> 8));
}

/*
 * The total number of different hardware interrupts.
 */
#if defined (__AVR_ATmega103__)
#	define __arch_max_irq	23
#elif defined (__AVR_ATmega128__)
#	define __arch_max_irq	34
#elif defined (__IOM161)
#	define __arch_max_irq	20
#elif defined (__AVR_ATmega2561__)
#	define __arch_max_irq	50
#else
#	error
#endif

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */

#define __arch_cli()			\
	__asm __volatile ("cli"		\
		::: "memory")

#define __arch_sti()			\
	__asm __volatile ("sei"		\
		::: "memory")

#define __arch_flags_save(flags)	\
		(*(flags) = inb (SREG))	\

#define __arch_flags_restore(flags)	\
		outb (flags, SREG)	\

#define __arch_intr_is_enabled(flags)	\
	(((flags) & (1 << SREG_I)) != 0)

#define __arch_idle() do {				\
		setb (SE, SLEEP_REG);			\
		__arch_sti ();				\
		for (;;)				\
			__asm __volatile ("sleep");	\
	} while (0)
#endif
