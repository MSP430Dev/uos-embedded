/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	/* Baud 9600. */
	UBRR = ((int) (KHZ * 1000L / 9600) + 8) / 16 - 1;

	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
