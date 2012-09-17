/*
 * Testing SLIP protocol.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/route.h>
#include <net/ip.h>
#include <timer/timer.h>
#include <uart/slip.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

mem_pool_t pool;
timer_t timer;
ip_t ip;
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
slip_t slip;
route_t route;

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	printf (&debug, "\nCPU speed is %d MHz\n", KHZ/1000);

#if 1
	mem_init (&pool, SDRAM_START, SDRAM_START + SDRAM_SIZE);
#else
	/* Используем только внутреннюю память CRAM.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
#endif
	timer_init (&timer, KHZ, 10);

	/*
	 * Create a group of two locks: timer and slip.
	 */
	mutex_group_t *g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &slip.netif.lock);
	mutex_group_add (g, &timer.decisec);

	ip_init (&ip, &pool, 70, &timer, 0, g);

	/*
	 * Create interface slip0 200.0.0.2 / 255.255.255.0
	 */
	slip_init (&slip, 1, "slip0", 80, &pool, KHZ, 115200);

	unsigned char my_ip[] = { 200, 0, 0, 2 };
	route_add_netif (&ip, &route, my_ip, 24, &slip.netif);
}
