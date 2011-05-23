/*
 * Testing TCP protocol: server side.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <random/rand15.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (test_stack, 1000);

mem_pool_t pool;

unsigned pool_start;
unsigned pool_size;

void do_test ()
{
#ifdef CRAM_CACHED
	unsigned offset = rand15 () * 2;
	if (offset > pool_size - 4) offset = pool_size - 4;
#else
	unsigned offset = rand15 () * 1024 + rand15 () / 64;
#endif
	unsigned *addr = (unsigned *)(pool_start + (offset & ~3));
	*addr += rand15 ();
}

void test_task (void *data)
{
	unsigned long count = 0;
	for (;;) {
		do_test ();
		count++;
		if (count % 1000000 == 0)
			debug_printf ("%12lu tests done\n", count);
	}
}

void uos_init (void)
{
	debug_printf ("\n\nTesting MIPS32 D-Cache\n");
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (3);		/* Wait states  */

	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_WS (0) |		/* Wait states  */
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

#ifdef CRAM_CACHED
	/* Используем только внутреннюю память CRAM.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	pool_start = (unsigned) __bss_end;
	pool_size = (unsigned) _estack - 256 - pool_start;
#else
	/* Используем внешнюю память SRAM. */
	pool_start = SDRAM_START;
	pool_size = SDRAM_SIZE;
#endif

debug_printf ("pool_start = %08X\n", pool_start);

	mem_init (&pool, pool_start, pool_start + pool_size);

	task_create (test_task, 0, "test dcache", 20, test_stack, sizeof (test_stack));
}
