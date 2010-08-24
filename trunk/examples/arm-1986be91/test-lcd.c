/*
 * Testing MT-12864J LCD (Samsung KS0108B) on
 * Milandr 1986BE91 evaluation board.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "board-1986be91.h"

gpanel_t display;

void draw (unsigned page)
{
	unsigned y;

	gpanel_clear (&display, 0);
	switch (page) {
	case 0:
		/* Show text. */
		y = 0;
		gpanel_move (&display, 0, y);
		puts (&display, "Десять негритят");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "отправились обедать,");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "Один поперхнулся,");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "их осталось девять.");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "Девять негритят,");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "поев, клевали носом,");
		gpanel_move (&display, 0, y += display.font->height);
		puts (&display, "Один не смог проснуться,");
		break;
	case 1:
		/* Boxes. */
		for (y=0; y<32; y+=5)
			gpanel_rect (&display, y+y, y, display.ncol-1-y-y, display.nrow-1-y, 1);
		break;
	}
}

void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page) {
	case 2:
		/* Rain. */
		if (radius == 0) {
			/* Generate next circle. */
			x0 = 10 + rand15() % (display.ncol - 20);
			y0 = 10 + rand15() % (display.nrow - 20);
		} else {
			/* Clear previous circle. */
			gpanel_circle (&display, x0, y0, radius, 0);
			gpanel_circle (&display, x0, y0, radius-1, 0);
		}
		radius += 2;
		if (radius > 10)
			radius = 0;
		else {
			/* Draw next circle. */
			gpanel_circle (&display, x0, y0, radius, 1);
			gpanel_circle (&display, x0, y0, radius-1, 1);
			mdelay (20);
		}
		break;
	case 3:
		/* Rectangles. */
		do {
			x0 = rand15() % display.ncol;
			y0 = rand15() % display.nrow;
			x1 = rand15() % display.ncol;
			y1 = rand15() % display.nrow;
		} while (abs (x0-x1) < 2 || abs (y0-y1) < 2);
		gpanel_rect (&display, x0, y0, x1, y1, 1);
		break;
	}
}

int main (void)
{
	unsigned pagenum = 0;
	unsigned up_pressed = 0, left_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;
	extern gpanel_font_t font_cronyxcourier9;

	debug_puts ("\nTesting LCD.\n");
	buttons_init ();
	gpanel_init (&display, &font_cronyxcourier9);

	draw (pagenum);

	/*
	 * Poll buttons.
	 */
	for (;;) {
		mdelay (20);
		draw_next (pagenum);

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up: ??? */
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Up: ??? */
		}

		if (! joystick_left ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left button: show previous page of symbols. */
			pagenum = (pagenum - 1) % 4;
			draw (pagenum);
		}

		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			pagenum = (pagenum + 1) % 4;
			draw (pagenum);
		}
	}
}