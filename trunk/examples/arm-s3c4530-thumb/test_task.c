/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"

ARRAY (task, 0x200);

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'! (Press Enter)\n", arg);
		debug_getchar ();
	}
}

void uos_init (void)
{
/* Baud 9600 at 50/2 MHz. */
ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	debug_puts ("\nTesting task.\n");
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
