/*
 * Проверка работы пользовательских таймеров.
 * Вывод на UART2 на скорости 115200 бод.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

ARRAY (stack1, 1000);
ARRAY (stack2, 1000);

timer_t timer;
user_timer_t utim1, utim2;

void task (void *arg)
{
    user_timer_t *ut = arg;
	for (;;) {
	    if (ut == &utim1)
	        debug_printf ("user_timer1: ");
	    else
    	    debug_printf ("user_timer2: ");
		debug_printf ("msec = %d, ", timer_milliseconds (&timer));
		
		int i;
		for (i = 0; i < 500000; i++)
		    asm volatile ("nop");
		    
		debug_printf ("msec = %d\n", timer_milliseconds (&timer));
		
		user_timer_wait (ut);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting user timers.\n");
	timer_init (&timer, KHZ, 10);
	user_timer_init (&utim1, 100);
	user_timer_init (&utim2, 1000);
	user_timer_add (&timer, &utim1);
	user_timer_add (&timer, &utim2);
	task_create (task, &utim1, "timer 1", 1, stack1, sizeof (stack1));
	task_create (task, &utim2, "timer 2", 1, stack2, sizeof (stack2));
}

