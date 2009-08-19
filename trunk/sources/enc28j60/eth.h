#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef ENC28J60_STACKSZ
#   define ENC28J60_STACKSZ	0x180
#endif

struct _mem_pool_t;

typedef struct _enc28j60_t {
	netif_t netif;			/* common network interface part */

	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	unsigned bank;			/* current bank of chip registers */
	unsigned next_packet_ptr;	/* next receive packet address */

	ARRAY (stack, ENC28J60_STACKSZ); /* stack for interrupt task */
} enc28j60_t;

void enc28j60_init (enc28j60_t *u, const char *name, int prio,
	struct _mem_pool_t *pool, arp_t *arp);
