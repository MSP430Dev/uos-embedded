/*
 * Testing IP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/ip.h>
#include <timer/timer.h>
#include <cs8900/cs8900.h>

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0x8000

mem_pool_t pool;
timer_t timer;
ip_t ip;
ARRAY (group, sizeof(lock_group_t) + 4 * sizeof(lock_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
arp_t *arp;
cs8900_t eth;
route_t route;
ARRAY (stack_poll, 0x100);	/* Задача: опрос по таймеру */

/*
 * Задача периодического опроса.
 */
void main_poll (void *data)
{
	for (;;) {
		cs8900_poll (&eth);
	}
}

void uos_init (void)
{
	lock_group_t *g;
	unsigned char my_ip[] = "\220\316\265\373";

	/* Baud 38400. */
	outb (((int) (KHZ * 1000L / 38400) + 8) / 16 - 1, UBRR);

	/* Установлена микросхема 62256 - имеем 32 килобайта памяти. */
	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);
	mem_init (&pool, RAM_START, RAM_END);

	arp = arp_init (arp_data, sizeof(arp_data), &ip);

	timer_init (&timer, KHZ, 10);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &eth.netif.lock);
	lock_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, arp, g);

	/*
	 * Create interface eth0 144.206.181.251 / 255.255.255.0
	 */
	/* Reset чипа CS8900A заведен на порт G3.
	 * Он в прямой логике, надо подать туда 0. */
	outb_far (0x08, DDRG);
	clearb_far (3, PORTG);

	/* Добавляем один wait state, т.к. иначе cs8900 не успевает. */
	setb (SRW, MCUCR);
	cs8900_init (&eth, "eth0", 80, &pool, arp);

	route_add_netif (&ip, &route, my_ip, 24, &eth.netif);

	task_create (main_poll, 0, "poll", 5, stack_poll, sizeof (stack_poll));

	/* Define an address of chip CS8900. */
	ASSIGN_VIRTUAL_ADDRESS (device_cs8900, 0x7f00);
}
