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
		mutex_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, 1193182, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
