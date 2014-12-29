/*
 * Проверка драйвера таймера.
 * Вывод на UART2 на скорости 115200 бод.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <kernel/internal.h>

ARRAY (task, 1000);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
		debug_printf ("%s: msec = %d, idle task available stack %d\n",
			arg, timer_milliseconds (&timer), task_stack_avail(task_idle));
		mutex_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, KHZ, 10);
	task_create (hello, "Timer", "hello", 1, task, sizeof (task));
}
