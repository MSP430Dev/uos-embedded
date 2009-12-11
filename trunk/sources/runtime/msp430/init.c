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
	 * Stop watchdog timer.
	 */
	WDTCTL = WDTPW | WDTHOLD;

	/*
	 * Enable the crystal on XT1 and use it as MCLK.
	 */
#ifdef USE_XT1_HIGH
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
#endif /* BCSCTL1_ */

	/*
	 * Setup the internal oscillator.
	 */
#ifdef USE_DCO
#ifdef __MSP430_HAS_UCS__
	UCSCTL3 = SELREF__REFOCLK;	/* Set DCO FLL reference = REFO */
	UCSCTL4 = (UCSCTL4 & ~SELA_7) |
		SELA__REFOCLK;		/* Set ACLK to REFO */
	__bis_SR_register (SCG0);	/* Disable the FLL control loop */
	UCSCTL0 = 0;			/* Set lowest possible DCOx, MODx */
#if KHZ < 700
	#error Too low KHZ, must be >=700
#elif KHZ < 1400
	UCSCTL1 = DCORSEL_3;		/* Select range 0.64-1.51 MHz */
#elif KHZ < 2800
	UCSCTL1 = DCORSEL_4;		/* Select range 1.3-3.2 MHz */
#elif KHZ < 5300
	UCSCTL1 = DCORSEL_5;		/* Select range 2.5-6.0 MHz */
#elif KHZ < 9600
	UCSCTL1 = DCORSEL_6;		/* Select range 4.6-10.7 MHz */
#elif KHZ < 19600
	UCSCTL1 = DCORSEL_7;		/* Select range 8.5-19.6 MHz */
#else
	#error Too high KHZ, must be <19600
#endif
	UCSCTL2 = (KHZ*1000L - 16384) / 32768;	/* Set DCO Multiplier for 8MHz */
					/* (N + 1) * FLLRef = Fdco */
	__bic_SR_register (SCG0);	/* Enable the FLL control loop */
	mdelay (30);			/* Time to settle: 1024 REFO cycles */

	/* Loop until XT1,XT2 & DCO fault flag is cleared */
	do {
		/* Clear XT2, XT1, DCO fault flags */
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;	/* Clear fault flags */
	} while (SFRIFG1 & OFIFG);	/* Test oscillator fault flag */

	UCSCTL4 = (UCSCTL4 & ~(SELS_7 | SELM_7)) |
		SELS__DCOCLK | SELM__DCOCLK; /* Set MCLK, SMCLK to DCO */
#else
#if KHZ == 16000
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;
#elif KHZ == 12000
	BCSCTL1 = CALBC1_12MHZ;
	DCOCTL = CALDCO_12MHZ;
#elif KHZ == 8000
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
#elif KHZ == 1000
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
#else
	#error Invalid KHZ, must be 16000, 12000, 8000 or 1000.
#endif
#endif
#endif

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
	UTCTL0 = SSEL_SMCLK | TXEPT;		/* select SMCLK for baudrate generator clock */

	UBR00 = KHZ * 1000L / 115200;
	UBR10 = (int) (KHZ * 1000L / 115200) >> 8;
#  if KHZ == 8000
	UMCTL0 = 0xDD;				/* optimal for 115200 and CLK=4 MHz */
#  else
	UMCTL0 = 0;
#  endif
	URCTL0 = 0;				/* init receiver control register */
#endif

#ifdef MSP430_DEBUG_USCIA0
	/* Use USCI_A0 for debug output. */
	P3SEL = BIT4 | BIT5;			/* P3.4, P3.5 = USCI_A0 TXD/RXD */
	UCA0CTL1 = UCSWRST;			/* Reset */
	UCA0CTL0 = 0;				/* Async 8N1 */
	UCA0CTL1 |= UCSSEL_SMCLK;		/* Clock source SMCLK */
	UCA0BR0 = KHZ * 1000L / 115200;
	UCA0BR1 = (int) (KHZ * 1000L / 115200) >> 8;
#if KHZ == 16000
	UCA0MCTL = UCBRS_7;
#elif KHZ == 12000
	UCA0MCTL = UCBRS_1;
#elif KHZ == 8000
	UCA0MCTL = UCBRS_4;
#elif KHZ == 1000
	UCA0MCTL = UCBRS_0;
#else
	#error Invalid KHZ, must be 16000, 12000, 8000 or 1000.
#endif
	UCA0CTL1 &= ~UCSWRST;			/* Clear reset */
	/* IE2 |= UCA0RXIE; */			/* Enable USCI_A0 RX interrupt */
#endif

#ifdef MSP430_DEBUG_USCIA3
	/* Use USCI_A3 for debug output. */
	P10SEL = BIT4 | BIT5;			/* P10.4, P10.5 = USCI_A3 TXD/RXD */
	UCA3CTL1 = UCSWRST |			/* Reset */
		UCSSEL_SMCLK;			/* Clock source SMCLK */
	UCA3CTL0 = 0;				/* Async 8N1 */
	UCA3BR0 = KHZ * 1000L / 115200;
	UCA3BR1 = (int) (KHZ * 1000L / 115200) >> 8;
#if KHZ == 16000
	UCA3MCTL = UCBRS_7;
#elif KHZ == 12000
	UCA3MCTL = UCBRS_1;
#elif KHZ == 12288
	UCA3MCTL = UCBRS_5;
#elif KHZ == 6144 || KHZ == 15124
	UCA3MCTL = UCBRS_2;
#elif KHZ == 8000
	UCA3MCTL = UCBRS_4;
#elif KHZ == 1000 || KHZ*10 % 1152 == 0 || KHZ == 14043
	UCA3MCTL = UCBRS_0;
#else
	#error Invalid KHZ for debug UART.
#endif
	UCA3CTL1 &= ~UCSWRST;			/* Clear reset */
	/* UCA3IE |= UCRXIE; */			/* Enable USCI_A3 RX interrupt */
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

	if (u >= (unsigned) &__data_start && u < (unsigned) &_etext)
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
