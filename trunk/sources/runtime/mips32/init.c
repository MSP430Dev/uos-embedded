#include <runtime/lib.h>
/*#include <kernel/uos.h>*/
/*#include <kernel/internal.h>*/

extern unsigned long _etext, __data_start, _edata, _end;

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from startup.S.
 * Attribute "naked" skips function prologue.
 */
void __attribute ((noreturn))_init_ (void)
{
	unsigned long *src, *dest, *limit;

#ifdef ELVEES_MC24

	/* Clear CAUSE register. */
	mips32_write_c0_register (C0_CAUSE, 0);

	/* Initialize STATUS register: CP0 usable, interrupts disabled,
	 * master interrupt disable, ROM vectors used. */
	mips32_write_c0_register (C0_STATUS, 0x10000000 | ST_BEV);

	/* Disable cache for kseg0 segment. */
	mips32_write_c0_register (C0_CONFIG, 2);

	/* Clear COMPARE register. */
	mips32_write_c0_register (C0_COMPARE, 0);

#endif /* ELVEES_MC24 */

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

	for (;;) main ();
}

#if 0
void __gccmain (void)
{
	/* Implicitly called just after entering main(). */
}
#endif

static void dump_of_death (unsigned int context[])
{
	debug_printf ("                t0 = %8x   s0 = %8x   t8 = %8x   lo = %8x\n",
		context [CONTEXT_R8], context [CONTEXT_R16],
		context [CONTEXT_R24], context [CONTEXT_LO]);
	debug_printf ("at = %8x   t1 = %8x   s1 = %8x   t9 = %8x   hi = %8x\n",
		context [CONTEXT_R1], context [CONTEXT_R9], context [CONTEXT_R17],
		context [CONTEXT_R25], context [CONTEXT_HI]);
	debug_printf ("v0 = %8x   t2 = %8x   s2 = %8x               status = %8x\n",
		context [CONTEXT_R2], context [CONTEXT_R10],
		context [CONTEXT_R18], context [CONTEXT_STATUS]);
	debug_printf ("v1 = %8x   t3 = %8x   s3 = %8x                  epc = %8x\n",
		context [CONTEXT_R3], context [CONTEXT_R11],
		context [CONTEXT_R19], context [CONTEXT_PC]);
	debug_printf ("a0 = %8x   t4 = %8x   s4 = %8x   gp = %8x\n",
		context [CONTEXT_R4], context [CONTEXT_R12],
		context [CONTEXT_R20], context [CONTEXT_GP]);
	debug_printf ("a1 = %8x   t5 = %8x   s5 = %8x   sp = %8x\n",
		context [CONTEXT_R5], context [CONTEXT_R13],
		context [CONTEXT_R21], context + CONTEXT_WORDS);
	debug_printf ("a2 = %8x   t6 = %8x   s6 = %8x   fp = %8x\n",
		context [CONTEXT_R6], context [CONTEXT_R14],
		context [CONTEXT_R22], context [CONTEXT_FP]);
	debug_printf ("a3 = %8x   t7 = %8x   s7 = %8x   ra = %8x\n",
		context [CONTEXT_R7], context [CONTEXT_R15],
		context [CONTEXT_R23], context [CONTEXT_RA]);

	debug_printf ("\nHalt...\n\n");
	asm volatile ("1: j 1b");
}

void _exception_handler_ (unsigned int context[])
{
	unsigned int cause, badvaddr, config, prid;

	debug_printf ("\n\n*** 0x%08x: exception\n", context [CONTEXT_PC]);

	cause = mips32_read_c0_register (C0_CAUSE);
	badvaddr = mips32_read_c0_register (C0_BADVADDR);
	config = mips32_read_c0_register (C0_CONFIG);
	prid = mips32_read_c0_register (C0_PRID);
	debug_printf ("*** cause=0x%08x, badvaddr=0x%08x, config=0x%08x, prid=0x%08x\n",
		cause, badvaddr, config, prid);

	dump_of_death (context);
}

void _pagefault_handler_ (unsigned int context[])
{
	unsigned int cause, badvaddr, config, prid;

	debug_printf ("\n\n*** 0x%08x: page fault\n", context [CONTEXT_PC]);

	cause = mips32_read_c0_register (C0_CAUSE);
	badvaddr = mips32_read_c0_register (C0_BADVADDR);
	config = mips32_read_c0_register (C0_CONFIG);
	prid = mips32_read_c0_register (C0_PRID);
	debug_printf ("*** cause=0x%08x, badvaddr=0x%08x, config=0x%08x, prid=0x%08x\n",
		cause, badvaddr, config, prid);

	dump_of_death (context);
}
