#ifndef __NETIF_H_
#define	__NETIF_H_ 1

#include <net/arp.h>

struct _buf_t;

/*
 * Интерфейс к драйверу сетевого адаптера (Etnernet, SLIP и прочее).
 * Состоит из четырех процедур: передача пакета,
 * регистрация обработчика принятых пакетов,
 * опрос MAC-адреса, установка MAC-адреса.
 */
typedef struct _netif_t {
	lock_t lock;
	struct _netif_interface_t *interface;

	const char *name;
	arp_t *arp;
	unsigned char ethaddr [6];	/* MAC-адрес */
	unsigned short mtu;		/* max packet length */
	unsigned char type;		/* SNMP-compatible */
	unsigned long bps;		/* speed in bits per second */
	unsigned short out_qlen;	/* number of packets in output queue */

	/* Statistics. */
	unsigned long in_bytes;
	unsigned long in_packets;
	unsigned long in_mcast_pkts;
	unsigned long in_errors;
	unsigned long in_discards;
	unsigned long in_unknown_protos;

	unsigned long out_bytes;
	unsigned long out_packets;
	unsigned long out_mcast_pkts;	/* not implemented yet */
	unsigned long out_errors;
	unsigned long out_discards;
	unsigned long out_collisions;
} netif_t;

/*
 * SNMP-compatible type of interface.
 */
#define NETIF_OTHER			1	/* none of the following */
#define NETIF_REGULAR1822		2
#define NETIF_HDH1822			3
#define NETIF_DDN_X25			4
#define NETIF_RFC877_X25		5
#define NETIF_ETHERNET_CSMACD		6
#define NETIF_ISO88023_CSMACD		7
#define NETIF_ISO88024_TOKENBUS		8
#define NETIF_ISO88025_TOKENRING	9
#define NETIF_ISO88026_MAN		10
#define NETIF_STARLAN			11
#define NETIF_PROTEON_10MBIT		12
#define NETIF_PROTEON_80MBIT		13
#define NETIF_HYPERCHANNEL		14
#define NETIF_FDDI			15
#define NETIF_LAPB			16
#define NETIF_SDLC			17
#define NETIF_DS1			18	/* T-1 */
#define NETIF_E1			19	/* european equiv. of T-1 */
#define NETIF_BASICISDN			20
#define NETIF_PRIMARYISDN		21	/* proprietary serial */
#define NETIF_PROPPOINTTOPOINTSERIAL	22
#define NETIF_PPP			23
#define NETIF_SOFTWARELOOPBACK		24
#define NETIF_EON			25	/* CLNP over IP [11] */
#define NETIF_ETHERNET_3MBIT		26
#define NETIF_NSIP			27	/* XNS over IP */
#define NETIF_SLIP			28	/* generic SLIP */
#define NETIF_ULTRA			29	/* ULTRA technologies */
#define NETIF_DS3			30	/* T-3 */
#define NETIF_SIP			31	/* SMDS */
#define NETIF_FRAME_RELAY		32

typedef struct _netif_interface_t {
	/* Передача пакета. */
	bool_t (*output) (netif_t *u, struct _buf_t *p,
		small_uint_t prio);

	/* Выборка пакета из очереди приема. */
	struct _buf_t *(*input) (netif_t *u);

	/* Установка MAC-адреса. */
	void (*set_address) (netif_t *u, unsigned char *address);
} netif_interface_t;

bool_t netif_output (netif_t *netif, struct _buf_t *p,
	unsigned char *ipdest, unsigned char *ipsrc);
bool_t netif_output_prio (netif_t *netif, struct _buf_t *p,
	unsigned char *ipdest, unsigned char *ipsrc, small_uint_t prio);
struct _buf_t *netif_input (netif_t *netif);
void netif_set_address (netif_t *netif, unsigned char *ethaddr);

#endif /* !__NETIF_H_ */
