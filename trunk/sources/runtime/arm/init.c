#include <runtime/lib.h>

extern unsigned long _etext, __data_start, _edata, _end;
extern void main (void);

void __attribute ((weak)) _irq_handler_ () {};
void _undef_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr);
void _swi_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr);
void _prefetch_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr);
void _fiq_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr);
void _abort_handler_ (unsigned long pc, unsigned long cpsr, unsigned long lr);


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

#if defined (ARM_AT91SAM) && !defined (AT91BOOTSTRAP)
	/* Enable RESET. */
	*AT91C_RSTC_RMR = 0xA5000000 |
		(AT91C_RSTC_ERSTL & (4 << 8)) | AT91C_RSTC_URSTEN;

#ifdef AT91C_MC_FMR
	/* Flash mode register: set 1 flash wait state and
	 * a number of master clock cycles in 1.5 microseconds. */
	*AT91C_MC_FMR = AT91C_MC_FWS_1FWS |
		(AT91C_MC_FMCN & (((KHZ * 3 + 1000) / 2000) << 16));
#endif
	/* Disable watchdog. */
	*AT91C_WDTC_WDMR = AT91C_WDTC_WDDIS;

	/* Main oscillator register: enabling the main oscillator.
	 * Slow clock is 32768 Hz, or 30.51 usec.
	 * Start up time = OSCOUNT * 8 / SCK = 1,46 msec.
	 * Worst case is 15ms, so OSCOUNT = 15 * SCK / 8000 = 61. */
	*AT91C_PMC_MOR = AT91C_CKGR_MOSCEN |
		(AT91C_CKGR_OSCOUNT & (61 << 8));
	while (! (*AT91C_PMC_SR & AT91C_PMC_MOSCS))
		continue;

	/* PLL register: set multiplier and divider. */
#ifdef AT91C_PMC_PLLR
	/* SAM7 development board: we have quartz 18.432 MHz.
	 * After multiplying by (25+1) and dividing by 5
	 * we have MCK = 95.8464 MHz.
	 * PLL startup time estimated at 0.844 msec. */
	*AT91C_PMC_PLLR = (AT91C_CKGR_DIV & 0x05) |
		(AT91C_CKGR_PLLCOUNT & (28 << 8)) |
		(AT91C_CKGR_MUL & (25 << 16));
	while (! (*AT91C_PMC_SR & AT91C_PMC_LOCK))
		continue;
#else
	/* SAM9 development board: quartz 12 MHz.
	 * Set PLLA to 200 MHz = 12 MHz * (99+1) / 6.
	 * PLL startup time is 63 slow clocks, or about 2ms. */
	*AT91C_PMC_PLLAR = AT91C_CKGR_SRCA |
		(AT91C_CKGR_MULA & (99 << 16)) |
		AT91C_CKGR_OUTA_2 |
		(AT91C_CKGR_PLLACOUNT & (63 << 8)) |
		(AT91C_CKGR_DIVA & 6);
	while (! (*AT91C_PMC_SR & AT91C_PMC_LOCKA))
		continue;
#endif
	while (! (*AT91C_PMC_SR & AT91C_PMC_MCKRDY))
		continue;

#ifdef ARM_AT91SAM7X256
	/* Master clock register: selection of processor clock.
	 * Use PLL clock divided by 2. */
	*AT91C_PMC_MCKR = AT91C_PMC_PRES_CLK_2;
#endif
#ifdef ARM_AT91SAM9260
	/* Set processor clock to PLLA=200MHz.
	 * For master clock MCK use PLL clock divided by 2. */
	*AT91C_PMC_MCKR = AT91C_PMC_PRES_CLK | AT91C_PMC_MDIV_2;
#endif
	while (! (*AT91C_PMC_SR & AT91C_PMC_MCKRDY))
		continue;

#ifdef AT91C_PMC_CSS_PLL_CLK
	*AT91C_PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
#else
	*AT91C_PMC_MCKR |= AT91C_PMC_CSS_PLLA_CLK;
#endif
	while (! (*AT91C_PMC_SR & AT91C_PMC_MCKRDY))
		continue;
#endif /* ARM_AT91SAM && !AT91BOOTSTRAP */

#ifdef ARM_OMAP44XX
	extern unsigned _undefined_ [];
	extern unsigned _swi_ [];
	extern unsigned _prefetch_ [];
	extern unsigned _abort_ [];
	extern unsigned _irq_ [];
	extern unsigned _fiq_ [];
	
	OMAP_UNDEF_EXCEPTION_VECT = (unsigned)_undefined_;
	OMAP_SWI_VECT             = (unsigned)_swi_;
	OMAP_PREFETCH_ABORT_VECT  = (unsigned)_prefetch_;
	OMAP_DATA_ABORT_VECT      = (unsigned)_abort_;
	OMAP_IRQ_VECT             = (unsigned)_irq_;
	OMAP_FIQ_VECT             = (unsigned)_fiq_;
	
	ARM_ICCPMR = 0xFF;

#if (KHZ_CLKIN==12000)
	CM_SYS_CLKSEL = 1;
#elif (KHZ_CLKIN==16800)
	CM_SYS_CLKSEL = 3;
#elif (KHZ_CLKIN==19200)
	CM_SYS_CLKSEL = 4;
#elif (KHZ_CLKIN==26000)
	CM_SYS_CLKSEL = 5;
#elif (KHZ_CLKIN==38400)
	CM_SYS_CLKSEL = 7;
#else
#error Bad KHZ_CLKIN in target.cfg
#endif
	
	/* DPLL_PER (OPP100) recommended settings */
	CM_CLKSEL_DPLL_PER = DPLL_DIV(1) | DPLL_MULT(40);
	CM_DIV_M2_DPLL_PER = DPLL_CLKx_DIV(8);
	CM_DIV_M3_DPLL_PER = DPLL_CLKx_DIV(6);
	CM_DIV_M4_DPLL_PER = DPLL_CLKx_DIV(12);
	CM_DIV_M5_DPLL_PER = DPLL_CLKx_DIV(9);
	CM_DIV_M6_DPLL_PER = DPLL_CLKx_DIV(4);
	CM_DIV_M7_DPLL_PER = DPLL_CLKx_DIV(5);
	
	/* DPLL_CORE (OPP100) recommended settings */
	CM_CLKSEL_DPLL_CORE = DPLL_DIV(5) | DPLL_MULT(125);
	CM_DIV_M2_DPLL_CORE = DPLL_CLKx_DIV(1);
	CM_DIV_M3_DPLL_CORE = DPLL_CLKx_DIV(5);
	CM_DIV_M4_DPLL_CORE = DPLL_CLKx_DIV(8);
	CM_DIV_M5_DPLL_CORE = DPLL_CLKx_DIV(4);
	CM_DIV_M6_DPLL_CORE = DPLL_CLKx_DIV(6);
	CM_DIV_M7_DPLL_CORE = DPLL_CLKx_DIV(6);
	
	/* DPLL_MPU (OPP100) recommended settings */
	CM_CLKSEL_DPLL_MPU = DPLL_DIV(7) | DPLL_MULT(125);
	CM_DIV_M2_DPLL_MPU = DPLL_CLKx_DIV(1);

#endif

#ifdef ARM_PANDABOARD
	/* Switch off watchdog timer */
	*((unsigned *) 0x4A314010) |= 2;
	/* Enable clocks for UART3 */
	CM_L4PER_UART3_CLKCTRL = MODULEMODE(2);
	/* Setting UART3 for debug output */
	UART_MDR1(3) = UART_MODE_DISABLE;
	/* Set 115200 kbps */
	UART_LCR(3) = UART_CONF_MODE_B;
	UART_DLL(3) = 0x1A;
	UART_DLH(3) = 0x00;
	/* Enable FIFOs */
	UART_EFR(3) |=  UART_EFR_ENHANCED_EN;
	UART_LCR(3)  =  UART_CONF_MODE_A;
	UART_MCR(3) |=  UART_MCR_TCR_TLR;
	UART_FCR(3) |=  UART_FCR_FIFO_EN;
	UART_MCR(3) &= ~UART_MCR_TCR_TLR;
	UART_LCR(3)  =  UART_CONF_MODE_B;
	UART_EFR(3) &= ~UART_EFR_ENHANCED_EN;
	/* Set frame format: 8 bit/char, no parity, 1 stop bit */
	UART_LCR(3) = UART_LCR_CHAR_LENGTH_8_BIT;
	/* Enable UART mode with 16x divisor */
	UART_MDR1(3) = UART_MODE_16X;
	/* Set pin functions for UART3 */
	CONTROL_CORE_PAD0_UART3_RX_IRRX_PAD1_UART3_TX_IRTX = MUXMODE1(0) | 
		INPUTENABLE1 | MUXMODE2(0) | INPUTENABLE2;
#endif /* ARM_PANDABOARD */


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

#ifdef ARM_S3C4530
        /* Uart 0 for debug output: baud 9600. */
        ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
        ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

        /* On Cronyx board, hardware watchdog is attached to pin P21. */
	ARM_IOPCON1 &= ~(1 << 21);
	ARM_IOPMOD |= 1 << 21;

	/* Global interrupt enable. */
	ARM_INTMSK = 0x1fffff;
#endif

#ifdef ARM_AT91SAM
#ifndef AT91BOOTSTRAP
        /* Set USART0 for debug output.
	 * RXD0 and TXD0 lines: disable PIO and assign to A function. */
	*AT91C_PIOA_PDR = 3;
	*AT91C_PIOA_ASR = 3;
	*AT91C_PIOA_BSR = 0;

	/* Enable the clock of USART and PIO/ */
	*AT91C_PMC_PCER = 1 << AT91C_ID_US0;
	*AT91C_PMC_PCER = 1 << AT91C_ID_PIOA;
	*AT91C_PMC_PCER = 1 << AT91C_ID_PIOB;

	/* Reset receiver and transmitter */
	*AT91C_US0_CR = AT91C_US_RSTRX | AT91C_US_RSTTX |
		AT91C_US_RXDIS | AT91C_US_TXDIS ;

	/* Set baud rate divisor register: baud 115200. */
	*AT91C_US0_BRGR = (KHZ * 1000 / 115200 + 8) / 16;

	/* Write the Timeguard Register */
	*AT91C_US0_TTGR = 0;

	/* Set the USART mode */
	*AT91C_US0_MR = AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE;

	/* Enable the RX and TX PDC transfer requests. */
	*AT91C_US0_PTCR = AT91C_PDC_TXTEN | AT91C_PDC_RXTEN;

	/* Enable USART0: RX receiver and TX transmiter. */
	*AT91C_US0_CR = AT91C_US_TXEN | AT91C_US_RXEN;
#endif /* !AT91BOOTSTRAP */

#ifdef ARM_AT91SAM9260
        /* Use DBGU for debug output.
	 * PIO and DBGU are already initialized by bootstrap. */

	/* Setup exception vectors at address 0. */
	{
	extern unsigned _start_ [];
	unsigned *p;
	for (p=0; p<(unsigned*)0x20; ++p) {
		*p = 0xe59ff018;	/* ldr pc, [pc, #24] */
		p[8] = *(unsigned*) ((unsigned) &_start_[8] + (unsigned) p);
	}
	}
#endif /* ARM_AT91SAM9260 */

	/* Setup interrupt vectors. */
	{
	unsigned i;
	for (i=0; i<32; ++i)
		AT91C_AIC_SVR[i] = i;
	*AT91C_AIC_SPU = 32;
	}

	/* Disable and clear all interrupts. */
	*AT91C_AIC_IDCR = ~0;
	*AT91C_AIC_ICCR = ~0;
	*AT91C_AIC_EOICR = 0;
#endif /* ARM_AT91SAM */

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

	if (u >= ARM_SRAM_BASE && u < ARM_SRAM_BASE + ARM_SRAM_SIZE)
		return 1;
#ifdef ARM_AT91SAM
	/* SAM9-S3E board: 64 Mbytes at NCS1. */
	if (u >= 0x20000000 && u < 0x24000000)
		return 1;
#endif
	return 0;
}

/*
 * This routine should be supplied by user.
 * Implementation of watchdog is different on different boards.
 */
void __attribute ((weak))
watchdog_alive ()
{
#ifdef ARM_S3C4530
        /* On Cronyx board, hardware watchdog is attached to pin P21.
         * Alive pulse must be >250 nsec. */
	ARM_IOPDATA &= ~(1ul << 21);
	udelay (1);
	ARM_IOPDATA |= 1ul << 21;
#endif
}

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
