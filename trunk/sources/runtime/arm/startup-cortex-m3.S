		.syntax unified

@---------------------------------------
@ Startup initialization.
@
		.section .text
		.thumb
		.thumb_func
_reset_:
		@ Jump to _init_.
		ldr	lr, =_init_
		bx	lr

@---------------------------------------
@ Macro: setup an interrupt vector.
@ By default, an _unexpected_interrupt_() routine is called.
@
		.macro	HANDLER name
		.weak	\name
		.set	\name, _unexpected_interrupt_
		.word	\name
		.endm

		.extern	_unexpected_interrupt_
		.extern	_fault_

@---------------------------------------
@ Vector table for Cortex M3.
@
		.section .init
		.align	2
		.thumb
_start_:	.global	_start_
		@
		@ End of user mode stack
		@
		.word  _estack			@ 0

		@
		@ Exception handlers.
		@
		.word	_reset_			@ 1 - reset
		.word	_reset_			@ 2 - non-maskable interrupt
		.word	_fault_			@ 3 - hard fault
		.word	_fault_			@ 4 - memory management fault
		.word	_fault_			@ 5 - bus fault
		.word	_fault_			@ 6 - usage fault
		.word	0			@ 7 - reserved
		.word	0			@ 8 - reserved
		.word	0			@ 9 - reserved
		.word	0			@ 10 - reserved
		.word	_fault_			@ 11 - supervisor call
		.word	0			@ 12 - reserved
		.word	0			@ 13 - reserved
		.word	_fault_			@ 14 - software interrupt
		HANDLER	_intr32			@ 15 - system timer tick

		@
		@ IRQs for 1986BE9x.
		@
		HANDLER	_intr0			@ 0 - CAN1
		HANDLER	_intr1			@ 1 - CAN2
		HANDLER	_intr2			@ 2 - USB
		.word	0			@ 3 - reserved
		.word	0			@ 4 - reserved
		HANDLER	_intr5			@ 5 - DMA
		HANDLER	_intr6			@ 6 - UART1
		HANDLER	_intr7			@ 7 - UART2
		HANDLER	_intr8			@ 8 - SSP1
		.word	0			@ 9 - reserved
		HANDLER	_intr10			@ 10 - I2C
		HANDLER	_intr11			@ 11 - POWER
		HANDLER	_intr12			@ 12 - WWDG
		.word	0			@ 13 - reserved
		HANDLER	_intr14			@ 14 - Timer1
		HANDLER	_intr15			@ 15 - Timer2
		HANDLER	_intr16			@ 16 - Timer3
		HANDLER	_intr17			@ 17 - ADC
		.word	0			@ 18 - reserved
		HANDLER	_intr19			@ 19 - COMPARATOR
		HANDLER	_intr20			@ 20 - SSP2
		.word	0			@ 21 - reserved
		.word	0			@ 22 - reserved
		.word	0			@ 23 - reserved
		.word	0			@ 24 - reserved
		.word	0			@ 25 - reserved
		.word	0			@ 26 - reserved
		HANDLER	_intr27			@ 27 - BACKUP
		HANDLER	_intr28			@ 28 - EXT_INT1, pin PA[0]
		HANDLER	_intr29			@ 29 - EXT_INT2, pin PB[10]
		HANDLER	_intr30			@ 30 - EXT_INT3, pin PE[15]
		HANDLER	_intr31			@ 31 - EXT_INT4, pin PC[13]
