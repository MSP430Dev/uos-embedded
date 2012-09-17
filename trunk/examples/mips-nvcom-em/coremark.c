/*
 * task_coremark.c
 *
 *  Created on: Apr 1, 2011
 *      Author: krabat
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <coremark/coremark.h>
#include <kernel/internal.h>
#include <runtime/math.h>
#include <timer/timer.h>

timer_t timer;

ARRAY (stack_console, 4000);                /* Task: console menu */

void coremark_run (void *data)
{
	debug_puts ("starting core_mark task...\n");
	for (;;) {
		coremark_main();
	}
}

void uos_init (void)
{

	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	timer_init (&timer, KHZ, 100);
	task_create (coremark_run, 0, "coremark_run", 10, stack_console, sizeof (stack_console));
}
