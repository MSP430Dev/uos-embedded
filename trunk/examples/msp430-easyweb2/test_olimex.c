/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include "lcd.h"
#include "msp430-easyweb2.h"

uart_t uart;
lcd_t line1, line2;
ARRAY (task, 280);

/*
 * Task of polling buttons.
 */
void poll_buttons (void *data)
{
	unsigned char c, b1_pressed = 0, b2_pressed = 0, b3_pressed = 0;

	printf (&line1, "  Easy WEB ][   ");
	printf (&line2, " powered by uOS ");
	puts (&uart, "\nhttp://uos.vak.ru\n");
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

		/* Button: clear display. */
		if (! button1_pressed ()) {
			if (b1_pressed) {
				b1_pressed = 0;
				led_control (0);
			}
		} else if (! b1_pressed) {
			b1_pressed = 1;
			led_control (1);
			lcd_clear_all (&line1, &line2);
		}

		/* Button 2: control relay 1. */
		if (! button2_pressed ()) {
			b2_pressed = 0;
			relay1_control (0);
		} else if (! b2_pressed) {
			b2_pressed = 1;
			relay1_control (1);
		}

		/* Button 3: control relay 2. */
		if (! button3_pressed ()) {
			b3_pressed = 0;
			relay2_control (0);
		} else if (! b3_pressed) {
			b3_pressed = 1;
			relay2_control (1);
		}

		/* Button 4: generate 4 kHz. */
		while (button4_pressed ()) {
			buzzer_control (1);
			udelay (125);
			buzzer_control (-1);
			udelay (125);
		}
		buzzer_control (0);
	}
}

void uos_init (void)
{
	uart_init (&uart, 0, 90, KHZ, 115200);
	lcd_init (&line1, &line2, 0);
	buzzer_init ();
	relay_init ();
	led_init ();
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
