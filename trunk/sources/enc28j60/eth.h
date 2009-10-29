#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef ENC28J60_STACKSZ
#   define ENC28J60_STACKSZ	200
#endif

struct _mem_pool_t;
struct _stream_t *stream;

typedef struct _enc28j60_t {
	netif_t netif;			/* common network interface part */

	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	buf_queue_t outq;		/* queue of packets to transmit */
	struct _buf_t *outqdata[8];

	unsigned bank;			/* current bank of chip registers */
	unsigned next_packet_ptr;	/* next receive packet address */

	ARRAY (stack, ENC28J60_STACKSZ); /* stack for interrupt task */
} enc28j60_t;

void enc28j60_init (enc28j60_t *u, const char *name, int prio,
	struct _mem_pool_t *pool, arp_t *arp);
void enc28j60_debug (enc28j60_t *u, struct _stream_t *stream);
void enc28j60_start_negotiation (enc28j60_t *u);
int enc28j60_get_carrier (enc28j60_t *u);
long enc28j60_get_speed (enc28j60_t *u, int *duplex);
void enc28j60_set_loop (enc28j60_t *u, int on);
void enc28j60_set_promisc (enc28j60_t *u, int station, int group);
