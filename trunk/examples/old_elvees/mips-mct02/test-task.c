/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"

ARRAY (task_space, 0x400);	/* Memory for task stack */
unsigned count_init;		/* Time when uos_init() started */
unsigned count_task;		/* Time when task() started */

void task (void *arg)
{
	unsigned count0;

	count_task = mips_read_c0_register (C0_COUNT);
	count0 = *(unsigned*) 0xb8400000;
	for (;;) {
		debug_printf ("Hello from `%s'!\n", arg);
		debug_printf ("Task space %d bytes, free %d bytes.\n",
			sizeof (task_space), task_stack_avail ((task_t*) task_space));
		debug_printf ("User initialization started after %u ticks.\n",
			count_init - count0);
		debug_printf ("First task started after %u ticks.\n",
			count_task - count0);
		debug_puts ("(Press Enter)\n");
		debug_getchar ();
	}
}

void uos_init (void)
{
	count_init = mips_read_c0_register (C0_COUNT);

	task_create (task, "task", "task", 1, task_space, sizeof (task_space));
}

