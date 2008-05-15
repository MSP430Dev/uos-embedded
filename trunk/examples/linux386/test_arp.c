/*
 * Testing ARP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/ip.h>
#include <tap/tap.h>

#define MEM_SIZE	15000

char task [6000];
char memory [MEM_SIZE];
char arp_data [sizeof(arp_t) + 10 * sizeof(arp_entry_t)];
arp_t *arp;
mem_pool_t pool;
tap_t tap;
ip_t ip;
route_t route;

void hello (void *data)
{
	buf_t *p;

	lock_take (&tap.netif.lock);
	for (;;) {
		lock_wait (&tap.netif.lock);
		p = netif_input (&tap.netif);
		if (p) {
			debug_printf ("received %d bytes\n", p->tot_len);
			buf_print_ip (p);
		}
	}
}

void uos_init (void)
{
	mem_init (&pool, (mem_size_t) memory, (mem_size_t) memory + MEM_SIZE);
	arp = arp_init (arp_data, sizeof(arp_data), &ip);

	/*
	 * Create interface tap0 200.0.0.1 / 255.255.255.0
	 */
	tap_init (&tap, "tap0", 80, &pool, arp);
	route_add_netif (&ip, &route, "\310\0\0\1", 24, &tap.netif);

	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
