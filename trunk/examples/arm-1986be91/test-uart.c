/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include "board-1986be91.h"

ARRAY (task, 1000);
uart_t uart;

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
				puts (&uart, "\nTesting UART, press any key to continue...");
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

void uos_init (void)
{
	debug_printf ("\nTesting UART.\n");

	/* Using UART2. */
	uart_init (&uart, 1, 90, KHZ, 115200);

	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
