/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include <gpanel/gpanel.h>
#include "board-1986be91.h"

ARRAY (task, 1500);
uart_t uart;
gpanel_t display;

unsigned down_pressed = 0;

extern gpanel_font_t font_fixed6x8;

void hello (void *data)
{
	int c = '!';
	for (;;) {
		int n;
		putchar (&uart, '\r');
		for (n=0; n<79; n++) {
			int k;
			for (k=0; k<50; k++) {
				putchar (&uart, c);
				putchar (&uart, '\b');
			}
			putchar (&uart, c);
			if (peekchar (&uart) >= 0) {
				getchar (&uart);
				break;
			}
		}
		putchar (&uart, '\n');
		c++;
		if (c > '~')
			c = '!';
	}
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
	debug_printf ("\nTesting UART.\n");
	buttons_init ();

	/* Use LCD panel for debug output. */
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	debug_redirect (gpanel_putchar, &display);
	debug_printf ("Testing UART.\n");

	/* Using UART2. */
	uart_init (&uart, 1, 90, KHZ, 115200);

	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
