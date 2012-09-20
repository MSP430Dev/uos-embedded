/*
 * Testing TCP protocol: server side.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/arp.h>
#include <timer/timer.h>
#include <elvees/eth.h>

ARRAY (stack_con, 1500);
ARRAY (stack_tcp, 1500);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
mem_pool_t pool;
arp_t *arp;
eth_t eth_data, *eth = &eth_data;
route_t route;
timer_t timer;
ip_t ip;
tcp_socket_t *user_socket;
task_t *console_task;

#define PORT 		0xBBBB
#define MSG_SIZE	    375
int     msg[MSG_SIZE];

unsigned count = 0;
unsigned old_count = 0;
unsigned errors = 0;

void tcp_task (void *data)
{
	tcp_socket_t *lsock;
	int sz;
	unsigned i;
	unsigned cycle = 0;

	lsock = tcp_listen (&ip, 0, PORT);
	if (! lsock) {
		printf (&debug, "Error on listen, aborted\n");
		uos_halt (0);
	}
	for (;;) {
		printf (&debug, "Server waiting on port %d...\n", PORT);
		printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));
		user_socket = tcp_accept (lsock);
		if (! user_socket) {
			printf (&debug, "Error on accept\n");
			uos_halt (0);
		}
		debug_printf ("Client connected\n");
		count = 0;
		old_count = 0;

		for (;;) {
			sz = tcp_read (user_socket, msg, sizeof(msg));

			if (sz < 0) break;

			if (cycle % 5 == 0)
				tcp_ack_now (user_socket);
			cycle++;
            
			sz >>= 2;

			for (i = 0; i < sz; ++i) {
				if (msg[i] != count) {
					//debug_printf("bad counter: %d, expected: %d\n", msg[i], count);
					count = msg[i] + 1;
					errors++;
					continue;
				}
				++count;
			}
		}
	
		tcp_close (user_socket);
		mem_free (user_socket);
		user_socket = 0;
	}
}

void console (void *unused)
{
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
	
	task_create (tcp_task, 0, "tcp", 20,
		stack_tcp, sizeof (stack_tcp));
		
    	console_task = task_create (console, 0, "con", 1,
		stack_con, sizeof (stack_con));
}
