/*
 * Testing IP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/route.h>
#include <net/ip.h>
#include <timer/timer.h>
#include <tap/tap.h>

#define MEM_SIZE	15000

ARRAY (memory, MEM_SIZE);
ARRAY (group, sizeof(lock_group_t) + 4 * sizeof(lock_slot_t));
mem_pool_t pool;
tap_t tap;
route_t route;
timer_t timer;
ip_t ip;

void uos_init (void)
{
	lock_group_t *g;
	unsigned char my_ip[] = "\310\0\0\2";

	timer_init (&timer, KHZ, 10);
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
	route_add_netif (&ip, &route, my_ip, 24, &tap.netif);
}
