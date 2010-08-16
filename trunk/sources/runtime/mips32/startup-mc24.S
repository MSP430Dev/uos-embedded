/*
 * Startup code for Elvees MC-24 microcontroller.
 * Using BEV mode: vectors in flash memory.
 */
#include <runtime/mips32/io.h>

		.section .init
		.set	noreorder
		.set	mips32

#---------------------------------------
# Reset vector at 0xBFC00000
#
		.org	0
_reset_vector_:	.globl	_reset_vector_

		.extern	_init_
		la	$sp, _estack-16			# Stack at end of internal SRAM

		j	_init_				# Jump to _init_()
		lui	$gp, 0xb800			# Set global pointer (delay slot)

#ifdef BOOT_CHECKSUM
#
# Длина и контрольная сумма flash-памяти по адресу 0xBFC00020.
# Заполняется утилитой загрузки mcprog.
#
		.org	0x020
		.word	0				# Длина в байтах
		.word	0				# Контрольная сумма
#endif

#---------------------------------------
# Macro: save all registers in stack
#
		.macro SAVE_REGS
#ifdef ENABLE_ICACHE
		la	$k0, 1f				# Jump to kseg0
		jr	$k0				# to enable cache
		nop					# (delay slot)
1:
#endif
		addi	$sp, $sp, -CONTEXT_WORDS*4	# Allocate space for register

		.set	noat
		sw	$1, (CONTEXT_R1 * 4) ($sp)	# Save general registers
		sw	$2, (CONTEXT_R2 * 4) ($sp)
		sw	$3, (CONTEXT_R3 * 4) ($sp)
		sw	$4, (CONTEXT_R4 * 4) ($sp)
		sw	$5, (CONTEXT_R5 * 4) ($sp)
		sw	$6, (CONTEXT_R6 * 4) ($sp)
		sw	$7, (CONTEXT_R7 * 4) ($sp)
		sw	$8, (CONTEXT_R8 * 4) ($sp)
		sw	$9, (CONTEXT_R9 * 4) ($sp)
		sw	$10, (CONTEXT_R10 * 4) ($sp)
		sw	$11, (CONTEXT_R11 * 4) ($sp)
		sw	$12, (CONTEXT_R12 * 4) ($sp)
		sw	$13, (CONTEXT_R13 * 4) ($sp)
		sw	$14, (CONTEXT_R14 * 4) ($sp)
		sw	$15, (CONTEXT_R15 * 4) ($sp)
		sw	$16, (CONTEXT_R16 * 4) ($sp)
		sw	$17, (CONTEXT_R17 * 4) ($sp)
		sw	$18, (CONTEXT_R18 * 4) ($sp)
		sw	$19, (CONTEXT_R19 * 4) ($sp)
		sw	$20, (CONTEXT_R20 * 4) ($sp)
		sw	$21, (CONTEXT_R21 * 4) ($sp)
		sw	$22, (CONTEXT_R22 * 4) ($sp)
		sw	$23, (CONTEXT_R23 * 4) ($sp)
		sw	$24, (CONTEXT_R24 * 4) ($sp)
		sw	$25, (CONTEXT_R25 * 4) ($sp)
		# Skip $26 - K0
		# Skip $27 - K1
		sw	$28, (CONTEXT_GP * 4) ($sp)
		# Skip $29 - SP
		sw	$30, (CONTEXT_FP * 4) ($sp)
		sw	$31, (CONTEXT_RA * 4) ($sp)
		.set	at

		mfhi	$a0				# Save special registers
		sw	$a0, (CONTEXT_HI * 4) ($sp)

		mflo	$a0
		sw	$a0, (CONTEXT_LO * 4) ($sp)

		mfc0	$a0, $C0_EPC
		sw	$a0, (CONTEXT_PC * 4) ($sp)

		mfc0	$a0, $C0_STATUS
		li	$a1, ~(ST_ERL | ST_EXL | ST_IE)
		and	$a0, $a0, $a1			# Clear ERL, EXL and IE bits
		sw	$a0, (CONTEXT_STATUS * 4) ($sp)
		mtc0	$a0, $C0_STATUS			# put SR back
		nop
		nop
		nop
		.endm

#---------------------------------------
# Hangle exceptions
#
exception:
		SAVE_REGS				# Save all registers in stack
		move	$a0, $sp			# Arg0: pointer to saved state
		addi	$sp, $sp, -16			# Make an arg frame

		.extern _exception_handler_
		j	_exception_handler_		# Jump to C code, no return
		lui	$gp, 0xb800			# Set global pointer (delay slot)

#---------------------------------------
# Restore CPU state and return from interrupt

_restore_regs_: .globl _restore_regs_
		addi	$sp, $sp, 16			# Remove an arg frame

		lw	$a0, (CONTEXT_LO * 4) ($sp)	# Load HI, LO registers
		mtlo	$a0
		lw	$a0, (CONTEXT_HI * 4) ($sp)
		mthi	$a0

		lw	$a0, (CONTEXT_PC * 4) ($sp)	# K1 = EPC
		mtc0	$a0, $C0_EPC			# put PC in EPC

		.set	noat
		lw	$1, (CONTEXT_R1 * 4) ($sp)	# Load general registers
		lw	$2, (CONTEXT_R2 * 4) ($sp)
		lw	$3, (CONTEXT_R3 * 4) ($sp)
		lw	$4, (CONTEXT_R4 * 4) ($sp)
		lw	$5, (CONTEXT_R5 * 4) ($sp)
		lw	$6, (CONTEXT_R6 * 4) ($sp)
		lw	$7, (CONTEXT_R7 * 4) ($sp)
		lw	$8, (CONTEXT_R8 * 4) ($sp)
		lw	$9, (CONTEXT_R9 * 4) ($sp)
		lw	$10, (CONTEXT_R10 * 4) ($sp)
		lw	$11, (CONTEXT_R11 * 4) ($sp)
		lw	$12, (CONTEXT_R12 * 4) ($sp)
		lw	$13, (CONTEXT_R13 * 4) ($sp)
		lw	$14, (CONTEXT_R14 * 4) ($sp)
		lw	$15, (CONTEXT_R15 * 4) ($sp)
		lw	$16, (CONTEXT_R16 * 4) ($sp)
		lw	$17, (CONTEXT_R17 * 4) ($sp)
		lw	$18, (CONTEXT_R18 * 4) ($sp)
		lw	$19, (CONTEXT_R19 * 4) ($sp)
		lw	$20, (CONTEXT_R20 * 4) ($sp)
		lw	$21, (CONTEXT_R21 * 4) ($sp)
		lw	$22, (CONTEXT_R22 * 4) ($sp)
		lw	$23, (CONTEXT_R23 * 4) ($sp)
		lw	$24, (CONTEXT_R24 * 4) ($sp)
		lw	$25, (CONTEXT_R25 * 4) ($sp)
		# Skip $26 - K0
		# Skip $27 - K1
		lw	$28, (CONTEXT_GP * 4) ($sp)
		# Skip $29 - SP
		lw	$30, (CONTEXT_FP * 4) ($sp)
		lw	$31, (CONTEXT_RA * 4) ($sp)
		.set	at

#		lw	$k0, (CONTEXT_STATUS * 4) ($sp)	# K0 = saved status
		mfc0	$k0, $C0_STATUS

		addi	$sp, $sp, CONTEXT_WORDS*4	# Deallocate saved context

		# Invoke CPU specific mechanism for returning from this exception
		.set mips3
		ori	$k0, $k0, ST_EXL | ST_IE	# prevent interrupts until eret
		mtc0	$k0, $C0_STATUS			# put SR back
		nop
		nop
		nop
		sync					# settle things down
		eret					# return
		nop					# just to be safe
		.set mips0

#---------------------------------------
# TLB refill vector at 0xBFC00200
#
		.org	0x0200
_pagefault_vector_: .globl _pagefault_vector_

		SAVE_REGS				# Save all registers in stack
		move	$a0, $sp			# Arg0: pointer to saved state
		addi	$sp, $sp, -16			# Make an arg frame

		.extern _pagefault_handler_
		j	_pagefault_handler_		# Jump to C code, no return
		lui	$gp, 0xb800			# Set global pointer (delay slot)

#---------------------------------------
# Exception vector at 0xBFC00380
#
		.org	0x0380
_exception_vector_: .globl _exception_vector_
		j	exception			# Not enough space here
		nop					# (delay slot)


#---------------------------------------
# Interrupt vector at 0xBFC00400
#
		.org	0x0400
_interrupt_vector_: .globl _interrupt_vector_

		SAVE_REGS				# Save all registers in stack
		addi	$sp, $sp, -16			# Make an arg frame

		.extern _irq_handler_
		j	_irq_handler_			# Jump to C code, no return
		lui	$gp, 0xb800			# Set global pointer (delay slot)
