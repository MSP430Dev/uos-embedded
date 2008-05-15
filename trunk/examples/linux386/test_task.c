/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"

char task [6000];

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%s'! (Press Enter)\n", arg);
		debug_getchar ();
	}
}

void uos_init (void)
{
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
