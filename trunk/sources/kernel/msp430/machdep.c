/*
 * Machine-dependent part of uOS for MSP430.
 *
 * Copyright (C) 2009 Serge Vakulenko, <serge@vak.ru>
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

	*--sp = (unsigned) func;	/* pc - callee address */
	*--sp = msp430_read_sr() | GIE;	/* enable interrupts */
	*--sp = 0;			/* r4 */
	*--sp = 0;			/* r5 */
	*--sp = 0;			/* r6 */
	*--sp = 0;			/* r7 */
	*--sp = 0;			/* r8 */
	*--sp = 0;			/* r9 */
	*--sp = 0;			/* r10 */
	*--sp = 0;			/* r11 */
	*--sp = 0;			/* r12 */
	*--sp = 0;			/* r13 */
	*--sp = 0;			/* r14 */
	*--sp = (unsigned) arg;		/* r15 - task argument */
	t->stack_context = (void*) sp;
}

/*
 * Save all registers in stack.
 */
#define SAVE_REGS() asm volatile (	\
	"	push r4 \n"		\
	"	push r5 \n"		\
	"	push r6 \n"		\
	"	push r7 \n"		\
	"	push r8 \n"		\
	"	push r9 \n"		\
	"	push r10 \n"		\
	"	push r11 \n"		\
	"	push r12 \n"		\
	"	push r13 \n"		\
	"	push r14 \n"		\
	"	push r15"		\
	);

/*
 * Perform the task switch.
 */
void __attribute__((naked))
arch_task_switch (task_t *target)
{
	/* Save status register. */
	asm volatile ("push r2");
	SAVE_REGS ();

	/* Save current task stack. */
	task_current->stack_context = msp430_get_stack_pointer ();

	task_current = target;

	/* Switch to the new task. */
	msp430_set_stack_pointer (task_current->stack_context);

	/* Restore registers. */
	asm volatile (
"_restore_regs_: \n"
	"	pop	r15 \n"
	"	pop	r14 \n"
	"	pop	r13 \n"
	"	pop	r12 \n"
	"	pop	r11 \n"
	"	pop	r10 \n"
	"	pop	r9 \n"
	"	pop	r8 \n"
	"	pop	r7 \n"
	"	pop	r6 \n"
	"	pop	r5 \n"
	"	pop	r4 \n"
	"	reti"
	);
}

/*
 * Interrupt handler.
 * The call is performed via the assembler label,
 * to skip the function prologue, generated by the compiler.
 */
void __attribute__((naked))
_irq_handler_ (lock_irq_t *h)
{
debug_printf ("/%d/\n", h->irq);
	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Lock is busy -- remember pending irq.
			 * Call fast handler later, in lock_release(). */
			h->pending = 1;
			asm volatile ("jump _restore_regs_");
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully
			 * serviced an interrupt. In this case
			 * there is no need to wake up the interrupt
			 * servicing task, stopped on lock_wait.
			 * Task switching is not performed. */
			asm volatile ("jump _restore_regs_");
		}
	}

	/* Signal the interrupt handler, if any. */
	lock_activate (h->lock, 0);

	/* LY: copy a few lines of code from task_schedule() here. */
	if (task_need_schedule)	{
		task_t *t;

		task_need_schedule = 0;
		t = task_policy ();
		if (t != task_current) {
			task_current->stack_context = msp430_get_stack_pointer ();
			task_current = t;
			t->ticks++;
			msp430_set_stack_pointer (t->stack_context);
		}
	}

	/* Restore registers. */
	asm volatile ("jump _restore_regs_");
}

/* TODO */
int _msp430_p2ie;
int _msp430_p1ie;
int _msp430_adc12ie;

/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void
arch_intr_allow (int irq)
{
debug_printf ("intr_allow (%d)\n", irq);
#if defined(__MSP430_147__) || defined(__MSP430_148__) || defined(__MSP430_149__)
	switch (irq) {
        case 1:  P2IE     = _msp430_p2ie;    break;	/* 0xFFE2 Port 2 */
        case 2:  /*U1IE  |= UTXIE1;*/        break;	/* 0xFFE4 USART 1 Transmit */
        case 3:  U1IE    |= URXIE1;          break;	/* 0xFFE6 USART 1 Receive */
        case 4:  P1IE     = _msp430_p1ie;    break;	/* 0xFFE8 Port 1 */
        case 5:  TACCTL1 |= CCIE;            break;	/* 0xFFEA Timer A CC1-2, TA */
        case 6:  TACCTL0 |= CCIE;            break;	/* 0xFFEC Timer A CC0 */
        case 7:  ADC12IE  = _msp430_adc12ie; break;	/* 0xFFEE ADC */
        case 8:  /*U0IE  |= UTXIE0;*/        break;	/* 0xFFF0 USART 0 Transmit */
        case 9:  U0IE    |= URXIE0;          break;	/* 0xFFF2 USART 0 Receive */
        case 10: IE1     |= WDTIE;           break;	/* 0xFFF4 Watchdog Timer */
        case 11: CACTL1  |= CAIE;            break;	/* 0xFFF6 Comparator A */
        case 12: TBCCTL1 |= CCIE;            break;	/* 0xFFF8 Timer B 1-7 */
        case 13: TBCCTL0 |= CCIE;            break;	/* 0xFFFA Timer B 0 */
        case 14:                             break;	/* 0xFFFC Non-maskable */
	}
#endif
}

/*
 * The interrupt handler pattern.
 */
#define HANDLE(n,mask)\
void __attribute__ ((naked, interrupt(n))) \
_intr##n (void) \
{\
	SAVE_REGS ();\
	mask; /* disable the interrupt, avoiding loops */\
	asm volatile ("mov %0, r15" : : "i" (&lock_irq [n/2]));\
	asm volatile ("jump _irq_handler_");\
}

#if defined(__MSP430_147__) || defined(__MSP430_148__) || defined(__MSP430_149__)
HANDLE (2,  P2IE = 0);		/* 0xFFE2 Port 2 */
HANDLE (4,  U1IE &= ~UTXIE1);	/* 0xFFE4 USART 1 Transmit */
HANDLE (6,  U1IE &= ~URXIE1);	/* 0xFFE6 USART 1 Receive */
HANDLE (8,  P1IE = 0);		/* 0xFFE8 Port 1 */
HANDLE (10, TACCTL1 &= ~CCIE);	/* 0xFFEA Timer A CC1-2, TA */
HANDLE (12, TACCTL0 &= ~CCIE);	/* 0xFFEC Timer A CC0 */
HANDLE (14, ADC12IE = 0);	/* 0xFFEE ADC */
HANDLE (16, U0IE &= ~UTXIE0);	/* 0xFFF0 USART 0 Transmit */
HANDLE (18, U0IE &= ~URXIE0);	/* 0xFFF2 USART 0 Receive */
HANDLE (20, IE1 &= ~WDTIE);	/* 0xFFF4 Watchdog Timer */
HANDLE (22, CACTL1 &= ~CAIE);	/* 0xFFF6 Comparator A */
HANDLE (24, TBCCTL1 &= ~CCIE);	/* 0xFFF8 Timer B 1-7 */
HANDLE (26, TBCCTL0 &= ~CCIE);	/* 0xFFFA Timer B 0 */
HANDLE (28, );			/* 0xFFFC Non-maskable */
#endif
