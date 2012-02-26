/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	unsigned i;

	debug_puts ("\33[H\33[2J");
	for (i=0;;i++) {
		debug_printf ("[%d] Hello, world!\n", i);
		debug_getchar();
	};
};
