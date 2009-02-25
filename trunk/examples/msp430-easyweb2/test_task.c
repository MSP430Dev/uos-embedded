/*
 * Testing task switching.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

ARRAY (task, 200);

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'! (Press Enter)\n", arg);
		debug_getchar ();
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting task.\n");
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
