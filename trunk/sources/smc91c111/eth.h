#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef SMC91C111_STACKSZ
#   define SMC91C111_STACKSZ	2000
#endif

struct _mem_pool_t;
struct _stream_t *stream;

typedef struct _smc91c111_t {
	netif_t netif;			/* common network interface part */

	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	buf_queue_t outq;		/* queue of packets to transmit */
	struct _buf_t *outqdata[8];

	unsigned bank;			/* current bank of chip registers */
	unsigned next_packet_ptr;	/* next receive packet address */

	unsigned long intr;		/* interrupt counter */

	ARRAY (stack, SMC91C111_STACKSZ); /* stack for interrupt task */
} smc91c111_t;

void smc91c111_init (smc91c111_t *u, const char *name, int prio,
	struct _mem_pool_t *pool, arp_t *arp);
void smc91c111_debug (smc91c111_t *u, struct _stream_t *stream);
void smc91c111_start_negotiation (smc91c111_t *u);
int smc91c111_get_carrier (smc91c111_t *u);
long smc91c111_get_speed (smc91c111_t *u, int *duplex);
void smc91c111_set_loop (smc91c111_t *u, int on);
void smc91c111_set_promisc (smc91c111_t *u, int station, int group);
void smc91c111_poll (smc91c111_t *u);
