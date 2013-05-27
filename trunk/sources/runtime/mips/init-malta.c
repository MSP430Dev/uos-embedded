/*
 * Startup initialization and exception handlers for MIPS MALTA.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
 *                    2013 Dmitry Podkhvaitlin, <vatilin@gmail.com>
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

extern void _etext(), _exception_base_();
extern unsigned __text_start[], __data_start, _edata, _end, _estack[];
extern int main ();

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from startup.S.
 * Attribute "naked" skips function prologue.
 */
void __attribute ((noreturn))_init_ (void)
{
	unsigned *dest, *limit;

	/* Initialize STATUS register: master interrupt disable.
	 * Setup interrupt vector base. */
	mips_write_c0_register (C0_STATUS, ST_CU0 | ST_BEV);
	mips_write_c0_select (C0_EBASE, 1, _exception_base_);
	mips_write_c0_register (C0_STATUS, ST_CU0);

	/* Set vector spacing: not used really, but must be nonzero. */
	mips_write_c0_select (C0_INTCTL, 1, 32);

	/* Clear CAUSE register: use special interrupt vector 0x200. */
	//mips_write_c0_register (C0_CAUSE, CA_IV);
	mips_write_c0_register (C0_CAUSE, 0);

	/*
	 * Setup UART registers.
	 * Compute the divisor for 115.2 kbaud.
	 * Malta board has reference clock of 3.6864 MHz specially for UART
	 */
	UART_LCR = LCR_DIV_EN;
	UART_DLL = 2;
	UART_DLM = 0;
	UART_LCR = LCR_LEN8;
	UART_FCR = FCR_XMIT_FIFO_RST | FCR_RCVR_FIFO_RST | FCR_FIFO_EN;
	
#ifndef SIMULATOR
	/* Copy the .data image from flash to ram.
	 * Linker places it at the end of .text segment. */
	unsigned *src = (unsigned*) &_etext;
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

bool_t __attribute__((weak))
uos_valid_memory_address (void *ptr)
{
	unsigned address = (unsigned) ptr;

	/* Text memory. */
	if (address >= (unsigned) __text_start &&
	    address < (unsigned) _etext)
		return 1;
		
	/* Data memory. */
	if (address >= (unsigned) &__data_start &&
	    address < (unsigned) _estack)
		return 1;

	return 0;
}

void __attribute__ ((weak))
_irq_handler_ ()
{
	/* This is needed when no kernel is present. */
}
