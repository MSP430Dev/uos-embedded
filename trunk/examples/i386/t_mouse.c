/*
 * Testing mouse.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "input/mouse.h"
#include "i8042/mouse.h"

char task [0x400];
mouse_ps2_t mouse;

void hello (void *arg)
{
	mouse_move_t move;

	debug_puts ("\nWaiting for mouse events.\n");
	for (;;) {
		mouse_wait_move (&mouse, &move);
		debug_printf ("Move: (%d, %d, %d) %d\n",
			move.dx, move.dy, move.dz, move.buttons);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting mouse.\n");
	mouse_ps2_init (&mouse, 90);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
