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

#ifdef __MSP430_MPY32_BASE__
/*
 * Current version of compiler - GCC 3.2.3 - knows nothing about
 * 32-bit multiplier. We replace two run-time routines, to make
 * 32-bit and 64-bit multiplication to work correctly.
 */
void __attribute__ ((naked))
__umulsi3hw (void)
{
	/* 32-bit multiplication. */
	asm volatile ("mov r10, %0" : "=m" (MPY32L));
	asm volatile ("mov r11, %0" : "=m" (MPY32H));
	asm volatile ("mov r12, %0" : "=m" (OP2L));
	asm volatile ("mov r13, %0" : "=m" (OP2H));
	asm volatile ("mov %0, r14" : : "m" (RES0));
	asm volatile ("mov %0, r15" : : "m" (RES1));
	asm volatile ("ret");
}

#if 0
void __attribute__ ((naked))
__muldi3 (void)
{
	/* 64-bit multiplication:
	 * ARG1*ARG2 = (ARG1L*ARG2L) + (ARG1L*ARG2H)<<32 + (ARG1H*ARG2L)<<32
	 */
	asm volatile ("push r2");				// save interrupt state
	asm volatile ("dint");					// set interrupt (the compiler did not itself)

	asm volatile ("mov 4+0(r1), %0"	: "=m" (MPY32L));	// 4 is the stack offset for ARG2 LSB (R8 and retadr)
	asm volatile ("mov 4+2(r1), %0"	: "=m" (MPY32H));
	asm volatile ("mov r12, %0"	: "=m" (OP2L));		// ARG2L * ARG1L
	asm volatile ("mov r13, %0"	: "=m" (OP2H));

	asm volatile ("push %0"		: : "m" (RES0));	// LSB result to stack
	asm volatile ("push %0"		: : "m" (RES1));

	asm volatile ("mov %1, %0"	: "=m" (RES0) : "m" (RES2));	// bit 32..63 'overflow' into bit 0..31 for nextoperations
	asm volatile ("mov %1, %0"	: "=m" (RES1) : "m" (RES3));

	asm volatile ("mov 8+0(r1), %0"	: "=m" (MAC32L));	// + ARG2L * ARG1H operand 2 has moved by 4 bytes (the 2 push above)
	asm volatile ("mov 8+2(r1), %0"	: "=m" (MAC32H));
	asm volatile ("mov r14, %0"	: "=m" (OP2L));
	asm volatile ("mov r15, %0"	: "=m" (OP2H));

	asm volatile ("mov 8+4(r1), %0"	: "=m" (MAC32L));	// + ARG2H * ARG1L
	asm volatile ("mov 8+6(r1), %0"	: "=m" (MAC32H));
	asm volatile ("mov r12, %0"	: "=m" (OP2L));
	asm volatile ("mov r13, %0"	: "=m" (OP2H));		// ARG1H*ARG2H is >64 bit, so skip this step

	asm volatile ("mov %0, r14"	: : "m" (RES0));	// LSBs as MSB result to result registers
	asm volatile ("mov %0, r15"	: : "m" (RES1));

	asm volatile ("pop r13");				// LSB result form stack
	asm volatile ("pop r12");
	asm volatile ("pop r2");				// restore interrupt state
	asm volatile ("ret");
}
#endif
#endif
