#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef CS8900_STACKSZ
#	if __AVR__
#		define CS8900_STACKSZ	0x180
#	endif
#	if __thumb__
#		define CS8900_STACKSZ	0x180
#	endif
#endif

struct _mem_pool_t;

typedef struct _cs8900_t {
	netif_t netif;			/* common network interface part */

	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	unsigned char stack [CS8900_STACKSZ];	/* task receive stack */
} cs8900_t;

void cs8900_init (cs8900_t *u, const char *name, int prio, struct _mem_pool_t *pool,
	arp_t *arp);
void cs8900_poll (cs8900_t *u);
bool_t cs8900_probe (void);

void cs8900_test_bus (void);
void cs8900_full_test_bus (void);
