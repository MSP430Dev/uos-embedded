/*
 * Testing task switching.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"

ARRAY (task_space, 0x400);	/* Memory for task stack */
unsigned count_init;		/* Time when uos_init() started */
unsigned count_task;		/* Time when task() started */

void task (void *arg)
{
	unsigned count0;

	count_task = mips_read_c0_register (C0_COUNT);
	count0 = *(unsigned*) 0xb8400000;
	for (;;) {
		debug_printf ("Hello from `%s'!\n", arg);
		debug_printf ("Task space %d bytes, free %d bytes.\n",
			sizeof (task_space), task_stack_avail ((task_t*) task_space));
		debug_printf ("User initialization started after %u ticks.\n",
			count_init - count0);
		debug_printf ("First task started after %u ticks.\n",
			count_task - count0);
		debug_puts ("(Press Enter)\n");
		debug_getchar ();
	}
}

void uos_init (void)
{

	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
	MC_CSCON_T |			/* Sync memory */
	MC_CSCON_CSBA (0x00000000) |	/* Base address */
	MC_CSCON_CSMASK (0xF8000000);	/* Address mask */

	MC_SDRCON = MC_SDRCON_PS_512 |		/* Page size 512 */
		MC_SDRCON_CL_3 |		/* CAS latency 3 cycles */
		MC_SDRCON_RFR (64000000/8192, MPORT_KHZ); /* Refresh period */

	MC_SDRTMR = MC_SDRTMR_TWR(2) |		/* Write recovery delay */
		MC_SDRTMR_TRP(2) |		/* Минимальный период Precharge */
		MC_SDRTMR_TRCD(2) |		/* Между Active и Read/Write */
		MC_SDRTMR_TRAS(5) |		/* Между * Active и Precharge */
		MC_SDRTMR_TRFC(15);		/* Интервал между Refresh */

	MC_SDRCSR = 1;				/* Initialize SDRAM */
	udelay (2);

	count_init = mips_read_c0_register (C0_COUNT);

	task_create (task, "task", "task", 1, task_space, sizeof (task_space));
}
