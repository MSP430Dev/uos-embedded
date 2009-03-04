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
#include <uart/slip.h>

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0xffff

mem_pool_t pool;
timer_t timer;
ip_t ip;
ARRAY (group, sizeof(lock_group_t) + 4 * sizeof(lock_slot_t));
slip_t slip;
route_t route;

void uos_init (void)
{
	lock_group_t *g;
	unsigned char my_ip[] = "\310\0\0\2";

/* Baud 38400. */
outb (((int) (KHZ * 1000L / 38400) + 8) / 16 - 1, UBRR);

	/* Установлена микросхема 62256 - имеем 32 килобайта памяти. */
	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);
	mem_init (&pool, RAM_START, RAM_END);

/*	timer_init (&timer, KHZ, 10);*/

	/*
	 * Create a group of two locks: timer and slip.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &slip.netif.lock);
	lock_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, 0, g);

	/*
	 * Create interface slip0 200.0.0.2 / 255.255.255.0
	 */
	slip_init (&slip, 0, "slip0", 80, &pool, KHZ, 38400);
	route_add_netif (&ip, &route, my_ip, 24, &slip.netif);
}
