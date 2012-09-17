/*
 * Debug console input/output for MIPS microcontrollers.
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

bool_t debug_onlcr = 1;

static int debug_char = -1;

#if defined (ELVEES)
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x = 0;
	int in_exception = mips_read_c0_register (C0_STATUS) & (ST_EXL | ST_ERL);

	if (! in_exception)
		mips_intr_disable (&x);

	/* Wait for transmitter holding register empty. */
	while (! (MC_LSR & MC_LSR_TXRDY))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	MC_THR = c;

/*	watchdog_alive ();*/
	if (debug_onlcr && c == '\n') {
		/* Wait for transmitter holding register empty. */
		while (! (MC_LSR & MC_LSR_TXRDY))
			continue;	
		c = '\r';
		goto again;
	}
	if (! in_exception)
		mips_intr_restore (x);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
/*debug_printf ("getchar -> 0x%02x\n", c);*/
		return c;
	}
	mips_intr_disable (&x);
	for (;;) {
		/* Wait until receive data available. */
		if (! (MC_LSR & MC_LSR_RXRDY)) {
/*			watchdog_alive ();*/
			mips_intr_restore (x);
			mips_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = MC_RBR;
		break;
	}
	mips_intr_restore (x);
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0)
		return debug_char;

	mips_intr_disable (&x);

	/* Wait until receive data available. */
	if (! (MC_LSR & MC_LSR_RXRDY)) {
		mips_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = MC_RBR;

	mips_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ELVEES */

#if defined (PIC32MX)

#ifdef OLIMEX_DUINOMITE_MEGA
#	define UxTXREG	U5TXREG
#	define UxRXREG	U5RXREG
#	define UxSTA	U5STA
#else
#	define UxTXREG	U1TXREG
#	define UxRXREG	U1RXREG
#	define UxSTA	U1STA
#endif
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x = 0;
	int in_exception = mips_read_c0_register (C0_STATUS) & (ST_EXL | ST_ERL);

	if (! in_exception)
		mips_intr_disable (&x);

	/* Wait for transmitter shift register empty. */
	while (! (UxSTA & PIC32_USTA_TRMT))
		continue;
		
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	UxTXREG = c;

	/* Wait for transmitter shift register empty. */
	while (! (UxSTA & PIC32_USTA_TRMT))
		continue;

/*	watchdog_alive ();*/
	if (debug_onlcr && c == '\n') {
		c = '\r';
		goto again;
	}
	if (! in_exception)
		mips_intr_restore (x);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;
	int x;
	
	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
		return c;
	}
	for (;;) {
                mips_intr_disable (&x);
/*		watchdog_alive ();*/

		/* Wait until receive data available. */
		if (UxSTA & PIC32_USTA_URXDA) {
                        c = UxRXREG;
                        break;
                }
                mips_intr_restore (x);
	}
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0)
		return debug_char;

	mips_intr_disable (&x);
	
	/* Wait until receive data available. */
	if (! (UxSTA & PIC32_USTA_URXDA)) {
		mips_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = UxRXREG;
	
	mips_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* PIC32MX */

void
debug_puts (const char *p)
{
	for (; *p; ++p)
		debug_putchar (0, *p);
}
