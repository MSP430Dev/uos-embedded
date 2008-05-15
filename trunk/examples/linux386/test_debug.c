/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_puts (CONST("Hello, World! (Press Enter)\n"));
		debug_getchar();
	}
}
