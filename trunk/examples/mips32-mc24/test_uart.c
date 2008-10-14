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
debug_puts ("3\n");
	for (;;) {
debug_puts ("4\n");
		puts (&uart, "\nHello, World! ");
debug_puts ("5\n");
		getchar (&uart);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting UART.\n");
	uart_init (&uart, 0, 90, KHZ, 115200);
debug_puts ("1\n");
	task_create (hello, 0, "hello", 1, task, sizeof (task));
debug_puts ("2\n");
}
