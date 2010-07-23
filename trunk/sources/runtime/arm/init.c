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

#ifdef ARM_1986BE9
        /* Set UART2 for debug output. */
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOF;	// вкл. тактирования PORTF
	ARM_GPIOF->FUNC |= ARM_FUNC_REDEF(0) |		// переопределенная функция для
			   ARM_FUNC_REDEF(1);		// PF0(UART2_RXD) и PF1(UART2_TXD)
	ARM_GPIOF->ANALOG |= 3;				// цифровые выводы
	ARM_GPIOF->PWR &= ~(ARM_PWR_MASK(0) || ARM_PWR_MASK(1));
	ARM_GPIOF->PWR |= ARM_PWR_SLOW(0) | ARM_PWR_SLOW(1);

	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_UART2;	// вкл. тактирования UART2
	ARM_RSTCLK->UART_CLOCK = ARM_UART_CLOCK_EN2 |	// разрешаем тактирование UART2
		ARM_UART_CLOCK_BRG2(2);			// HCLK/4 (20 МГц)

	/* Set baud rate divisor: 115200 bit/sec. */
	ARM_UART2->IBRD = ARM_UART_IBRD (KHZ*1000/4, 115200);
	ARM_UART2->FBRD = ARM_UART_IBRD (KHZ*1000/4, 115200);

	/* Enable UART2, transmiter only. */
	ARM_UART2->LCR_H = ARM_UART_LCRH_WLEN8;		// длина слова 8 бит
	ARM_UART2->CR = ARM_UART_CR_UARTEN |		// пуск приемопередатчика
			ARM_UART_CR_TXE;		// передача разрешена
#endif /* ARM_1986BE9 */

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

#if __thumb2__
void _fault_ ()
{
	debug_printf ("\n\n*** fault\n\n");
	/* TODO */
	debug_printf ("\nReset...\n\n");
	asm volatile ("ldr r0, =0 \n bx r0");
}

void _unexpected_interrupt_ ()
{
	debug_printf ("\n\n*** unexpected_interrupt\n\n");
	/* TODO */
	debug_printf ("\nReset...\n\n");
	asm volatile ("ldr r0, =0 \n bx r0");
}
#else
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
#endif /*__thumb2__*/
