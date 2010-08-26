/*
 * Testing task switching.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include "board-1986be91.h"

ARRAY (task, 1000);
gpanel_t display;

extern gpanel_font_t font_fixed6x8;

void hello (void *arg)
{
	for (;;) {
		printf (&display, "Hello from `%s'!\n", arg);
		printf (&display, "Task space %d bytes\n",
			sizeof (task));
		printf (&display, "Free %d bytes\n",
			task_stack_avail ((task_t*) task));
		printf (&display, "(Press DOWN)\n");

		while (! joystick_down ())
			mdelay (20);
	}
}

void uos_init (void)
{
	buttons_init ();
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	puts (&display, "Testing task.\r\n");

	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
