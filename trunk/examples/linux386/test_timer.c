/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

ARRAY (task, 6000);
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
	timer_init (&timer, KHZ, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
