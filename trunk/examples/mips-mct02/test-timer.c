/*
 * Testing timer.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <uart/uart.h>
#include <elvees/mct-02.h>

ARRAY (task, 8000);
timer_t timer;

uart_t uart;
unsigned int uart_num __attribute__ ((section (".data")))=UART0;

void hello (void *arg)
{
	for (;;) {
		printf (&debug, "Hello from `%s'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		mutex_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	uart_init (&uart, uart_num, 90, KHZ, 115200);
	printf (&uart, "\33[H\33[2J");
	puts (&uart, "\nTesting timer.\n");
	timer_init (&timer, KHZ, 100);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
