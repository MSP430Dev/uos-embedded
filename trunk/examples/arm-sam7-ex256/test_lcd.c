/*
 * Testing Nokia 6610 LCD (Philips PCF8833) on
 * Olimex SAM7-EX256 evaluation board.
 * Based on article from James Lynch.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <lcd6100/lcd.h>
#include <random/rand15.h>
#include "sam7-ex256.h"

#include "image_frog.c"

ARRAY (task, 400);
lcd_t display;

void draw (unsigned page)
{
	lcd_clear (&display, LCD_BLACK);
	switch (page % 5) {
	case 0:
		/* Color test - show boxes of different colors. */
		lcd_rect_filled (&display, 0, 0, 129, 15, LCD_RED);
		lcd_move (&display, 0, 0);
		lcd_color (&display, LCD_WHITE, LCD_RED);
		puts (&display, "Красный");

		lcd_rect_filled (&display, 0, 16, 129, 16+15, LCD_GREEN);
		lcd_move (&display, 0, 16);
		lcd_color (&display, LCD_WHITE, LCD_GREEN);
		puts (&display, "Зелёный");

		lcd_rect_filled (&display, 0, 2*16, 129, 2*16+15, LCD_BLUE);
		lcd_move (&display, 0, 2*16);
		lcd_color (&display, LCD_WHITE, LCD_BLUE);
		puts (&display, "Синий");

		lcd_rect_filled (&display, 0, 3*16, 129, 3*16+15, LCD_MAGENTA);
		lcd_move (&display, 0, 3*16);
		lcd_color (&display, LCD_WHITE, LCD_MAGENTA);
		puts (&display, "Лиловый");

		lcd_rect_filled (&display, 0, 4*16, 129, 4*16+15, LCD_CYAN);
		lcd_move (&display, 0, 4*16);
		lcd_color (&display, LCD_BLACK, LCD_CYAN);
		puts (&display, "Бирюзовый");

		lcd_rect_filled (&display, 0, 5*16, 129, 5*16+15, LCD_YELLOW);
		lcd_move (&display, 0, 5*16);
		lcd_color (&display, LCD_BLACK, LCD_YELLOW);
		puts (&display, "Жёлтый");

		lcd_rect_filled (&display, 0, 6*16, 129, 6*16+15, LCD_WHITE);
		lcd_move (&display, 0, 6*16);
		lcd_color (&display, LCD_BLACK, LCD_WHITE);
		puts (&display, "Белый");

		lcd_move (&display, 0, 7*16);
		lcd_color (&display, LCD_WHITE, LCD_BLACK);
		puts (&display, "Чёрный");
		break;
	case 1:
		/* Boxes. */
		lcd_rect (&display,    0,    0, LCD_NCOL-1,      LCD_NROW-1,      LCD_CYAN);
		lcd_rect (&display,   11,   11, LCD_NCOL-1-11,   LCD_NROW-1-11,   LCD_GREEN);
		lcd_rect (&display, 2*11, 2*11, LCD_NCOL-1-2*11, LCD_NROW-1-2*11, LCD_YELLOW);
		lcd_rect (&display, 3*11, 3*11, LCD_NCOL-1-3*11, LCD_NROW-1-3*11, LCD_RED);
		lcd_rect (&display, 4*11, 4*11, LCD_NCOL-1-4*11, LCD_NROW-1-4*11, LCD_MAGENTA);
		lcd_rect (&display, 5*11, 5*11, LCD_NCOL-1-5*11, LCD_NROW-1-5*11, LCD_BLUE);
		break;
	case 4:
		/* Image. */
		lcd_image (&display, 0, 0, LCD_NROW, LCD_NCOL, image_frog);
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
			x0 = 10 + rand15() % (LCD_NCOL - 20);
			y0 = 10 + rand15() % (LCD_NROW - 20);
		} else {
			/* Clear previous circle. */
			lcd_circle (&display, x0, y0, radius, LCD_BLACK);
			lcd_circle (&display, x0, y0, radius-1, LCD_BLACK);
		}
		radius += 2;
		if (radius > 10)
			radius = 0;
		else {
			/* Draw next circle. */
			lcd_circle (&display, x0, y0, radius, LCD_WHITE);
			lcd_circle (&display, x0, y0, radius-1, LCD_WHITE);
			mdelay (20);
		}
		break;
	case 3:
		/* Rectangles. */
		do {
			x0 = rand15() % LCD_NCOL;
			y0 = rand15() % LCD_NROW;
			x1 = rand15() % LCD_NCOL;
			y1 = rand15() % LCD_NROW;
		} while (abs (x0-x1) < 2 || abs (y0-y1) < 2);
		lcd_rect_filled (&display, x0, y0, x1, y1, rand15());
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
			lcd_contrast (&display, display.contrast + 1);
			lcd_move (&display, 10, 130-17);
			lcd_color (&display, LCD_WHITE, LCD_BLACK);
			printf (&display, "Contrast = %02x", display.contrast);
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Up: decrease contrast. */
			lcd_contrast (&display, display.contrast - 1);
			lcd_move (&display, 10, 130-17);
			lcd_color (&display, LCD_WHITE, LCD_BLACK);
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
	lcd_init (&display);
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
