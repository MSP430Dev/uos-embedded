#include <unistd.h>
#include <runtime/lib.h>

static int debug_char = -1;

void
debug_putchar (void *arg, short c)
{
	if (write (2, &c, 1) != 1)
		/* ignore */;
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
		return c;
	}
	for (;;)
		if (read (2, &c, 1) == 1)
			return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c;

	if (debug_char >= 0)
		return debug_char;

	c = debug_getchar ();
	debug_char = c;
	return c;
}

void
debug_puts (const char *p)
{
	char c;

	for (;;) {
		c = *p;
		if (! c)
			return;
		debug_putchar (0, c);
		++p;
	}
}
