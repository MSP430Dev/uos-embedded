/*
 * Testing a timer driver.
 * Output to a graphics panel.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>
#include "shield-lcd4884.h"
#include "devcfg.h"

ARRAY (task, 2000);
gpanel_t display;
timer_t timer;

#define FONT font_middigits

extern gpanel_font_t FONT;

void hello ()
{
	unsigned prev = -1;

	for (;;) {
		/* Wait for time change. */
		unsigned sec = timer_milliseconds (&timer) / 1000;
		if (sec == prev) {
			mutex_wait (&timer.decisec);
			continue;
		}
		prev = sec;

		/* Print the time. */
		unsigned min = sec / 60;
		gpanel_clear (&display, 0);
		gpanel_move (&display, 0, (display.nrow - display.font->height) / 2);
		printf (&display, "%02u", min % 100);
		printf (&display, ":%02u", sec - min*60);
	}
}

void uos_init (void)
{
        led_init();
	joystick_init ();
	gpanel_init (&display, &FONT);
	gpanel_clear (&display, 0);
        gpanel_backlight (&display, 1);

	timer_init (&timer, KHZ, 10);

	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
