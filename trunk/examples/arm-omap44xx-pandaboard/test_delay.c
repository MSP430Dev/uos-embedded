/*
 * Testing udelay.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

ARRAY (task, 400);

void hello (void *arg)
{
	unsigned secs = 0;
	for (;;) {
		debug_printf ("Seconds passed = %u\n", secs++);
		mdelay(1000);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
