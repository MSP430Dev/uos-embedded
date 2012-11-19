/*
 * Проверка LCD-индикатора LCD4884 на плате SainSmart Graphic Shield.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "shield-lcd4884.h"
#include "devcfg.h"

#define MASKB_LED1	(1 << 15)   /* RB15: green */
#define MASKA_LED2	(1 << 10)   /* RA10: red */

gpanel_t display;

static void draw (unsigned page)
{
        unsigned y;

	gpanel_clear (&display, 0);
	switch (page) {
	case 0:
		/* Show text. */
		gpanel_move (&display, 0, 0);
		puts (&display, "Hello, World!\r\n");
		break;
	case 1:
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
//	unsigned left_pressed = 0;
//	unsigned right_pressed = 0;
	extern const gpanel_font_t font_fixed6x8;

        /* Use all ports as digital. */
        ANSELA = 0;
        ANSELB = 0;
        ANSELC = 0;

	LATBCLR = MASKB_LED1;
	TRISBCLR = MASKB_LED1;
	LATACLR = MASKA_LED2;
	TRISACLR = MASKA_LED2;

	joystick_init ();
	gpanel_init (&display, &font_fixed6x8);

	draw (pagenum);
        gpanel_backlight (&display, 1);

	/*
	 * Poll buttons.
	 */
	for (;;) {
//		mdelay (20);
//		draw_next (pagenum);

#if 1
static unsigned n = 0;
printf (&display, "%u) %x. \r\n", ++n, ADC1BUF0);
mdelay (500);
#else
                int key = joystick_get();
		if (key > JOYSTICK_IDLE)
                    LATBSET = MASKB_LED1;
                else
                    LATBCLR = MASKB_LED1;

		if (key != JOYSTICK_LEFT)
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left button: show previous page of symbols. */
			pagenum = (pagenum - 1) % 4;
			draw (pagenum);
		}

		if (key != JOYSTICK_RIGHT)
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			pagenum = (pagenum + 1) % 4;
			draw (pagenum);
		}
#endif
	}
}
