/*
 * Testing SPI master.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <milandr/spi.h>
#include "board-1986be91.h"

gpanel_t display;
spi_t spi;

void send_receive (unsigned sent)
{
	unsigned received;
	int retry;

	printf (&debug, "'%c' ", sent);
	spi_output (&spi, sent);
	udelay (20);
	for (retry=0; retry<3; ++retry) {
		printf (&debug, "- ");
		if (spi_input (&spi, &received)) {
			printf (&debug, "'%c'\n", received);
			return;
		}
		udelay (100);
	}
	printf (&debug, "failed.\n");
}

/*
 * Redirect debug output.
 */
void gpanel_putchar (void *arg, short c)
{
	putchar ((stream_t*) arg, c);
}

void uos_init (void)
{
	unsigned up_pressed = 0, left_pressed = 0, select_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;

	buttons_init ();

	/* Use LCD panel for debug output. */
	extern gpanel_font_t font_fixed6x8;
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	debug_redirect (gpanel_putchar, &display);
	printf (&debug, "Testing SPI master.\n");

	/* SPI2 master, 16-bit words, at 1 Mbit/sec. */
	spi_init (&spi, 1, 1, 16, 1000);

	/* Poll buttons. */
	for (;;) {
		mdelay (20);

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up */
			send_receive ('U');
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Down */
			send_receive ('D');
		}

		if (! joystick_left ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left */
			send_receive ('L');
		}

		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right. */
			send_receive ('R');
		}

		if (! joystick_select ())
			select_pressed = 0;
		else if (! select_pressed) {
			select_pressed = 1;

			/* Select. */
			send_receive ('S');
		}
	}
}
