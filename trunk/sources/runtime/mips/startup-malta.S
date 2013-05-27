#
# Startup code for Microchip PIC32 microcontrollers.
# Using HID bootloader.
#
# Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
#
# This file is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.
#
# You can redistribute this file and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software Foundation;
# either version 2 of the License, or (at your discretion) any later version.
# See the accompanying file "COPYING.txt" for more details.
#
# As a special exception to the GPL, permission is granted for additional
# uses of the text contained in this file.  See the accompanying file
# "COPY-UOS.txt" for details.
#
#include <runtime/mips/io.h>

		.set	noreorder
		.set	mips32r2

#---------------------------------------
# Reset vector at 0x9D006000
#
		.section .init
		.org	0
		.type	_reset_vector_, @function
_reset_vector_:	.globl	_reset_vector_

		.extern	_init_
		la	$sp, _estack			# Stack at end of internal SRAM
		la	$v0, _init_
		jr	$v0				# Jump to _init_()
		lui	$gp, 0x8020			# Set global pointer (delay slot)

#---------------------------------------
# Base of exception handlers
#
		.section .exception
		.org	0
_exception_base_: .globl _exception_base_

                # Vector NMI interrupts
		.type	_nmi_vector_, @function
_nmi_vector_:   .globl  _nmi_vector_
                la      $k0, _reset_vector_
                jr      $k0
                nop

#---------------------------------------
# Restore CPU state and return from interrupt
#
		.type	_restore_regs_, @function
_restore_regs_: .globl  _restore_regs_
		addi	$sp, MIPS_FSPACE		# Remove an arg frame

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

		mfc0	$k0, $C0_STATUS			# Get STATUS
		li	$k1, -1 - ST_IE			# Prepare bitmask
		and	$k0, $k1			# Clear IE bit
		ori	$k0, ST_EXL			# Prevent interrupts until eret
		lw	$k1, (CONTEXT_STATUS * 4) ($sp)	# K0 = saved status, only IE bit
		andi	$k1, ST_IE			# Restore only IE bit
		or	$k0, $k1			# Add saved IE bit
		mtc0	$k0, $C0_STATUS			# put SR back
		ehb

		addi	$sp, CONTEXT_WORDS*4		# Deallocate saved context

		# Return from exception
		eret					# PC <= EPC; EXL <= 0
		nop					# just to be safe

#---------------------------------------
# Exception vector at 0x0x9D005200
#
		.org	0x200
		.type	_exception_vector_, @function
_exception_vector_: .globl _exception_vector_
		addi	$sp, -CONTEXT_WORDS*4		# Allocate space for registers

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
		sw	$a0, (CONTEXT_STATUS * 4) ($sp)

		addi	$sp, -MIPS_FSPACE		# Make an arg frame

		.extern _irq_handler_
		j	_irq_handler_			# Jump to C code, no return
		lui	$gp, 0x8020			# Set global pointer (delay slot)
