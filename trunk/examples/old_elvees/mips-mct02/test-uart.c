/*
 * Testing UART.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "elvees/mct-02.h"

ARRAY (task, 0x400);

uart_t uart;
unsigned int uart_num __attribute__ ((section (".data")))=UART0;

void hello (void *data)
{
	int i;
	for (i=0;;i++) {
		printf (&uart, "\n[%d] Hello, world! >> UART%d ", i, uart_num);
		getchar (&uart);
	}
}

void uos_init (void)
{
	uart_init (&uart, uart_num, 90, KHZ, 115200);
	printf (&uart, "\33[H\33[2J");
	printf (&uart, "\nTesting UART. >> UART%d ", uart_num);
	task_create (hello, 0, "hello", 2, task, sizeof (task));
}
