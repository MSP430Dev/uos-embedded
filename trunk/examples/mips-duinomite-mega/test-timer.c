/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

ARRAY (task, 1000);
timer_t timer;

void hello (void *arg)
{
	for (;;) {
	        //debug_printf ("STATUS = %08x\n", mips_read_c0_register (C0_STATUS));
	        //debug_printf ("CAUSE  = %08x\n", mips_read_c0_register (C0_CAUSE));
	        //debug_printf ("INTCTL = %08x\n", mips_read_c0_select (C0_INTCTL, 1));
	        //debug_printf ("EBASE  = %08x\n", mips_read_c0_select (C0_EBASE, 1));
	        //debug_printf ("INTCON = %08x\n", INTCON);
		debug_printf ("Hello from `%s'! msec = %d\n",
			arg, timer_milliseconds (&timer));
		mutex_wait (&timer.decisec);
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting timer.\n");
	timer_init (&timer, KHZ, 100);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
