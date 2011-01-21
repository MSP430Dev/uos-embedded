/*
 * Startup initialization and exception handlers for MIPS microcontrollers.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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

extern void _etext();
extern unsigned __data_start, _edata, _end, _estack[];
extern int main ();

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from startup.S.
 * Attribute "naked" skips function prologue.
 */
void __attribute ((noreturn))_init_ (void)
{
	unsigned *src, *dest, *limit;

	/*
	 * Setup UART registers.
	 * Compute the divisor for 115.2 kbaud.
	 * Assume we have 80 MHz cpu clock.
	 */
	U1BRG = PIC32_BRG_BAUD (KHZ * 1000, 115200);
	U1STA = 0;
	U1MODE = PIC32_UMODE_PDSEL_8NPAR |	/* 8-bit data, no parity */
		 PIC32_UMODE_ON;		/* UART Enable */
	U1STASET = PIC32_USTA_URXEN |		/* Receiver Enable */
		   PIC32_USTA_UTXEN;		/* Transmit Enable */

	/*
	 * Setup interrupt controller.
	 */
	INTCON = 0;				/* Interrupt Control */
	IPTMR = 0;				/* Temporal Proximity Timer */
	IFS(0) = IFS(1) = IFS(2) = 0;		/* Interrupt Flag Status */
	IEC(0) = IEC(1) = IEC(2) = 0;		/* Interrupt Enable Control */
	IPC(0) = IPC(1) = IPC(2) = IPC(3) = 	/* Interrupt Priority Control */
	IPC(4) = IPC(5) = IPC(6) = IPC(7) =
	IPC(8) = IPC(9) = IPC(10) = IPC(11) =
		PIC32_IPC_IP0(1) | PIC32_IPC_IP1(1) |
		PIC32_IPC_IP2(1) | PIC32_IPC_IP3(1);

	/* Copy the .data image from flash to ram.
	 * Linker places it at the end of .text segment. */
	src = (unsigned*) &_etext;
	dest = &__data_start;
	limit = &_edata;
	while (dest < limit)
		*dest++ = *src++;

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

	/* Internal SRAM. */
	if (address >= (unsigned) &__data_start &&
	    address < (unsigned) _estack)
		return 1;

	/* Boot flash. */
	if (address >= 0xbfc00000 && address < 0xbfc03000)
		return 1;

	/* Program flash. */
	if (address >= 0x9d000000 && address < 0x9d080000)
		return 1;
	return 0;
}

void __attribute__ ((weak))
_irq_handler_ ()
{
	/* This is needed when no kernel is present. */
}
