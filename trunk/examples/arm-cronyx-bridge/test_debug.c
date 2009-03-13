/*
 * Testing debug output.
 */
#include <runtime/lib.h>

void _irq_handler_ ()
{
}

int main (void)
{
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
