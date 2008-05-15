/*
 * Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * without any conditions or restrictions.  This software is provided
 * "as is" without express or implied warranty.
 */
#include <runtime/lib.h>
#include <watchdog/watchdog.h>

void watchdog_enable (uint_t timeout)
{
#ifdef __AVR_ATmega2561__
        asm volatile (
	"	in __tmp_reg__, __SREG__ \n"
	"	cli \n"
	"	wdr \n"
	"	sts %1,%2 \n"
	"	sts %1,%0 \n"
	"	out __SREG__, __tmp_reg__"
	: : "r" ((char) ((timeout & 0x08 ? _WD_PS3_MASK : 0x00) | (timeout) | 1<<WDE)),
	    "M" (_SFR_MEM_ADDR(WDTCR)),
	    "r" ((char) (3<<WDE)));
#else
	asm volatile (
	"	in __tmp_reg__, __SREG__ \n"
	"	cli \n"
	"	wdr \n"
	"	out %1, %2 \n"
	"	out %1, %0 \n"
	"	out __SREG__, __tmp_reg__"
	: : "r" ((char) ((timeout) | 1<<WDE)),
	    "I" (_SFR_IO_ADDR(WDTCR)),
	    "r" ((char) (3<<WDE)));
#endif
}

void watchdog_disable (void)
{
#ifdef __AVR_ATmega2561__
      	asm volatile (
	"	in __tmp_reg__, __SREG__ \n"
	"	push r16 \n"
	"	cli \n"
	"	wdr \n"
	"	in   r16, %2 \n"
	"	andi r16, %3 \n"
	"	out  %2, r16 \n"
	"	sts  %1, %0 \n"
	"	sts  %1,__zero_reg__ \n"
	"	pop  r16 \n"
	"	out __SREG__, __tmp_reg__"
	: : "r" ((char) (3<<WDE)),
	    "M" (_SFR_MEM_ADDR(WDTCR)),
	    "I" (_SFR_IO_ADDR(MCUSR)),
	    "i" ((unsigned char) (0xff & ~(1<<WDRF))));
#else
	asm volatile (
	"	in __tmp_reg__, __SREG__ \n"
	"	cli \n"
	"	wdr \n"
	"	out %1, %0 \n"
	"	out %1, __zero_reg__ \n"
	"	out __SREG__, __tmp_reg__"
	: : "r" ((char) (3<<WDE)),
	    "I" (_SFR_IO_ADDR(WDTCR)));
#endif
}
