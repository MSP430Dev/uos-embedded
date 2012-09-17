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

ARRAY (stack_con, 1500);
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
task_t *console_task;

#define PORT 		0xBBBB

unsigned count = 0;
unsigned errors = 0;

void udp_task (void *data)
{
	int sz;
	unsigned i;
	unsigned char addr [4];
	unsigned short port;
	buf_t *p;
	unsigned *msg;	
	
	udp_socket (&sock, &ip, PORT);
	printf (&debug, "Server waiting on port %d...\n", PORT);
	printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));

	for (;;) {
	    p = udp_recvfrom (&sock, addr, &port);
	    
		if (!p) continue;
		
		sz = (p->tot_len >> 2);
		msg = (unsigned *) p->payload;

		for (i = 0; i < sz; ++i) {
			if (msg[i] != count) {
				//debug_printf("bad counter: %d, expected: %d\n", msg[i], count);
				count = msg[i] + 1;
				errors++;
				continue;
			}
			++count;
		}
		
		buf_free (p);
	}
}

void console (void *unused)
{
	unsigned old_count = 0;
	unsigned long start, end, elapsed;
	unsigned long long bytes;
	
	start = timer_milliseconds (&timer);

	for (;;) {
		timer_delay (&timer, 1000);
		end = timer_milliseconds (&timer);
		elapsed = end - start;
		bytes = (count - old_count) << 2;
		old_count = count;
		start = end;
		task_set_priority (console_task, 1);
		debug_printf ("rcv rate: %lu (bytes/sec), errors: %u\n", (unsigned) (bytes * 1000 / elapsed), errors);
		task_set_priority (console_task, 100);
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

	timer_init (&timer, KHZ, 50);

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

	task_create (udp_task, 0, "udp", 75, stack_udp, sizeof (stack_udp));		
	console_task = task_create (console, 0, "con", 1, stack_con, sizeof (stack_con));
}
