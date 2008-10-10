#include <runtime/lib.h>

extern unsigned long _etext, __data_start, _edata, _end;
extern void main (void);

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from gcrt1.S.
 * Attribute "naked" skips function prologue.
 */
void __attribute ((naked))
_init_ (void)
{
	unsigned long *src, *dest, *limit;

#ifdef ARM_S3C4530
	unsigned long syscfg;

	/* Set special register base address to 0x03ff0000. */
	syscfg = ARM_SYSCFG_SRBBP_MASK;

	/* Set internal SRAM base address. */
	syscfg |= ARM_SRAM_BASE >> 10;

	/* Cache mode - 4-kbyte SRAM, 4-kbyte cache. */
	syscfg |= ARM_SYSCFG_CM_4R_4C;

	/* Enable write buffer. */
	syscfg |= ARM_SYSCFG_WE;

	/* Disable round-robin for DMA-channels. */
	syscfg |= ARM_SYSCFG_FP;

	ARM_SYSCFG = syscfg;

#ifndef EMULATOR /* not needed on emulator */
	/* Invalidate the entire cache.
	 * Clear 1-kbyte tag memory. */
	dest = (unsigned long*) ARM_CACHE_TAG_ADDR;
	limit = dest + 1024/4;
	while (dest < limit)
		*dest++ = 0;
#endif
	/* Enable the cache */
	syscfg |= ARM_SYSCFG_CE;
	ARM_SYSCFG = syscfg;
#endif /* ARM_S3C4530 */

#ifndef EMULATOR /* not needed on emulator */
	/* Copy the .data image from flash to ram.
	 * Linker places it at the end of .text segment. */
	src = &_etext;
	dest = &__data_start;
	limit = &_edata;
	while (dest < limit)
		*dest++ = *src++;
#endif

	/* Initialize .bss segment by zeroes. */
	dest = &_edata;
	limit = &_end;
	while (dest < limit)
		*dest++ = 0;

	/* Set stack to end of internal SRAM. */
	arm_set_stack_pointer ((void*) (ARM_SRAM_BASE + ARM_SRAM_SIZE));

	/*
	 * Global interrupt enable.
	 */
#ifdef ARM_S3C4530
	ARM_INTMSK = 0x1fffff;
#endif
#ifdef ARM_AT91SAM
	/* TODO */
#endif

	main ();
}

/*
 * Check memory address.
 * Board-dependent function, should be replaced by user.
 */
bool_t __attribute ((weak))
uos_valid_memory_address (void *ptr)
{
	unsigned u = (unsigned) ptr;

#ifdef ARM_S3C4530
	if (u >= ARM_SRAM_BASE && u < ARM_SRAM_BASE + 0x1000)
		return 1;
	return 0;
#else
	return 1;
#endif
}

#ifdef ARM_S3C4530
void __attribute ((weak))
watchdog_alive ()
{
	/* This routine should be supplied by user.
	 * Implementation of watchdog is different on different boards. */
}
#endif

unsigned long _dump_stack_ [13];

static void dump_of_death (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("r0 = %8x     r4 = %8x     r8  = %8x     r12  = %8x\n",
		_dump_stack_[0], _dump_stack_[4], _dump_stack_[8], _dump_stack_[12]);
	debug_printf ("r1 = %8x     r5 = %8x     r9  = %8x     cpsr = %8x\n",
		_dump_stack_[1], _dump_stack_[5], _dump_stack_[9], cpsr);
	debug_printf ("r2 = %8x     r6 = %8x     r10 = %8x     lr   = %8x\n",
		_dump_stack_[2], _dump_stack_[6], _dump_stack_[10], lr);
	debug_printf ("r3 = %8x     r7 = %8x     r11 = %8x     pc   = %8x\n",
		_dump_stack_[3], _dump_stack_[7], _dump_stack_[11], pc);
	debug_printf ("\nReset...\n\n");
	asm volatile ("ldr r0, =0 \n bx r0");
}

void _undef_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("\n\n*** 0x%08x: undefined instruction\n\n", pc);
	dump_of_death (pc, cpsr, lr);
}

void _swi_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("\n\n*** 0x%08x: unexpected software interrupt\n\n", pc);
	dump_of_death (pc, cpsr, lr);
}

void _prefetch_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("\n\n*** 0x%08x: instruction prefetch exception\n\n", pc);
	dump_of_death (pc, cpsr, lr);
}

void _fiq_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("\n\n*** 0x%08x: unexpected fast interrupt\n\n", pc);
	dump_of_death (pc, cpsr, lr);
}

void _abort_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr)
{
	debug_printf ("\n\n*** 0x%08x: data access exception\n", pc);
	dump_of_death (pc, cpsr, lr);
}
