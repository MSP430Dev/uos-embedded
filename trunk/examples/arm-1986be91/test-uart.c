/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include <gpanel/gpanel.h>
#include "board-1986be91.h"

ARRAY (task, 1000);
uart_t uart;
gpanel_t display;

unsigned down_pressed = 0;

extern gpanel_font_t font_fixed6x8;

void hello (void *data)
{
	puts (&uart, "\nTesting UART.");
	for (;;) {
		printf (&uart, "\nHello, World! ");
#if 0
		getchar (&uart);
#else
		while (peekchar (&uart) < 0) {
#if 0
			static int count;
			if (++count >= 100) {
				count = 0;
				debug_printf ("FR=%04x ", ARM_UART2->FR);
//				debug_printf ("CR=%04x ", ARM_UART2->CR);
				debug_printf ("IMSC=%04x ", ARM_UART2->IMSC);
//				debug_printf ("LCR_H=%04x ", ARM_UART2->LCR_H);
				debug_printf ("RIS=%04x ", ARM_UART2->RIS);
				debug_printf ("MIS=%04x ", ARM_UART2->MIS);
				break;
			}
#endif
			if (! joystick_down ())
				down_pressed = 0;
			else if (! down_pressed) {
				down_pressed = 1;
				printf (&uart, "\nHello, World! ");
			}
			mdelay (10);
		}
#endif
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
