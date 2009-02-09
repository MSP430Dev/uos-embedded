/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	/* Baud 38400. */
	UBRR1L = ((int) (KHZ * 1000L / 38400) + 8) / 16 - 1;

	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
