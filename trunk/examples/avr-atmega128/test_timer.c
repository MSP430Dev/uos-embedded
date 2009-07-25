/*
 * Testing timer.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

ARRAY (task, 200);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%S'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		mutex_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	/* Baud 9600. */
	UBRR = ((int) (KHZ * 1000L / 9600) + 8) / 16 - 1;

	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, KHZ, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
