/*
 * Testing TCP protocol: server side.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/arp.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <elvees/eth.h>

ARRAY (stack_udp, 1500);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
mem_pool_t pool;
arp_t *arp;
eth_t eth_data, *eth = &eth_data;
route_t route;
timer_t timer;
ip_t ip;
udp_socket_t sock;

#define PORT 		0xBBBB
#define BUF_SIZE	    365
unsigned buf [BUF_SIZE];
unsigned char server_ip [] = { 192, 168, 1, 52 };

unsigned count = 0;

void udp_task (void *data)
{
	int i;
	buf_t *p;
	unsigned old_count = 0;
	unsigned long start, end, elapsed;
	unsigned long long bytes;
	
	start = timer_milliseconds (&timer);

	debug_printf ("Press ENTER to start sending to %d.%d.%d.%d:%d\n", server_ip[0],
		server_ip[1], server_ip[2], server_ip[3], PORT);
	debug_getchar ();
	
	udp_socket (&sock, &ip, PORT);

	for (;;) {
		p = buf_alloc (&pool, BUF_SIZE * 4, 42);

		for (i = 0; i < BUF_SIZE; ++i) 
			buf[i] = count++;
		memcpy (p->payload, buf, sizeof(buf));

		udp_sendto (&sock, p, server_ip, PORT);
		
		if (timer_passed (&timer, start, 1000)) {
			end = timer_milliseconds (&timer);
			elapsed = end - start;
			bytes = (count - old_count) << 2;
			old_count = count;
			debug_printf ("snd rate: %lu (bytes/sec)\n", (unsigned) (bytes * 1000 / elapsed));
			start = end;
		}
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	/* Выделяем место для динамической памяти */
	extern unsigned __bss_end[];
#ifdef ELVEES_DATA_SDRAM
	/* Динамическая память в SDRAM */
	if (((unsigned) __bss_end & 0xF0000000) == 0x80000000)
		mem_init (&pool, (unsigned) __bss_end, 0x82000000);
	else
		mem_init (&pool, (unsigned) __bss_end, 0xa2000000);
#else
	/* Динамическая память в CRAM */
	extern unsigned _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
#endif

	timer_init (&timer, KHZ, 20);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	mutex_group_t *g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &eth->netif.lock);
	mutex_group_add (g, &timer.decisec);

	arp = arp_init (arp_data, sizeof(arp_data), &ip);
	ip_init (&ip, &pool, 70, &timer, arp, g);

	/*
	 * Create interface eth0
	 */
	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (eth, "eth0", 80, &pool, arp, my_macaddr);

	unsigned char my_ip[] = { 192, 168, 1, 20 };
	route_add_netif (&ip, &route, my_ip, 24, &eth->netif);
	
	task_create (udp_task, 0, "udp", 65,	stack_udp, sizeof (stack_udp));
}
