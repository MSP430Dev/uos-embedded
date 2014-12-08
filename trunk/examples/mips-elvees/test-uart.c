/*
 * Testing UART.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"

ARRAY (task, 0x400);
uart_t uart;

void hello (void *data)
{
	for (;;) {
		puts (&uart, "\nHello, World! >> UART0 ");
		getchar (&uart);
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	debug_puts ("\nTesting UART. >> UART0\n");
	uart_init (&uart, 0, 90, KHZ, 115200);
	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
