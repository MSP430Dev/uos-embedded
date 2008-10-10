/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

ARRAY (task, 0x200);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		lock_wait (&timer.decisec);
	}
}

void uos_init (void)
{
/* Baud 9600 at 50/2 MHz. */
ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, 100, KHZ, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
