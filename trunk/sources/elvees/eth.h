/*
 * Ethernet controller driver for Elvees Multicore NVCom.
 * Copyright (c) 2010 Serge Vakulenko.
 * Based on sources from Ildar F Kaibyshev skif@elvees.com.
 */
#ifndef __NVCOM_ETH_H
#define __NVCOM_ETH_H

#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef ETH_STACKSZ
#   define ETH_STACKSZ	1000
#endif

#define ETH_MTU		1518		/* maximum ethernet frame length */

struct _mem_pool_t;
struct _stream_t *stream;

typedef struct _eth_t {
	netif_t netif;			/* common network interface part */
	mutex_t tx_lock;		/* get tx interrupts here */
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[16];

	buf_queue_t outq;		/* queue of packets to transmit */
	struct _buf_t *outqdata[8];

	unsigned phy;			/* address of external PHY */
	unsigned long intr;		/* interrupt counter */
	unsigned char rxbuf_data [ETH_MTU + 8];
	unsigned char txbuf_data [ETH_MTU + 8];
	unsigned char *rxbuf;		/* aligned rxbuf[] */
	unsigned char *txbuf;		/* aligned txbuf[] */
	unsigned rxbuf_physaddr;	/* phys address of rxbuf[] */
	unsigned txbuf_physaddr;	/* phys address of txbuf[] */

	ARRAY (stack, ETH_STACKSZ);	/* stack for receive task */
	ARRAY (tstack, ETH_STACKSZ);	/* stack for transmit task */
} eth_t;

void eth_init (eth_t *u, const char *name, int prio,
	struct _mem_pool_t *pool, arp_t *arp, const unsigned char *macaddr);
void eth_debug (eth_t *u, struct _stream_t *stream);
void eth_start_negotiation (eth_t *u);
int eth_get_carrier (eth_t *u);
long eth_get_speed (eth_t *u, int *duplex);
void eth_set_loop (eth_t *u, int on);
void eth_set_promisc (eth_t *u, int station, int group);
void eth_poll (eth_t *u);

#endif /* __NVCOM_ETH_H */
