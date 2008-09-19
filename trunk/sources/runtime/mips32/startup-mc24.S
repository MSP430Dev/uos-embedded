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
		j	_init_			# Jump to _init_()
		nop				# (delay slot)

#---------------------------------------
# Macro: save all registers in stack
#
		.macro SAVE_ALL_REGS
		addi    $sp, $sp, -32*4		# Allocate space for registers

		.set    noat
		sw      $1, (0 * 4) ($sp)	# Save general registers
		sw      $2, (1 * 4) ($sp)
		sw      $3, (2 * 4) ($sp)
		sw      $4, (3 * 4) ($sp)
		sw      $5, (4 * 4) ($sp)
		sw      $6, (5 * 4) ($sp)
		sw      $7, (6 * 4) ($sp)
		sw      $8, (7 * 4) ($sp)
		sw      $9, (8 * 4) ($sp)
		sw      $10, (9 * 4) ($sp)
		sw      $11, (10 * 4) ($sp)
		sw      $12, (11 * 4) ($sp)
		sw      $13, (12 * 4) ($sp)
		sw      $14, (13 * 4) ($sp)
		sw      $15, (14 * 4) ($sp)
		sw      $16, (15 * 4) ($sp)
		sw      $17, (16 * 4) ($sp)
		sw      $18, (17 * 4) ($sp)
		sw      $19, (18 * 4) ($sp)
		sw      $20, (19 * 4) ($sp)
		sw      $21, (20 * 4) ($sp)
		sw      $22, (21 * 4) ($sp)
		sw      $23, (22 * 4) ($sp)
		sw      $24, (23 * 4) ($sp)
		sw      $25, (24 * 4) ($sp)
		# Skip $26 - K0
		# Skip $27 - K1
		sw      $28, (25 * 4) ($sp)	# GP
		# Skip $29 - SP
		sw      $30, (26 * 4) ($sp)	# FP
		sw      $31, (27 * 4) ($sp)	# RA
		.set    at

		mfhi    $a1			# Save special registers
		mflo    $a0
		mfc0    $a2, C0_STATUS
		mfc0    $a3, C0_EPC
		sw      $a0, (28 * 4) ($sp)	# LO
		sw      $a1, (29 * 4) ($sp)	# HI
		sw      $a2, (30 * 4) ($sp)	# STATUS
		sw      $a3, (31 * 4) ($sp)	# EPC
		.endm

#---------------------------------------
# Hangle exceptions
#
exception:
		SAVE_ALL_REGS			# Save all registers in stack

		.extern _exception_handler_
		move    $a0, $sp		# Arg0: pointer to saved state
		j	_exception_handler_	# Jump to C code, no return
		addi    $sp, $sp, -16		# Make an arg frame (delay slot)

#---------------------------------------
# Restore CPU state and return from interrupt

_restore_state_: .globl _restore_state_

		addi	$sp, $sp, 16		# Deallocate an arg frame

		.set    noat
		lw      $1, (0 * 4) ($sp)	# Load general registers
		lw      $2, (1 * 4) ($sp)
		lw      $3, (2 * 4) ($sp)
		lw      $4, (3 * 4) ($sp)
		lw      $5, (4 * 4) ($sp)
		lw      $6, (5 * 4) ($sp)
		lw      $7, (6 * 4) ($sp)
		lw      $8, (7 * 4) ($sp)
		lw      $9, (8 * 4) ($sp)
		lw      $10, (9 * 4) ($sp)
		lw      $11, (10 * 4) ($sp)
		lw      $12, (11 * 4) ($sp)
		lw      $13, (12 * 4) ($sp)
		lw      $14, (13 * 4) ($sp)
		lw      $15, (14 * 4) ($sp)
		lw      $16, (15 * 4) ($sp)
		lw      $17, (16 * 4) ($sp)
		lw      $18, (17 * 4) ($sp)
		lw      $19, (18 * 4) ($sp)
		lw      $20, (19 * 4) ($sp)
		lw      $21, (20 * 4) ($sp)
		lw      $22, (21 * 4) ($sp)
		lw      $23, (22 * 4) ($sp)
		lw      $24, (23 * 4) ($sp)
		lw      $25, (24 * 4) ($sp)
		# Skip $26 - K0
		# Skip $27 - K1
		lw      $28, (25 * 4) ($sp)	# GP
		# Skip $29 - SP
		lw      $30, (26 * 4) ($sp)	# FP
		lw      $31, (27 * 4) ($sp)	# RA
		.set    at

		lw	$k0, (28 * 4) ($sp)	# Load special registers
		lw	$k1, (29 * 4) ($sp)
		mthi    $k1
		mtlo    $k0
		lw      $k0, (30 * 4) ($sp)	# K0 = saved status
		lw      $k1, (31 * 4) ($sp)	# K1 = return address

		addi	$sp, $sp, 32*4		# Deallocate space for saved registers

		# Invoke CPU specific mechanism for returning from this exception
		.set mips3
		ori     $k0, $k0, 2		# prevent interrupts until eret
		mtc0    $k0, C0_STATUS		# put SR back
		nop
		nop
		nop
		mtc0    $k1, C0_EPC		# put PC in EPC
		nop
		nop
		nop
		sync				# settle things down
		eret				# return
		nop				# just to be safe
		.set mips0

#---------------------------------------
# TLB refill vector at 0xBFC00200
#
		.org	0x0200
_pagefault_vector_: .globl _pagefault_vector_

		SAVE_ALL_REGS			# Save all registers in stack

		.extern _pagefault_handler_
		move    $a0, $sp		# Arg0: pointer to saved state
		j	_pagefault_handler_		# Jump to C code, no return
		addi    $sp, $sp, -16		# Make an arg frame (delay slot)

#---------------------------------------
# Exception vector at 0xBFC00380
#
		.org	0x0380
_exception_vector_: .globl _exception_vector_
		j	exception		# Not enough space here


#---------------------------------------
# Interrupt vector at 0xBFC00400
#
		.org	0x0400
_interrupt_vector_: .globl _interrupt_vector_

		SAVE_ALL_REGS			# Save all registers in stack

		.extern _interrupt_handler_
		move    $a0, $sp		# Arg0: pointer to saved state
		j	_interrupt_handler_	# Jump to C code, no return
		addi    $sp, $sp, -16		# Make an arg frame (delay slot)
