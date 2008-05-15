/*
 * Testing SNMP protocol.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/arp.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/udp.h>
#include <snmp/snmp.h>
#include <timer/timer.h>
#include <cs8900/cs8900.h>

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0x8000

mem_pool_t pool;
timer_t timer;
ip_t *ip;
char group [sizeof(lock_group_t) + 4 * sizeof(lock_slot_t)];
char arp_data [sizeof(arp_t) + 10 * sizeof(arp_entry_t)];
arp_t *arp;
cs8900_t eth;
route_t route;
route_t default_route;
snmp_t snmp;
udp_socket_t sock;
char task [0x180];
char stack_poll [0x100];	/* Задача: опрос по таймеру */

/*
 * Declare get/getnext/set functions.
 */
#include <snmp/snmp-var.h>
#include <snmp/snmp-system.h>
#include <snmp/snmp-netif.h>
#include <snmp/snmp-ip.h>
#include <snmp/snmp-snmp.h>
#include <snmp/snmp-icmp.h>
#include <snmp/snmp-udp.h>

#include <snmp/snmp-vardecl.h>
SYSTEM_VARIABLE_LIST
IF_VARIABLE_LIST
IP_VARIABLE_LIST
ICMP_VARIABLE_LIST
/*TCP_VARIABLE_LIST*/
UDP_VARIABLE_LIST
SNMP_VARIABLE_LIST

snmp_var_t snmp_tab [] __flash__ = {
#include <snmp/snmp-vardef.h>
	SYSTEM_VARIABLE_LIST
	IF_VARIABLE_LIST
	IP_VARIABLE_LIST
	ICMP_VARIABLE_LIST
/*	TCP_VARIABLE_LIST*/
	UDP_VARIABLE_LIST
	SNMP_VARIABLE_LIST
};

void main_task (void *data)
{
	buf_t *p, *r;
	unsigned char addr [4], *output;
	unsigned short port;
#if 1
	lock_group_t *g;

	debug_printf ("\n\n*** Testing SNMP on UART 1 ***\n\n");

	arp = arp_init (arp_data, sizeof(arp_data), 0);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &eth.netif.lock);
	lock_group_add (g, &timer.decisec);
	ip = mem_alloc (&pool, sizeof (*ip));
	ip_init (ip, &pool, 70, &timer, arp, g);
	arp->ip = ip;

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
	route_add_netif (ip, &route, "\220\316\265\373", 24, &eth.netif);

	/*
	 * Add default route to 144.206.181.254
	 */
	route_add_gateway (ip, &default_route, "\0\0\0\0", 0, "\220\316\265\376");
	if (! default_route.netif)
		debug_printf ("test_snmp: no interface for default route!\n");

	snmp_init (&snmp, &pool, ip, snmp_tab, sizeof(snmp_tab),
		SNMP_SERVICE_REPEATER, "Testing SNMP",
		"1.3.6.1.4.1.20520.1.1");

#endif
	udp_socket (&sock, ip, 161);
	debug_printf ("test_snmp: mem available = %u bytes\n",
		mem_available (&pool));

	for (;;) {
		p = udp_recvfrom (&sock, addr, &port);
		debug_printf ("test_snmp: mem available = %u bytes\n",
			mem_available (&pool));

		r = buf_alloc (&pool, 1500, 50);
		if (! r) {
			debug_printf ("test_snmp: out of memory!\n");
			buf_free (p);
			continue;
		}

		output = snmp_execute (&snmp, p->payload, p->len, r->payload,
			r->len, addr);
		buf_free (p);
		if (! output) {
			buf_free (r);
			continue;
		}

		buf_add_header (r, - (output - r->payload));
		udp_sendto (&sock, r, addr, port);
	}
}

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
/* Baud 38400. */
outb (((int) (KHZ * 1000L / 38400) + 8) / 16 - 1, UBRR);

	/* Установлена микросхема 62256 - имеем 32 килобайта памяти. */
	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);
	mem_init (&pool, RAM_START, RAM_END);

	timer_init (&timer, 100, KHZ, 10);

	task_create (main_task, 0, "main", 5, task, sizeof (task));
	task_create (main_poll, 0, "poll", 1, stack_poll, sizeof (stack_poll));
}
