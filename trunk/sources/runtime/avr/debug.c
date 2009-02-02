#include <runtime/lib.h>
#include <watchdog/watchdog.h>

#ifdef __AVR__
#include <kernel/uos.h>
task_t *task_broken			/* LY: task_current value on reset/jmp0. */
	__attribute__((section(".ly")));
void *task_broken_stack			/* LY: stack_context value from task_current on reset/jmp0. */
	__attribute__((section(".ly")));
void *broken_stack			/* LY: SP value on reset/jmp0. */
	__attribute__((section(".ly")));
#endif

#ifdef AVR_DEBUG_UART1
/* Use UART1 for debug output (default UART0). */
#  undef UCR
#  define UCR UCSR1B
#  undef USR
#  define USR UCSR1A
#  undef UDR
#  define UDR UDR1
#  undef UBRR
#  define UBRR UBRR1L
#endif

static int debug_char;

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	small_uint_t x;

	x = inb (SREG);
	cli();
	setb (TXEN, UCR);

	/* Wait for transmitter idle. */
	while (! testb (UDRE, USR))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	outb (c, UDR);

	/* Wait for transmitter idle. */
	while (! testb (UDRE, USR))
		continue;

	if (c == '\n') {
		c = '\r';
		goto again;
	}
	watchdog_alive ();

#if 0
	if (testb (RXC, USR)) {
		debug_char = inb (UDR);
		if (debug_char == 3) {
			debug_char = -1;
			breakpoint ();
		}
	}
#endif
	outb (x, SREG);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	small_uint_t c, x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
		return c;
	}

	x = inb (SREG);
	cli();
	setb (RXEN, UCR);
	for (;;) {
		/* Wait until receive data available. */
		while (! testb (RXC, USR)) {
			sei();
			cli();
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = inb (UDR);
#if 0
		if (c == 3) {
			breakpoint ();
			continue;
		}
#endif
		break;
	}
	outb (x, SREG);
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c, x;

	if (debug_char >= 0)
		return debug_char;

	x = inb (SREG);
	cli();
	setb (RXEN, UCR);
	if (! testb (RXC, USR)) {
		outb (x, SREG);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = inb (UDR);
#if 0
	if (c == 3) {
		breakpoint ();
		outb (x, SREG);
		return -1;
	}
#endif
	outb (x, SREG);
	debug_char = c;
	return c;
}

void
debug_puts (const char *p)
{
	unsigned char c, x;

	x = inb (SREG);
	cli();
	for (;;) {
		c = readb ((int) p);
		if (! c)
			break;
		debug_putchar (0, c);
		++p;
	}
	outb (x, SREG);
}
