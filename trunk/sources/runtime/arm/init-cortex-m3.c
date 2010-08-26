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

#ifdef ARM_1986BE9
	/* Enable JTAG A and B debug ports. */
//	ARM_BACKUP->BKP_REG_0E |= ARM_BKP_REG_0E_JTAG_A | ARM_BKP_REG_0E_JTAG_B;
//	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_GPIOB;

	/* Enable HSE generator. */
	ARM_RSTCLK->HS_CONTROL = ARM_HS_CONTROL_HSE_ON;
	while (! (ARM_RSTCLK->CLOCK_STATUS & ARM_CLOCK_STATUS_HSE_RDY))
		continue;

	/* Use HSE for CPU_C1 clock. */
	ARM_RSTCLK->CPU_CLOCK = ARM_CPU_CLOCK_C1_HSE;

	/* Setup PLL for CPU. */
	ARM_RSTCLK->PLL_CONTROL = ARM_PLL_CONTROL_CPU_MUL (KHZ / KHZ_CLKIN);
	ARM_RSTCLK->PLL_CONTROL = ARM_PLL_CONTROL_CPU_MUL (KHZ / KHZ_CLKIN) |
		ARM_PLL_CONTROL_CPU_ON;
	ARM_RSTCLK->PLL_CONTROL = ARM_PLL_CONTROL_CPU_MUL (KHZ / KHZ_CLKIN) |
		ARM_PLL_CONTROL_CPU_ON | ARM_PLL_CONTROL_CPU_RLD;
	ARM_RSTCLK->PLL_CONTROL = ARM_PLL_CONTROL_CPU_MUL (KHZ / KHZ_CLKIN) |
		ARM_PLL_CONTROL_CPU_ON;
	while (! (ARM_RSTCLK->CLOCK_STATUS & ARM_CLOCK_STATUS_PLL_CPU_RDY))
		continue;

	/* Use PLLCPUo for CPU_C2, CPU_C3 and HCLK. */
	ARM_RSTCLK->CPU_CLOCK = ARM_CPU_CLOCK_HCLK_C3 |
				ARM_CPU_CLOCK_C3_C2 |
				ARM_CPU_CLOCK_C2_PLLCPUO |
				ARM_CPU_CLOCK_C1_HSE;

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
	ARM_UART2->IBRD = ARM_UART_IBRD (KHZ*1000/8, 115200);
	ARM_UART2->FBRD = ARM_UART_FBRD (KHZ*1000/8, 115200);

	/* Enable UART2, transmiter only. */
	ARM_UART2->LCR_H = ARM_UART_LCRH_WLEN8;		// длина слова 8 бит
	ARM_UART2->CR = ARM_UART_CR_UARTEN |		// пуск приемопередатчика
			ARM_UART_CR_TXE;		// передача разрешена
#endif /* ARM_1986BE9 */

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
	return 0;
}

/*
 * This routine should be supplied by user.
 * Implementation of watchdog is different on different boards.
 */
void __attribute ((weak))
watchdog_alive ()
{
	/*TODO*/
}

static void dump_of_death (unsigned *frame, unsigned ipsr)
{
	debug_printf ("r0 = %8x     r5 = %8x     r10 = %8x     pc   = %8x\n",
		       frame[8],    frame[1],    frame[6],     frame[14]);
	debug_printf ("r1 = %8x     r6 = %8x     r11 = %8x     xpsr = %8x\n",
		       frame[9],    frame[2],    frame[7],     frame[15]);
	debug_printf ("r2 = %8x     r7 = %8x     r12 = %8x     ipsr = %8x\n",
		       frame[10],   frame[3],    frame[12],    ipsr);
	debug_printf ("r3 = %8x     r8 = %8x     sp  = %8x\n",
		       frame[11],   frame[4],    frame+16);
	debug_printf ("r4 = %8x     r9 = %8x     lr  = %8x\n",
		       frame[0],    frame[5],    frame[13]);

	/* Reset the system. */
	debug_printf ("\nReset...\n\n");
	mdelay (1000);

	/* This does not work as expected. */
	ARM_RSTCLK->CPU_CLOCK = 0;
	ARM_NVIC->AIRCR = ARM_AIRCR_VECTKEY | ARM_AIRCR_SYSRESETREQ;
	for (;;)
		asm volatile ("dmb");
}

void __attribute__ ((naked))
 _fault_ ()
{
	/* Save registers R4-R11 in stack. */
	asm volatile ("push	{r4-r11}");

	unsigned *frame = arm_get_stack_pointer ();
	unsigned ipsr = arm_get_ipsr ();
	char *message = "fault";
	switch (ipsr) {
	case 3:  message = "hard fault"; break;
        case 4:  message = "memory management fault"; break;
        case 5:  message = "bus fault"; break;
        case 6:  message = "usage fault"; break;
        case 14: message = "software interrupt"; break;
	}
	debug_printf ("\n\n*** 0x%08x: %s\n\n",
		frame[14], message);
	dump_of_death (frame, ipsr);
}

void __attribute__ ((naked))
_unexpected_interrupt_ ()
{
	/* Save registers R4-R11 in stack. */
	asm volatile ("push	{r4-r11}");

	unsigned *frame = arm_get_stack_pointer ();
	unsigned ipsr = arm_get_ipsr ();

	debug_printf ("\n\n*** 0x%08x: unexpected interrupt #%d\n\n",
		frame[14], ipsr);
	dump_of_death (frame, ipsr);
}

void __attribute__ ((naked, weak))
_svc_ ()
{
}
