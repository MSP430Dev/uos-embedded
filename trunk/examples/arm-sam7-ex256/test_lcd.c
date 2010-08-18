/*
 * Testing Nokia 6610 LCD (Philips PCF8833) on
 * Olimex SAM7-EX256 evaluation board.
 * Based on article from James Lynch.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "sam7-ex256.h"

#include "image_frog.c"

ARRAY (task, 400);
gpanel_t display;

void draw (unsigned page)
{
	gpanel_clear (&display, GPANEL_BLACK);
	switch (page % 5) {
	case 0:
		/* Color test - show boxes of different colors. */
		gpanel_rect_filled (&display, 0, 0, 129, 15, GPANEL_RED);
		gpanel_move (&display, 0, 0);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_RED);
		puts (&display, "Красный");

		gpanel_rect_filled (&display, 0, 16, 129, 16+15, GPANEL_GREEN);
		gpanel_move (&display, 0, 16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_GREEN);
		puts (&display, "Зелёный");

		gpanel_rect_filled (&display, 0, 2*16, 129, 2*16+15, GPANEL_BLUE);
		gpanel_move (&display, 0, 2*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLUE);
		puts (&display, "Синий");

		gpanel_rect_filled (&display, 0, 3*16, 129, 3*16+15, GPANEL_MAGENTA);
		gpanel_move (&display, 0, 3*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_MAGENTA);
		puts (&display, "Лиловый");

		gpanel_rect_filled (&display, 0, 4*16, 129, 4*16+15, GPANEL_CYAN);
		gpanel_move (&display, 0, 4*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_CYAN);
		puts (&display, "Бирюзовый");

		gpanel_rect_filled (&display, 0, 5*16, 129, 5*16+15, GPANEL_YELLOW);
		gpanel_move (&display, 0, 5*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_YELLOW);
		puts (&display, "Жёлтый");

		gpanel_rect_filled (&display, 0, 6*16, 129, 6*16+15, GPANEL_WHITE);
		gpanel_move (&display, 0, 6*16);
		gpanel_color (&display, GPANEL_BLACK, GPANEL_WHITE);
		puts (&display, "Белый");

		gpanel_move (&display, 0, 7*16);
		gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
		puts (&display, "Чёрный");
		break;
	case 1:
		/* Boxes. */
		gpanel_rect (&display,    0,    0, display.ncol-1,      display.nrow-1,      GPANEL_CYAN);
		gpanel_rect (&display,   11,   11, display.ncol-1-11,   display.nrow-1-11,   GPANEL_GREEN);
		gpanel_rect (&display, 2*11, 2*11, display.ncol-1-2*11, display.nrow-1-2*11, GPANEL_YELLOW);
		gpanel_rect (&display, 3*11, 3*11, display.ncol-1-3*11, display.nrow-1-3*11, GPANEL_RED);
		gpanel_rect (&display, 4*11, 4*11, display.ncol-1-4*11, display.nrow-1-4*11, GPANEL_MAGENTA);
		gpanel_rect (&display, 5*11, 5*11, display.ncol-1-5*11, display.nrow-1-5*11, GPANEL_BLUE);
		break;
	case 4:
		/* Image. */
		gpanel_image (&display, 0, 0, display.nrow, display.ncol, image_frog);
		break;
	}
}

void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page % 5) {
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

/*
 * Task of polling buttons.
 */
void poll_buttons (void *arg)
{
	unsigned pagenum = 0;
	unsigned up_pressed = 0, left_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;

	draw (pagenum);
	for (;;) {
		mdelay (20);
		draw_next (pagenum);

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up: increase contrast. */
			gpanel_contrast (&display, display.contrast + 1);
			gpanel_move (&display, 10, 130-17);
			gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
			printf (&display, "Contrast = %02x", display.contrast);
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Up: decrease contrast. */
			gpanel_contrast (&display, display.contrast - 1);
			gpanel_move (&display, 10, 130-17);
			gpanel_color (&display, GPANEL_WHITE, GPANEL_BLACK);
			printf (&display, "Contrast = %02x", display.contrast);
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

void uos_init (void)
{
	/*debug_puts ("\nTesting LCD.\n");*/
	gpanel_init (&display);
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
