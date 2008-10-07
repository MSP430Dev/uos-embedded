/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_printf ("Hello, World!\n");
/*		debug_printf ("LSR=%02x, IIR=%02x, MSR=%02x\n", MC_LSR, MC_IIR, MC_MSR);*/
		debug_getchar();
	}
}

void _interrupt_handler_ ()
{
	uos_halt (0);
}
