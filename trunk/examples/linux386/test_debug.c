/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_puts ("Hello, World! (Press Enter)\n");
		debug_getchar();
	}
}
