#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef TAP_STACKSZ
#   if LINUX386
#      define TAP_STACKSZ	4000
#   endif
#endif

struct _mem_pool_t;

typedef struct _tap_t {
	netif_t netif;			/* common network interface part */
	OPACITY (stack, TAP_STACKSZ);	/* task receive stack */
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */
	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	/* Add whatever per-interface state that is needed here. */
	int pid;
	int fd;
} tap_t;

void tap_init (tap_t *u, char *name, int prio, struct _mem_pool_t *pool,
	arp_t *arp);
