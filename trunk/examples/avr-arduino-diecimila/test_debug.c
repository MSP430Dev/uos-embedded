/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	/* Baud 19200. */
	outw (((int) (KHZ * 1000L / 19200) + 8) / 16 - 1, UBRR);
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
