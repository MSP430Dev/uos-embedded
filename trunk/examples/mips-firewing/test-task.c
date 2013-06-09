/*
 * Testing task switch.
 * Output to graphics panel LCD4884.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include "shield-lcd4884.h"
#include "devcfg.h"

ARRAY (task, 1000);
gpanel_t display;

extern gpanel_font_t font_fixed6x8;

void hello (void *arg)
{
	unsigned counter = 0;

	for (;;) {
		printf (&display, "%s #%d\n", arg, ++counter);
		printf (&display, "Space %d b.\n",
			sizeof (task));
		printf (&display, "Free %d b.\n",
			task_stack_avail ((task_t*) task));
		puts (&display, "(Press DOWN)\n");

		/* Wait for DOWN released. */
		while (joystick_get() == JOYSTICK_DOWN)
			mdelay (20);

		/* Wait for DOWN pressed. */
		while (joystick_get() != JOYSTICK_DOWN)
			mdelay (20);

		gpanel_clear (&display, 0);
		puts (&display, "Key pressed.\n\n");
	}
}

void uos_init (void)
{
        led_init();
	joystick_init ();
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
        gpanel_backlight (&display, 1);

	puts (&display, "Task created.\n\n");

	task_create (hello, "Task", "hello", 1, task, sizeof (task));
}
