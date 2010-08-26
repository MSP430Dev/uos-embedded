/*
 * Testing timer.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

ARRAY (task, 1000);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		mutex_wait (&timer.decisec);
//		mdelay (1000);
//		debug_printf ("CTRL=%08x, VAL=%08x\n",
//			ARM_SYSTICK->CTRL, ARM_SYSTICK->VAL);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, KHZ, 200);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
