/*
 * Testing UDP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/udp.h>
#include <timer/timer.h>
#include <tap/tap.h>

#define MEM_SIZE	15000

char task [6000];
char memory [MEM_SIZE];
char group [sizeof(lock_group_t) + 4 * sizeof(lock_slot_t)];
mem_pool_t pool;
tap_t tap;
route_t route;
timer_t timer;
ip_t ip;
udp_socket_t sock;

void print_packet (buf_t *p, unsigned char *addr, unsigned short port)
{
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
}

void main_task (void *data)
{
	buf_t *p;
	unsigned char addr [4];
	unsigned short port;

	udp_socket (&sock, &ip, 161);

	for (;;) {
		p = udp_recvfrom (&sock, addr, &port);

		print_packet (p, addr, port);

		udp_sendto (&sock, p, addr, port);
	}
}

void uos_init (void)
{
	lock_group_t *g;

	timer_init (&timer, 100, KHZ, 10);
	mem_init (&pool, (size_t) memory, (size_t) memory + MEM_SIZE);

	/*
	 * Create a group of two locks: timer and tap.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &tap.netif.lock);
	lock_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, 0, g);

	/*
	 * Create interface tap0 200.0.0.2 / 255.255.255.0
	 */
	tap_init (&tap, "tap0", 80, &pool, 0);
	route_add_netif (&ip, &route, "\310\0\0\2", 24, &tap.netif);

	task_create (main_task, 0, "main", 1, task, sizeof (task));
}
