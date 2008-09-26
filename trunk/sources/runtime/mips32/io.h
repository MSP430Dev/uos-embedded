/*
 * Hardware register defines for MIPS32 architecture.
 */
#ifdef ELVEES_MC24
#   include <runtime/mips32/io-mc24.h>
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

#ifndef __ASSEMBLER__

/*
 * Set value of stack pointer register.
 */
static void inline __attribute__ ((always_inline))
mips32_set_stack_pointer (void *x)
{
	asm volatile (
	"move	$sp, %0"
	: : "r" (x));
}

/*
 * Get value of stack pointer register.
 */
static inline __attribute__ ((always_inline))
void *mips32_get_stack_pointer ()
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
#define mips32_read_c0_register(reg)				\
({ int __value;							\
	asm volatile (						\
	"mfc0	%0, $%1"					\
	: "=r" (__value) : "K" (reg));				\
	__value;						\
})

/*
 * Write C0 coprocessor register.
 */
#define mips32_write_c0_register(reg, value)			\
do {								\
	asm volatile (						\
	"mtc0	%z0, $%1 \n	nop \n	nop \n	nop"		\
	: : "r" ((unsigned int) (value)), "K" (reg));		\
} while (0)

/*
 * Disable the hardware interrupts.
 * saving the interrupt state into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_disable (int *x)
{
	int status;

	status = mips32_read_c0_register (C0_STATUS);
	*x = status;
	mips32_write_c0_register (C0_STATUS, status & ~ST_IE);
}

/*
 * Restore the hardware interrupt mode using the saved interrupt state.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_restore (int x)
{
	mips32_write_c0_register (C0_STATUS, x);
}

/*
 * Enable hardware interrupts.
 */
static void inline __attribute__ ((always_inline))
mips32_intr_enable ()
{
	int status;

	status = mips32_read_c0_register (C0_STATUS);
	mips32_write_c0_register (C0_STATUS, status | ST_IE);
}

/*
 * Count a number of leading (most significant) zero bits in a word.
 */
static int inline __attribute__ ((always_inline))
mips32_count_leading_zeroes (unsigned x)
{
	int n;

	asm volatile (
	"	.set	mips32 \n"
	"	clz	%0, %1"
	: "=r" (n) : "r" (x));
	return n;
}

#endif /* __ASSEMBLER__ */
