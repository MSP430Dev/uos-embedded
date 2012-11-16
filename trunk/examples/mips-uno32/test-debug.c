/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
        debug_puts ("--- Test started ---\n");
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
