/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	int c;

	debug_puts ("Hello, World!\n");
	for (;;) {
		c = debug_getchar();
		debug_printf ("c = 0x%04x\n", c);
	}
}
