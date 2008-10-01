/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "stream/pipe.h"

OPACITY (task1, 0x1000);
OPACITY (task2, 0x1000);

void main_master (void *arg)
{
	stream_t *stream = arg;
	unsigned ch;

	for (;;) {
/*		debug_printf ("Hello from `%s'! (Press Enter)\n", arg);*/
		ch = debug_getchar ();
		if (ch != 0xffff)
			putchar (stream, ch);
	}
}

void main_slave (void *arg)
{
	stream_t *stream = arg;
	unsigned ch;

	for (;;) {
		ch = getchar (stream);
		debug_printf ("<%x>\n", ch);
	}
}

void uos_init (void)
{
	pipe_t *pipe;
	char pipe_buf [sizeof(pipe_t) + 500];
	stream_t *master, *slave;

	debug_puts ("\nTesting pipe.\n");
	pipe = pipe_init (pipe_buf, sizeof (pipe_buf), &master, &slave);

	task_create (main_master, master, "master", 2, task2, sizeof (task2));
	task_create (main_slave, slave, "slave", 1, task1, sizeof (task1));
}
