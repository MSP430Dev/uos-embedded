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
		la	$sp, 0xb8008000			# Stack at end of internal SRAM
		la	$gp, _gp			# Set global pointer

		j	_init_				# Jump to _init_()
		nop					# (delay slot)

#---------------------------------------
# Macro: save all registers in stack
#
		.macro SAVE_REGS
		addi	$sp, $sp, -CONTEXT_WORDS*4	# Allocate space for registers

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

		mfhi	$a1				# Save special registers
		mflo	$a0
		mfc0	$a2, $C0_STATUS
		mfc0	$a3, $C0_EPC
		sw	$a0, (CONTEXT_LO * 4) ($sp)
		sw	$a1, (CONTEXT_HI * 4) ($sp)
		sw	$a2, (CONTEXT_STATUS * 4) ($sp)
		sw	$a3, (CONTEXT_PC * 4) ($sp)
		.endm

#---------------------------------------
# Hangle exceptions
#
exception:
		SAVE_REGS				# Save all registers in stack
		la	$gp, _gp			# Set global pointer

		.extern _exception_handler_
		move	$a0, $sp			# Arg0: pointer to saved state
		j	_exception_handler_		# Jump to C code, no return
		addi	$sp, $sp, -4			# Make an arg frame (delay slot)

#---------------------------------------
# Restore CPU state and return from interrupt

_restore_regs_: .globl _restore_regs_

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

		lw	$k0, (CONTEXT_LO * 4) ($sp)	# Load special registers
		lw	$k1, (CONTEXT_HI * 4) ($sp)
		mthi	$k1
		mtlo	$k0
		lw	$k0, (CONTEXT_STATUS * 4) ($sp)	# K0 = saved status
		lw	$k1, (CONTEXT_PC * 4) ($sp)	# K1 = EPC

		addi	$sp, $sp, CONTEXT_WORDS*4	# Deallocate saved context

		# Invoke CPU specific mechanism for returning from this exception
		.set mips3
		ori	$k0, $k0, 2			# prevent interrupts until eret
		mtc0	$k0, $C0_STATUS			# put SR back
		nop
		nop
		nop
		mtc0	$k1, $C0_EPC			# put PC in EPC
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
		la	$gp, _gp			# Set global pointer

		.extern _pagefault_handler_
		move	$a0, $sp			# Arg0: pointer to saved state
		j	_pagefault_handler_		# Jump to C code, no return
		addi	$sp, $sp, -4			# Make an arg frame (delay slot)

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
		la	$gp, _gp			# Set global pointer

		.extern _interrupt_handler_
		j	_interrupt_handler_		# Jump to C code, no return
		nop					# (delay slot)
