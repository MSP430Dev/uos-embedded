/*
 * Testing task switching.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

ARRAY (task, 400);

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'!\n", arg);
		debug_printf ("Task space %d bytes, free %d bytes\n",
			sizeof (task), task_stack_avail ((task_t*) task));
		debug_printf ("(Press Enter)\n");
		debug_getchar ();
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting task.\n");
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
