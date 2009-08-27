#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef SLIP_STACKSZ
#   if __AVR__
#      define SLIP_STACKSZ	0x180		/* 100 enough for AVR */
#   endif
#   if defined (__arm__) || defined (__thumb__)
#      define SLIP_STACKSZ	0x180
#   endif
#   if MIPS32
#      define SLIP_STACKSZ	0x400
#   endif
#   if MSP430
#      define SLIP_STACKSZ	0x180
#   endif
#   if LINUX386
#      define SLIP_STACKSZ	4000
#   endif
#endif
#define SLIP_INBUFSZ	8

#define SLIP_OUTBUFSZ	32

typedef struct _slip_t {
	netif_t netif;			/* common network interface part */
	mutex_t transmitter;
	small_uint_t port;
	unsigned int khz;
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */

	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

	buf_queue_t outq;		/* queue of packets to transmit */
        struct _buf_t *outqdata[8];

	struct _buf_t *in;		/* packet currently being received */
	unsigned char *in_ptr, *in_limit;
	unsigned char in_escape;

	struct _buf_t *out;		/* packet currently in transmit */
	struct _buf_t *outseg;		/* segment currently in transmit */
	struct _buf_t *out_free;	/* already transmitted packet */
	unsigned char *out_first, *out_limit;
	unsigned char out_flag;		/* need to transmit flag */

	bool_t (*cts_query) (struct _slip_t*);

	ARRAY (rstack, SLIP_STACKSZ);	/* task receive stack */
	ARRAY (tstack, SLIP_STACKSZ);	/* task receive stack */
} slip_t;

void slip_init (slip_t *u, small_uint_t port, const char *name, int prio,
	mem_pool_t *pool, unsigned int khz, unsigned long baud);
void slip_set_cts_poller (slip_t *u, bool_t (*) (slip_t*));
void slip_cts_ready (slip_t *u);
struct _buf_t *slip_recv (slip_t *u);
bool_t slip_send (slip_t *u, struct _buf_t *p);
