/*
 * task_coremark.c
 *
 *  Created on: Apr 1, 2011
 *      Author: krabat
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <whetstone/whetstone.h>
#include <timer/timer.h>

ARRAY (stack_console, 4000);                /* Task: console menu */
timer_t timer;

#define SIZEOF_ARRAY(array) (sizeof array)/(sizeof array[0])

void whetstone_run (void *data)
{
	const char * argv [] = {"whetstone_uos","-c", "500"};
	for (;;) 
	{
		debug_printf("whetstone task start\n");
			main_whetstone(SIZEOF_ARRAY(argv),argv);
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	timer_init(&timer, KHZ, 10);
	task_create (whetstone_run, 0, "whetstone_run", 10, stack_console, sizeof (stack_console));
}
