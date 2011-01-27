/*
 * Ethernet controller driver for Milandr 5600ВГ1.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#ifndef __5600BG1_ETH_H
#define __5600BG1_ETH_H

#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef K5600BG1_STACKSZ
#   define K5600BG1_STACKSZ	1000
#endif

struct _mem_pool_t;
struct _stream_t *stream;

typedef struct _k5600bg1_t {
	netif_t netif;			/* common network interface part */
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	buf_queue_t outq;		/* queue of packets to transmit */
	struct _buf_t *outqdata[8];

	unsigned rn;			/* next receive descriptor number */
	unsigned intr_flags;		/* interrupt flags */
	unsigned long intr;		/* interrupt counter */

	ARRAY (stack, K5600BG1_STACKSZ); /* stack for irq task */
} k5600bg1_t;

void k5600bg1_init (k5600bg1_t *u, const char *name, int prio,
	struct _mem_pool_t *pool, struct _arp_t *arp, const unsigned char *macaddr);
void k5600bg1_debug (k5600bg1_t *u, struct _stream_t *stream);
int k5600bg1_get_carrier (k5600bg1_t *u);
long k5600bg1_get_speed (k5600bg1_t *u, int *duplex);
void k5600bg1_set_loop (k5600bg1_t *u, int on);
void k5600bg1_set_promisc (k5600bg1_t *u, int station, int group);
void k5600bg1_poll (k5600bg1_t *u);

#endif /* __5600BG1_ETH_H */
