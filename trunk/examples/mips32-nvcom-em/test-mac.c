/*
 * Testing Ethernet.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <elvees/eth.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_poll, 1000);	/* Task: polling */
mem_pool_t pool;
eth_t eth;

void command (int c)
{
	switch (c) {
	case '\n': case '\r':
		putchar (&debug, '\n');
		eth_debug (&eth, &debug);
		putchar (&debug, '\n');
		break;
	case 't' & 037:
		printf (&debug, "\nFree memory: %u bytes\n\n",
			mem_available (&pool));
		task_print (&debug, 0);
		task_print (&debug, (task_t*) stack_poll);
		task_print (&debug, (task_t*) eth.stack);
		putchar (&debug, '\n');
		break;
	}
}

void poll_eth (void *data)
{
	buf_t *p;
	unsigned long rx_packets, rx_bytes;

	rx_packets = 0;
	rx_bytes = 0;
	for (;;) {
		puts (&debug, "\rWaiting... ");
		mdelay (10);
		if (peekchar (&debug) >= 0)
			command (getchar (&debug));

		p = eth.netif.interface->input (&eth.netif);
		if (p) {
			++rx_packets;
			rx_bytes += p->tot_len;
			printf (&debug, "Packet #%lu:\n", rx_packets);
			buf_print_ethernet (p);
			buf_free (p);
		}
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (3);		/* Wait states  */

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

	mem_init (&pool, SDRAM_START, SDRAM_START + SDRAM_SIZE);

	puts (&debug, "\nTesting Ethernet.\n");
	printf (&debug, "Free %u bytes\n", mem_available (&pool));

	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (&eth, "eth0", 80, &pool, 0, my_macaddr);

	task_create (poll_eth, 0, "poll", 1, stack_poll, sizeof (stack_poll));
}
