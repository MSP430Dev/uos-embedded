/*
 * Hardware register defines for MIPS32 architecture.
 */
#ifdef ELVEES_MC24
#   include <runtime/mips32/io-mc24.h>
#endif

#ifndef __ASSEMBLER__

/*
 * Set value of stack pointer register.
 */
static void inline __attribute__ ((always_inline))
mips32_set_stack_pointer (int x)
{
	asm volatile (
	"mov	sp, %0"
	: : "r" (x));
}

/*
 * Get value of stack pointer register.
 */
static int inline __attribute__ ((always_inline))
mips32_get_stack_pointer ()
{
	int x;

	asm volatile (
	"mov	%0, sp"
	: "=r" (x));
	return x;
}

/*
 * Disable the hardware interrupts.
 * saving the interrupt state into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_disable (int_t *x)
{
	int_t temp;

	asm volatile (
"	mrs	%1, cpsr \n"
"	orr	%0, %1, #0x80 \n"
"	msr	cpsr, %0 \n"
	: "=r" (temp), "=r" (*(x)) : : "memory", "cc");
}

/*
 * Restore the hardware interrupt mode using the saved interrupt state.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_restore (int_t x)
{
	int_t temp;

	asm volatile (
	"msr	cpsr, %1"
	: "=r" (temp) : "r" (x) : "memory", "cc");
}

/*
 * Enable hardware interrupts.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_enable ()
{
	int_t temp;

	asm volatile (
	"mrs	%0, cpsr \n"
"	bic	%0, %0, #0x80 \n"
"	msr	cpsr, %0"
	: "=r" (temp) : : "memory", "cc");
}

#endif /* __ASSEMBLER__ */
