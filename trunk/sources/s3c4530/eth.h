#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef ETH_STACKSZ
#   define ETH_STACKSZ		0x200
#endif

struct _mem_pool_t;
struct _buf_t;
struct _stream_t;

typedef struct _eth_desc_t {
	unsigned long data;		/* buffer data pointer */
#define EOWNER_DMA	0x80000000	/* ownership - dma */

	unsigned long control;		/* for transmit only */
#define ECONTROL_P	0x0001		/* no-padding mode */
#define ECONTROL_C	0x0002		/* no crc mode */
#define ECONTROL_T	0x0004		/* enable interrupt */
#define ECONTROL_L	0x0008		/* little-endian */
#define ECONTROL_A	0x0010		/* increment data pointer */
#define ECONTROL_WA_NO	0x0000		/* widget alignment: no invalid bytes */
#define ECONTROL_WA_1	0x0020		/* 1 invalid byte */
#define ECONTROL_WA_2	0x0040		/* 2 invalid bytes */
#define ECONTROL_WA_3	0x0060		/* 3 invalid bytes */

	unsigned short length;		/* buffer length */

	unsigned short status;
#define ESTAT_RX_OVMAX		0x0008		/* over maximum size */
#define ESTAT_RX_CTLRCV		0x0020		/* control received */
#define ESTAT_RX_INT		0x0040		/* interrupt on receive */
#define ESTAT_RX_10STAT		0x0080		/* receive 10 Mb/s status */
#define ESTAT_RX_ALIGNERR	0x0100		/* alignment error */
#define ESTAT_RX_CRCERR		0x0200		/* crc error */
#define ESTAT_RX_OVERFLOW	0x0400		/* overflow error */
#define ESTAT_RX_LONGERR	0x0800		/* frame length >1518 bytes */
#define ESTAT_RX_PAR		0x2000		/* receive parity error */
#define ESTAT_RX_GOOD		0x4000		/* good received */
#define ESTAT_RX_HALTED		0x8000		/* reception halted */

#define ESTAT_TX_COLLCNT	0x000f		/* collision count */
#define ESTAT_TX_EXCOLL		0x0010		/* excessive collisions */
#define ESTAT_TX_TXDEFER	0x0020		/* transmit deferred */
#define ESTAT_TX_PAUSED		0x0040		/* paused */
#define ESTAT_TX_INT		0x0080		/* interrupt on transmit */
#define ESTAT_TX_UNDER		0x0100		/* underrun */
#define ESTAT_TX_DEFER		0x0200		/* deferral */
#define ESTAT_TX_NCARR		0x0400		/* no carrier */
#define ESTAT_TX_SQERR		0x0800		/* SQE error */
#define ESTAT_TX_LATECOLL	0x1000		/* late collision */
#define ESTAT_TX_PAR		0x2000		/* transmit parity error */
#define ESTAT_TX_COMP		0x4000		/* completion */
#define ESTAT_TX_HALTED		0x8000		/* transmission halted */

	volatile struct _eth_desc_t *next;	/* next descriptor pointer */
} eth_desc_t;

typedef struct _eth_t {
	netif_t netif;			/* common network interface part */
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */
	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];	/* queue of received packets */
	mutex_t transmitter;

#define ETH_NRBUF	64		/* number of receive buffers */
#define ETH_NTBUF	64		/* number of transmit buffers */
	eth_desc_t rdesc_mem [ETH_NRBUF];
	eth_desc_t tdesc_mem [ETH_NTBUF];
	volatile eth_desc_t *rdesc;	/* non-cacheable pointers */
	volatile eth_desc_t *tdesc;	/* (A26 bit set) */
	struct _buf_t *rbuf [ETH_NRBUF];
	struct _buf_t *tbuf [ETH_NTBUF];

	unsigned tn;			/* first active transmit buffer */
	unsigned te;			/* first empty transmit buffer */
	unsigned rn;			/* first active receive buffer */

	unsigned long rintr;		/* receive interrupts */
	unsigned long tintr;		/* transmit interrupts */
	unsigned long underrun;		/* output underrun errors */
	unsigned long overrun;		/* input overrun errors */
	unsigned long frame;		/* input parity/alignment errors */
	unsigned long crc;		/* input crc errors */

	unsigned long tx_big;
	unsigned long tx_nomem;
	unsigned long tx_lc;
	unsigned long tx_qo;
	unsigned long rx_qo;
	unsigned long rx_big;

	unsigned long phy_addr;		/* PHY device address */

	struct _buf_t *(*callback_receive) (struct _eth_t *, struct _buf_t *);
	struct _buf_t *(*callback_transmit) (struct _eth_t *, struct _buf_t *);
	void (*callback_error) (struct _eth_t *);
	void (*callback_collision) (struct _eth_t *, int);

	ARRAY (rstack, ETH_STACKSZ);	/* task receive stack */
	ARRAY (tstack, ETH_STACKSZ);	/* task transmit stack */
} eth_t;

void eth_init (eth_t *c, const char *name, int rprio, int tprio,
	struct _mem_pool_t *pool, arp_t *arp);
int eth_transmit_space (eth_t *c);
void eth_set_mode (eth_t *c, int speed100, int fdx, int disneg);
void eth_start_negotiation (eth_t *c);
int eth_get_carrier (eth_t *c);
int eth_get_speed (eth_t *c, int *duplex);
void eth_set_loop (eth_t *c, int on);
void eth_set_promisc (eth_t *c, int station, int group);
void eth_debug (eth_t *c, struct _stream_t *stream);
void eth_kick_tx (eth_t *c, bool_t force);
void eth_poll_rx (eth_t *c);
void eth_poll_tx (eth_t *c);

inline extern bool_t eth_is_tx_queue_empty (eth_t *c)
{
	return (volatile int) (c->tn) == (volatile int) (c->te);
}
