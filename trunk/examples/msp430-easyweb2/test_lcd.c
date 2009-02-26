#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include "lcd.h"
#include "msp430-easyweb2.h"

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
	unsigned char b1_pressed = 0, b2_pressed = 0;
	unsigned char b3_pressed = 0, b4_pressed = 0;

	printf (&line1, "Testing LCD.");
	printf (&line2, "Use buttons.");

	for (;;) {
		timer_delay (&timer, 10);

		if (! button1_pressed ())
			b1_pressed = 0;
		else if (! b1_pressed) {
			b1_pressed = 1;

			/* Button 1: clear screen. */
			lcd_clear_all (&line1, &line2);
			printf (&line1, "Cleared.");
		}
		if (! button2_pressed ())
			b2_pressed = 0;
		else if (! b2_pressed) {
			b2_pressed = 1;

			/* Button 2: show previous page of symbols. */
			display_page (--pagenum);
		}
		if (! button3_pressed ())
			b3_pressed = 0;
		else if (! b3_pressed) {
			b3_pressed = 1;

			/* Button 3: show next page of symbols. */
			display_page (++pagenum);
		}
		if (! button4_pressed ())
			b4_pressed = 0;
		else if (! b4_pressed) {
			b4_pressed = 1;

			/* Button 4: scroll long message. */
			printf (&line2, message1);
			timer_delay (&timer, 500);
			printf (&line2, message2);
		}
	}
}

void uos_init (void)
{
	timer_init (&timer, 100, KHZ, 10);
	lcd_init (&line1, &line2, &timer);
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
