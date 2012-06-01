/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_printf ("DEVCFG0 = %08X, DEVCFG1 = %08X, DEVCFG2 = %08X, DEVCFG3 = %08X, OSCCON = %08X\n",
			DEVCFG0, DEVCFG1, DEVCFG2, DEVCFG3, OSCCON);
		debug_getchar();
	}
}
