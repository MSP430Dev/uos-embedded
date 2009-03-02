#include <runtime/lib.h>

extern unsigned long _etext, __data_start, _edata, _end, __stack;
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

	msp430_set_stack_pointer (&__stack);

	/*
	 * Enable the crystal on XT1 and use it as MCLK.
	 */
	WDTCTL = WDTPW | WDTHOLD;		/* stop watchdog timer */
	BCSCTL1 |= XTS;				/* XT1 as high-frequency */
	_BIC_SR (OSCOFF);			/* turn on XT1 oscillator */
	do {					/* wait in loop until crystal is stable */
		IFG1 &= ~OFIFG;
	} while (IFG1 & OFIFG);

	BCSCTL1 |= DIVA0;			/* ACLK = XT1 / 2 */
	BCSCTL1 &= ~DIVA1;
	IE1 &= ~WDTIE;				/* disable WDT int. */
	IFG1 &= ~WDTIFG;			/* clear WDT int. flag */

	/* Use WDT as timer, flag each 512 pulses from ACLK. */
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS1;
	while (! (IFG1 & WDTIFG))		/* count 1024 pulses from XT1 (until XT1's */
		continue;			/* amplitude is OK) */
	IFG1 &= ~OFIFG;				/* clear osc. fault int. flag */
	BCSCTL2 = SELM0 | SELM1;		/* set XT1 as MCLK */

	/*
	 * Enable USART0 module.
	 */
#ifdef __MSP430_HAS_UART0__
	P3SEL |= BIT4 | BIT5;			/* enable UART0 */
	P3DIR |= BIT4;				/* enable TXD0 as output */
	P3DIR &= ~BIT5;				/* enable RXD0 as input */

	U0ME |= UTXE0 + URXE0;
	UCTL0 = SWRST;				/* reset the USART */
	UCTL0 = CHAR;				/* set the 8-bit byte, 1 stop bit, no parity */
	UTCTL0 = SSEL_ACLK | TXEPT;		/* select ACLK for baudrate generator clock */

	UBR00 = KHZ * 500L / 115200;
	UBR10 = (int) (KHZ * 500L / 115200) >> 8;
#  if KHZ == 8000
	UMCTL0 = 0xDD;				/* optimal for 115200 and ACLK=4 MHz */
#  else
	UMCTL0 = 0;
#  endif
	URCTL0 = 0;				/* init receiver control register */
#endif

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

	for (;;)
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

	if (u >= 0x200 && u < 0xA00)
		return 1;
	return 0;
}

#if 0
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
#endif

#if 0
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
	asm volatile ("1: j 1b; nop");
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
#endif
