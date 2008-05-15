#ifdef ARM_S3C4530
#	include <runtime/arm/io-s3c4530.h>
#endif
#ifdef ARM_NET50
#	include <runtime/arm/io-net50.h>
#endif
#ifdef ARM_AT91SAM
#include <runtime/arm/io-at91sam.h>
#endif

/*
 * Handle stack pointer register.
 */
static void inline __attribute__ ((always_inline))
arm_set_stack_pointer (int x)
{
	asm volatile (
	"mov	sp, %0"
	: : "r" (x));
}

/* Save current task stack. */
static int inline __attribute__ ((always_inline))
arm_get_stack_pointer ()
{
	int x;

	asm volatile (
	"mov	%0, sp"
	: "=r" (x));
	return x;
}

/*
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
arm_intr_disable (int_t *x)
{
	int_t temp;

	asm volatile (
#if __thumb__
	".balignw 4, 0x46c0 \n"
"	bx	pc \n"			/* switch to ARM mode */
"	nop \n"
"	.code	32 \n\t"
#endif

"	mrs	%1, cpsr \n"
"	orr	%0, %1, #0x80 \n"
"	msr	cpsr, %0 \n"

#if defined (ARM_CLI_PARANOIA)
"	msr	cpsr, %0 \n"
#endif

#if __thumb__
"	ldr	%0, [pc] \n"		/* switch back to Thumb mode */
"	bx	%0 \n"
"	.word	.+5"
#endif
	: "=r" (temp), "=r" (*(x)) : : "memory", "cc");
}

static void inline __attribute__ ((always_inline))
arm_intr_restore (int_t x)
{
	int_t temp;

	asm volatile (
#if __thumb__
	".balignw 4, 0x46c0 \n"
"	bx	pc \n"			/* switch to ARM mode */
"	nop \n"
"	.code	32 \n\t"
#endif
	"msr	cpsr, %1"
#if __thumb__
"\n	ldr	%0, [pc] \n"		/* switch back to Thumb mode */
"	bx	%0 \n"
"	.word	.+5"
#endif
	: "=r" (temp) : "r" (x) : "memory", "cc");
}

static void inline __attribute__ ((always_inline))
arm_intr_enable ()
{
	int_t temp;

	asm volatile (
#if __thumb__
	".balignw 4, 0x46c0 \n"
"	bx	pc \n"			/* switch to ARM mode */
"	nop \n"
"	.code	32 \n\t"
#endif
	"mrs	%0, cpsr \n"
"	bic	%0, %0, #0x80 \n"
"	msr	cpsr, %0"
#if __thumb__
"\n	ldr	%0, [pc] \n"		/* switch back to Thumb mode */
"	bx	%0 \n"
"	.word	.+5"
#endif
	: "=r" (temp) : : "memory", "cc");
}

inline extern void console_enable_receiver ()
{
}

#ifdef __thumb__
#	define arm_bus_yield()				\
	do {						\
		unsigned dummy = 0xDEADBEEFul;		\
		__asm__ __volatile (			\
			"mul %0, %0, %0 \n\r"		\
			"mul %0, %0, %0 \n\r"		\
			"mul %0, %0, %0 \n\r"		\
			: "+r" (dummy) : : "cc");	\
	} while (0)
#else
#	define arm_bus_yield()				\
	do {						\
		unsigned dummy = 0xDEADBEEFul;		\
		__asm__ __volatile (			\
			"mla %0, %1, %0, %0 \n\r"	\
			"mla %0, %1, %0, %0 \n\r"	\
			"mla %0, %1, %0, %0 \n\r"	\
			: "+&r" (dummy) : "r" (dummy) : "cc");\
	} while (0)
#endif
