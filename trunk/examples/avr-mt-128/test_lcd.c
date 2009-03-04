#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <lcd/lcd.h>
#include "avr-mt-128.h"

timer_t timer;
lcd_t line1, line2;
ARRAY (task, 280);

const char message1[] = "\fКто ходит в гости по утрам, тот поступает мудро.";
const char message2[] = "\fДа!";

void display_page (unsigned char n)
{
	unsigned char i;

	lcd_clear_all (&line1, &line2);
	line1.raw = 1;
	line2.raw = 1;
	n <<= 5;
	for (i=0; i<8; ++i)
		putchar (&line1, n + i);
	if (n)
		for (i=8; i<16; ++i)
			putchar (&line1, n + i);
	for (i=16; i<32; ++i)
		putchar (&line2, n + i);
	line1.raw = 0;
	line2.raw = 0;
}

/*
 * Task of polling buttons.
 */
void poll_buttons (void *data)
{
	unsigned char pagenum = 0;
	unsigned char up_pressed = 0, left_pressed = 0;
	unsigned char center_pressed = 0, right_pressed = 0;
	unsigned char down_pressed = 0;

	printf (&line1, "Testing LCD.");
	printf (&line2, "Use buttons.");

	for (;;) {
		timer_delay (&timer, 10);

		if (! button_up_pressed ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up button: clear screen. */
			lcd_clear_all (&line1, &line2);
			printf (&line1, "Cleared.");
		}
		if (! button_left_pressed ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left button: show previous page of symbols. */
			display_page (--pagenum);
		}
		if (! button_center_pressed ())
			center_pressed = 0;
		else if (! center_pressed) {
			center_pressed = 1;

			/* Center button: show current page of symbols. */
			display_page (pagenum);
		}
		if (! button_right_pressed ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			display_page (++pagenum);
		}
		if (! button_down_pressed ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Down button: scroll long message. */
			printf (&line2, message1);
			timer_delay (&timer, 500);
			printf (&line2, message2);
		}
	}
}

void uos_init (void)
{
	timer_init (&timer, KHZ, 10);
	lcd_init (&line1, &line2, &timer);
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
