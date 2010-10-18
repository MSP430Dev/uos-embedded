#ifdef ARM_1986BE9
#	include <runtime/cortex-m3/io-1986ve9x.h>
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

/*
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
arm_intr_disable (int *x)
{
	int temp;

	asm volatile (
#if 0
	"cpsid	i \n"
"	mrs	%1, basepri \n"		/* Cortex-M3 mode */
"	mov	%0, #32 \n"		/* basepri := 16 */
"	msr	basepri, %0 \n"
"	nop \n"
"	nop \n"
"	nop \n"
"	cpsie	i"
#else
	"mrs	%1, basepri \n"		/* Cortex-M3 mode */
"	mov	%0, #32 \n"		/* basepri := 16 */
"	msr	basepri, %0"
#endif
	: "=r" (temp), "=r" (*(x)) : : "memory", "cc");
}

static void inline __attribute__ ((always_inline))
arm_intr_restore (int x)
{
	asm volatile (
	"msr	basepri, %0"		/* Cortex-M3 mode */
	: : "r" (x) : "memory", "cc");
}

static void inline __attribute__ ((always_inline))
arm_intr_enable ()
{
	asm volatile (
	"msr	basepri, %0"		/* Cortex-M3 mode */
	: : "r" (0) : "memory", "cc");
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

static void inline __attribute__ ((always_inline))
arm_set_basepri (unsigned val)
{
	asm volatile (
	"msr	basepri, %0"
	: : "r" (val) : "memory", "cc");
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

	asm volatile (
	"clz	%0, %1"
	: "=r" (n) : "r" (x));
	return n;
}
