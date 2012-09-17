/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */
	
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
