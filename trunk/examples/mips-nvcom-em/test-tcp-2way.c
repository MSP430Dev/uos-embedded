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

ARRAY (stack_tcp, 1500);
ARRAY (stack_con, 1500);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
mem_pool_t pool;
arp_t *arp;
eth_t eth_data, *eth = &eth_data;
route_t route;
timer_t timer;
ip_t ip;
tcp_socket_t *user_socket;

#define PORT 		0xBBBB
#define MSG_SIZE	365
int     tmsg[MSG_SIZE];
int     rmsg[MSG_SIZE];

unsigned tcount = 0, rcount = 0;
unsigned errors = 0;

void tcp_task (void *data)
{
	tcp_socket_t *lsock;
	int sz, i;

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

		for (;;) {
			for (i = 0; i < MSG_SIZE; ++i) tmsg[i] = tcount++;

			/* send a message to the server PORT on machine HOST */
			if (tcp_write (user_socket, tmsg, sizeof(tmsg)) < 0) break;

			while ((sz = tcp_read_poll (user_socket, rmsg, sizeof(rmsg), 1)) > 0) {
				sz >>= 2;

				for (i = 0; i < sz; ++i) {
					if (rmsg[i] != rcount) {
						debug_printf("bad counter: %d, expected: %d\n", rmsg[i], rcount);
						rcount = rmsg[i] + 1;
						errors++;
						continue;
					}
					++rcount;
				}
			}
			if (sz < 0) break;
			
			if (debug_peekchar () >= 0) {
			    debug_getchar ();
			    debug_printf ("KEY PRESSED\n");
			}
		}
	
		debug_printf ("Disconnected\n");
		tcp_close (user_socket);
		mem_free (user_socket);
		user_socket = 0;
	}
}

void console (void *unused)
{
	unsigned old_tcount = 0, old_rcount = 0;
	unsigned long start, end, elapsed;
	unsigned long long tbytes, rbytes;
	
	start = timer_milliseconds (&timer);

    for (;;) {
        timer_delay (&timer, 1000);
		end = timer_milliseconds (&timer);
		elapsed = end - start;
		tbytes = (tcount - old_tcount) << 2;
		rbytes = (rcount - old_rcount) << 2;
		old_tcount = tcount;
		old_rcount = rcount;
		debug_printf ("snd rate: %lu, rcv rate: %lu (bytes/sec), errors: %u, tcp_in_errors: %u, tcp_in_discards: %u, mem: %d\n", (unsigned) (tbytes * 1000 / elapsed), (unsigned) (rbytes * 1000 / elapsed), errors, ip.tcp_in_errors, ip.tcp_in_discards, mem_available (&pool));
		start = end;
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

	/* Используем только внутреннюю память CRAM.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);

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
	
	task_create (tcp_task, 0, "tcp", 75,
		stack_tcp, sizeof (stack_tcp));
		
    	task_create (console, 0, "con", 1,
		stack_con, sizeof (stack_con));
}
