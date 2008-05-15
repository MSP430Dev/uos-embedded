#include <unistd.h>
#include <runtime/lib.h>

void
debug_putchar (void *arg, short c)
{
	write (2, &c, 1);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;

	for (;;)
		if (read (2, &c, 1) == 1)
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
