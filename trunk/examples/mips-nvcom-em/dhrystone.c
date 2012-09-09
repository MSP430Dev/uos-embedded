/*
 * dhrystone.c
 *
 *  Created on: Apr 1, 2011
 *      Author: krabat
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <dhrystone/dhry.h>
#include <kernel/internal.h>
#include <runtime/math.h>
#include <timer/timer.h>

ARRAY (stack_console, 4000);                /* Task: console menu */
timer_t timer;

#define SIZEOF_ARRAY(array) (sizeof array)/(sizeof array[0])

void dhrystone_run (void *data)
{
	debug_printf("dhrystone task start\n");
	char *argv[] = {"dhrystone","1000000"};
	for (;;) {
		main_dhr(SIZEOF_ARRAY(argv), argv);
	}
}

void uos_init (void)
{
	timer_init (&timer, KHZ, 10);
	task_create (dhrystone_run, 0, "dhrystone_run", 10, stack_console, sizeof (stack_console));
}
