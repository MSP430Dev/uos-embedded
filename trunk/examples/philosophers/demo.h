#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

#define LIGHTRED		1
#define LIGHTGREEN		2
#define LIGHTBLUE		4

#ifdef __MSDOS__
#   define TIMER_MSEC		55
#   define PRINTF(fmt,arg)	{ debug_printf (fmt, arg); debug_putchar (0, '\n'); }
#   define PUTCHAR(sym)		debug_putchar (0, sym)
#   define textattr(c)		/*void*/
#   define gotoxy(x,y)		/*void*/
#   define clrscr()		/*void*/
#endif /* __MSDOS__ */

#if defined (__AVR__) || defined (ARM_SAMSUNG)
#   include <uart/uart.h>
#   define TIMER_MSEC		50

#   define PRINTF(fmt,arg)	printf (&uart, CONST(fmt), arg)
#   define PUTCHAR(sym)		putchar (&uart, sym)

uart_t uart;			/* Console driver */

static inline void textattr (int c)
{
	printf (&uart, CONST("\033[1;3%dm"), c);
}

static inline void gotoxy (int x, int y)
{
	printf (&uart, CONST("\033[%d;%dH"), y, x);
}

static inline void clrscr (void)
{
	printf (&uart, CONST("\033[2J"));
}
#endif /* __AVR__ */

#ifdef LINUX386
#   include <stdio.h>
#   define TIMER_MSEC		10

#   define LIGHTRED		1
#   define LIGHTGREEN		2
#   define LIGHTBLUE		4

#   define PRINTF(fmt,arg)	{ printf (fmt, arg); fflush (stdout); }

#   define PUTCHAR(sym)		{ putchar (sym); fflush (stdout); }

static inline void textattr (int c)
{
	printf ("\033[1;3%dm", c);
	fflush (stdout);
}

static inline void gotoxy (int x, int y)
{
	printf ("\033[%d;%dH", y, x);
	fflush (stdout);
}

static inline void clrscr (void)
{
	printf ("\033[2J");
	fflush (stdout);
}
#endif /* LINUX386 */

#if 0 /*def __thumb__*/
#   define TIMER_MSEC		10
#   define PRINTF(fmt,arg)	debug_printf (CONST(fmt), arg)
#   define PUTCHAR(sym)		debug_putchar (0, sym)

static inline void textattr (int c)
{
	debug_printf ("\033[1;3%dm", c);
}

static inline void gotoxy (int x, int y)
{
	debug_printf ("\033[%d;%dH", y, x);
}

static inline void clrscr (void)
{
	debug_printf ("\033[2J");
}
#endif /* __thumb__ */
