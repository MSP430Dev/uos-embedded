/*
 * Testing UART.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>

ARRAY (task, 280);
uart_t uart;

void hello (void *data)
{
	for (;;) {
		puts (&uart, "\nHello, World! ");
		getchar (&uart);
	}
}

void uos_init (void)
{
	uart_init (&uart, 0, 90, KHZ, 115200);
	debug_puts ("\nTesting uart.\n");
	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
