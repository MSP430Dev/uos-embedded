/*
 * Machine-dependent part of uOS for: Intel i386, GCC.
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
#include "runtime/lib.h"
#include <runtime/i386/i8259.h>
#include "kernel/uos.h"
#include "kernel/internal.h"

/*
 * Perform the task switch.
 * Always called with interrupts disabled.
 * The call is performed via the assembler label,
 * to skip the function prologue, generated by the compiler.
 * When entering interrupt or switching task,
 * we save the following data in the stack:
 *	ES	- at SP-4
 *	DS	- at SP-8
 *	EDI	\
 *	ESI	|
 *	EBP	|
 *	ESP	| pushal
 *	EBX	|
 *	EDX	|
 *	ECX	|
 *	EAX	/
 *	ERR	- intr/trap info
 *	EIP
 *	CS
 *	EFLAGS	- at SP-56
 * Total 14 words or 56 bytes.
 */
void
_arch_task_switch (void)
{
	task_t *target;

	asm volatile (
"arch_task_switch: .globl arch_task_switch \n"
"	mov	4(%%esp), %0 \n"
"	push	$0 \n"
"	pushal \n"
"	push	%%ds \n"
"	push	%%es" : "=r" (target) : : "sp");

	/* Perform the context switch to the most priority task. */
	asm volatile (
"switch_task:");
	/* Save current task stack. */
	task_current->stack_context = i386_get_stack_pointer ();

	/* Compute new active task. */
	task_current = target;

	/* Switch to the new task. */
	i386_set_stack_pointer (task_current->stack_context);

	/* Restore registers. */
	asm volatile (
"restore_and_ret: \n"
"	pop	%es \n"
"	pop	%ds \n"
"	popal \n"
"	addl	$4,%esp \n"
"	iret");
}

/*
 * The common part of the interrupt handler.
 * It never returns.
 */
void
i386_intr (trapframe_t *f)
{
	mutex_irq_t *h = &mutex_irq [f->err];

	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Remember pending irq.
			 * Call fast handler later, in mutex_unlock(). */
			h->pending = 1;

			i386_set_stack_pointer (f);
			asm ("jmp restore_and_ret");
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully serviced
			 * an interrupt. In this case there is no need to
			 * wake up the interrupt handling task, stopped on
			 * mutex_wait. Task switching is not performed. */
			i386_set_stack_pointer (f);
			asm ("jmp restore_and_ret");
		}
	}

	/* Signal the interrupt handler, if any. */
	mutex_activate (h->lock, 0);

	/* TODO: task_policy */

	i386_set_stack_pointer (f);
	asm ("jmp switch_task");
}

/*
 * The common part of the trap handler.
 */
void
i386_trap (int trapno, trapframe_t *f)
{
	unsigned long ss;

	/*
	 * Fatal traps:
	 * 1) Nonmaskable interrupt.
	 * 2) Double page fault.
	 * 3) Invalid task segment.
	 */
	switch (trapno) {
	case I386_TRAP_NMI:
		debug_printf ("*** Nonmaskable interrupt ***\n\n");
		break;

	case I386_TRAP_DBLFAULT:
		debug_printf ("*** Double page fault ***\n\n");
		break;

	case I386_TRAP_INVTSS:
		debug_printf ("*** Invalid task segment ***\n\n");
		break;
	/*
	 * Unknown interrupt.
	 */
	case I386_TRAP_STRAY:
		debug_printf ("*** Stray interrupt ***\n");
		i386_set_stack_pointer (f);
		asm ("jmp restore_and_ret");
	/*
	 * Task-dependent traps.
	 */
	default:
		debug_printf ("*** Trap %d ***\n\n", trapno);
		break;
	}
	/*
	 * Print CPU state and halt.
	 */
	debug_printf ("Fatal error: %d", f->err);
	if (task_current)
		debug_printf (", task_current=%s\n\n",
			task_current->name);

	ss = i386_get_stack_segment();
	debug_printf ("cs:ip=%02x:%08x  ss:sp=%02x:%08x  ds=%02x  es=%02x  flags=%08x\n",
		f->cs, f->eip, ss, f->esp, f->ds, f->es, f->eflags);
	debug_printf ("ax=%08x  bx=%08x  cx=%08x  dx=%08x\n",
		f->eax, f->ebx, f->ecx, f->edx);
	debug_printf ("si=%08x  di=%08x  bp=%08x\n",
		f->esi, f->edi, f->ebp);
	debug_printf ("cr0=%08lx cr2=%08lx cr3=%08lx\n",
		rcr0 (), rcr2 (), rcr3 ());

	debug_printf ("\nPress <Reset> button to reboot\n");
	for (;;)
		asm ("hlt");
}

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void
arch_intr_allow (int irq)
{
	if (irq < 8) {
		outb (inb (PIC1_MASK) & ~(1 << irq), PIC1_MASK);
	} else {
		outb (inb (PIC2_MASK) & ~(1 << (irq & 7)), PIC2_MASK);
	}
}

/*
 * Build the initial task's stack frame.
 * Arguments:
 * t	- the task object
 * func	- the task function to call
 * arg	- the function argument
 * sp	- the pointer to (end of) stack space
 */
void
arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg,
	unsigned stacksz)
{
	unsigned oldsp, *sp = (unsigned*) ((char*) t + stacksz);

	*--sp = (unsigned) arg;		/* task argument */
	*--sp = (unsigned) func;	/* return address */
	*--sp = 0x0200;			/* flags - enable interrupts */
	*--sp = I386_CS;		/* cs */
	*--sp = (unsigned) func;	/* ip */
	*--sp = 0;			/* err */
	oldsp = (unsigned long) sp;
	*--sp = 0;			/* ax */
	*--sp = 0;			/* cx */
	*--sp = 0;			/* dx */
	*--sp = 0;			/* bx */
	*--sp = oldsp;			/* sp */
	*--sp = 0;			/* bp */
	*--sp = 0;			/* si */
	*--sp = 0;			/* di */
	*--sp = I386_DS;		/* ds */
	*--sp = I386_DS;		/* es */

	t->stack_context = (void*) sp;
}
