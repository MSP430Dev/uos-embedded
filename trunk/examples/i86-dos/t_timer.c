/*
 * Testing timer.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

char task [6000];
timer_t timer;

void hello (void *arg)
{
	while (debug_peekchar() < 0) {
		debug_printf ("Hello from `%s'! msec = %lu\n",
			arg, timer_milliseconds (&timer));
		lock_wait (&timer.second);
	}
	debug_getchar();
	dos_halt();
}

void uos_init (void)
{
	timer_init (&timer, 100, 10000, 55);
	task_create (hello, "timer-test", "hello", 1, task, sizeof (task), 0);
}
