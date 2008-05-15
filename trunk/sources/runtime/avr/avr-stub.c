/*
 * Atmel AVR microcontroller family: low level support for gdb debugger.
 * Only works on target hardware equipped with UART.
 * The stub uses about 750 bytes of code, and 70 bytes of target memory.
 * Up to 8 breakpoints are supported.
 * The stub must be compiled with flag -O (-O2, -O3, -Os).
 *
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 *
 * To enable debugger support, two things need to happen.  One, an
 * initialization of UART baud rate generator register is necessary
 * in order to communicate with GDB with the proper speed.
 * Two, a breakpoint needs to be generated to begin communication.  This
 * is most easily accomplished by a call to breakpoint().
 *
 * Because gdb will sometimes write to the stack area to execute function
 * calls, this program cannot rely on using the supervisor stack so it
 * uses it's own stack area reserved in the array `stack'.
 *
 * To implement the single instruction stepping, the analog
 * comparator interrupt is used. The debugged program must not
 * use the comparator interrupt, and the device must tolerate
 * short (a few clock cycles) artificial glitches on comparator
 * pins, caused by stub to simulate an interrupt.
 */

#include <kernel/arch.h>

#define NUMREGBYTES	37			/* Total bytes in registers */
#define NUMBREAKS	8			/* Number of breakpoints */
#define STACKSIZE 	16			/* Internal stack size */

#define EVENT_INTR	'I'			/* Interrupt signal */
#define EVENT_TRAP	'T'			/* Trap signal */

#define R_PC	*(unsigned short*)(regs+35)	/* Copy of PC register */

#if 0

/*
 * Macros for access to 8-bit I/O registers.
 */
#define inb(port) ({				\
	register unsigned char t;		\
	asm volatile (				\
		"in %0, %1"			\
		: "=r" (t)			\
		: "I" ((char)(port)));		\
	t; })

#define outb(val, port)				\
	asm volatile (				\
		"out %1, %0"			\
		: /* no outputs */		\
		: "r" ((char)(val)),		\
		  "I" ((char)(port)))

/*
 * Bit handling macros.
 */
#define setb(bit, port) 			\
	asm volatile (				\
		"sbi %0, %1"			\
		: /* no outputs */		\
		: "I" ((char)(port)),		\
		  "I" ((char)(bit)))

#define clearb(bit, port) 			\
	asm volatile (				\
		"cbi %0, %1"			\
		: /* no outputs */		\
		: "I" ((char)(port)),		\
		  "I" ((char)(bit)))

#define testb(bit, port) ({			\
	register unsigned char t;		\
	asm volatile (				\
		"clr %0" "\n"			\
		"sbic %1, %2" "\n"		\
		"inc %0"			\
		: "=r" (t)			\
		: "I" ((char)(port)),		\
		  "I" ((char)(bit)));		\
	t; })


/*
 * AVR i/o registers.
 */
#define ACSR		0x08			/* analog comparator */
#   define ACIE		3			/* ac interrupt enable */
#   define ACI		4			/* ac interrupt flag */

#define UBRR		0x09			/* uart baud rate */

#define UCR		0x0a			/* uart control */
#   define TXEN		3			/* transmitter enable */
#   define RXEN		4			/* receiver enable */

#define USR		0x0b			/* uart status */
#   define UDRE		5			/* transmitter empty */
#   define RXC		7			/* receive complete */

#define UDR		0x0c			/* uart data */
#define RAMPZ		0x3b			/* flash page select */

#endif /* 0 */

/*
 * Analog comparator pins.
 */
#if defined(AVR_AT90S2313) || defined(__AVR_AT90S2313_)
#   define AC_DDR	DDRB	/* 0x17 DDRB */
#   define AC_PORT	PORTB	/* 0x18	PORTB */
#   define AC_AIN0	0
#   define AC_AIN1	1
#endif

#if defined(AVR_AT90S2333) || defined(__AVR_AT90S2333__) ||\
    defined(AVR_AT90S4433) || defined(__AVR_AT90S4433__)
#   define AC_DDR	DDRD	/* 0x11 DDRD */
#   define AC_PORT	PORT_D	/* 0x12 PORTD */
#   define AC_AIN0	6
#   define AC_AIN1	7
#endif

#if defined(AVR_AT90S4414) || defined(__AVR_AT90S4414__) ||\
    defined(AVR_AT90S4434) || defined(__AVR_AT90S4434__) ||\
    defined(AVR_AT90S8515) || defined(__AVR_AT90S8515__) ||\
    defined(AVR_AT90S8535) || defined(__AVR_AT90S8535__) ||\
    defined(AVR_ATmega161) || defined(__AVR_ATmega161__)
#   define AC_DDR	DDRB	/* 0x17	DDRB */
#   define AC_PORT	PORTB	/* 0x18 PORTB */
#   define AC_AIN0	2
#   define AC_AIN1	3
#endif

#if defined(AVR_ATmega603) || defined(__AVR_ATmega603__) ||\
    defined(AVR_ATmega103) || defined(__AVR_ATmega103__) ||\
    defined(AVR_ATmega128) || defined(__AVR_ATmega128__) ||\
    defined(AVR_ATmega2561)|| defined(__AVR_ATmega2561__)
#   define AC_DDR	DDRE	/* 0x02 DDRE */
#   define AC_PORT	PORTE	/* 0x03 PORTE */
#   define AC_AIN0	2
#   define AC_AIN1	3
#endif

static unsigned short breaks [NUMBREAKS];	/* Breakpoints */
static char stack [STACKSIZE];			/* Internal stack */
static unsigned char regs [NUMREGBYTES];	/* Copy of all registers */
static char onestep;				/* Stop on every trap */

static inline unsigned char
get_char (void)
{
	char c;

	do {
		/* Wait until receive data available. */
		while (! testb (RXC, USR))
			continue;
		c = inb (UDR);
	} while (! (c & 0x80));
	return c & 0x7f;
}

static inline void
put_char (unsigned char c)
{
	/* Wait until transmit data empty. */
	while (! testb (UDRE, USR))
		continue;
	outb (c | 0x80, UDR);
}

static inline void
put_hexchar (unsigned char c)
{
	c >>= 4;
	c += '0';
	if (c > '9')
		c += 'a' - ':';
	put_char (c);
}

static void
put_hex (unsigned char c)
{
	put_hexchar (c);
	asm volatile ("swap %0" : "=r" (c) : "0" (c));
	put_hexchar (c);
}

/*
 * Convert the flash memory pointed to by word address `addr' into hex,
 * placing result in buf. Return a pointer to the last char put in buf (null).
 */
static unsigned short
fetch (unsigned short addr)
{
	unsigned short w;

#if defined(AVR_ATmega103)
	/* High 64k of flash memory - for ATmega only. */
	if (addr >= 0x8000) {
		outb (1, RAMPZ);
		addr += addr;
		asm volatile (
		"elpm\n"
		"mov	%A0,r0\n"
		"adiw	r30,1\n"
		"elpm\n"
		"mov	%B0,r0\n"
		: "=r" (w) : "z" (addr));
		outb (0, RAMPZ);
		return w;
	}
#endif
	addr += addr;
	asm volatile (
	"lpm\n"
	"mov	%A0,r0\n"
	"adiw	r30,1\n"
	"lpm\n"
	"mov	%B0,r0\n"
	: "=r" (w) : "z" (addr));
	return w;
}

/*
 * This function does all command procesing for interfacing to gdb.
 * The command set implemented is similar to simple stack calculator.
 * There are two virtual `registers' - A and B.
 * Commands:
 *	:	clear A
 *	0-9a-f	input hex digit to A, set A to A<<4+digit
 *	M	B := A; clear A
 *	R	B := &regs+A; clear A
 *	B	B := &breaks+A; clear A
 *	*	print A bytes of ram from byte address B, increment B
 *	@	print A words of flash from word address B, increment B
 *	,	store A to ram address B; clear A
 *	C	continue execution
 *	S	execute one instruction and stop
 *	?	tell the last stop reason: T - trap, I - interrupt
 *	Z	perform the `soft' reset
 * Examples:
 *	:R25*		read all registers
 *	:Rxx,...xx,	set all registers
 *	:xxRvv,		set single register value
 *	:xxMnn*		read memory
 *	:xxMvv,		write memory
 *	:xxMnn@		read flash
 *	:xxBvv,		set breakpoint
 */
static void
communicate (char event)
{
	int a, b;
	char c;

	if (event == EVENT_TRAP && ! onestep) {
		unsigned short *p = breaks;

		a = R_PC;
		for (c=7; c>=0; --c)
			if (*p++ == a)
				goto loop;
		goto run;
	}

	/* Reply to host that an exception has occurred */
loop:	put_char (event);
	b = 0;
zero:	a = 0;
	for (;;) {
		c = get_char ();
		if (c >= '0' && c <= '9') {
digit:			a = a << 4 | (c & 0xf);
			continue;
		}
		if (c >= 'a' && c <= 'f') {
			c += 9;
			goto digit;
		}
		switch (c) {
		case '?':
			goto loop;

		case ',':
			*(char*) b++ = a;
			/* fall through */

		case ':':
			goto zero;

		case 'M':
			b = a;
			goto zero;

		case 'R':
			b = a + (int) &regs;
			goto zero;

		case 'B':
			b = a + (int) &breaks;
			goto zero;

		case '*':
			do {
				put_hex (*(char*) b++);
			} while (--a > 0);
			goto zero;

		case '@':
			do {
				unsigned short w;

				w = fetch (b++);
				put_hex (w);
				put_hex (w >> 8);
			} while (--a > 0);
			goto zero;

		case 'Z':
			asm volatile (ASM_GOTO " 0");

		case 'C':
			onestep = 0;
			goto run;

		case 'S':
			onestep = 1;
			goto run;
		}
	}
run:
	if (onestep || breaks[0]) {
		char d;

		/* Save both DDRE and PORTE */
		d = inb (AC_DDR);
		c = inb (AC_PORT);

		/* Enable analog comparator interrupt */
		outb (0x0a, ACSR);

		/* Artificially simulate the comparator event,
		 * turning E2/E3 pins up-down. */
		setb (AC_AIN0, AC_DDR);
		setb (AC_AIN1, AC_DDR);
		do {
			clearb (AC_AIN0, AC_PORT);
			setb (AC_AIN1, AC_PORT);
			setb (AC_AIN0, AC_PORT);
			clearb (AC_AIN1, AC_PORT);
			/* Wait for ac interrupt. */
		} while (! testb (ACI, ACSR));

		/* Restore DDRE and PORTE */
		outb (d, AC_DDR);
		outb (c, AC_PORT);
	} else
		/* Disable analog comparator interrupt */
		clearb (ACIE, ACSR);
}

static inline void
save_regs1 (void)
{
	asm volatile (
	"sts	regs+31, r31\n"		/* save R31 */
	"sts	regs+30, r30\n"		/* save R30 */
	"ldi	r31, hi8(regs)\n"	/* Z points to regs */
	"ldi	r30, lo8(regs)\n"
	"std	Z+29, r29\n"		/* save R29 */
	"in	r29, __SREG__\n");	/* get SREG */
}

static inline void
save_regs2 (void)
{
	asm volatile (
	"std	Z+32, r29\n"		/* put SREG value to his plase */
	"std	Z+28, r28\n"		/* save R28 */
	"ldi	r29, 0\n"		/* Y points to 0 */
	"ldi	r28, 0\n"

	"std	Z+27, r27\n"		/* save R27 */
"1:	ld	r27, Y+\n"		/* load register 0..26 */
	"st	Z+, r27\n"		/* save it */
	"cpi	r28, 27\n"
	"brne	1b\n"

	"pop	r27\n"			/* pop ret address, high byte */
	"pop	r26\n"			/* pop return address, low byte */
	"cpi	r27, hi8 (pm (breakpoint))\n"
	"ldi	r24, lo8 (pm (breakpoint))\n"
	"cpc	r26, r24\n"
	"brne	1f\n"
	"pop	r27\n"			/* pop return address */
	"pop	r26\n"
"1:	 std	Z+35-27, r26\n"		/* save return address (PC) */
	"std	Z+36-27, r27\n"
	"in	r26, __SP_L__\n"
	"in	r27, __SP_H__\n"
	"std	Z+33-27, r26\n"		/* save SP */
	"std	Z+34-27, r27\n"
	"clr	__zero_reg__\n");

	/* Setup internal stack */
	asm volatile (
	"out __SP_H__, %B0\n"
	"out __SP_L__, %A0"
	: : "r" (stack+STACKSIZE-1));
}

static inline void
restore_regs (void)
{
	asm volatile (
	"ldi	r31, hi8(regs)\n"	/* Z points to regs */
	"ldi	r30, lo8(regs)\n"
	"ldi	r29, 0\n"		/* Y points to 0 */
	"ldi	r28, 0\n"
"1:	ld	r27, Z+\n"
	"st	Y+, r27\n"		/* restore register 0..27 */
	"cpi	r28, 28\n"
	"brne	1b\n"

	"ldd	r29, Z+33-28\n"		/* SP low */
	"out	__SP_L__, r29\n"
	"ldd	r29, Z+34-28\n"		/* SP high */
	"out	__SP_H__, r29\n"
	"ldd	r29, Z+35-28\n"		/* PC low */
	"push	r29\n"
	"ldd	r29, Z+36-28\n"		/* PC high */
	"push	r29\n"

	"ldd	r28, Z+28-28\n"		/* restore R28 */
	"ldd	r29, Z+29-28\n"		/* restore R29 */
	"ldd	r30, Z+30-28\n"		/* restore R30 */
	"lds	r31, regs+32\n");	/* r31 = sreg */
}

/*
 * Hardware interrupt - trap into debugger.
 */
void
_comparator_ (void)
{
	save_regs1 ();
	asm volatile ("ori r29, 0x80");	/* user must see interrupts enabled */
	save_regs2 ();

	communicate (EVENT_TRAP);

	asm volatile (
"restore_registers:");
	restore_regs ();

	asm volatile (
	"sbrs	r31, 7\n"		/* test I flag */
	"rjmp	1f\n"
	"andi	r31, 0x7f\n"		/* clear I flag */
	"out	__SREG__, r31\n"	/* restore SREG */
	"lds	r31, regs+31\n"		/* real value of r31 */
	"reti\n"			/* exit with interrupts enabled */
"1:	out	__SREG__, r31\n"	/* exit with interrupts disabled */
	"lds	r31, regs+31\n");	/* real value of r31 */
}

/*
 * Call this to trap into debugger.
 */
void
breakpoint (void)
{
	save_regs1 ();
	asm volatile ("cli");		/* disable interrupts */
	save_regs2 ();

	/* In case the user forgot to initialize the baud rate,
	 * set it to 9600 baud, at clock 4 MHz. */
	if (inb (UBRR) == 0)
		outb (25, UBRR);

	/* Enable receiver. */
	setb (RXEN, UCR);

	/* Enable transmitter. */
	setb (TXEN, UCR);

	communicate (EVENT_INTR);

	asm volatile (ASM_GOTO " restore_registers");
}
