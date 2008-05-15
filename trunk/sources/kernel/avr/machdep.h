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
 * Type for saving task stack context.
 */
#define MACHDEP_SAVED_STATE_T	unsigned short

/*
 * Build the initial task's stack frame.
 * Arguments:
 *	t  - the task object, with the stack space appended
 *	f  - the task function to call
 *	a  - the function argument
 *	sz - stack size in bytes
 */
#define MACHDEP_BUILD_STACK_FRAME(t,f,a,sz) avr_build_stack_frame (t, \
	(unsigned short) f, (unsigned short) a, (char*)t + sz-1)
struct _task_t;
void avr_build_stack_frame (struct _task_t *t, unsigned short func,
	unsigned short arg, char *stack);

/*
 * Perform the task switch.
 */
#define MACHDEP_TASK_SWITCH(t)	avr_task_switch(t)
void avr_task_switch (struct _task_t *target);

/*
 * The total number of different hardware interrupts.
 */
#define MACHDEP_INTERRUPTS	34

/*
 * The global interrupt control.
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
#define MACHDEP_INTR_DISABLE(x)	{ *(uint_t*)(x) = inb (SREG); cli (); }
#define MACHDEP_INTR_RESTORE(x)	{ outb ((uint_t) (x), SREG); }
#define MACHDEP_INTR_IS_ENABLED() ((inb (SREG) & (1 << SREG_I)) != 0)

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
#define MACHDEP_INTR_ALLOW(n)	avr_intr_allow (n)
void avr_intr_allow (unsigned char irq);

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

#define MACHDEP_IDLE()		{				\
				setb (SE, SLEEP_REG);		\
				asm volatile ("sei");		\
				for (;;)			\
					asm volatile ("sleep");	\
				}

/*
 * Uncomment these lines for remote debugging.
 */
#ifndef NDEBUG
#define MACHDEP_HALT()		breakpoint()
void breakpoint (void);
#endif

#if FLASHEND > 0x1FFFF
#	define ASM_GOTO "jmp "
#elif FLASHEND > 0x2000
#	define ASM_GOTO "jmp "
#else
#	define ASM_GOTO "rjmp "
#endif

/* LY: temporary, until full commit ----------------------------------------- */

/*
 * Type for saving task stack context.
 */
typedef unsigned short __arch_sp_t;

/*
 * Type for saving cpu and/or irq flags.
 */
typedef unsigned char arch_flags_t;

#define __arch_get_sp()		inw (SP)
#define __arch_set_sp(addr)	outw (addr, SP)

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

/*
 * Uncomment these lines for remote debugging.
 */
#ifndef NDEBUG
#	define __arch_halt() breakpoint()
	void breakpoint (void);
#endif
