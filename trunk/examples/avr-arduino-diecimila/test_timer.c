/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

ARRAY (task, 200);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%S'! msec = %u\n",
			arg, timer_milliseconds (&timer));
		lock_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	/* Baud 38400. */
	UBRR = ((int) (KHZ * 1000L / 38400) + 8) / 16 - 1;
	debug_puts ("\nTesting timer.\n");

	timer_init (&timer, KHZ, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
