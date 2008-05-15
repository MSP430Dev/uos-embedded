#include <net/netif.h>
#include <buf/buf-queue.h>
#include <buf/buf-prio.h>

#ifndef HDLC_STACKSZ
#   define HDLC_STACKSZ		0x200
#endif

struct _mem_pool_t;
struct _buf_t;

typedef struct _hdlc_desc_t {
	unsigned long data;		/* buffer data pointer */
#define HOWNER_DMA	0x80000000	/* ownership - dma */

	unsigned short control; 	/* for transmit only */
#define HCONTROL_P	0x0001		/* preamble */
#define HCONTROL_N	0x0002		/* no crc mode */
#define HCONTROL_E	0x0004		/* little-endian */
#define HCONTROL_L	0x0008		/* last buffer in the frame */
#define HCONTROL_D	0x0010		/* decrement data pointer */
#define HCONTROL_WA_NO	0x0000		/* widget alignment: no invalid bytes */
#define HCONTROL_WA_1	0x0020		/* 1 invalid byte */
#define HCONTROL_WA_2	0x0040		/* 2 invalid bytes */
#define HCONTROL_WA_3	0x0060		/* 3 invalid bytes */

	unsigned short reserved;

	unsigned short length;		/* buffer length */

	unsigned short status;
#define HSTATUS_RX_CD	0x0001		/* CD lost */
#define HSTATUS_RX_CE	0x0002		/* CRC error */
#define HSTATUS_RX_NO	0x0004		/* Non-octet aligned frame */
#define HSTATUS_RX_OV	0x0008		/* overrun */
#define HSTATUS_RX_DTM	0x0010		/* DPLL two miss clock */
#define HSTATUS_RX_ABT	0x0020		/* received frame aborted */
#define HSTATUS_RX_F	0x0040		/* first buffer in the frame */
#define HSTATUS_RX_L	0x0080		/* last buffer in the frame */
#define HSTATUS_RX_FLV	0x0100		/* received frame exceeds mtu */
#define HSTATUS_TX_T	0x0400		/* one frame complete (with L bit) */

	volatile struct _hdlc_desc_t *next;	/* next descriptor pointer */
} hdlc_desc_t;

typedef struct _hdlc_t {
	netif_t netif;			/* common network interface part */
	lock_t transmitter;
	struct _mem_pool_t *pool;	/* memory pool for allocating packets */
	buf_queue_t inq;		/* queue of received packets */
	struct _buf_t *inqdata[8];

#define HDLC_NTXQ	128		/* max size of transmit queue */
	buf_prio_queue_t outq;		/* queue of packets to transmit */
	struct _buf_t *outqdata[8*HDLC_NTXQ];

	int port;			/* port number */
	int hz;				/* oscillator frequency */

#define HDLC_NRBUF	32		/* number of receive buffers */
#define HDLC_NTBUF	8		/* number of transmit buffers */
	hdlc_desc_t rdesc_mem [HDLC_NRBUF];
	hdlc_desc_t tdesc_mem [HDLC_NTBUF];
	volatile hdlc_desc_t *rdesc;	/* non-cacheable pointers */
	volatile hdlc_desc_t *tdesc;	/* (A26 bit set) */
	struct _buf_t *rbuf [HDLC_NRBUF];
	struct _buf_t *tbuf [HDLC_NTBUF];

	unsigned tn;			/* first active transmit buffer */
	unsigned te;			/* first empty transmit buffer */
	unsigned rn;			/* first active receive buffer */

	unsigned long rintr;		/* receive interrupts */
	unsigned long tintr;		/* transmit interrupts */
	unsigned long underrun;		/* output underrun errors */
	unsigned long overrun;		/* input overrun errors */
	unsigned long frame;		/* input frame errors */
	unsigned long abort;
	unsigned long crc;		/* input crc errors */
	unsigned long error_storm;	/* sequential receive errors */

	unsigned long tx_big;
	unsigned long tx_nomem;
	unsigned long tx_qo;
	unsigned long rx_qo;
	unsigned long rx_abort;
	unsigned long rx_big;

	struct _buf_t *(*callback_receive) (struct _hdlc_t *, struct _buf_t *);
	struct _buf_t *(*callback_transmit) (struct _hdlc_t *, struct _buf_t *);
	void (*callback_error) (struct _hdlc_t *, uint_t error);

	OPACITY (rstack, HDLC_STACKSZ);	 /* task receive stack */
	OPACITY (tstack, HDLC_STACKSZ);	 /* task transmit stack */
} hdlc_t;

#define HDLC_OK		0
#define HDLC_ABORT	1
#define HDLC_FRAME	2
#define HDLC_CRC	3
#define HDLC_UNDERRUN	4
#define HDLC_OVERRUN	5
#define HDLC_RX_BIG	6
#define HDLC_TX_BIG	7
#define HDLC_MEM	8

void hdlc_init (hdlc_t *c, unsigned char port, const char *name, int rprio, int tprio,
	struct _mem_pool_t *pool, unsigned long hz);
void hdlc_set_baud (hdlc_t *c, unsigned long bps);
void hdlc_set_loop (hdlc_t *c, int on);
void hdlc_set_txcinv (hdlc_t *c, int on);
int hdlc_transmit_space (hdlc_t *c);
void hdlc_set_dtr (hdlc_t *c, int on);
void hdlc_set_rts (hdlc_t *c, int on);
int hdlc_get_dtr (hdlc_t *c);
int hdlc_get_rts (hdlc_t *c);
int hdlc_get_cts (hdlc_t *c);
int hdlc_get_dcd (hdlc_t *c);
void hdlc_receiver_enable_irq (hdlc_t *c, int on);
void hdlc_kick_tx (hdlc_t *c, bool_t force);
void hdlc_poll_rx (hdlc_t *c);
void hdlc_poll_tx (hdlc_t *c);

inline extern bool_t hdlc_is_tx_queue_empty (hdlc_t *c)
{
	return (volatile int) (c->tn) == (volatile int) (c->te);
}
