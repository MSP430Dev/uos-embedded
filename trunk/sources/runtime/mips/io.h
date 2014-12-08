/*
 * Hardware register defines for MIPS32 architecture.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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
#ifdef ELVEES_NVCOM02T
#define ELVEES_NVCOM01 1
#endif

#ifdef PIC32MX
#   include <runtime/mips/io-pic32mx.h>
#endif
#ifdef MALTA
#   include <runtime/mips/io-malta.h>
#endif
#ifdef ELVEES_MC24
#   define ELVEES	1
#   include <runtime/mips/io-mc24.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_MC24R2
#   define ELVEES	1
#   include <runtime/mips/io-mc24r2.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_NVCOM01
#   define ELVEES	1
#   include <runtime/mips/io-nvcom01.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_NVCOM02
#   define ELVEES	1
#   include <runtime/mips/io-nvcom02.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_MCT02
#   define ELVEES	1
#   include <runtime/mips/io-mct02.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_MCT03P
#   define ELVEES	1
#   include <runtime/mips/io-mct02.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_MC0226
#   define ELVEES	1
#   include <runtime/mips/io-mc0226.h>
#   include <runtime/mips/io-elvees.h>
#endif
#ifdef ELVEES_MC0428
#   define ELVEES	1
#   include <runtime/mips/io-mc0428.h>
#   include <runtime/mips/io-elvees.h>
#endif

/*
 * Offsets of register values in saved context.
 */
#define CONTEXT_R1	0
#define CONTEXT_R2	1
#define CONTEXT_R3	2
#define CONTEXT_R4	3
#define CONTEXT_R5	4
#define CONTEXT_R6	5
#define CONTEXT_R7	6
#define CONTEXT_R8	7
#define CONTEXT_R9	8
#define CONTEXT_R10	9
#define CONTEXT_R11	10
#define CONTEXT_R12	11
#define CONTEXT_R13	12
#define CONTEXT_R14	13
#define CONTEXT_R15	14
#define CONTEXT_R16	15
#define CONTEXT_R17	16
#define CONTEXT_R18	17
#define CONTEXT_R19	18
#define CONTEXT_R20	19
#define CONTEXT_R21	20
#define CONTEXT_R22	21
#define CONTEXT_R23	22
#define CONTEXT_R24	23
#define CONTEXT_R25	24
#define CONTEXT_GP	25
#define CONTEXT_FP	26
#define CONTEXT_RA	27
#define CONTEXT_LO	28
#define CONTEXT_HI	29
#define CONTEXT_STATUS	30
#define CONTEXT_PC	31

#define CONTEXT_WORDS	32

/*
 * Exception handlers, written in C, require some space in stack
 * for local storage. Minimum requirement is 16 bytes.
 * To compute needed space, you should get disassembled code of
 * _arch_interrupt_() and look at the function entry instructions.
 * For example:
 *	addiu   sp,sp,-64
 * 	sw      ra,60(sp)
 *	...
 *	sw      s0,24(sp)
 * Number in the last instruction is the frame space, needed for
 * local storage. When it is greater than 16, you must put it
 * in a macro definition here.
 */
#ifdef ELVEES_MC24R2
#   define MIPS_FSPACE		24	/* for Elvees MC24R2 */
#endif
#ifdef ELVEES_NVCOM01
#   define MIPS_FSPACE		24	/* for Elvees NVCom-01 */
#endif
#ifdef ELVEES_NVCOM02
#   define MIPS_FSPACE		24	/* TODO: for Elvees NVCom-02 */
#endif
#ifdef ELVEES_MCT02
#   define MIPS_FSPACE		24	/* for Elvees MCT02-01 */
#endif
#ifdef ELVEES_MCT03P
#   define MIPS_FSPACE		24	/* for Elvees MCT-03P */
#endif
#ifdef ELVEES_MC0226
#   define MIPS_FSPACE		24	/* for Elvees MC0226 */
#endif
#ifdef ELVEES_MC0428
#   define MIPS_FSPACE		24	/* for Elvees MC-0428 */
#endif
#ifndef MIPS_FSPACE
#   define MIPS_FSPACE		16	/* default minimum */
#endif

#ifndef __ASSEMBLER__

/*
 * Set value of stack pointer register.
 */
static void inline __attribute__ ((always_inline))
mips_set_stack_pointer (void *x)
{
	asm volatile (
	"move	$sp, %0"
	: : "r" (x) : "sp");
}

/*
 * Get value of stack pointer register.
 */
static inline __attribute__ ((always_inline))
void *mips_get_stack_pointer ()
{
	void *x;

	asm volatile (
	"move	%0, $sp"
	: "=r" (x));
	return x;
}

/*
 * Read C0 coprocessor register.
 */
#define mips_read_c0_register(reg)				\
({ int __value;							\
	asm volatile (						\
	"mfc0	%0, $%1"					\
	: "=r" (__value) : "K" (reg));				\
	__value;						\
})

#define mips_read_c0_select(reg,sel)				\
	({ int __value;						\
	asm volatile (						\
	"mfc0	%0, $%1, %2"					\
	: "=r" (__value) : "K" (reg), "K" (sel));		\
	__value;						\
})

/*
 * Write C0 coprocessor register.
 */
#if defined (ELVEES)

#define mips_write_c0_register(reg, value)			\
	do {							\
	asm volatile (						\
	"mtc0	%z0, $%1 \n	nop \n	nop \n	nop"		\
	: : "r" ((unsigned int) (value)), "K" (reg));		\
	} while (0)

#define mips_write_c0_select(reg, sel, value)			\
do {								\
	asm volatile (						\
	"mtc0	%z0, $%1, %2 \n	nop \n	nop \n	nop"		\
	: : "r" ((unsigned int) (value)), "K" (reg), "K" (sel));\
} while (0)

#else

#define mips_write_c0_register(reg, value)			\
	do {							\
	asm volatile (						\
	"mtc0	%z0, $%1 \n	ehb"                            \
	: : "r" ((unsigned int) (value)), "K" (reg));		\
	} while (0)

#define mips_write_c0_select(reg, sel, value)			\
do {								\
	asm volatile (						\
	"mtc0	%z0, $%1, %2 \n	ehb"                            \
	: : "r" ((unsigned int) (value)), "K" (reg), "K" (sel));\
} while (0)

#endif

/*
 * Read FPU (C1 coprocessor) register.
 */
#define mips_read_fpu_register(reg)				\
({ int __value;							\
	asm volatile (						\
	"mfc1	%0, $%1"					\
	: "=r" (__value) : "K" (reg));				\
	__value;						\
})

/*
 * Write FPU (C1 coprocessor) register.
 */
#define mips_write_fpu_register(reg, value)			\
do {								\
	asm volatile (						\
	"mtc1	%z0, $%1"					\
	: : "r" (value), "K" (reg));				\
} while (0)

/*
 * Read FPU control register.
 */
#define mips_read_fpu_control(reg)				\
({ int __value;							\
	asm volatile (						\
	"cfc1	%0, $%1"					\
	: "=r" (__value) : "K" (reg));				\
	__value;						\
})

/*
 * Write FPU control register.
 */
#define mips_write_fpu_control(reg, value)			\
do {								\
	asm volatile (						\
	"ctc1	%z0, $%1"					\
	: : "r" ((unsigned int) (value)), "K" (reg));		\
} while (0)

/*
 * Disable the hardware interrupts,
 * saving the interrupt state into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
mips_intr_disable (int *x)
{
#if defined (ELVEES_MCT02) || defined(ELVEES_MCT03P)
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("syscall");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile (
"	move	%0, $a0"
	: "=r" (*x) : "K" (C0_STATUS) : "a0");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");

#elif defined (ELVEES)
	/* This must be atomic operation.
	 * On MIPS1 this could be done only using system call exception. */
	asm volatile (
	"syscall \n"
"	move	%0, $a0"
	: "=r" (*x) : "K" (C0_STATUS) : "a0");
#else
	*x = mips_read_c0_register (C0_STATUS);
	asm volatile ("di");
#endif
}

/*
 * Restore the hardware interrupt mode using the saved interrupt state.
 */
static void inline __attribute__ ((always_inline))
mips_intr_restore (int x)
{
#if defined (ELVEES) || defined (MIPS32_4KEC)
	int status;

	status = mips_read_c0_register (C0_STATUS);
	mips_write_c0_register (C0_STATUS, status | (x & ST_IE));
#else
	mips_write_c0_register (C0_STATUS, x);
#endif
}

/*
 * Enable hardware interrupts.
 */
static void inline __attribute__ ((always_inline))
mips_intr_enable ()
{
#if defined (ELVEES) || defined (MIPS32_4KEC)
	int status;

	status = mips_read_c0_register (C0_STATUS);
	mips_write_c0_register (C0_STATUS, status | ST_IE);
#else
	asm volatile ("ei");
#endif
}

/*
 * Count a number of leading (most significant) zero bits in a word.
 */
static int inline __attribute__ ((always_inline))
mips_count_leading_zeroes (unsigned x)
{
	int n;

	asm volatile (
	"	.set	mips32 \n"
	"	clz	%0, %1"
	: "=r" (n) : "r" (x));
	return n;
}

/*
 * Translate virtual address to physical one.
 * Only for fixed mapping.
 */
static unsigned int inline
mips_virtual_addr_to_physical (unsigned int virt)
{
	unsigned segment_desc = virt >> 28;
	if (segment_desc <= 0x7) {
		// kuseg
		if (mips_read_c0_register(C0_STATUS) & ST_ERL) {
			// ERL == 1, no mapping
			return virt;
		} else {
			// Assume fixed-mapped
			return (virt + 0x40000000);
		}
	} else {
		// kseg0, or kseg1, or kseg2, or kseg3
		if (segment_desc <= 0xb) {
			// kseg0 или kseg1, cut bits A[31:29].
			return (virt & 0x1fffffff);
		} else {
			// Fixed-mapped - no mapping
			return virt;
		}
	}
}

#endif /* __ASSEMBLER__ */
