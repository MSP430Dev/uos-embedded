/*
 * task_coremark.c
 *
 *  Created on: Apr 1, 2011
 *      Author: krabat
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <linpack/linpack.h>

#define SIZEOF_ARRAY(array) (sizeof array)/(sizeof array[0])

ARRAY (stack_console, 6000);                /* Task: console menu */

timer_t timer;

#ifdef ENABLE_DCACHE
unsigned _mempool_start = 0x81000000;
#else
unsigned _mempool_start = 0xa1000000;
#endif

void linpack_run (void *data)
{
	int lp_size = 20;
	for (;;) 
	{    
		debug_printf("linpack task start %ld\n", lp_size);
			main_linpack(lp_size);
		lp_size = lp_size + 250;
		if(lp_size > 2000) lp_size = 500;
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */
      
        timer_init (&timer, KHZ, 10);
	task_create (linpack_run, 0, "linpack_run", 10, stack_console, sizeof (stack_console));
}
