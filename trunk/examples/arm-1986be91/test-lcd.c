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
	gpanel_clear (&display, GPANEL_BLACK);
	switch (page % 5) {
	case 0:
		/* Show text. */
		gpanel_move (&display, 0, 0);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
		puts (&display, "Альфа");

		gpanel_move (&display, 0, 16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_WHITE);
		puts (&display, "Бета");

		gpanel_move (&display, 0, 2*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
		puts (&display, "Гамма");

		gpanel_move (&display, 0, 3*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_WHITE);
		puts (&display, "Дельта");

		gpanel_move (&display, 0, 4*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
		puts (&display, "Эпсилон");

		gpanel_move (&display, 0, 5*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_WHITE);
		puts (&display, "Дзета");

		gpanel_move (&display, 0, 6*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
		puts (&display, "Эта");

		gpanel_move (&display, 0, 7*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_WHITE);
		puts (&display, "Тета");
		break;
	case 1:
		/* Boxes. */
		gpanel_rect (&display,    0,    0, display.ncol-1,      display.nrow-1,      GPANEL_WHITE);
		gpanel_rect (&display,   11,   11, display.ncol-1-11,   display.nrow-1-11,   GPANEL_WHITE);
		gpanel_rect (&display, 2*11, 2*11, display.ncol-1-2*11, display.nrow-1-2*11, GPANEL_WHITE);
		gpanel_rect (&display, 3*11, 3*11, display.ncol-1-3*11, display.nrow-1-3*11, GPANEL_WHITE);
		gpanel_rect (&display, 4*11, 4*11, display.ncol-1-4*11, display.nrow-1-4*11, GPANEL_WHITE);
		gpanel_rect (&display, 5*11, 5*11, display.ncol-1-5*11, display.nrow-1-5*11, GPANEL_WHITE);
		break;
	}
}

void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page % 4) {
	case 2:
		/* Rain. */
		if (radius == 0) {
			/* Generate next circle. */
			x0 = 10 + rand15() % (display.ncol - 20);
			y0 = 10 + rand15() % (display.nrow - 20);
		} else {
			/* Clear previous circle. */
			gpanel_circle (&display, x0, y0, radius, GPANEL_BLACK);
			gpanel_circle (&display, x0, y0, radius-1, GPANEL_BLACK);
		}
		radius += 2;
		if (radius > 10)
			radius = 0;
		else {
			/* Draw next circle. */
			gpanel_circle (&display, x0, y0, radius, GPANEL_WHITE);
			gpanel_circle (&display, x0, y0, radius-1, GPANEL_WHITE);
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
		gpanel_rect_filled (&display, x0, y0, x1, y1, rand15());
		break;
	}
}

int main (void)
{
	unsigned pagenum = 0;
	unsigned up_pressed = 0, left_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;

	debug_puts ("\nTesting LCD.\n");
	buttons_init ();
	gpanel_init (&display);
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
			draw (--pagenum);
		}

		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			draw (++pagenum);
		}
	}
}
