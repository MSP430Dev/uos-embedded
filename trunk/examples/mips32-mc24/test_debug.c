/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_printf ("Hello, World!\n");
		debug_getchar();
	}
}

void _irq_handler_ ()
{
/*	uos_halt (0);*/
}
