/*
 * Проверка работы пользовательских таймеров.
 * Вывод на UART2 на скорости 115200 бод.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

ARRAY (sstack1, 1000);
ARRAY (sstack2, 1000);
ARRAY (ftask, 1000);

timer_t timer;
user_timer_t utim1, utim2;

int fast_counter;

void slow_task (void *arg)
{
    user_timer_t *ut = arg;
    int cur_fast_counter = fast_counter;
    
	for (;;) {
	    if (ut == &utim1)
	        debug_printf ("user_timer1: ");
	    else
    	    debug_printf ("user_timer2: ");
		debug_printf ("msec = %d, ", timer_milliseconds (&timer));
		
		int i;
		for (i = 0; i < 500000; i++)
		    asm volatile ("nop");
		    
		debug_printf ("msec = %d, fast %d\n", timer_milliseconds (&timer),
			cur_fast_counter);
		
		user_timer_wait (ut);
		cur_fast_counter = fast_counter;
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
	debug_puts ("\nTesting user timers.\n");
#ifdef USEC_TIMER
	timer_init_us (&timer, KHZ, 100);
	user_timer_init_us (&utim1, 100000);
	user_timer_init_us (&utim2, 1000000);
#else
	timer_init (&timer, KHZ, 10);
	user_timer_init (&utim1, 100);
	user_timer_init (&utim2, 1000);
#endif
	user_timer_add (&timer, &utim1);
	user_timer_add (&timer, &utim2);
	task_create (slow_task, &utim1, "timer 1", 1, sstack1, sizeof (sstack1));
	task_create (slow_task, &utim2, "timer 2", 1, sstack2, sizeof (sstack2));
	task_create (fast_task, "Fast timer", 0, 2, ftask, sizeof (ftask));
}

