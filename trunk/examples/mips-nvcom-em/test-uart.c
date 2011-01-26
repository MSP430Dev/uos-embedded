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
		puts (&uart, "\nHello, World! >> UART1 ");
		getchar (&uart);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting UART. >> UART0\n");
	uart_init (&uart, 1, 90, KHZ, 115200);
	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
