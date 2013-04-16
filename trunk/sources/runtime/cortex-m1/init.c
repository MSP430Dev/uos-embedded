/*
 * Startup initialization and exception handlers for Cortex-M3 architecture.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
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

//	ARM_RSTCLK->HS_CONTROL = 0x00000003;			//HSE - On; Generator mode On
//	while((ARM_RSTCLK->CLOCK_STATUS&0x04)!=0x04);	//Wait until HSE not ready
//	ARM_RSTCLK->CPU_CLOCK = 0x00000102;			//CPU Clock = HSE (25MHz)
//	ARM_RSTCLK->PER_CLOCK |= 1<<24; 				//clock of PORTD ON

	unsigned long *src, *dest, *limit;

#ifndef SETUP_HCLK_HSI
#ifdef ARM_1986BE1

	/* Enable HSE generator. */
#ifdef ARM_EXT_GEN
	ARM_RSTCLK->HS_CONTROL = ARM_HS_CONTROL_HSE_ON | ARM_HS_CONTROL_HSE_BYP; 	// HSE External Generator
#else
	ARM_RSTCLK->HS_CONTROL = ARM_HS_CONTROL_HSE_ON;								// HSE External Oscillator
#endif

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
#endif

	/* Use PLLCPUo for CPU_C2, CPU_C3 and HCLK. */
	ARM_RSTCLK->CPU_CLOCK = ARM_CPU_CLOCK_C3_C2 |
				ARM_CPU_CLOCK_C2_PLLCPUO |
				ARM_CPU_CLOCK_C1_HSE |
#ifdef SETUP_HCLK_HSI
				ARM_CPU_CLOCK_HCLK_HSI;
#else
				ARM_CPU_CLOCK_HCLK_C3;
#endif

#ifndef NDEBUG
#ifdef ARM_UART1_DEBUG
	/* Set UART1 for debug output. */
	milandr_init_pin (UART1_RX_GPIO, PORT(UART1_RX), PIN(UART1_RX), UART1_RX_FUNC);
	milandr_init_pin (UART1_TX_GPIO, PORT(UART1_TX), PIN(UART1_TX), UART1_TX_FUNC);
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_UART1;	// вкл. тактирования UART1

	/* Set baud rate divisor: 115200 bit/sec. */
#ifdef SETUP_HCLK_HSI
	ARM_RSTCLK->UART_CLOCK = ARM_UART_CLOCK_EN1 |	// разрешаем тактирование UART1
		ARM_UART_CLOCK_BRG1(0);			// HCLK (8 МГц)
	ARM_UART1->IBRD = ARM_UART_IBRD (8000000, 115200);
	ARM_UART1->FBRD = ARM_UART_FBRD (8000000, 115200);
#else
	ARM_RSTCLK->UART_CLOCK = ARM_UART_CLOCK_EN1 |	// разрешаем тактирование UART1
		ARM_UART_CLOCK_BRG1(2);			// HCLK/4 (KHZ/4)
	ARM_UART1->IBRD = ARM_UART_IBRD (KHZ*1000/4, 115200);
	ARM_UART1->FBRD = ARM_UART_FBRD (KHZ*1000/4, 115200);
#endif
	/* Enable UART1, transmiter only. */
	ARM_UART1->LCRH = ARM_UART_LCRH_WLEN8;		// длина слова 8 бит
	ARM_UART1->CTL = ARM_UART_CTL_UARTEN |		// пуск приемопередатчика
			ARM_UART_CTL_TXE;		// передача разрешена

#else
/* Set UART2 for debug output. */
	milandr_init_pin (UART2_RX_GPIO, PORT(UART2_RX), PIN(UART2_RX), UART2_RX_FUNC);
	milandr_init_pin (UART2_TX_GPIO, PORT(UART2_TX), PIN(UART2_TX), UART2_TX_FUNC);
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_UART2;	// вкл. тактирования UART2

/* Set baud rate divisor: 115200 bit/sec. */
#ifdef SETUP_HCLK_HSI
	ARM_RSTCLK->UART_CLOCK = ARM_UART_CLOCK_EN2 |	// разрешаем тактирование UART2
		ARM_UART_CLOCK_BRG2(0);			// HCLK (8 МГц)
	ARM_UART2->IBRD = ARM_UART_IBRD (8000000, 115200);
	ARM_UART2->FBRD = ARM_UART_FBRD (8000000, 115200);
#else
	ARM_RSTCLK->UART_CLOCK = ARM_UART_CLOCK_EN2 |	// разрешаем тактирование UART2
		ARM_UART_CLOCK_BRG2(2);			// HCLK/4 (KHZ/4)
	ARM_UART2->IBRD = ARM_UART_IBRD (KHZ*1000/4, 115200);
	ARM_UART2->FBRD = ARM_UART_FBRD (KHZ*1000/4, 115200);
#endif
/* Enable UART2, transmiter only. */
	ARM_UART2->LCRH = ARM_UART_LCRH_WLEN8;		// длина слова 8 бит
	ARM_UART2->CTL = ARM_UART_CTL_UARTEN |		// пуск приемопередатчика
			ARM_UART_CTL_TXE;		// передача разрешена
#endif /* ARM_UART1_DEBUG */
#endif /* NDEBUG */

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

	/* Initialize priority of exceptions.
	 * Only faults and SVC are permitted when interrupts are disabled
	 * (priority level = 0).  All other interrupts have level 32. */
	ARM_SCB->SHPR1 = ARM_SHPR1_UFAULT(0) |	/* usage fault */
			 ARM_SHPR1_BFAULT(0) |	/* bus fault */
			 ARM_SHPR1_MMFAULT(0);	/* memory management fault */
        ARM_SCB->SHPR2 = ARM_SHPR2_SVCALL(0);	/* SVCall */
        ARM_SCB->SHPR3 = ARM_SHPR3_SYSTICK(32) | /* SysTick */
			 ARM_SHPR3_PENDSV(32);	/* PendSV */

	ARM_NVIC_IPR(0) = 0x20202020;		/* CAN1, CAN2, USB */
	ARM_NVIC_IPR(1) = 0x20202020;		/* DMA, UART1, UART2 */
	ARM_NVIC_IPR(2) = 0x20202020;		/* SSP1, I2C, POWER */
	ARM_NVIC_IPR(3) = 0x20202020;		/* WWDG, Timer1, Timer2 */
	ARM_NVIC_IPR(4) = 0x20202020;		/* Timer3, ADC, COMPARATOR */
	ARM_NVIC_IPR(5) = 0x20202020;		/* SSP2 */
	ARM_NVIC_IPR(6) = 0x20202020;		/* BACKUP */
	ARM_NVIC_IPR(7) = 0x20202020;		/* external INT[1:4] */

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
		       frame[9],    frame[1],    frame[6],     frame[15]);
	debug_printf ("r1 = %8x     r6 = %8x     r11 = %8x     xpsr = %8x\n",
		       frame[10],   frame[2],    frame[7],     frame[16]);
	debug_printf ("r2 = %8x     r7 = %8x     r12 = %8x     ipsr = %8x\n",
		       frame[11],   frame[3],    frame[13],    ipsr);
	debug_printf ("r3 = %8x     r8 = %8x     sp  = %8x     primask = %8x\n",
		       frame[12],   frame[4],    frame[17],     frame[8]);
	debug_printf ("r4 = %8x     r9 = %8x     lr  = %8x\n",
		       frame[0],    frame[5],    frame[14]);

	/* Reset the system. */
	debug_printf ("\nReset...\n\n");
	mdelay (1000);

	/* This does not work as expected. */
	ARM_RSTCLK->CPU_CLOCK = 0;
	ARM_SCB->AIRCR = ARM_AIRCR_VECTKEY | ARM_AIRCR_SYSRESETREQ;
	for (;;)
		asm volatile ("dmb");
}

void __attribute__ ((naked))
_fault_ ()
{
	/* Save registers R4-R11 in stack. */
	asm volatile (
	"push	{r4-r7} \n\t"
	"mov    r0, r8 \n\t"
	"mov    r1, r9 \n\t"
	"mov    r2, r10 \n\t"
	"mov    r3, r11 \n\t"
	"push	{r0-r3} \n\t");

	unsigned *frame = arm_get_stack_pointer ();
	unsigned ipsr = arm_get_ipsr ();
	char *message = "fault";
	switch (ipsr) {
	case 2:  message = "non-maskable interrupt"; break;
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
	asm volatile (
	"push	{r4-r7} \n\t"
	"mov    r0, r8 \n\t"
	"mov    r1, r9 \n\t"
	"mov    r2, r10 \n\t"
	"mov    r3, r11 \n\t"
	"push	{r0-r3} \n\t");

	unsigned *frame = arm_get_stack_pointer ();
	unsigned ipsr = arm_get_ipsr ();

	debug_printf ("\n\n*** 0x%08x: unexpected interrupt #%d\n\n",
		frame[14], ipsr);
	dump_of_death (frame, ipsr);
}

void __attribute__ ((naked, weak))
_svc_ ()
{
	/* This is needed when no kernel is present. */
}

void __attribute__ ((naked, weak))
_irq_handler_ ()
{
	/* This is needed when no kernel is present. */
}
