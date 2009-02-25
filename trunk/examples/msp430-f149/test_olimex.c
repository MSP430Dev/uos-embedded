/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include "lcd.h"
#include "avr-mt-128.h"

uart_t uart;
lcd_t line1, line2;
ARRAY (task, 280);

/*
 * Task of polling buttons.
 */
void poll_buttons (void *data)
{
	unsigned char up_pressed = 0, left_pressed = 0;
	unsigned char center_pressed = 0, down_pressed = 0;
	unsigned char c;

	for (;;) {
		/* Receive data from UART and put them on screen. */
		while (peekchar (&uart) >= 0) {
			c = getchar (&uart);
			if (line1.col >= NCOL && line2.col >= NCOL)
				lcd_clear_all (&line1, &line2);
			if (line1.col < NCOL)
				putchar (&line1, c);
			else
				putchar (&line2, c);
		}

		/* Up button: control relay and LED. */
		if (! button_up_pressed ()) {
			up_pressed = 0;
			relay_control (0);
		} else if (! up_pressed) {
			up_pressed = 1;
			relay_control (1);
		}

		/* Left button: clear display. */
		if (! button_left_pressed ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;
			lcd_clear_all (&line1, &line2);
		}

		/* Center button: print message on LCD. */
		if (! button_center_pressed ())
			center_pressed = 0;
		else if (! center_pressed) {
			center_pressed = 1;
			printf (&line1, "\f Visit the site");
			printf (&line2, "\f   uos.vak.ru");
		}

		/* Right button: generate 4 kHz. */
		while (button_right_pressed ()) {
			buzzer_control (1);
			udelay (125);
			buzzer_control (-1);
			udelay (125);
		}
		buzzer_control (0);

		/* Down button: send message to UART. */
		if (! button_down_pressed ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;
			printf (&uart, " uos.vak.ru ");
			printf (&line1, "\fSending to RS232");
		}
	}
}

void uos_init (void)
{
	uart_init (&uart, 1, 90, KHZ, 38400);
	lcd_init (&line1, &line2, 0);
	buzzer_init ();
	relay_init ();
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
