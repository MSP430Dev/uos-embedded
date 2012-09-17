/*
 * Testing TCP protocol: server side.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/arp.h>
#include <net/udp.h>
#include <timer/timer.h>
#include <elvees/eth.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_udp, 1500);
ARRAY (stack_console, 1000);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
mem_pool_t pool;
arp_t *arp;
eth_t eth;
route_t route;
timer_t timer;
ip_t ip;
udp_socket_t sock;
unsigned char buf [1024];

static void print_udp_socket (stream_t *stream, udp_socket_t *s)
{
	printf (stream, "%u\t\t", s->local_port);

	if (s->peer_ip[0])
		printf (stream, "%d.%d.%d.%d\t", s->peer_ip[0],
			s->peer_ip[1], s->peer_ip[2], s->peer_ip[3]);
	else
		puts (stream, "*\t\t");

	if (s->peer_port)
		printf (stream, "%u", s->peer_port);
	else
		puts (stream, "*");

	putchar (stream, '\n');
}

void console_task (void *data)
{
	int c;
	long speed;
	int full_duplex;

	for (;;) {
		if (peekchar (&debug) < 0) {
			timer_delay (&timer, 50);
			continue;
		}
		c = getchar (&debug);
		switch (c) {
		case '\n': case '\r':
			putchar (&debug, '\n');
			printf (&debug, "Ethernet: %s",
				eth_get_carrier (&eth) ? "Cable OK" : "No cable");
			speed = eth_get_speed (&eth, &full_duplex);
			if (speed) {
				printf (&debug, ", %s %s",
					speed == 100000000 ? "100Base-TX" : "10Base-TX",
					full_duplex ? "Full Duplex" : "Half Duplex");
			}
			printf (&debug, ", %lu interrupts\n", eth.intr);
			printf (&debug, "Transmit: %ld packets, %ld collisions, %ld errors\n",
					eth.netif.out_packets, eth.netif.out_collisions,
					eth.netif.out_errors);
			printf (&debug, "Receive: %ld packets, %ld errors, %ld lost\n",
					eth.netif.in_packets, eth.netif.in_errors,
					eth.netif.in_discards);
			printf (&debug, "Free memory: %u bytes\n",
				mem_available (&pool));
			eth_debug (&eth, &debug);
			puts (&debug, "Local port      Peer address    Port\n");
			print_udp_socket (&debug, &sock);
			putchar (&debug, '\n');
			break;
		case 't' & 037:
			task_print (&debug, 0);
			task_print (&debug, (task_t*) stack_console);
			task_print (&debug, (task_t*) stack_udp);
			task_print (&debug, (task_t*) eth.stack);
			task_print (&debug, (task_t*) eth.tstack);
			task_print (&debug, (task_t*) ip.stack);
			putchar (&debug, '\n');
			break;
		}
	}
}

void print_packet (buf_t *p, unsigned char *addr, unsigned short port)
{
#if 1
/*	debug_printf (" <%d>", p->tot_len);*/
#else
	unsigned char *s, c;

	debug_printf ("received %d bytes from %d.%d.%d.%d port %d\n",
		p->tot_len, addr[0], addr[1], addr[2], addr[3], port);
	debug_printf ("data = \"");
	for (s=p->payload; s<p->payload+p->tot_len; ++s) {
		c = *s;

		switch (c) {
		case '"':	debug_puts ("\\\"");	break;
		case '\r':	debug_puts ("\\r");	break;
		case '\n':	debug_puts ("\\n");	break;
		case '\\':	debug_puts ("\\\\");	break;
		default:
			if (c >= ' ' && c < '~')
				debug_putchar (0, c);
			else
				debug_printf ("\\x%02x", c);
			break;
		}
	}
	debug_printf ("\"\n");
#endif
}

void udp_task (void *data)
{
	const unsigned serv_port = 7777;
	buf_t *p;
	unsigned char addr [4] = {192, 168, 20, 53};
	unsigned short port = 7777;

	udp_socket (&sock, &ip, serv_port);
	printf (&debug, "Server waiting on port %d...\n", serv_port);
	printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));
	
	for (;;) {
		//p = udp_recvfrom (&sock, addr, &port);

		//print_packet (p, addr, port);

		p = buf_alloc (&pool, 1500, 42);
		udp_sendto (&sock, p, addr, port);
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

	printf (&debug, "\nCPU speed is %d MHz\n", KHZ/1000);

#if 0
	mem_init (&pool, SDRAM_START, SDRAM_START + SDRAM_SIZE);
#else
	/* Используем только внутреннюю память CRAM.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
#endif
	timer_init (&timer, KHZ, 50);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	mutex_group_t *g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &eth.netif.lock);
	mutex_group_add (g, &timer.decisec);

	arp = arp_init (arp_data, sizeof(arp_data), &ip);
	ip_init (&ip, &pool, 70, &timer, arp, g);

	/*
	 * Create interface eth0
	 */
	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (&eth, "eth0", 80, &pool, arp, my_macaddr);

	unsigned char my_ip[] = { 192, 168, 20, 222 };
	route_add_netif (&ip, &route, my_ip, 24, &eth.netif);
	
	task_create (udp_task, 0, "udp", 10,
		stack_udp, sizeof (stack_udp));
	task_create (console_task, 0, "cons", 20,
		stack_console, sizeof (stack_console));
}
