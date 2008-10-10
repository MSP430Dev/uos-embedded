/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "stream/pipe.h"

ARRAY (task1, 0x1000);
ARRAY (task2, 0x1000);
char pipe_buf [sizeof(pipe_t) + 4];

void main_master (void *arg)
{
	stream_t *stream = arg;
	unsigned ch;

	debug_printf ("Master at %x\n", stream);
	putchar (stream, '1');
	putchar (stream, '2');
	putchar (stream, '3');
	putchar (stream, '\n');
	for (;;) {
		ch = debug_getchar ();
/*		debug_printf ("(%x)\n", ch);*/
		if (ch != 0xffff)
			putchar (stream, ch);
	}
}

void main_slave (void *arg)
{
	stream_t *stream = arg;
	unsigned ch;

	debug_printf ("Slave at %x\n", stream);
	for (;;) {
		ch = getchar (stream);
/*		debug_printf ("<%x>\n", ch);*/
		debug_putchar (0, ch);
	}
}

void uos_init (void)
{
	stream_t *master, *slave;

	debug_puts ("\nTesting pipe.\n");
	pipe_init (pipe_buf, sizeof (pipe_buf), &master, &slave);

	task_create (main_master, master, "master", 1, task2, sizeof (task2));
	task_create (main_slave, slave, "slave", 2, task1, sizeof (task1));
}
