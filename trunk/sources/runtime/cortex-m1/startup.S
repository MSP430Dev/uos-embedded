@
@ Startup code for Cortex-M3 microcontrollers.
@
@ Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
@
@ This file is distributed in the hope that it will be useful, but WITHOUT
@ ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
@ FITNESS FOR A PARTICULAR PURPOSE.
@
@ You can redistribute this file and/or modify it under the terms of the GNU
@ General Public License (GPL) as published by the Free Software Foundation;
@ either version 2 of the License, or (at your discretion) any later version.
@ See the accompanying file "COPYING.txt" for more details.
@
@ As a special exception to the GPL, permission is granted for additional
@ uses of the text contained in this file.  See the accompanying file
@ "COPY-UOS.txt" for details.
@
		.syntax unified

@---------------------------------------
@ Vector table for Cortex M3.
@
		.section .init
		.align	2
		.extern	_init_			@ startup
		.extern	_fault_			@ handle faults and software interrupt
		.extern	_svc_			@ handle supervisor call
		.extern	_irq_handler_		@ handle interrupts and systick

		.thumb
_start_:	.global	_start_
		@
		@ End of user mode stack
		@
		.word  _estack			@ 0

		@
		@ Exception handlers.
		@
		.word	_init_			@ 1 - reset
		.word	_fault_			@ 2 - non-maskable interrupt
		.word	_fault_			@ 3 - hard fault
		.word	_fault_			@ 4 - memory management fault
		.word	_fault_			@ 5 - bus fault
		.word	_fault_			@ 6 - usage fault
		.word	_fault_			@ 7 - reserved
		.word	_fault_			@ 8 - reserved
		.word	_fault_			@ 9 - reserved
		.word	_fault_			@ 10 - reserved
		.word	_svc_			@ 11 - supervisor call
		.word	_fault_			@ 12 - reserved
		.word	_fault_			@ 13 - reserved
		.word	_fault_			@ 14 - software interrupt
		.word	_irq_handler_		@ 15 - system timer tick

		@
		@ IRQs.
		@
		.word	_irq_handler_		@ 0 - CAN1
		.word	_irq_handler_		@ 1 - CAN2
		.word	_irq_handler_		@ 2 - USB
		.word	_irq_handler_		@ 3 - reserved
		.word	_irq_handler_		@ 4 - reserved
		.word	_irq_handler_		@ 5 - DMA
		.word	_irq_handler_		@ 6 - UART1
		.word	_irq_handler_		@ 7 - UART2
		.word	_irq_handler_		@ 8 - SSP1
		.word	_irq_handler_		@ 9 - reserved
		.word	_irq_handler_		@ 10 - I2C
		.word	_irq_handler_		@ 11 - POWER
		.word	_irq_handler_		@ 12 - WWDG
		.word	_irq_handler_		@ 13 - reserved
		.word	_irq_handler_		@ 14 - Timer1
		.word	_irq_handler_		@ 15 - Timer2
		.word	_irq_handler_		@ 16 - Timer3
		.word	_irq_handler_		@ 17 - ADC
		.word	_irq_handler_		@ 18 - reserved
		.word	_irq_handler_		@ 19 - COMPARATOR
		.word	_irq_handler_		@ 20 - SSP2
		.word	_irq_handler_		@ 21 - reserved
		.word	_irq_handler_		@ 22 - reserved
		.word	_irq_handler_		@ 23 - reserved
		.word	_irq_handler_		@ 24 - reserved
		.word	_irq_handler_		@ 25 - reserved
		.word	_irq_handler_		@ 26 - reserved
		.word	_irq_handler_		@ 27 - BACKUP
		.word	_irq_handler_		@ 28 - EXT_INT1, pin PA[0]
		.word	_irq_handler_		@ 29 - EXT_INT2, pin PB[10]
		.word	_irq_handler_		@ 30 - EXT_INT3, pin PE[15]
		.word	_irq_handler_		@ 31 - EXT_INT4, pin PC[13]
