/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

ARRAY (task, 0x400);
timer_t timer;

void hello (void *arg)
{
	debug_puts (arg);
	for (;;) {
		debug_printf (": msec = %d\n", timer_milliseconds (&timer));
		mutex_wait (&timer.decisec);
		debug_puts (arg);
	}
}

#include "kernel/internal.h"

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, KHZ, 100);
	task_create (hello, "Testing timer", "hello", 1, task, sizeof (task));
}
