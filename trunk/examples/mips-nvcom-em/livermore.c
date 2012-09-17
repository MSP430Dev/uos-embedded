/*
 * task_coremark.c
 *
 *  Created on: Apr 1, 2011
 *      Author: krabat
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <livermore/livermore.h>
#include <timer/timer.h>

ARRAY (stack_console, 4000);                /* Task: console menu */
timer_t timer;

#define SIZEOF_ARRAY(array) (sizeof array)/(sizeof array[0])

void livermore_run (void *data)
{
	const char * argv [] = {"livermore_uos"};
	for (;;) 
	{    
		debug_printf("livermore task start\n");
		main_livermore(SIZEOF_ARRAY(argv), argv);
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */
	
	timer_init(&timer, KHZ, 10);

	task_create (livermore_run, 0, "linpack_run", 10, stack_console, sizeof (stack_console));
}
