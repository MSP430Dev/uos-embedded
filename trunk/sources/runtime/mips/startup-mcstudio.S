#
# Startup code for Elvees MIPS32 microcontrollers.
# Using BEV mode: vectors in flash memory.
#
# Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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

#ifndef RAM_HI
#define RAM_HI  0xb800
#endif

        .section .init
        .set    noreorder
        .set    mips32

#---------------------------------------
# Macro: flush cache
#
    .macro FLUSH_CACHE
#ifdef FLUSH_CACHE_IN_EXCEPTION
        li  $k0, 0xB82F4008
        lw  $k1, 0($k0)
        ori $k1, 0x5000
        sw  $k1, 0($k0)
        lw  $k1, 0($k0)
#endif
    .endm

#---------------------------------------
# Reset vector at 0xBFC00000
#
        .org    0
_reset_vector_: .globl  _reset_vector_

        .extern _init_
        mtc0    $zero, $C0_COUNT    # Zero count registers (needed for RTL model)
        la      $sp, _estack        # Stack at end of internal SRAM
#if 1
        lui     $gp, 0xb840         # Set global pointer
        mfc0    $a0, $C0_COUNT          # Get CPU tick counter
        sw      $a0, ($gp)          # Store start time to XYRAM0
#endif

        lui     $gp, RAM_HI         # Set global pointer
        j       _init_              # Jump to _init_()
        nop

#ifdef BOOT_CHECKSUM
        #
        # Длина и контрольная сумма flash-памяти по адресу 0xBFC00020.
        # Заполняется утилитой загрузки mcprog.
        #
        .org    0x020
        .word   0                   # Длина в байтах
        .word   0                   # Контрольная сумма
#endif

#---------------------------------------
# Macro: save all registers in stack
#
        .macro SAVE_REGS
#ifdef ENABLE_ICACHE
        la  $k0, 1f             # Jump to kseg0
        jr  $k0                 # to enable cache
        nop                     # (delay slot)
1:
#endif
        addi    $sp, -CONTEXT_WORDS*4   # Allocate space for register

        .set    noat
        sw  $1, (CONTEXT_R1 * 4) ($sp)  # Save general registers
        sw  $2, (CONTEXT_R2 * 4) ($sp)
        sw  $3, (CONTEXT_R3 * 4) ($sp)
        sw  $4, (CONTEXT_R4 * 4) ($sp)
        sw  $5, (CONTEXT_R5 * 4) ($sp)
        sw  $6, (CONTEXT_R6 * 4) ($sp)
        sw  $7, (CONTEXT_R7 * 4) ($sp)
        sw  $8, (CONTEXT_R8 * 4) ($sp)
        sw  $9, (CONTEXT_R9 * 4) ($sp)
        sw  $10, (CONTEXT_R10 * 4) ($sp)
        sw  $11, (CONTEXT_R11 * 4) ($sp)
        sw  $12, (CONTEXT_R12 * 4) ($sp)
        sw  $13, (CONTEXT_R13 * 4) ($sp)
        sw  $14, (CONTEXT_R14 * 4) ($sp)
        sw  $15, (CONTEXT_R15 * 4) ($sp)
        sw  $16, (CONTEXT_R16 * 4) ($sp)
        sw  $17, (CONTEXT_R17 * 4) ($sp)
        sw  $18, (CONTEXT_R18 * 4) ($sp)
        sw  $19, (CONTEXT_R19 * 4) ($sp)
        sw  $20, (CONTEXT_R20 * 4) ($sp)
        sw  $21, (CONTEXT_R21 * 4) ($sp)
        sw  $22, (CONTEXT_R22 * 4) ($sp)
        sw  $23, (CONTEXT_R23 * 4) ($sp)
        sw  $24, (CONTEXT_R24 * 4) ($sp)
        sw  $25, (CONTEXT_R25 * 4) ($sp)
        # Skip $26 - K0
        # Skip $27 - K1
        sw  $28, (CONTEXT_GP * 4) ($sp)
        # Skip $29 - SP
        sw  $30, (CONTEXT_FP * 4) ($sp)
        sw  $31, (CONTEXT_RA * 4) ($sp)
        .set    at

        mfhi    $a0                     # Save special registers
        sw      $a0, (CONTEXT_HI * 4) ($sp)

        mflo    $a0
        sw      $a0, (CONTEXT_LO * 4) ($sp)

        mfc0    $a0, $C0_EPC
        sw      $a0, (CONTEXT_PC * 4) ($sp)

        mfc0    $a0, $C0_STATUS
        sw      $a0, (CONTEXT_STATUS * 4) ($sp)
        .endm

#ifndef MIPS_NOBEV

#---------------------------------------
# TLB refill vector at 0x....0200
#
        .org    0x0200
_pagefault_vector_: .globl _pagefault_vector_
        FLUSH_CACHE
        SAVE_REGS                       # Save all registers in stack
        move    $a0, $sp                # Arg0: pointer to saved state
        addi    $sp, -MIPS_FSPACE       # Make an arg frame

        .extern _pagefault_handler_
        lui     $gp, RAM_HI             # Set global pointer
        j       _pagefault_handler_     # Jump to C code, no return
        nop

#endif

#---------------------------------------
# Exception vector at 0x....0380
#

#ifdef MIPS_NOBEV
        .org    0x0180
#else
        .org    0x0380
#endif

_exception_vector_: .globl _exception_vector_

        FLUSH_CACHE
        mfc0    $k0, $C0_CAUSE          # Get CAUSE
        andi    $k0, CA_EXC_CODE        # Extract exception code
        li      $k1, CA_Sys             # Is it syscall?
        bne     $k1, $k0, exception     # No - jump to handler.
        nop                             # (delay slot)

        mfc0    $k0, $C0_EPC            # Get EPC
        add     $k0, 4                  # Return to next instruction
        mtc0    $k0, $C0_EPC            # Put EPC back

        #
        # Инструкция SYSCALL: чтение регистра STATUS и запрет прерываний.
        #
        mfc0    $a0, $C0_STATUS         # Get SR
        li      $k0, -1 - ST_IE         # Prepare bitmask
        and     $k0, $a0                # Clear IE bit
        mtc0    $k0, $C0_STATUS         # Put SR back
        xori    $a0, ST_EXL             # Clear EXL for user

        # Return from exception
        .set mips3
#ifdef ELVEES_MCT02
        nop                             # settle things down
        nop
        nop
        nop
        eret                            # PC <= EPC; EXL <= 0
        nop                             # just to be safe
        nop
        nop
        nop
#else
        sync                            # Settle things down
        eret                            # PC <= EPC; EXL <= 0
        nop                             # just to be safe
#endif
        .set mips0

#---------------------------------------
# Interrupt vector at 0x....0400
#

#ifdef MIPS_NOBEV
        .org    0x0200
#else
        .org    0x0400
#endif
_interrupt_vector_: .globl _interrupt_vector_

        FLUSH_CACHE
        SAVE_REGS                       # Save all registers in stack
        addi    $sp, -MIPS_FSPACE       # Make an arg frame

        .extern _irq_handler_
        lui     $gp, RAM_HI             # Set global pointer
        j       _irq_handler_           # Jump to C code, no return
        nop

#---------------------------------------
# Hangle exceptions
#
exception:
        SAVE_REGS                       # Save all registers in stack
        move    $a0, $sp                # Arg0: pointer to saved state
        addi    $sp, -MIPS_FSPACE       # Make an arg frame

        .extern _exception_handler_
        lui     $gp, RAM_HI                 # Set global pointer
        j       _exception_handler_         # Jump to C code, no return
        nop

#---------------------------------------
# Restore CPU state and return from interrupt
_restore_regs_: .globl _restore_regs_
        addi    $sp, MIPS_FSPACE        # Remove an arg frame

        lw      $a0, (CONTEXT_LO * 4) ($sp) # Load HI, LO registers
        mtlo    $a0
        lw      $a0, (CONTEXT_HI * 4) ($sp)
        mthi    $a0

        lw      $a0, (CONTEXT_PC * 4) ($sp) # K1 = EPC
        mtc0    $a0, $C0_EPC                # put PC in EPC

        .set    noat
        lw  $1, (CONTEXT_R1 * 4) ($sp)      # Load general registers
        lw  $2, (CONTEXT_R2 * 4) ($sp)
        lw  $3, (CONTEXT_R3 * 4) ($sp)
        lw  $4, (CONTEXT_R4 * 4) ($sp)
        lw  $5, (CONTEXT_R5 * 4) ($sp)
        lw  $6, (CONTEXT_R6 * 4) ($sp)
        lw  $7, (CONTEXT_R7 * 4) ($sp)
        lw  $8, (CONTEXT_R8 * 4) ($sp)
        lw  $9, (CONTEXT_R9 * 4) ($sp)
        lw  $10, (CONTEXT_R10 * 4) ($sp)
        lw  $11, (CONTEXT_R11 * 4) ($sp)
        lw  $12, (CONTEXT_R12 * 4) ($sp)
        lw  $13, (CONTEXT_R13 * 4) ($sp)
        lw  $14, (CONTEXT_R14 * 4) ($sp)
        lw  $15, (CONTEXT_R15 * 4) ($sp)
        lw  $16, (CONTEXT_R16 * 4) ($sp)
        lw  $17, (CONTEXT_R17 * 4) ($sp)
        lw  $18, (CONTEXT_R18 * 4) ($sp)
        lw  $19, (CONTEXT_R19 * 4) ($sp)
        lw  $20, (CONTEXT_R20 * 4) ($sp)
        lw  $21, (CONTEXT_R21 * 4) ($sp)
        lw  $22, (CONTEXT_R22 * 4) ($sp)
        lw  $23, (CONTEXT_R23 * 4) ($sp)
        lw  $24, (CONTEXT_R24 * 4) ($sp)
        lw  $25, (CONTEXT_R25 * 4) ($sp)
        # Skip $26 - K0
        # Skip $27 - K1
        lw  $28, (CONTEXT_GP * 4) ($sp)
        # Skip $29 - SP
        lw  $30, (CONTEXT_FP * 4) ($sp)
        lw  $31, (CONTEXT_RA * 4) ($sp)
        .set    at

        mfc0    $k0, $C0_STATUS         # Get STATUS
        li      $k1, -1 - ST_IE         # Prepare bitmask
        and     $k0, $k1                # Clear IE bit
        ori     $k0, ST_EXL             # Prevent interrupts until eret
        lw      $k1, (CONTEXT_STATUS * 4) ($sp) # K0 = saved status, only IE bit
        andi    $k1, ST_IE              # Restore only IE bit
        or      $k0, $k1                # Add saved IE bit
        mtc0    $k0, $C0_STATUS         # put SR back
        nop
        nop

        addi    $sp, CONTEXT_WORDS*4        # Deallocate saved context

        # Return from exception
        .set mips3
#ifdef ELVEES_MCT02
        nop                             # settle things down
        nop
        nop
        nop
        eret                            # PC <= EPC; EXL <= 0
        nop                             # just to be safe
        nop
        nop
        nop
#else
        sync                            # settle things down
        eret                            # PC <= EPC; EXL <= 0
        nop                             # just to be safe
#endif
        .set mips0

