/* Copyright (c) 2002,2003 Marek Michalkiewicz, Joerg Wunsch
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/** \defgroup avr_io AVR device-specific IO definitions
    \code #include <avr/io.h> \endcode

    This header file includes the apropriate IO definitions for the
    device that has been specified by the <tt>-mmcu=</tt> compiler
    command-line switch.  This is done by diverting to the appropriate
    file <tt>&lt;avr/io</tt><em>XXXX</em><tt>.h&gt;</tt> which should
    never be included directly.  Some register names common to all
    AVR devices are defined directly within <tt>&lt;avr/io.h&gt;</tt>,
    but most of the details come from the respective include file.

    Note that this file always includes
    \code #include <avr/sfr_defs.h> \endcode
    See \ref avr_sfr for the details.

    Included are definitions of the IO register set and their
    respective bit values as specified in the Atmel documentation.
    Note that Atmel is not very consistent in its naming conventions,
    so even identical functions sometimes get different names on
    different devices.

    Also included are the specific names useable for interrupt
    function definitions as documented
    \ref avr_signames "here".

    Finally, the following macros are defined:

    - \b RAMEND
    <br>
    A constant describing the last on-chip RAM location.
    <br>
    - \b XRAMEND
    <br>
    A constant describing the last possible location in RAM.
    This is equal to RAMEND for devices that do not allow for
    external RAM.
    <br>
    - \b E2END
    <br>
    A constant describing the address of the last EEPROM cell.
    <br>
    - \b FLASHEND
    <br>
    A constant describing the last byte address in flash ROM.
    <br>
    - \b SPM_PAGESIZE
    <br>
    For devices with bootloader support, the flash pagesize
    (in bytes) to be used for the \c SPM instruction. */

#ifndef _AVR_IO_H_
#define _AVR_IO_H_

#include <runtime/avr/sfr_defs.h>

/*
 * Registers common to all AVR devices.
 */

#if __AVR_ARCH__ != 1
/*
 * AVR architecture 1 has no RAM, thus no stack pointer.
 *
 * All other archs do have a stack pointer.  Some devices have only
 * less than 256 bytes of possible RAM locations (128 Bytes of SRAM
 * and no option for external RAM), thus SPH is officially "reserved"
 * for them.  We catch this case below after including the
 * device-specific ioXXXX.h file, by examining XRAMEND, and
 * #undef-ining SP and SPH in that case.
 */
/* Stack Pointer */
#define SP        _SFR_IO16(0x3D)
#define SPL       _SFR_IO8(0x3D)
#define SPH       _SFR_IO8(0x3E)
#endif /* #if __AVR_ARCH__ != 1 */

/* Status REGister */
#define SREG      _SFR_IO8(0x3F)

/* Status Register - SREG */
#define    SREG_I       7
#define    SREG_T       6
#define    SREG_H       5
#define    SREG_S       4
#define    SREG_V       3
#define    SREG_N       2
#define    SREG_Z       1
#define    SREG_C       0

/* Pointer definition */
#if __AVR_ARCH__ != 1
/* avr1 has only the Z pointer */
#define    XL           r26
#define    XH           r27
#define    YL           r28
#define    YH           r29
#endif /* #if __AVR_ARCH__ != 1 */
#define    ZL           r30
#define    ZH           r31

/*
 * Only few devices come without EEPROM.  In order to assemble the
 * EEPROM library components without defining a specific device, we
 * keep the EEPROM-related definitions here, and catch the devices
 * without EEPROM (E2END == 0) below.  Obviously, the EEPROM library
 * functions will not work for them. ;-)
 */
/* EEPROM Control Register */
#define EECR	_SFR_IO8(0x1C)

/* EEPROM Data Register */
#define EEDR	_SFR_IO8(0x1D)

/* EEPROM Address Register */
#define EEAR	_SFR_IO16(0x1E)
#define EEARL	_SFR_IO8(0x1E)
#define EEARH	_SFR_IO8(0x1F)

/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0

#if defined (__AVR_AT94K__)
#  include <runtime/avr/ioat94k.h>
#elif defined (__AVR_AT43USB320__)
#  include <runtime/avr/io43u32x.h>
#elif defined (__AVR_AT43USB355__)
#  include <runtime/avr/io43u35x.h>
#elif defined (__AVR_AT76C711__)
#  include <runtime/avr/io76c711.h>
#elif defined (__AVR_AT86RF401__)
#  include <runtime/avr/io86r401.h>
#elif defined (__AVR_ATmega128__)
#  include <runtime/avr/iom128.h>
#elif defined (__AVR_AT90CAN128__)
#  include <runtime/avr/iocan128.h>
#elif defined (__AVR_ATmega64__)
#  include <runtime/avr/iom64.h>
#elif defined (__AVR_ATmega645__)
#  include <runtime/avr/iom645.h>
#elif defined (__AVR_ATmega6450__)
#  include <runtime/avr/iom6450.h>
#elif defined (__AVR_ATmega103__)
#  include <runtime/avr/iom103.h>
#elif defined (__AVR_ATmega32__)
#  include <runtime/avr/iom32.h>
#elif defined (__AVR_ATmega323__)
#  include <runtime/avr/iom323.h>
#elif defined (__AVR_ATmega325__)
#  include <runtime/avr/iom325.h>
#elif defined (__AVR_ATmega3250__)
#  include <runtime/avr/iom3250.h>
#elif defined (__AVR_ATmega16__)
#  include <runtime/avr/iom16.h>
#elif defined (__AVR_ATmega161__)
#  include <runtime/avr/iom161.h>
#elif defined (__AVR_ATmega162__)
#  include <runtime/avr/iom162.h>
#elif defined (__AVR_ATmega163__)
#  include <runtime/avr/iom163.h>
#elif defined (__AVR_ATmega165__)
#  include <runtime/avr/iom165.h>
#elif defined (__AVR_ATmega168__)
#  include <runtime/avr/iom168.h>
#elif defined (__AVR_ATmega169__)
#  include <runtime/avr/iom169.h>
#elif defined (__AVR_ATmega8__)
#  include <runtime/avr/iom8.h>
#elif defined (__AVR_ATmega48__)
#  include <runtime/avr/iom48.h>
#elif defined (__AVR_ATmega88__)
#  include <runtime/avr/iom88.h>
#elif defined (__AVR_ATmega8515__)
#  include <runtime/avr/iom8515.h>
#elif defined (__AVR_ATmega8535__)
#  include <runtime/avr/iom8535.h>
#elif defined (__AVR_AT90S8535__)
#  include <runtime/avr/io8535.h>
#elif defined (__AVR_AT90C8534__)
#  include <runtime/avr/io8534.h>
#elif defined (__AVR_AT90S8515__)
#  include <runtime/avr/io8515.h>
#elif defined (__AVR_AT90S4434__)
#  include <runtime/avr/io4434.h>
#elif defined (__AVR_AT90S4433__)
#  include <runtime/avr/io4433.h>
#elif defined (__AVR_AT90S4414__)
#  include <runtime/avr/io4414.h>
#elif defined (__AVR_ATtiny22__)
#  include <runtime/avr/iotn22.h>
#elif defined (__AVR_ATtiny26__)
#  include <runtime/avr/iotn26.h>
#elif defined (__AVR_AT90S2343__)
#  include <runtime/avr/io2343.h>
#elif defined (__AVR_AT90S2333__)
#  include <runtime/avr/io2333.h>
#elif defined (__AVR_AT90S2323__)
#  include <runtime/avr/io2323.h>
#elif defined (__AVR_AT90S2313__)
#  include <runtime/avr/io2313.h>
#elif defined (__AVR_ATtiny2313__)
#  include <runtime/avr/iotn2313.h>
#elif defined (__AVR_ATtiny13__)
#  include <runtime/avr/iotn13.h>
/* avr1: the following only supported for assembler programs */
#elif defined (__AVR_ATtiny28__)
#  include <runtime/avr/iotn28.h>
#elif defined (__AVR_AT90S1200__)
#  include <runtime/avr/io1200.h>
#elif defined (__AVR_ATtiny15__)
#  include <runtime/avr/iotn15.h>
#elif defined (__AVR_ATtiny12__)
#  include <runtime/avr/iotn12.h>
#elif defined (__AVR_ATtiny11__)
#  include <runtime/avr/iotn11.h>
#elif defined (__AVR_ATmega2561__) || defined (__AVR_ATmega2560__)
#  include <runtime/avr/iom256x.h>
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "device type not defined"
#  endif
#endif

#include <runtime/avr/portpins.h>

#if __AVR_ARCH__ != 1
#  if XRAMEND < 0x100 && !defined(__COMPILING_AVR_LIBC__)
#    undef SP
#    define SP     _SFR_IO8(0x3D)
#    undef SPH
#  endif
#endif

#if E2END == 0 && !defined(__COMPILING_AVR_LIBC__)
# undef EECR
# undef EEDR
# undef EEARL
# undef EEMWE
# undef EEWE
# undef EERE
#endif
#if E2END < 0x100 && !defined(__COMPILING_AVR_LIBC__)
# undef EEAR
# if E2END > 0
#   define EEAR	_SFR_IO8(0x1E)
# endif
# undef EEARH
#endif
#if !defined(SIG_EEPROM_READY)
# undef EERIE
#endif

/*
 * Make traditional UART0 register names.
 */
#if !defined (UCR) && defined (UCSR0B)
#define UCR UCSR0B
#endif
#if !defined (USR) && defined (UCSR0A)
#define USR UCSR0A
#endif
#if !defined (UDR) && defined (UDR0)
#define UDR UDR0
#endif
#if !defined (UBRR) && defined (UBRR0L)
#define UBRR UBRR0L
#endif

/*
 *  Make traditional reg names for compatible.
 */
#ifndef MCUCSR
#define MCUCSR	MCUSR
#endif

#ifndef __ASSEMBLER__

/*
 * Global interrupt control.
 */
#define sei()  asm volatile ("sei" ::: "memory")
#define cli()  asm volatile ("cli" ::: "memory")

/*
 * Read a byte from the program space (flash).
 * Only low 64 kbytes are addressable.
 */
#define readb_near(addr) ({				\
	register unsigned char t;			\
	asm (						\
		"lpm %0, Z" "\n"			\
		: "=r,t" (t)				\
		: "z,z" ((int)(addr)));			\
	t; })

#define readw_near(addr) ({				\
	register unsigned short t;			\
	register unsigned short z = (unsigned) addr;	\
	asm (						\
		"lpm %A0, Z+" "\n"			\
		"lpm %B0, Z" "\n"			\
		: "=r" (t), "+z" (z)			\
		: );					\
	t; })

#define readl_near(addr) ({				\
	register unsigned long t;			\
	register unsigned short z = (unsigned) addr;	\
	asm (						\
		"lpm %A0, Z+" "\n"			\
		"lpm %B0, Z+" "\n"			\
		"lpm %C0, Z+" "\n"			\
		"lpm %D0, Z" "\n"			\
		: "=r" (t), "+z" (z)			\
		: );					\
	t; })

/*
 * Read a byte from all  program space (flash) depending RAMZ.
 * Register RAMZ must load before!
 * (for example once(!) in file startupXXXX.s)
 */
#define readb_far(addr) ({				\
	register unsigned char t;			\
	asm (						\
		"elpm %0, Z" "\n"			\
		: "=r,t" (t)				\
		: "z,z" ((int)(addr)));			\
	t; })

#define readw_far(addr) ({				\
	register unsigned short t;			\
	register unsigned short z = (unsigned) addr;	\
	asm (						\
		"elpm %A0, Z+" "\n"			\
		"elpm %B0, Z" "\n"			\
		: "=r" (t), "+z" (z)			\
		: );					\
	t; })

#define readl_far(addr) ({				\
	register unsigned long t;			\
	register unsigned short z = (unsigned) addr;	\
	asm (						\
		"elpm %A0, Z+" "\n"			\
		"elpm %B0, Z+" "\n"			\
		"elpm %C0, Z+" "\n"			\
		"elpm %D0, Z+" "\n"			\
		: "=r" (t), "+z" (z)			\
		: );					\
	t; })

#if FLASHEND > 0x1FFFF
#	define readb(addr)	readb_far(addr)
#	define readw(addr)	readw_far(addr)
#	define readl(addr)	readl_far(addr)
#else
#	define readb(addr)	readb_near(addr)
#	define readw(addr)	readw_near(addr)
#	define readl(addr)	readl_near(addr)
#endif

/*
 * Dumb versions of I/O macros.
 * The order of parameters is taken from Linux.
 */
#define inb_far(port)		(port)
#define outb_far(val, port)	do { (port) = (val); } while (0)
#define inw_far(port)		(port)
#define outw_far(val, port)	do { (port) = (val); } while (0)
#define setb_far(bit, port)	do { (port) |= 1 << (bit); } while (0)
#define clearb_far(bit, port)	do { (port) &= ~(1 << (bit)); } while (0)
#define testb_far(bit, port)	((port) & (1 << (bit)))

/*
 * Macros for access to 8-bit I/O registers with constant address.
 * The order of parameters is as in Linux.
 */
#define outb_const(val, port)				\
	asm volatile (					\
		"out %1, %0"				\
		: /* no outputs */			\
		: "r" ((char)(val)),			\
		  "I" ((char)_SFR_IO_ADDR(port)))

#define outb_const_zero(port)				\
	asm volatile (					\
		"out %0, __zero_reg__"			\
		: /* no outputs */			\
		: "I" ((char)_SFR_IO_ADDR(port)))

#define inb_const(port) ({				\
	register unsigned char t;			\
	asm volatile (					\
		"in %0, %1"				\
		: "=r" (t)				\
		: "I" ((char)_SFR_IO_ADDR(port)));	\
	t; })

/*
 * Macros for access to 16-bit I/O registers (ADC, ICR1, OCR1, TCNT1) -
 * read low byte first, then high byte (from the TEMP register),
 * write high byte first (to the TEMP register), then low byte.
 */
#define outw_const(val, port)				\
	asm volatile (					\
		"out (%1)+1, %B0" "\n"			\
		"out %1, %A0"				\
		: /* no outputs */			\
		: "r" ((short)(val)),			\
		  "I" ((char)_SFR_IO_ADDR(port)))

#define inw_const(port) ({				\
	register unsigned short t;			\
	asm volatile (					\
		"in %A0, %1" "\n"			\
		"in %B0, (%1)+1"			\
		: "=r" (t)				\
		: "I" ((char)_SFR_IO_ADDR(port)));	\
	t; })

/*
 * Bit handling macros.
 */
#define setb_const(bit, port) 				\
	asm volatile (					\
		"sbi %0, %1"				\
		: /* no outputs */			\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "I" ((char)(bit)))

#define clearb_const(bit, port) 			\
	asm volatile (					\
		"cbi %0, %1"				\
		: /* no outputs */			\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "I" ((char)(bit)))

#define testb_const(bit, port) ({			\
	register unsigned char t;			\
	asm volatile (					\
		"clr %0" "\n"				\
		"sbic %1, %2" "\n"			\
		"inc %0"				\
		: "=r" (t)				\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "I" ((char)(bit)));			\
	t; })

/*
 * Optimized versions of I/O macros.
 */
#ifdef __OPTIMIZE__

#define inb(port) (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40 ? \
	inb_far (port) : inb_const (port))

#define outb(val, port) do {				\
	if (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40) \
		outb_far (val, port);			\
	else if (__builtin_constant_p (val) && (val) == 0) \
		outb_const_zero (port);			\
	else						\
		outb_const ((val), (port));		\
	} while (0)

#define inw(port) (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40 ? \
	inw_far (port) : inw_const (port))

#define outw(val, port) do {				\
	if (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40) \
		outw_far (val, port);			\
	else						\
		outw_const ((val), (port));		\
	} while (0)

#define setb(bit, port) do {				\
	if (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40) \
		setb_far (bit, port);			\
	else if (! __builtin_constant_p (bit))		\
		asm volatile (				\
		"in __tmp_reg__, %0" "\n"		\
		"or __tmp_reg__, %1" "\n"		\
		"out %0, __tmp_reg__"			\
		: /* no outputs */			\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "r" ((char)1 << (bit)));		\
	else if (_SFR_IO_ADDR(port) >= 0x20) {		\
		register char dummy;			\
		asm volatile (				\
		"in %0, %1" "\n"			\
		"ori %0, %2" "\n"			\
		"out %1, %0"				\
		: "=r" (dummy)				\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "M" ((unsigned char)1 << (bit)));	\
	} else						\
		setb_const (bit, port);			\
	} while (0)

#define clearb(bit, port) do {				\
	if (! __builtin_constant_p(_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40) \
		clearb_far (bit, port);			\
	else if (! __builtin_constant_p(bit))		\
		asm volatile (				\
		"in __tmp_reg__, %0" "\n"		\
		"and __tmp_reg__, %1" "\n"		\
		"out %0, __tmp_reg__"			\
		: /* no outputs */			\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "r" ((char)~((char)1 << (bit))));	\
	else if (_SFR_IO_ADDR(port) >= 0x20) {		\
		register char dummy;			\
		asm volatile (				\
		"in %0, %1" "\n"			\
		"andi %0, %2" "\n"			\
		"out %1, %0"				\
		: "=r" (dummy)				\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "M" (0xff & ~((char)1 << (bit))));	\
	} else						\
		clearb_const (bit, port);		\
	} while (0)

#define testb(bit, port) ({				\
	unsigned char t;				\
	if (! __builtin_constant_p (_SFR_MEM_ADDR(port)) || _SFR_IO_ADDR(port)>=0x40) \
		t = testb_far (bit, port);		\
	else if (! __builtin_constant_p (bit))		\
		t = (inb_const (port) & (1 << (bit))) != 0; \
	else if (_SFR_IO_ADDR(port) >= 0x20)		\
		asm volatile (				\
		"clr %0" "\n"				\
		"in __tmp_reg__, %1" "\n"		\
		"sbrc __tmp_reg__, %2" "\n"		\
		"inc %0"				\
		: "=r" (t)				\
		: "I" ((char)_SFR_IO_ADDR(port)),	\
		  "I" ((char)(bit)));			\
	else						\
		t = testb_const (bit, port);		\
	t; })

#else /*__OPTIMIZE__*/

#define inb	inb_far
#define outb	outb_far
#define inw	inw_far
#define outw	outw_far
#define setb	setb_far
#define clearb	clearb_far
#define testb	testb_far

#endif /*__OPTIMIZE__*/

#endif /* __ASSEMBLER__ */
#endif /* _AVR_IO_H_ */
