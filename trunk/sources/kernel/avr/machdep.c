/*
 * Machine-dependent part of uOS for: Atmel AVR, GCC.
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
#include "kernel/uos.h"
#include "kernel/internal.h"

/*
 * Clear bit in far register, using r30.
 */
#define clearb_r30(bit, port) do {			\
		asm volatile (				\
		"in r30, %0" "\n"			\
		"andi r30, %1" "\n"			\
		"out %0, r30"				\
		: /* no outputs */			\
		: "I" (_SFR_IO_ADDR(port)),		\
		  "M" (0xff & ~(1 << (bit))) : "cc", "r30");	\
	} while (0)

#define clearb_far30(bit, port) do {			\
		asm volatile (				\
		"lds r30, %0" "\n"			\
		"andi r30, %1" "\n"			\
		"sts %0, r30"				\
		: /* no outputs */			\
		: "M" (_SFR_MEM_ADDR(port)),		\
		  "M" (0xff & ~(1 << (bit))) : "cc", "r30");	\
	} while (0)

#define SAVE_REGS() asm volatile (\
	"push	r29\n	push	r28\n"\
	"push	r27\n	push	r26\n	push	r25\n	push	r24\n"\
	"push	r23\n	push	r22\n	push	r21\n	push	r20\n"\
	"push	r19\n	push	r18\n	push	r17\n	push	r16\n"\
	"push	r15\n	push	r14\n	push	r13\n	push	r12\n"\
	"push	r11\n	push	r10\n	push	r9\n	push	r8\n"\
	"push	r7\n	push	r6\n	push	r5\n	push	r4\n"\
	"push	r3\n	push	r2\n	push	r1\n	push	r0\n"\
	"clr	__zero_reg__\n")

/*
 * Perform the task switch.
 * The call is performed via the assembler label,
 * to skip the function prologue, generated by the compiler.
 */
void
arch_task_switch (task_t *target)
{
	/* Skip function prologue. */
	asm volatile (
	"push	r31\n"
	"in	r31, __SREG__\n"
	"cli\n"
	"push	r31\n"
	"push	r30\n");
	SAVE_REGS();

	/* Save current task stack. */
	task_current->stack_context = arch_get_stack_pointer ();

	task_current = target;

	/* Switch to the new task. */
	arch_set_stack_pointer (task_current->stack_context);

	asm volatile (
"restore_and_ret:" );

	/* Restore registers. */
	asm volatile (
	"pop	r0\n	pop	r1\n	pop	r2\n	pop	r3\n"
	"pop	r4\n	pop	r5\n	pop	r6\n	pop	r7\n"
	"pop	r8\n	pop	r9\n	pop	r10\n	pop	r11\n"
	"pop	r12\n	pop	r13\n	pop	r14\n	pop	r15\n"
	"pop	r16\n	pop	r17\n	pop	r18\n	pop	r19\n"
	"pop	r20\n	pop	r21\n	pop	r22\n	pop	r23\n"
	"pop	r24\n	pop	r25\n	pop	r26\n	pop	r27\n"
	"pop	r28\n	pop	r29\n	pop	r30\n	pop	r31\n"
	"sbrc	r31, 7\n"		/* test I flag */
	"rjmp	enable_interrupts\n"
	"out	__SREG__, r31\n"
	"pop	r31\n"
	"ret\n"				/* exit with interrupts disabled */
"enable_interrupts:\n"
	"andi	r31, 0x7f\n"		/* clear I flag */
	"out	__SREG__, r31\n"
	"pop	r31\n"
	"reti\n");			/* exit with interrupts enabled */
}

/*
 * The common part of the interrupt handler,
 * to minimize the code size.
 */
void  __attribute__ ((noreturn /* LY: ICE for 4.1.2, naked */))
_avr_intr (void)
{
	register lock_irq_t *h;

	asm volatile (
"handle_interrupt: .globl handle_interrupt");	/* vch: "супербыстрый" обработчик может сюда прыгать */

	SAVE_REGS();

	/* Assign h = r30:r31. */
	asm volatile ("" : "=z" (h));

	if (h->handler) {
		/* If the lock is free -- call fast handler. */
		if (h->lock->master) {
			/* Remember pending irq.
			 * Call fast handler later, in lock_release(). */
			h->pending = 1;
			asm volatile ("rjmp restore_and_ret");
		}
		if ((h->handler) (h->arg) != 0) {
			/* The fast handler returns 1 when it fully serviced
			 * an interrupt. In this case there is no need to
			 * wake up the interrupt handling task, stopped on
			 * lock_wait. Task switching is not performed. */
			asm volatile ("rjmp restore_and_ret");
		}
	}

	/* Signal the interrupt handler, if any. */
	lock_activate (h->lock, 0);

	/* LY: copy few lines of code from task_schedule() here. */
	if (task_need_schedule)	{
		task_t *new;

		task_need_schedule = 0;
		new = task_policy ();
		if (new != task_current) {
			task_current->stack_context = arch_get_stack_pointer ();
			task_current = new;
			new->ticks++;
			arch_set_stack_pointer (task_current->stack_context);
		}
	}
	asm volatile ("rjmp restore_and_ret");
	for (;;);
}

/*
 * The interrupt handler pattern.
 */
#define HANDLE(n,name,mask)\
void \
_intr##n (void) \
{\
	asm volatile (#name": .globl "#name"\n"\
	"push	r31\n"\
	"in	r31, __SREG__\n"\
	"ori	r31, 0x80\n"		/* set I flag on exit */\
	"push	r31\n"\
	"push	r30\n");\
	mask;				/* disable the irq, avoiding loops */\
	asm volatile ("rjmp handle_interrupt"\
		: : "z" (&lock_irq[n]));\
}

#if defined (__AVR_ATmega103__) || defined (__AVR_ATmega128__)
	/*
	 * ATmega 103, ATmega 128
	 */
	HANDLE(0, _interrupt0_, clearb_r30 (0, EIMSK))
	HANDLE(1, _interrupt1_, clearb_r30 (1, EIMSK))
	HANDLE(2, _interrupt2_, clearb_r30 (2, EIMSK))
	HANDLE(3, _interrupt3_, clearb_r30 (3, EIMSK))
	HANDLE(4, _interrupt4_, clearb_r30 (4, EIMSK))
	HANDLE(5, _interrupt5_, clearb_r30 (5, EIMSK))
	HANDLE(6, _interrupt6_, clearb_r30 (6, EIMSK))
	HANDLE(7, _interrupt7_, clearb_r30 (7, EIMSK))
	HANDLE(8, _output_compare2_, clearb_r30 (7, TIMSK))
	HANDLE(9, _overflow2_, clearb_r30 (6, TIMSK))
	HANDLE(10, _input_capture1_, clearb_r30 (5, TIMSK))
	HANDLE(11, _output_compare1a_, clearb_r30 (4, TIMSK))
	HANDLE(12, _output_compare1b_, clearb_r30 (3, TIMSK))
	HANDLE(13, _overflow1_, clearb_r30 (2, TIMSK))
	HANDLE(14, _output_compare0_, clearb_r30 (1, TIMSK))
	HANDLE(15, _overflow0_, clearb_r30 (0, TIMSK))
	HANDLE(16, _spi_, clearb_const (SPIE, SPCR))
	/* vch: прерывания от UART (UART0) могут перекрываться "супербыстрыми"
	        обработчиками, например, реализация sbus делает это. */
	asm (".weak _uart_recv_");
	HANDLE(17, _uart_recv_, clearb_const (RXCIE, UCR))
	asm (".weak _uart_data_");
	HANDLE(18, _uart_data_, clearb_const (UDRIE, UCR))
	asm (".weak _uart_trans_");
	HANDLE(19, _uart_trans_, clearb_const (TXCIE, UCR))
	HANDLE(20, _adc_, clearb_const (ADIE, ADCSR))
	HANDLE(21, _eeprom_ready_, clearb_const (EERIE, EECR))

	/* Allow gdb-stub to grab the analog comparator interrupt. */
	asm (".weak _comparator_");
	HANDLE(22, _comparator_, clearb_const (ACIE, ACSR))

#ifdef __AVR_ATmega128__
	/*
	 * ATmega 128
	 */
	HANDLE(23, _output_compare1c_, clearb_far30 (0, ETIMSK))
	HANDLE(24, _input_capture3_, clearb_far30 (5, ETIMSK))
	HANDLE(25, _output_compare3a_, clearb_far30 (4, ETIMSK))
	HANDLE(26, _output_compare3b_, clearb_far30 (3, ETIMSK))
	HANDLE(27, _output_compare3c_, clearb_far30 (1, ETIMSK))
	HANDLE(28, _overflow3_, clearb_far30 (2, ETIMSK))
	/* vch: прерывания от UART1 могут перекрываться "супербыстрыми"
	        обработчиками, например, реализация sbus делает это. */
	asm (".weak _uart1_recv_");
	HANDLE(29, _uart1_recv_, clearb_far30 (RXCIE, UCSR1B))
	asm (".weak _uart1_data_");
	HANDLE(30, _uart1_data_, clearb_far30 (UDRIE, UCSR1B))
	asm (".weak _uart1_trans_");
	HANDLE(31, _uart1_trans_, clearb_far30 (TXCIE, UCSR1B))
	HANDLE(32, _twi_, clearb_far30 (TWIE, TWCR))
	HANDLE(33, _spm_ready_, clearb_far30 (SPMIE, SPMCR))
#endif
#endif /* __AVR_ATmega103__ || __AVR_ATmega128__ */

#ifdef __IOM161
	/*
	 * ATmega 161
	 */
	HANDLE(0, _interrupt0_, clearb_r30 (6, GIMSK))
	HANDLE(1, _interrupt1_, clearb_r30 (7, GIMSK))
	HANDLE(2, _interrupt2_, clearb_r30 (5, GIMSK))
	HANDLE(3, _output_compare2_, clearb_r30 (7, TIMSK))
	HANDLE(4, _overflow2_, clearb_r30 (6, TIMSK))
	HANDLE(5, _input_capture1_, clearb_r30 (5, TIMSK))
	HANDLE(6, _output_compare1a_, clearb_r30 (4, TIMSK))
	HANDLE(7, _output_compare1b_, clearb_r30 (3, TIMSK))
	HANDLE(8, _overflow1_, clearb_r30 (2, TIMSK))
	HANDLE(9, _output_compare0_, clearb_r30 (1, TIMSK))
	HANDLE(10, _overflow0_, clearb_r30 (0, TIMSK))
	HANDLE(11, _spi_, clearb_const (SPIE, SPCR))
	HANDLE(12, _uart_recv_, clearb_const (RXCIE, UCR))
	HANDLE(13, _uart1_recv_, clearb_const (RXCIE, UCSR1B))
	HANDLE(14, _uart_data_, clearb_const (UDRIE, UCR))
	HANDLE(15, _uart1_data_, clearb_const (UDRIE, UCSR1B))
	HANDLE(16, _uart_trans_, clearb_const (TXCIE, UCR))
	HANDLE(17, _uart1_trans_, clearb_const (TXCIE, UCSR1B))
	HANDLE(18, _eeprom_ready_, clearb_const (EERIE, EECR))

	/* Allow gdb-stub to grab the analog comparator interrupt. */
	asm (".weak _comparator_");
	HANDLE(19, _comparator_, clearb_const (ACIE, ACSR))

#endif

#ifdef __AVR_ATmega2561__
        /*
	 * ATmega 2561
	 */
	HANDLE(0, _interrupt0_, clearb_r30 (0, EIMSK))
	HANDLE(1, _interrupt1_, clearb_r30 (1, EIMSK))
	HANDLE(2, _interrupt2_, clearb_r30 (2, EIMSK))
	HANDLE(3, _interrupt3_, clearb_r30 (3, EIMSK))
	HANDLE(4, _interrupt4_, clearb_r30 (4, EIMSK))
	HANDLE(5, _interrupt5_, clearb_r30 (5, EIMSK))
	HANDLE(6, _interrupt6_, clearb_r30 (6, EIMSK))
	HANDLE(7, _interrupt7_, clearb_r30 (7, EIMSK))
	HANDLE(8, _pcinterrupt0_, clearb_far30 (PCIE0, PCICR))
	HANDLE(9, _pcinterrupt1_, clearb_far30 (PCIE1, PCICR))
	HANDLE(11, _wdt_timeout_, clearb_far30 (WDIE,WDTCSR))
	HANDLE(12, _output_compare2a_, clearb_far30 (OCIE2A, TIMSK2))
	HANDLE(13, _output_compare2b_, clearb_far30 (OCIE2B, TIMSK2))
	HANDLE(14, _overflow2_, clearb_far30 (TOIE2 ,TIMSK2))
	HANDLE(15, _input_capture1_, clearb_far30 (ICIE3, TIMSK3))
	HANDLE(16, _output_compare1a_, clearb_far30 (OCIE1A, TIMSK1))
	HANDLE(17, _output_compare1b_, clearb_far30 (OCIE1B, TIMSK1))
	HANDLE(18, _output_compare1c_, clearb_far30 (OCIE1C, TIMSK1))
	HANDLE(19, _overflow1_, clearb_far30 (TOIE1 ,TIMSK1))
	HANDLE(20, _output_compare0a_, clearb_far30 (OCIE0A, TIMSK0))
	HANDLE(21, _output_compare0b_, clearb_far30 (OCIE0B, TIMSK0))
	HANDLE(22, _overflow0_, clearb_far30 (TOIE0 ,TIMSK0))
	HANDLE(23, _spi_, clearb_r30 (SPIE, SPCR))
	/* vch: прерывания от UART (UART0) могут перекрываться "супербыстрыми"
	        обработчиками, например, реализация sbus делает это. */
	asm (".weak _uart_recv_");
	HANDLE(24, _uart_recv_, clearb_far30 (RXCIE, UCR))
	asm (".weak _uart_data_");
	HANDLE(25, _uart_data_, clearb_far30 (UDRIE, UCR))
	asm (".weak _uart_trans_");
	HANDLE(26, _uart_trans_, clearb_far30 (TXCIE, UCR))
	/* Allow gdb-stub to grab the analog comparator interrupt. */
	asm (".weak _comparator_");
	HANDLE(27, _comparator_, clearb_far30 (ACIE, ACSR))
	HANDLE(28, _adc_, clearb_far30 (ADIE, ADCSRA))
	HANDLE(29, _eeprom_ready_, clearb_r30 (EERIE, EECR))
	HANDLE(30, _input_capture3_, clearb_far30 (ICIE3, TIMSK3))
	HANDLE(31, _output_compare3a_, clearb_far30 (OCIE3A, TIMSK3))
	HANDLE(32, _output_compare3b_, clearb_far30 (OCIE3B, TIMSK3))
	HANDLE(33, _output_compare3c_, clearb_far30 (OCIE3C, TIMSK3))
	HANDLE(34, _overflow3_, clearb_far30 (TOIE3 ,TIMSK3))
	asm (".weak _uart1_recv_");
	HANDLE(35, _uart1_recv_, clearb_far30 (RXCIE1, UCSR1B))
	asm (".weak _uart1_data_");
	HANDLE(36, _uart1_data_,  clearb_far30 (UDRIE1, UCSR1B))
	asm (".weak _uart1_trans_");
	HANDLE(37, _uart1_trans_, clearb_far30 (TXCIE1, UCSR1B))
	HANDLE(38, _twi_, clearb_far30 (TWIE, TWCR))
	HANDLE(39, _spm_ready_, clearb_r30 (SPMIE, SPMCSR))
	HANDLE(41, _output_compare4a_, clearb_far30 (OCIE4A, TIMSK4))
	HANDLE(42, _output_compare4b_, clearb_far30 (OCIE4B, TIMSK4))
	HANDLE(43, _output_compare4c_, clearb_far30 (OCIE4C, TIMSK4))
	HANDLE(44, _overflow4_, clearb_far30 (TOIE4 ,TIMSK4))
	HANDLE(46, _output_compare5a_, clearb_far30 (OCIE5A, TIMSK5))
	HANDLE(47, _output_compare5b_, clearb_far30 (OCIE5B, TIMSK5))
	HANDLE(48, _output_compare5c_, clearb_far30 (OCIE5C, TIMSK5))
	HANDLE(49, _overflow5_, clearb_far30 (TOIE5 ,TIMSK5))
#endif


/*
 * Allow the given hardware interrupt,
 * unmasking it in the interrupt controller.
 */
void
arch_intr_allow (int irq)
{
	switch ((unsigned char) irq) {
#if defined (__AVR_ATmega103__) || defined (__AVR_ATmega128__)
	case 0: setb (0, EIMSK); break;		/* External interrupts. */
	case 1: setb (1, EIMSK); break;
	case 2: setb (2, EIMSK); break;
	case 3: setb (3, EIMSK); break;
	case 4: setb (4, EIMSK); break;
	case 5: setb (5, EIMSK); break;
	case 6: setb (6, EIMSK); break;
	case 7: setb (7, EIMSK); break;

	case 8: setb (7, TIMSK); break;		/* Timer/counter interrupts. */
	case 9: setb (6, TIMSK); break;
	case 10: setb (5, TIMSK); break;
	case 11: setb (4, TIMSK); break;
	case 12: setb (3, TIMSK); break;
	case 13: setb (2, TIMSK); break;
	case 14: setb (1, TIMSK); break;
	case 15: setb (0, TIMSK); break;

	case 16: setb (SPIE, SPCR); break;	/* SPI event */
	case 17: setb (RXCIE, UCR); break;	/* UART receive complete */
						/* UART transmitter empty */
	case 18: /* Do not set UDRIE here! setb (UDRIE, UCR); */ break;
	case 19: setb (TXCIE, UCR); break;	/* UART transmit complete */
	case 20: setb (ADIE, ADCSR); break;	/* Analog-digital conversion complete */
	case 21: setb (EERIE, EECR); break;	/* EEPROM ready */
	case 22: setb (ACIE, ACSR); break;	/* Analog comparator event */

#ifdef __AVR_ATmega128__
	/*
	 * ATmega 128
	 */
	case 23: setb_far (0, ETIMSK); break;
	case 24: setb_far (5, ETIMSK); break;
	case 25: setb_far (4, ETIMSK); break;
	case 26: setb_far (3, ETIMSK); break;
	case 27: setb_far (1, ETIMSK); break;
	case 28: setb_far (2, ETIMSK); break;
	case 29: setb_far (RXCIE, UCSR1B); break;	/* UART 1 receive complete */
	case 30: /*setb_far (UDRIE, UCSR1B);*/ break; /* UART 1 transmitter empty */
	case 31: setb_far (TXCIE, UCSR1B); break;	/* UART 1 transmit complete */
	case 32: setb_far (TWIE, TWCR); break;
	case 33: setb_far (SPMIE, SPMCR); break;
#endif
#endif /* __AVR_ATmega103__ || __AVR_ATmega128__ */

#ifdef __IOM161
	/*
	 * ATmega 161
	 */
	case 0: setb (6, GIMSK); break;		/* External interrupts. */
	case 1: setb (7, GIMSK); break;
	case 2: setb (5, GIMSK); break;

	case 3: setb (7, TIMSK); break;		/* Timer/counter interrupts. */
	case 4: setb (6, TIMSK); break;
	case 5: setb (5, TIMSK); break;
	case 6: setb (4, TIMSK); break;
	case 7: setb (3, TIMSK); break;
	case 8: setb (2, TIMSK); break;
	case 9: setb (1, TIMSK); break;
	case 10: setb (0, TIMSK); break;

	case 11: setb (SPIE, SPCR); break;	/* SPI event */
	case 12: setb (RXCIE, UCR); break;	/* UART receive complete */
	case 13: setb (RXCIE, UCSR1B); break;	/* UART 1 receive complete */
						/* UART transmitter empty */
	case 14: /* Do not set UDRIE here! setb (UDRIE, UCR); */ break;
	case 15: /*setb (UDRIE, UCSR1B);*/ break;	/* UART 1 transmitter empty */
	case 16: setb (TXCIE, UCR); break;	/* UART transmit complete */
	case 17: setb (TXCIE, UCSR1B); break;	/* UART 1 transmit complete */
	case 18: setb (EERIE, EECR); break;	/* EEPROM ready */
	case 19: setb (ACIE, ACSR); break;	/* Analog comparator event */
#endif

#ifdef __AVR_ATmega2561__
        case 0:  setb (0, EIMSK); break;
	case 1:  setb (1, EIMSK); break;
	case 2:  setb (2, EIMSK); break;
	case 3:  setb (3, EIMSK); break;
	case 4:  setb (4, EIMSK); break;
	case 5:  setb (5, EIMSK); break;
	case 6:  setb (6, EIMSK); break;
	case 7:  setb (7, EIMSK); break;
	case 8:  setb (PCIE0, PCICR); break;
	case 9:  setb (PCIE1, PCICR); break;
	case 11: setb (WDIE,WDTCSR); break;
	case 12: setb (OCIE2A, TIMSK2); break;
	case 13: setb (OCIE2B, TIMSK2); break;
	case 14: setb (TOIE2 ,TIMSK2); break;
	case 15: setb (ICIE3, TIMSK3); break;
	case 16: setb (OCIE1A, TIMSK1); break;
	case 17: setb (OCIE1B, TIMSK1); break;
	case 18: setb (OCIE1C, TIMSK1); break;
	case 19: setb (TOIE1 ,TIMSK1); break;
	case 20: setb (OCIE0A, TIMSK0); break;
	case 21: setb (OCIE0B, TIMSK0); break;
	case 22: setb (TOIE0 ,TIMSK0); break;
	case 23: setb (SPIE, SPCR); break;
    	case 24: setb (RXCIE, UCR); break;
	case 25: /* Do not set UDRIE here! setb (UDRIE, UCR); */  break;
	case 26: setb (TXCIE, UCR); break;
	case 27: setb (ACIE, ACSR); break;
	case 28: setb (ADIE, ADCSRA); break;
	case 29: setb (EERIE, EECR); break;
	case 30: setb (ICIE3, TIMSK3); break;
	case 31: setb (OCIE3A, TIMSK3); break;
	case 32: setb (OCIE3B, TIMSK3); break;
	case 33: setb (OCIE3C, TIMSK3); break;
	case 34: setb (TOIE3 ,TIMSK3); break;
	case 35: setb (RXCIE1, UCSR1B); break;
	case 36: /* Do not set UDRIE here! setb (UDRIE, UCR); */  break;
	case 37: setb (TXCIE1, UCSR1B); break;
	case 38: setb (TWIE, TWCR); break;
	case 39: setb (SPMIE, SPMCSR); break;
	case 41: setb (OCIE4A, TIMSK4); break;
	case 42: setb (OCIE4B, TIMSK4); break;
	case 43: setb (OCIE4C, TIMSK4); break;
	case 44: setb (TOIE4 ,TIMSK4); break;
	case 46: setb (OCIE5A, TIMSK5); break;
	case 47: setb (OCIE5B, TIMSK5); break;
	case 48: setb (OCIE5C, TIMSK5); break;
	case 49: setb (TOIE5 ,TIMSK5); break;
#endif
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
arch_build_stack_frame (task_t *t, void (*func) (void*), void *arg, unsigned stacksz)
{
	char *sp = (char*) t + stacksz;

	*sp-- = 0;			/* caller address low... */
	*sp-- = 0;			/* ...high */
#ifdef __AVR_ATmega2561__
	*sp-- = 0;                      /* ...additional */
#endif
	*sp-- = (unsigned) func;	/* callee address low... */
	*sp-- = (unsigned) func >> 8;	/* ...high */
#ifdef __AVR_ATmega2561__
	*sp-- = 0;                      /* ...additional */
#endif
        *sp-- = 0;			/* r31 */
	*sp-- = 0x80;			/* SREG - enable interrupts */
	*sp-- = 0;			/* r30 */
	*sp-- = 0;			/* r29 */
	*sp-- = 0;			/* r28 */
	*sp-- = 0;			/* r27 */
	*sp-- = 0;			/* r26 */
	*sp-- = (unsigned) arg >> 8;	/* r25 - task argument high... */
	*sp-- = (unsigned) arg;		/* r24 ...low */
	*sp-- = 0;			/* r23 */
	*sp-- = 0;			/* r22 */
	*sp-- = 0;			/* r21 */
	*sp-- = 0;			/* r20 */
	*sp-- = 0;			/* r19 */
	*sp-- = 0;			/* r18 */
	*sp-- = 0;			/* r17 */
	*sp-- = 0;			/* r16 */
	*sp-- = 0;			/* r15 */
	*sp-- = 0;			/* r14 */
	*sp-- = 0;			/* r13 */
	*sp-- = 0;			/* r12 */
	*sp-- = 0;			/* r11 */
	*sp-- = 0;			/* r10 */
	*sp-- = 0;			/* r9 */
	*sp-- = 0;			/* r8 */
	*sp-- = 0;			/* r7 */
	*sp-- = 0;			/* r6 */
	*sp-- = 0;			/* r5 */
	*sp-- = 0;			/* r4 */
	*sp-- = 0;			/* r3 */
	*sp-- = 0;			/* r2 */
	*sp-- = 0;			/* r1 */
	*sp-- = 0;			/* r0 */
	t->stack_context = (void*) sp;
}

/*
 * Check that the memory address is correct.
 */
bool_t __attribute__((weak))
uos_valid_memory_address (void *ptr)
{
	unsigned u = (unsigned) ptr;
	extern char __stack;

	return u > 0x7FFF || (u > 0xFF && u <= (unsigned) &__stack);
}

/*
 * Halt uOS, return to the parent operating system.
 */
void
arch_halt (int dump_flag)
{
	unsigned char n;
	task_t *t;
	void *callee = __builtin_return_address (0);
	void *sp = arch_get_stack_pointer (SP);

	if (dump_flag) {
		debug_task_print (0);
		n = 0;
		list_iterate (t, &task_active) {
			if (t != task_idle && t != task_current)
				debug_task_print (t);
			if (! uos_valid_memory_address (t))
				break;
			if (++n > 32 || list_is_empty (&t->item)) {
				debug_puts ("...\n");
				break;
			}
		}
		if (task_current && task_current != task_idle)
			debug_task_print (task_current);

		debug_dump_stack (__debug_task_name (task_current), sp,
			(void*) task_current->stack_context, callee);
		debug_printf ("\n*** Please report this information\n");
	}
	/*
	 * Define assembler constant task_stack_context_offset,
	 * which is a byte offset of stack_context field in tast_t structure.
	 * Needed for _init_ for saving task_current.
	 */
#define DEFINE_DEVICE_ADDR(name, val)                   \
		__asm (					\
		".globl " #name "\n\t"                  \
		".set " #name ", %0"                    \
		:: "n" (val)                            \
	)
	DEFINE_DEVICE_ADDR (task_stack_context_offset,
		__builtin_offsetof (task_t, stack_context));
}
