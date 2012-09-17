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
#include <elvees/eth.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_snmp, 1500);
ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));
timer_t timer;
mem_pool_t pool;
arp_t *arp;
ip_t ip;
eth_t eth_data, *eth = &eth_data;
route_t route;
route_t default_route;
snmp_t snmp;
udp_socket_t sock;

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

static const snmp_var_t snmp_tab [] = {
#include <snmp/snmp-vardef.h>
	SYSTEM_VARIABLE_LIST
	IF_VARIABLE_LIST
	IP_VARIABLE_LIST
	ICMP_VARIABLE_LIST
/*	TCP_VARIABLE_LIST*/
	UDP_VARIABLE_LIST
	SNMP_VARIABLE_LIST
};

void snmp_task (void *data)
{
	buf_t *p, *r;
	unsigned char addr [4], *output;
	unsigned short port;

	udp_socket (&sock, &ip, 161);
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

		output = snmp_execute (&snmp, p->payload, p->len,
			r->payload, r->len);
		buf_free (p);
		if (! output) {
			buf_free (r);
			continue;
		}

		buf_add_header (r, - (output - r->payload));
		udp_sendto (&sock, r, addr, port);
	}
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (4);		/* Wait states  */

#if 0
	/* Используем внешнюю память SRAM. */
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
	mutex_group_add (g, &eth->netif.lock);
	mutex_group_add (g, &timer.decisec);

	arp = arp_init (arp_data, sizeof(arp_data), &ip);
	ip_init (&ip, &pool, 70, &timer, arp, g);

	/*
	 * Create interface eth0
	 */
	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (eth, "eth0", 80, &pool, arp, my_macaddr);

	unsigned char my_ip[] = { 192, 168, 20, 222 };
	route_add_netif (&ip, &route, my_ip, 24, &eth->netif);

	/*
	 * Add default route to 192.168.20.254
	 */
	unsigned char zero[] = { 0, 0, 0, 0 };
	unsigned char gateway[] = { 192, 168, 20, 254 };
	route_add_gateway (&ip, &default_route, zero, 0, gateway);
	if (! default_route.netif)
		debug_printf ("test-snmp: no interface for default route!\n");

	snmp_init (&snmp, &pool, &ip, snmp_tab, sizeof(snmp_tab),
		20520, SNMP_SERVICE_REPEATER, "Testing SNMP",
		"1.3.6.1.4.1.20520.1.1", "Test", "1.3.6.1.4.1.20520.6.1");

	task_create (snmp_task, 0, "snmp", 5,
		stack_snmp, sizeof (stack_snmp));
}
