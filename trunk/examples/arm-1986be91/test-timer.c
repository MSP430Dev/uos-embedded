/*
 * Проверка драйвера таймера.
 * Вывод на UART2 на скорости 115200 бод.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <kernel/internal.h>

ARRAY (stask, 1000);
ARRAY (ftask, 1000);
timer_t timer;

int fast_counter;

void slow_task (void *arg)
{
	for (;;) {
		debug_printf ("%s: msec = %d, fast counter = %d\n",
			arg, timer_milliseconds (&timer), fast_counter);
		mutex_wait (&timer.decisec);
	}
}

void fast_task (void *arg)
{
	for (;;) {
		fast_counter++;
		mutex_wait (&timer.lock);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
#ifdef USEC_TIMER
	timer_init_us (&timer, KHZ, 100);
#else
	timer_init (&timer, KHZ, 10);
#endif
	task_create (fast_task, "Fast timer", 0, 2, ftask, sizeof (ftask));
	task_create (slow_task, "Slow timer", 0, 1, stask, sizeof (stask));
}
