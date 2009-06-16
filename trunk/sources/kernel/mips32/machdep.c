/*
 * Machine-dependent part of uOS for MIPS32: Elvees Multicore.
 *
 * Copyright (C) 2008 Serge Vakulenko, <serge@vak.ru>
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
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "kernel/internal.h"

/*
 * Perform the task switch.
 * The call is performed via the assembler label,
 * to skip the function prologue, generated by the compiler.
 */
void
_arch_task_switch_ ()
{
	task_t *target;

	/* Save all registers in stack. */
	asm volatile (
"arch_task_switch: .globl arch_task_switch");
	asm volatile ("move	%0, $a0" : "=r" (target));

	asm volatile ("addi	$sp, $sp, -%0" : : "i" (CONTEXT_WORDS * 4 + 16));
	asm volatile (".set	noat");
	asm volatile ("sw	$1, %0 ($sp)" : : "i" (CONTEXT_R1 * 4 + 16));
	asm volatile ("sw	$2, %0 ($sp)" : : "i" (CONTEXT_R2 * 4 + 16));
	asm volatile ("sw	$3, %0 ($sp)" : : "i" (CONTEXT_R3 * 4 + 16));
	asm volatile ("sw	$4, %0 ($sp)" : : "i" (CONTEXT_R4 * 4 + 16));
	asm volatile ("sw	$5, %0 ($sp)" : : "i" (CONTEXT_R5 * 4 + 16));
	asm volatile ("sw	$6, %0 ($sp)" : : "i" (CONTEXT_R6 * 4 + 16));
	asm volatile ("sw	$7, %0 ($sp)" : : "i" (CONTEXT_R7 * 4 + 16));
	asm volatile ("sw	$8, %0 ($sp)" : : "i" (CONTEXT_R8 * 4 + 16));
	asm volatile ("sw	$9, %0 ($sp)" : : "i" (CONTEXT_R9 * 4 + 16));
	asm volatile ("sw	$10, %0 ($sp)" : : "i" (CONTEXT_R10 * 4 + 16));
	asm volatile ("sw	$11, %0 ($sp)" : : "i" (CONTEXT_R11 * 4 + 16));
	asm volatile ("sw	$12, %0 ($sp)" : : "i" (CONTEXT_R12 * 4 + 16));
	asm volatile ("sw	$13, %0 ($sp)" : : "i" (CONTEXT_R13 * 4 + 16));
	asm volatile ("sw	$14, %0 ($sp)" : : "i" (CONTEXT_R14 * 4 + 16));
	asm volatile ("sw	$15, %0 ($sp)" : : "i" (CONTEXT_R15 * 4 + 16));
	asm volatile ("sw	$16, %0 ($sp)" : : "i" (CONTEXT_R16 * 4 + 16));
	asm volatile ("sw	$17, %0 ($sp)" : : "i" (CONTEXT_R17 * 4 + 16));
	asm volatile ("sw	$18, %0 ($sp)" : : "i" (CONTEXT_R18 * 4 + 16));
	asm volatile ("sw	$19, %0 ($sp)" : : "i" (CONTEXT_R19 * 4 + 16));
	asm volatile ("sw	$20, %0 ($sp)" : : "i" (CONTEXT_R20 * 4 + 16));
	asm volatile ("sw	$21, %0 ($sp)" : : "i" (CONTEXT_R21 * 4 + 16));
	asm volatile ("sw	$22, %0 ($sp)" : : "i" (CONTEXT_R22 * 4 + 16));
	asm volatile ("sw	$23, %0 ($sp)" : : "i" (CONTEXT_R23 * 4 + 16));
	asm volatile ("sw	$24, %0 ($sp)" : : "i" (CONTEXT_R24 * 4 + 16));
	asm volatile ("sw	$25, %0 ($sp)" : : "i" (CONTEXT_R25 * 4 + 16));
	/* Skip $26 - K0*/
	/* Skip $27 - K1*/
	asm volatile ("sw	$28, %0 ($sp)" : : "i" (CONTEXT_GP * 4 + 16));
	/* Skip $29 - SP*/
	asm volatile ("sw	$30, %0 ($sp)" : : "i" (CONTEXT_FP * 4 + 16));
	asm volatile ("sw	$31, %0 ($sp)" : : "i" (CONTEXT_RA * 4 + 16));
	asm volatile (".set	at");

	/* Save special registers. */
	asm volatile ("mfhi	$a0" : : : "a0");
	asm volatile ("sw	$a0, %0 ($sp)" : : "i" (CONTEXT_HI * 4 + 16));

	asm volatile ("mflo	$a0" : : : "a0");
	asm volatile ("sw	$a0, %0 ($sp)" : : "i" (CONTEXT_LO * 4 + 16));

	asm volatile ("mfc0	$a0, $%0" : : "i" (C0_STATUS) : "a0");
	asm volatile ("sw	$a0, %0 ($sp)" : : "i" (CONTEXT_STATUS * 4 + 16));

	asm volatile ("sw	$ra, %0 ($sp)" : : "i" (CONTEXT_PC * 4 + 16));

	/* Save current task stack. */
	task_current->stack_context = mips32_get_stack_pointer ();

	task_current = target;

	/* Switch to the new task. */
	mips32_set_stack_pointer (task_current->stack_context);

	/* Restore registers. */
	asm volatile (
		".globl	_restore_regs_ \n"
	"	j	_restore_regs_ \n"
	"	nop"
	);
}

/*
 * Interrupt handler.
 * The call is performed via the assembler label,
 * to skip the function prologue, generated by the compiler.
 */
void
_arch_interrupt_ (void)
{
	lock_irq_t *h;
	int irq;

	asm volatile (
"_irq_handler_: .globl _irq_handler_"
	);
	for (;;) {
		/* Get the current irq number */
#ifdef ELVEES_MC24
		irq = 31 - mips32_count_leading_zeroes (MC_QSTR & MC_MASKR);
		if (irq < 0)
			break;
/*debug_printf ("<%d>", irq);*/
#endif
		if (irq >= ARCH_INTERRUPTS)
			break;

		/* Disable the irq, to avoid loops */
#ifdef ELVEES_MC24
		MC_MASKR &= ~(1 << irq);	/* disable */
#endif
		h = &lock_irq [irq];
		if (! h->lock)
			continue;

		if (h->handler) {
			/* If the lock is free -- call fast handler. */
			if (h->lock->master) {
				/* Lock is busy -- remember pending irq.
				 * Call fast handler later, in lock_release(). */
				h->pending = 1;
				continue;
			}
			if ((h->handler) (h->arg) != 0) {
				/* The fast handler returns 1 when it fully
				 * serviced an interrupt. In this case
				 * there is no need to wake up the interrupt
				 * servicing task, stopped on lock_wait.
				 * Task switching is not performed. */
				continue;
			}
		}

		/* Signal the interrupt handler, if any. */
		lock_activate (h->lock, 0);
	}

	/* LY: copy a few lines of code from task_schedule() here. */
	if (task_need_schedule)	{
		task_t *t;

		task_need_schedule = 0;
		t = task_policy ();
		if (t != task_current) {
			task_current->stack_context = mips32_get_stack_pointer ();
			task_current = t;
			t->ticks++;
			mips32_set_stack_pointer (t->stack_context);
		}
	}

	/* Restore registers. */
	asm volatile (
		".globl	_restore_regs_ \n"
	"	j	_restore_regs_ \n"
	"	nop"
	);
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void
arch_intr_allow (int irq)
{
#ifdef ELVEES_MC24
	MC_MASKR |= 1 << irq;
/*debug_printf ("enable irq %d, MASKR=%#x\n", irq, MC_MASKR);*/
#endif
}

/*
 * Build the initial task's stack frame.
 * Arguments:
 * t	   - task object
 * func	   - function to call
 * arg	   - argument of function
 * stacksz - size of stack space
 */
void
arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg,
	unsigned stacksz)
{
	unsigned *sp = (unsigned*) ((char*) t + stacksz);
	extern void _gp;

	*--sp = 0;			/* space for saving arg0 */
	*--sp = (unsigned) func;	/* epc - callee address */
	*--sp = mips32_read_c0_register (C0_STATUS) | ST_IE; /* enable interrupts */
	*--sp = 0;			/* hi */
	*--sp = 0;			/* lo */
	*--sp = 0;			/* ra */
	*--sp = 0;			/* fp */
	*--sp = (unsigned) &_gp;	/* gp */
	*--sp = 0;			/* r25 */
	*--sp = 0;			/* r24 */
	*--sp = 0;			/* r23 */
	*--sp = 0;			/* r22 */
	*--sp = 0;			/* r21 */
	*--sp = 0;			/* r20 */
	*--sp = 0;			/* r19 */
	*--sp = 0;			/* r18 */
	*--sp = 0;			/* r17 */
	*--sp = 0;			/* r16 */
	*--sp = 0;			/* r15 */
	*--sp = 0;			/* r14 */
	*--sp = 0;			/* r13 */
	*--sp = 0;			/* r12 */
	*--sp = 0;			/* r11 */
	*--sp = 0;			/* r10 */
	*--sp = 0;			/* r9 */
	*--sp = 0;			/* r8 */
	*--sp = 0;			/* r7 */
	*--sp = 0;			/* r6 */
	*--sp = 0;			/* r5 */
	*--sp = (unsigned) arg;		/* r4 - task argument */
	*--sp = 0;			/* r3 */
	*--sp = 0;			/* r2 */
	*--sp = 0;			/* r1 */
	*--sp = 0;			/* arg4 */
	*--sp = 0;			/* arg3 */
	*--sp = 0;			/* arg2 */
	*--sp = 0;			/* arg1 */
	t->stack_context = (void*) sp;
}
