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

extern void _etext(), _exception_base_();
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
	unsigned *dest, *limit;

	/* Initialize STATUS register: master interrupt disable.
	 * Setup interrupt vector base. */
	mips_write_c0_register (C0_STATUS, ST_CU0 | ST_BEV);
	mips_write_c0_select (C0_EBASE, 1, _exception_base_);
	mips_write_c0_register (C0_STATUS, ST_CU0);

	/* Set vector spacing: not used really, but must be nonzero. */
	mips_write_c0_select (C0_INTCTL, 1, 32);

	/* Clear CAUSE register: use special interrupt vector 0x200. */
	mips_write_c0_register (C0_CAUSE, CA_IV);

	/*
	 * Setup UART registers.
	 * Compute the divisor for 115.2 kbaud.
	 * Assume we have (KHZ * 1000) MHz cpu clock.
	 */

#ifdef OLIMEX_DUINOMITE_MEGA
	/* Set all I/O digital */
	AD1PCFG = 0xFFFF;

	/* Enable power for UART and other peripherals */
	PORTB = (1 << 13);
	TRISB &= ~(1 << 13);
	PORTBCLR = (1 << 13);
	mdelay (5);

	/* Setup debug UART */
	U5BRG = PIC32_BRG_BAUD (KHZ * 500, 115200);
	U5MODE = 0;
	U5STA = 0;
	U5MODE = PIC32_UMODE_PDSEL_8NPAR |	/* 8-bit data, no parity */
		 PIC32_UMODE_ON;		/* UART Enable */
	U5STASET = PIC32_USTA_URXEN |		/* Receiver Enable */
		   PIC32_USTA_UTXEN;		/* Transmit Enable */
#else
	U1BRG = PIC32_BRG_BAUD (KHZ * 1000, 115200);
	U1STA = 0;
	U1MODE = PIC32_UMODE_PDSEL_8NPAR |	/* 8-bit data, no parity */
		 PIC32_UMODE_ON;		/* UART Enable */
	U1STASET = PIC32_USTA_URXEN |		/* Receiver Enable */
		   PIC32_USTA_UTXEN;		/* Transmit Enable */
#endif

        /*
         * Setup wait states.
         */
        CHECON = 2;
        BMXCONCLR = 0x40;
        CHECONSET = 0x30;

	/*
	 * Setup interrupt controller.
	 */
	INTCON = 0;				/* Interrupt Control */
	IPTMR = 0;				/* Temporal Proximity Timer */
	IFS(0) = IFS(1) = 0;                    /* Interrupt Flag Status */
	IEC(0) = IEC(1) = 0;                    /* Interrupt Enable Control */
	IPC(0) = IPC(1) = IPC(2) = IPC(3) = 	/* Interrupt Priority Control */
	IPC(4) = IPC(5) = IPC(6) = IPC(7) =
	IPC(8) = IPC(9) = IPC(11) =
		PIC32_IPC_IP0(1) | PIC32_IPC_IP1(1) |
		PIC32_IPC_IP2(1) | PIC32_IPC_IP3(1);
#if defined(PIC32MX7)
	IFS(2) = 0;                             /* MX5/6/7 series has */
	IEC(2) = 0;                             /* additional registers */
	IPC(10) = IPC(12) =
		PIC32_IPC_IP0(1) | PIC32_IPC_IP1(1) |
		PIC32_IPC_IP2(1) | PIC32_IPC_IP3(1);
#endif
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
