/*
 * CPU-dependent inline routines for Cortex-M3 architecture.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *               2013 Dmitry Podkhvatilin <vatilin@gmail.com>
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
#ifdef ARM_1986BE9
#   include <runtime/cortex-m/io-1986ve9x.h>
#endif
#ifdef ARM_1986BE1
#   include <runtime/cortex-m/io-1986ve1.h>
#endif
#ifdef ARM_STM32F4
#   include <runtime/cortex-m/io-stm32f4.h>
#endif

/*
 * Handle stack pointer register.
 */
static inline __attribute__ ((always_inline))
void arm_set_stack_pointer (void *x)
{
	asm volatile (
	"mov	sp, %0"
	: : "r" (x));
}

static inline __attribute__ ((always_inline))
void *arm_get_stack_pointer ()
{
	void *x;

	asm volatile (
	"mov	%0, sp"
	: "=r" (x));
	return x;
}

/*
 * Handle any register R0-R14.
 */
static inline __attribute__ ((always_inline))
void arm_set_register (int reg, unsigned val)
{
	asm volatile (
	"mov	r%c1, %0"
	: : "r" (val), "i" (reg));
}

static inline __attribute__ ((always_inline))
unsigned arm_get_register (int reg)
{
	unsigned val;

	asm volatile (
	"mov	%0, r%c1"
	: "=r" (val) : "i" (reg));
	return val;
}

#ifdef ARM_CORTEX_M1
/*
 * Read PRIMASK register.
 */
static inline __attribute__ ((always_inline))
unsigned arm_get_primask ()
{
	unsigned x;

	asm volatile (
	"mrs	%0, primask"
	: "=r" (x));
	return x;
}

/*
 * Set PRIMASK register.
 */
static void inline __attribute__ ((always_inline))
arm_set_primask (unsigned val)
{
	asm volatile (
	"msr	primask, %0"
	: : "r" (val) : "memory", "cc");
}
#else
/*
 * Read BASEPRI register.
 */
static inline __attribute__ ((always_inline))
unsigned arm_get_basepri ()
{
	unsigned x;

	asm volatile (
	"mrs	%0, basepri"
	: "=r" (x));
	return x;
}

/*
 * Set BASEPRI register.
 */
static void inline __attribute__ ((always_inline))
arm_set_basepri (unsigned val)
{
	asm volatile (
	"msr	basepri, %0"
	: : "r" (val) : "memory", "cc");
}
#endif

/*
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
arm_intr_disable (unsigned long *x)
{
#ifdef ARM_CORTEX_M1
	register int primask = arm_get_primask();
	arm_set_primask(1);
    *x = ARM_NVIC_ISER(0);
    ARM_NVIC_ICER(0) = 0xFFFFFFFF;
	arm_set_primask(primask);
#else
	unsigned long temp;
	
	asm volatile (
	"mrs	%1, basepri \n"		/* Cortex-M3 mode */
	"mov	%0, #64 \n"		/* basepri := 64 */
	"msr	basepri, %0"
	: "=r" (temp), "=r" (*(x)) : : "memory", "cc");
#endif
}

static void inline __attribute__ ((always_inline))
arm_intr_restore (unsigned long x)
{
#ifdef ARM_CORTEX_M1
    ARM_NVIC_ISER(0) = x;
#else
	asm volatile (
	"msr	basepri, %0"		/* Cortex-M3 mode */
	: : "r" (x) : "memory", "cc");
#endif
}

static void inline __attribute__ ((always_inline))
arm_intr_enable ()
{
#ifdef ARM_CORTEX_M1
	extern unsigned __cortex_m1_iser0;
	ARM_NVIC_ISER(0) = __cortex_m1_iser0;
	/*
	asm volatile (
	"msr	primask, %0"
	: : "r" (0) : "memory", "cc");
	*/
#else
	asm volatile (
	"msr	basepri, %0"
	: : "r" (0) : "memory", "cc");
#endif
}

static void inline __attribute__ ((always_inline))
arm_bus_yield ()
{
	asm volatile (
	"wfi"
	: : : "memory", "cc");
}

/*
 * Read IPSR register.
 */
static inline __attribute__ ((always_inline))
unsigned arm_get_ipsr ()
{
	unsigned x;

	asm volatile (
	"mrs	%0, ipsr"
	: "=r" (x));
	return x;
}

static void inline __attribute__ ((always_inline))
arm_set_control (unsigned val)
{
	asm volatile (
	"msr	control, %0"
	: : "r" (val) : "memory", "cc");
}

/*
 * Count a number of leading (most significant) zero bits in a word.
 */
static int inline __attribute__ ((always_inline))
arm_count_leading_zeroes (unsigned x)
{
	int n;

#ifdef ARM_CORTEX_M1
	for (n = 0; n < 32; ++n)
		if ((x >> (31 - n)) != 0) break;
#else
	asm volatile (
	"clz	%0, %1"
	: "=r" (n) : "r" (x));
#endif

	return n;
}
