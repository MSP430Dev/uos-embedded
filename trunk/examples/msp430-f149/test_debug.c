/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}

void __attribute__((interrupt(0)))
_unexpected_interrupt_ ()
{
}
