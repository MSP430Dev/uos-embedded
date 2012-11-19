/*
 * Проверка LCD-индикатора LCD4884 на плате SainSmart Graphic Shield.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "shield-lcd4884.h"
#include "devcfg.h"

gpanel_t display;

static void draw (unsigned page)
{
        unsigned y;

	gpanel_clear (&display, 0);
	switch (page) {
	case 0:
		/* Show latin text. */
		gpanel_move (&display, 0, 0);
		puts (&display, "0123456789+=*\n");
		puts (&display, "abcdefghijklm\n");
		puts (&display, "nopqrstuvwxyz\n");
		puts (&display, "ABCDEFGHIJKLM\n");
		puts (&display, "NOPQRSTUVWXYZ\n");
		puts (&display, "[]{}()!?~$%&#\n");
		break;
	case 1:
		/* Show cyrillic text. */
		gpanel_move (&display, 0, 0);
		puts (&display, "абвгдеёжзийклм\n");
                puts (&display, "нопрстуфхцчшщъ\n");
                puts (&display, "ыьэюя\n");
		puts (&display, "АБВГДЕЁЖЗИЙКЛМ\n");
                puts (&display, "НОПРСТУФХЦЧШЩЪ\n");
                puts (&display, "ЫЬЭЮЯ\n");
		break;
	case 2:
		/* Boxes. */
		for (y=0; y<display.ncol/4; y+=4)
			gpanel_rect (&display, y+y, y, display.ncol-1-y-y, display.nrow-1-y, 1);
		break;
	}
}

void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page) {
	case 3:
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
	case 4:
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
	unsigned pagenum = 0, backlight, contrast = 0x3f;
	unsigned left_pressed = 0, right_pressed = 0;
	unsigned up_pressed = 0, down_pressed = 0;
	extern const gpanel_font_t font_fixed6x8;

        led_init();
	joystick_init ();
	gpanel_init (&display, &font_fixed6x8);

	draw (pagenum);
        backlight = 1;
        gpanel_backlight (&display, backlight);

	/*
	 * Poll buttons.
	 */
	for (;;) {
		mdelay (20);
		draw_next (pagenum);

                int key = joystick_get();

                /* Light LED when a key is pressed. */
		if (key > JOYSTICK_IDLE)
		    led_control (1);
                else
		    led_control (0);

		if (key != JOYSTICK_LEFT)
			left_pressed = 0;
		else if (! left_pressed) {
			/* Left button: show previous page of symbols. */
			left_pressed = 1;
			pagenum = (pagenum - 1) % 4;
			draw (pagenum);
		}

		if (key != JOYSTICK_RIGHT)
			right_pressed = 0;
		else if (! right_pressed) {
			/* Right button: show next page of symbols. */
			right_pressed = 1;
			pagenum = (pagenum + 1) % 4;
			draw (pagenum);
		}

		if (key != JOYSTICK_DOWN)
			down_pressed = 0;
		else if (! down_pressed) {
			/* Down button: switch backlight. */
			down_pressed = 1;
                        backlight = !backlight;
                        gpanel_backlight (&display, backlight);
		}

		if (key != JOYSTICK_UP)
			up_pressed = 0;
		else if (! up_pressed) {
			/* Up button: contrast control. */
			up_pressed = 1;
                        contrast++;
                        if (contrast >= 127)
                            contrast = 0;
                        gpanel_contrast (&display, contrast);
                        gpanel_move (&display, 0, 0);
                        printf (&display, "Vop = %-8u\n", contrast);
		}
	}
}
