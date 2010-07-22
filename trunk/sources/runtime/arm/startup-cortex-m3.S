		.syntax unified
		.section .init
		.align	2
		.thumb
		@
		@ Vector table for Cortex M3.
		@
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
		.word	_intr32			@ 15 - system timer tick

		@
		@ IRQs for 1986BE9x.
		@
		.word	_intr0			@ 0 - CAN1
		.word	_intr1			@ 1 - CAN2
		.word	_intr2			@ 2 - USB
		.word	0			@ 3 - reserved
		.word	0			@ 4 - reserved
		.word	_intr5			@ 5 - DMA
		.word	_intr6			@ 6 - UART1
		.word	_intr7			@ 7 - UART2
		.word	_intr8			@ 8 - SSP1
		.word	0			@ 9 - reserved
		.word	_intr10			@ 10 - I2C
		.word	_intr11			@ 11 - POWER
		.word	_intr12			@ 12 - WWDG
		.word	0			@ 13 - reserved
		.word	_intr14			@ 14 - Timer1
		.word	_intr15			@ 15 - Timer2
		.word	_intr16			@ 16 - Timer3
		.word	_intr17			@ 17 - ADC
		.word	0			@ 18 - reserved
		.word	_intr19			@ 19 - COMPARATOR
		.word	_intr20			@ 20 - SSP2
		.word	0			@ 21 - reserved
		.word	0			@ 22 - reserved
		.word	0			@ 23 - reserved
		.word	0			@ 24 - reserved
		.word	0			@ 25 - reserved
		.word	0			@ 26 - reserved
		.word	_intr27			@ 27 - BACKUP
		.word	_intr28			@ 28 - EXT_INT1, pin PA[0]
		.word	_intr29			@ 29 - EXT_INT2, pin PB[10]
		.word	_intr30			@ 30 - EXT_INT3, pin PE[15]
		.word	_intr31			@ 31 - EXT_INT4, pin PC[13]

		.section .text
		@
		@ Startup initialization.
		@
_reset_:
		@ Jump to _init_.
		ldr	lr, =_init_
		bx	lr
