#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef RTL8139_STACKSZ
#   if __AVR__
#      define RTL8139_STACKSZ	0x180
#   endif
#   if __thumb__
#      define RTL8139_STACKSZ	0x180
#   endif
#endif

struct _mem_pool_t;

typedef struct _rtl8139_t {
	netif_t netif;			/* common network interface part */

	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	char stack [RTL8139_STACKSZ];	/* task receive stack */
} rtl8139_t;

void rtl8139_init (rtl8139_t *u, char *name, int prio, struct _mem_pool_t *pool,
	arp_t *arp);
unsigned char rtl8139_probe (void);
