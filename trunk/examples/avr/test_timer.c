/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

char task [0x100];
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("Hello from `%S'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		lock_wait (&timer.decisec);
	}
}

void uos_init (void)
{
/* Baud 9600. */
outb (((int) (KHZ * 1000L / 9600) + 8) / 16 - 1, UBRR);

	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, 100, KHZ, 10);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
