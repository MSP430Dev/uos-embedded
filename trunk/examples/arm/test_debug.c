/*
 * Testing debug output.
 */
#include <runtime/lib.h>

void _irq_handler_ ()
{
}

int main (void)
{
/* Baud 9600 at 50/2 MHz. */
ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

ARM_IOPMOD = 0xff; ARM_IOPDATA = ~3; /* P0, P1 */
	for (;;) {
		debug_puts ("Hello, World!\n");
		debug_getchar();
	}
}
