/*
 * Testing UART.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"
#include "uart/uart.h"

ARRAY (task, 400);
ARRAY (task_msec, 400);
timer_t timer;
uart_t uart;

void msec (void *arg)
{
	for (;;) {
		printf (&uart, "msec = %d\n", timer_milliseconds (&timer));
/*		printf (&uart, "timer=%d, uart=%d\n",
			task_stack_avail ((task_t*)timer.stack),
			task_stack_avail ((task_t*)uart.rstack)); */
		lock_wait (&timer.decisec);
	}
}

void hello (void *data)
{
	puts (&uart, "\nTesting UART.\n");
	for (;;) {
		puts (&uart, "Hello, World!\n");
		getchar (&uart);
	}
}

void uos_init (void)
{
	timer_init (&timer, KHZ, 10);
	uart_init (&uart, 0, 90, KHZ, 9600);
	task_create (hello, 0, "hello", 1, task, sizeof (task));
	task_create (msec, 0, "msec", 2, task_msec, sizeof (task_msec));
}
