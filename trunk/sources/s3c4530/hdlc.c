#include "runtime/lib.h"
#include "kernel/uos.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "s3c4530/hdlc.h"

#define HDLC_TRANSMIT_IRQ(p)	((p) ? 14 : 12) /* transmit interrupt */
#define HDLC_RECEIVE_IRQ(p)	((p) ? 15 : 13) /* receive interrupt */

/*
 * Maximum packet size.
 * 1514 - traditional value: 1500 for IP and 14 for Ethernet header.
 * 1518 - for VLAN support: extra 4 bytes for VLAN tag.
 * 4224 - for NetWare 5.x
 * 9216 - Cisco "Jumbo" packets
 */
#define HDLC_MTU		4224
#define HDLC_EXTRA		4		/* extra space for Rx DMA */

/*
 * Compute and set the value of HBRGTC register.
 */
static void
hdlc_set_brg (hdlc_t *c, unsigned long bps)
{
	unsigned long cnt, div;

	if (bps == 0) {
		/* External clock - set max baud rate. */
		ARM_HBRGTC (c->port) = 0;
		c->netif.bps = 0;
		return;
	}
	cnt = (c->hz / bps + 1) / 2;
	if (cnt <= 0x1000) {
		div = 0;
		c->netif.bps = c->hz / 2 / cnt;
	} else if (cnt <= 0x10000) {
		div = ARM_HBRGTC_DIV_16;
		cnt = (cnt >> 4);
		c->netif.bps = c->hz / 2 / 16 / cnt;
	} else if (cnt <= 0x20000) {
		div = ARM_HBRGTC_DIV_32;
		cnt = (cnt >> 5);
		c->netif.bps = c->hz / 2 / 32 / cnt;
	} else if (cnt <= 0x100000) {
		div = ARM_HBRGTC_DIV_256;
		cnt = (cnt >> 8);
		c->netif.bps = c->hz / 2 / 256 / cnt;
	} else {
		div = ARM_HBRGTC_DIV_512;
		cnt = (cnt >> 9);
		if (cnt > 0x1000) {
			/* Too low baud rate, cannot handle. */
			cnt = 0x1000;
		}
		c->netif.bps = c->hz / 2 / 512 / cnt;
	}
	ARM_HBRGTC (c->port) = ARM_HBRGTC_CNT0 (cnt - 1) | div;
}

/*
 * Set up baud rate.
 * This function must be called from user level only.
 */
void
hdlc_set_baud (hdlc_t *c, unsigned long bps)
{
	unsigned long hmode;

	lock_take (&c->netif.lock);

	/* Set up baud rate generator. */
	hdlc_set_brg (c, bps);

	/* Set transmit clock source. */
	hmode = ARM_HMODE (c->port) & ~ARM_HMODE_TXCLK;
	if (bps == 0) {
		/* Use external clock TXC. */
		hmode |= ARM_HMODE_TXCLK_TXC;
	} else {
		/* Use internal baud rate generator. */
		hmode |= ARM_HMODE_TXCLK_BRGOUT2;
	}
	ARM_HMODE (c->port) = hmode;

	lock_release (&c->netif.lock);
}

/*
 * Set up baud rate.
 * This function must be called from user level only.
 */
void
hdlc_set_loop (hdlc_t *c, int on)
{
	unsigned long hmode;

	lock_take (&c->netif.lock);

	/* Set transmit/receive clock source. */
	hmode = ARM_HMODE (c->port) & ~(ARM_HMODE_TXCLK | ARM_HMODE_RXCLK);
	if (on) {
		/* Set Tx loop-back mode. */
		ARM_HCON (c->port) |= ARM_HCON_TXLOOP;

		if (c->netif.bps == 0) {
			/* Set up baud rate generator. */
			hdlc_set_brg (c, 64000);
		}

		/* Use internal baud rate generator for TXCLK and RXCLK. */
		hmode |= ARM_HMODE_TXCLK_BRGOUT2 | ARM_HMODE_RXCLK_BRGOUT2;
	} else {
		/* Disable Tx loop-back mode. */
		ARM_HCON (c->port) &= ~ARM_HCON_TXLOOP;

		/* For receive, use external clock RXC. */
		hmode |= ARM_HMODE_RXCLK_RXC;

		if (c->netif.bps == 0) {
			/* For transmit, use external clock TXC. */
			hmode |= ARM_HMODE_TXCLK_TXC;
		} else {
			/* For transmit, use internal baud rate generator. */
			hmode |= ARM_HMODE_TXCLK_BRGOUT2;
		}
	}
	ARM_HMODE (c->port) = hmode;

	lock_release (&c->netif.lock);
}

/*
 * Invert TX clock.
 * This function must be called from user level only.
 */
void
hdlc_set_txcinv (hdlc_t *c, int on)
{
	unsigned long hmode;

	lock_take (&c->netif.lock);

	hmode = ARM_HMODE (c->port);
	if (on) {
		hmode |= ARM_HMODE_TXCINV;
	} else {
		hmode &= ~ARM_HMODE_TXCINV;
	}
	ARM_HMODE (c->port) = hmode;

	lock_release (&c->netif.lock);
}

/*
 * Return an amount of free packets available in transmit queue.
 */
int hdlc_transmit_space (hdlc_t *c)
{
	int space;

	lock_take (&c->transmitter);
	space = (HDLC_NTBUF + c->tn - c->te - 1) % HDLC_NTBUF;

	/* Transmitter could hang up -- kick it. */
	if (space == 0 && ! (c->tdesc[c->tn].data & HOWNER_DMA)) {
		/*debug_printf ("\n* kick tx\n");*/
		lock_signal (&c->transmitter, 0);
	}

	/* Count space of priority queue. */
	space += c->outq.size - c->outq.count;

	lock_release (&c->transmitter);
	return space;
}

static void
transmit_enqueue (hdlc_t *c, buf_t *p)
{
	volatile hdlc_desc_t *desc;

	c->tbuf[c->te] = p;

	/* Set up the tx descriptor. */
	desc = c->tdesc + c->te;
	desc->length = p->tot_len;
	desc->status = 0;

	/* When source data are not aligned on word boundary,
	 * we must skip 1, 2 or 3 heading bytes. */
	desc->control = HCONTROL_E | HCONTROL_L;
	switch ((int) p->payload & 3) {
	case 1:	desc->control |= HCONTROL_WA_1; break;
	case 2:	desc->control |= HCONTROL_WA_2; break;
	case 3:	desc->control |= HCONTROL_WA_3; break;
	}

	/* отдаем в пользование DMA в самом конце */
	desc->data = ((long) p->payload & ~3L) | HOWNER_DMA;
	arm_bus_yield ();
	/*debug_printf ("hdlc_output: enqueue #%d, tn=%d, %d bytes\n",
		c->te, c->tn, desc->length);*/

	c->te = (c->te + 1) % HDLC_NTBUF;
}

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
bool_t
hdlc_output (hdlc_t *c, buf_t *p, small_uint_t prio)
{
	int status;

	lock_take (&c->transmitter);

	if (p->tot_len <= 0 || p->tot_len > HDLC_MTU) {
		/* invalid packet length */
		/*debug_printf ("hdlc_output: invalid packet length %d bytes\n",
			p->tot_len);*/
		++c->netif.out_errors;
		++c->tx_big;
		lock_release (&c->transmitter);
		buf_free (p);
		if (c->callback_error)
			c->callback_error (c, HDLC_TX_BIG);
		return 0;
	}

	/* Convert to a single buffer */
	p = buf_make_continuous (p);
	if (! p) {
		lock_release (&c->transmitter);
		/*debug_printf ("hdlc_output: cannot make continuous packet\n");*/
		++c->netif.out_errors;
		++c->tx_nomem;
		if (c->callback_error)
			c->callback_error (c, HDLC_MEM);
		return 0;
	}

	if ((c->te + 1) % HDLC_NTBUF == c->tn) {
		/* Нет места в очереди устройства.
		 * Ставим в очередь приоритетов. */
		status = buf_prio_queue_put (&c->outq, p, prio);
		if (status != 1)
			++c->tx_qo;
		lock_release (&c->transmitter);
		if (status == 0) {
			/* No space in queue. */
			/*debug_printf ("hdlc_output: no free space, te=%d, tn=%d\n",
				c->te, c->tn);*/
			buf_free (p);
		}
		return status;
	}
	/* Есть место в очереди устройства. Поставим новый
	 * пакет в очередь приоритетов и выберем оттуда старый. */
	p = buf_prio_queue_exchange (&c->outq, p, prio);
	transmit_enqueue (c, p);

	if (! (ARM_HCON (c->port) & ARM_HCON_DTXEN)) {
		/* Start the transmitter. */
		ARM_HDMATXPTR (c->port) = (long) (c->tdesc + c->tn);
		ARM_HCON (c->port) |= ARM_HCON_DTXEN;	/* DMA Tx enable */
		/*debug_printf ("hdlc_output: start tx\n");*/
		arm_bus_yield ();
	}
	lock_release (&c->transmitter);
	return 1;
}

static buf_t *
hdlc_input (hdlc_t *c)
{
	buf_t *p;

	lock_take (&c->netif.lock);
	p = buf_queue_get (&c->inq);
	lock_release (&c->netif.lock);
	return p;
}

/*
 * Fetch and process the received packet from the network controller.
 */
static buf_t *
hdlc_receive_data (hdlc_t *c, buf_t *p, int len)
{
	buf_t *q;

	if (buf_queue_is_full (&c->inq)) {
		/*debug_printf ("hdlc_receive_data: input overflow\n");*/
		++c->netif.in_discards;
		return p;
	}

	/* Allocate a buf chain with total length 'len' */
	q = buf_alloc (c->pool, HDLC_MTU + HDLC_EXTRA, 16);
	if (! q) {
		/* Could not allocate a buf - skip received frame */
		/*debug_printf ("hdlc_receive_data: ignore packet - out of memory\n");*/
		++c->netif.in_discards;
		return p;
	}
	q = (buf_t*) ARM_NONCACHED (q);
	q->payload = (unsigned char*) ARM_NONCACHED (q->payload);

	/* Get received packet. */
	buf_truncate (p, len);
	if (c->callback_receive)
		p = c->callback_receive (c, p);
	if (p)
		buf_queue_put (&c->inq, p);
	return q;
}

static void
hdlc_receive_done (hdlc_t *c)
{
	unsigned long st;
	int len;

	st = c->rdesc[c->rn].status;
	len = c->rdesc[c->rn].length - 2;
	/*debug_printf ("* received frame #%d, %d bytes, status 0x%x\n",
		c->rn, len, st);*/
	if (st & HSTATUS_RX_OV) {
		/* Receive overrun error */
		++c->overrun;
		if (c->callback_error)
			c->callback_error (c, HDLC_OVERRUN);
error:
		++c->netif.in_errors;

		if (c->error_storm > 20) {
			/* Disable Rx interrupts. */
			ARM_HINTEN (c->port) &= ~(ARM_HINTEN_RXABTIE |
				ARM_HINTEN_RXCRCEIE | ARM_HINTEN_RXNOIE |
				ARM_HINTEN_RXOVIE | ARM_HINTEN_RXMOVIE |
				ARM_HINTEN_DRXFDIE);
		} else
			++c->error_storm;

	} else if (st & HSTATUS_RX_NO) { /* non-octet aligned */
		++c->frame;
		if (c->callback_error)
			c->callback_error (c, HDLC_FRAME);
		goto error;

	} else if (st & HSTATUS_RX_ABT) { /* frame aborted */
		++c->rx_abort;
		if (c->callback_error)
			c->callback_error (c, HDLC_ABORT);
		goto error;

	} else if (! (st & HSTATUS_RX_F) || ! (st & HSTATUS_RX_L)) {	/* Too long frame */
		++c->rx_big;
		if (c->callback_error)
			c->callback_error (c, HDLC_RX_BIG);
		goto error;

	} else if (st & HSTATUS_RX_CE) {
		/* Receive CRC error */
		++c->crc;
		if (c->callback_error)
			c->callback_error (c, HDLC_CRC);
		goto error;

	} else if (len > HDLC_MTU) {
		/* Too long frame - skip it */
		++c->rx_big;
		if (c->callback_error)
			c->callback_error (c, HDLC_RX_BIG);
		goto error;

	} else {
		/* Valid packet */
		++c->netif.in_packets;
		c->netif.in_bytes += len;
		c->rbuf[c->rn] = hdlc_receive_data (c, c->rbuf[c->rn], len);

		if (c->error_storm) {
			/* Enable Rx interrupts. */
			ARM_HINTEN (c->port) |= (ARM_HINTEN_RXABTIE |
				ARM_HINTEN_RXCRCEIE | ARM_HINTEN_RXNOIE |
				ARM_HINTEN_RXOVIE | ARM_HINTEN_RXMOVIE |
				ARM_HINTEN_DRXFDIE);
			c->error_storm = 0;
		}
	}
	c->rdesc[c->rn].length = HDLC_MTU+2;
	c->rdesc[c->rn].status = 0;
	/* отдаем в пользование DMA в самом конце */
	c->rdesc[c->rn].data = (long) c->rbuf[c->rn]->payload | HOWNER_DMA;
	arm_bus_yield ();

	if (! (c->rdesc[(c->rn - 1) % HDLC_NRBUF].data & HOWNER_DMA))
		c->rx_qo++; /* LY: считаем переполнением ситуацию,
				   когда в rx-очереди нет пустых элементов. */
	c->rn = (c->rn + 1) % HDLC_NRBUF;
}

/*
 * Process a pending interrupt.
 */
static void
hdlc_handle_receive (hdlc_t *c, small_int_t limit)
{
	unsigned long st;

	st = ARM_HSTAT (c->port) &
		(ARM_HSTAT_RXABT |	/* Rx abort */
		ARM_HSTAT_RXCRCE |	/* Rx CRC error */
		ARM_HSTAT_RXNO |	/* Rx non-octet align */
		ARM_HSTAT_RXOV |	/* Rx overrun */
		ARM_HSTAT_RXMOV |	/* DMA Rx memory overflow */
		ARM_HSTAT_RXIERR |	/* Rx internal error */
		ARM_HSTAT_DRXFD |	/* DMA Rx frame done */
		ARM_HSTAT_DRXNL |	/* DMA Rx null list */
		ARM_HSTAT_DRXNO |	/* DMA Rx not owner */
		ARM_HSTAT_DPLLOM |	/* DPLL one clock missing */
		ARM_HSTAT_DPLLTM);	/* DPLL two clocks missing */
	if (st) {
		ARM_HSTAT (c->port) = st;

		if (st & ARM_HSTAT_RXABT) {
			/*debug_printf ("* Rx abort\n");*/
			++c->rx_abort;
			++c->netif.in_errors;
			if (c->callback_error)
				c->callback_error (c, HDLC_ABORT);
		}
		/* if (st & ARM_HSTAT_RXCRCE)
			debug_printf ("* Rx CRC error\n"); */
		/* if (st & ARM_HSTAT_RXNO)
			debug_printf ("* Rx non-octet align\n"); */
		/* if (st & ARM_HSTAT_RXOV)
		debug_printf ("* Rx overrun\n"); */
		/*if (st & ARM_HSTAT_RXMOV)
			debug_printf ("* DMA Rx memory overflow\n");*/
		/*if (st & ARM_HSTAT_RXIERR)
			debug_printf ("* Rx internal error\n");*/
		/*if (st & ARM_HSTAT_DRXNL)
			debug_printf ("* DMA Rx null list\n");*/
		/*if (st & ARM_HSTAT_DPLLOM)
			debug_printf ("* DPLL one clock missing\n");*/
		/*if (st & ARM_HSTAT_DPLLTM)
			debug_printf ("* DPLL two clocks missing\n");*/
	}

	if (1 /* LY: HOWNER_DMA is more robust */
	|| (st & ARM_HSTAT_DRXFD)) {
		/*debug_printf ("* DMA Rx frame done, %d bytes, rn=%d, status=%04x\n",
			c->rdesc[c->rn].length, c->rn, c->rdesc[c->rn].status);*/
		for (; limit > 0 && ! (c->rdesc[c->rn].data & HOWNER_DMA); --limit)
			hdlc_receive_done (c);
	}

	/* If the receive DMA halted - force it to continue. */
	if (! (ARM_HCON (c->port) & ARM_HCON_DRXEN)) {
		++c->overrun;
		++c->netif.in_errors;
		if (c->callback_error)
			c->callback_error (c, HDLC_OVERRUN);
		ARM_HDMARXPTR (c->port) = (long) (c->rdesc + c->rn);
		ARM_HCON (c->port) |= ARM_HCON_DRXEN;	/* DMA Rx enable */
		arm_bus_yield ();
		/*debug_printf ("\n* start rx\n");*/
	}
}

static void
hdlc_handle_transmit (hdlc_t *c, small_int_t limit)
{
	unsigned long st;
	volatile hdlc_desc_t *desc;
	buf_t *p;
	int len;

	st = ARM_HSTAT (c->port) &
		(ARM_HSTAT_TXU | 	/* Tx underrun */
		ARM_HSTAT_DTXABT |	/* DMA Tx abort */
		ARM_HSTAT_DTXFD |	/* DMA Tx frame done */
		ARM_HSTAT_DTXNL |	/* DMA Tx null list */
		ARM_HSTAT_DTXNO);	/* DMA Tx not owner */
	ARM_HSTAT (c->port) = st;

	/*if (st & ARM_HSTAT_DTXNL)
		debug_printf ("* DMA Tx null list\n");*/
	if (st & (ARM_HSTAT_TXU | ARM_HSTAT_DTXABT)) {
		/*debug_printf ("* Tx underrun\n");*/
		++c->underrun;
		++c->netif.out_errors;
		if (c->callback_error)
			c->callback_error (c, HDLC_UNDERRUN);
	}

	for (; limit > 0 && (c->te + 1) % HDLC_NTBUF != c->tn; --limit) {
		/* Есть место в очереди устройства. Берем пакет
		 * из очереди приоритетов и ставим на передачу. */
		p = buf_prio_queue_get (&c->outq);
		if (! p)
			break;
		transmit_enqueue (c, p);
	}

	/* Use owner bit to find finished descriptors.
	 * Do not use status T bit here, because it could fail
	 * on high data rates. */
	for (; limit > 0 && c->te != c->tn; --limit) {
		desc = c->tdesc + c->tn;
		if (desc->data & HOWNER_DMA) {
			arm_bus_yield ();
			if (desc->data & HOWNER_DMA)
				break;
		}

		len = desc->length;
		/*debug_printf ("* Tx done #%d, te=%d, %d bytes, status=%04x, data=%08x\n",
			c->tn, c->te, len, desc->status, desc->data);*/

		++c->netif.out_packets;
		c->netif.out_bytes += len;
		if (c->callback_transmit)
			c->tbuf [c->tn] = c->callback_transmit (c, c->tbuf [c->tn]);
		if (c->tbuf [c->tn]) {
			buf_free (c->tbuf [c->tn]);
			c->tbuf [c->tn] = 0;
		}
		desc->status = 0;
		c->tn = (c->tn + 1) % HDLC_NTBUF;

		/* Появилось место в очереди устройства. Берем пакет
		 * из очереди приоритетов и ставим на передачу. */
		p = buf_prio_queue_get (&c->outq);
		if (p)
			transmit_enqueue (c, p);
	}

	/* On high data rates, the transmit DMA could halt.
	 * Here, we force it to continue. */
	if (c->te != c->tn && ! (ARM_HCON (c->port) & ARM_HCON_DTXEN)) {
		/* Start the transmitter. */
		ARM_HDMATXPTR (c->port) = (long) (c->tdesc + c->tn);
		ARM_HCON (c->port) |= ARM_HCON_DTXEN;	/* DMA Tx enable */
		arm_bus_yield ();
		/*debug_printf ("\n* start tx\n");*/
	}
}

void hdlc_kick_tx (hdlc_t *c, bool_t force)
{
	lock_take (&c->transmitter);

	if (c->te != c->tn) {
		if (force)
			ARM_HCON (c->port) &= ~ARM_HCON_DTXEN;
		hdlc_handle_transmit (c, 1);
	}

	lock_release (&c->transmitter);
}

static void hdlc_prepare (hdlc_t *c)
{
	int i;
	buf_t *q;

	/*
	 * Initialize hardware.
	 */
	ARM_HCON (c->port) =
		ARM_HCON_TXRS | 	/* reset HTxFIFO and Tx block */
		ARM_HCON_RXRS | 	/* reset HRxFIFO and Rx block */
		ARM_HCON_DTXRS |	/* reset DMA Tx block */
		ARM_HCON_DRXRS; 	/* reset DMA Rx block */

	ARM_HMODE (c->port) =
		ARM_HMODE_RXLITTLE |	/* little-endian HRxFIFO */
		ARM_HMODE_TXLITTLE |	/* little-endian HTxFIFO */
		ARM_HMODE_DF_NRZ |	/* NRZ data format */
		ARM_HMODE_DPLLCLK_BRGOUT2 | /* DPLL clock := BRG output */
		ARM_HMODE_BRGCLK |	/* use MCLK2 as BRG source */
		ARM_HMODE_TXCLK_TXC |	/* Tx clock := TXC pin */
		ARM_HMODE_RXCLK_RXC |	/* Rx clock := RXC pin */
		ARM_HMODE_TXCOPS_TXC;	/* TXC pin := Tx clock */

	/* Do not use RX4WD - Rx 4 word mode.
	 * At high data rates, it causes receiver hangups.
	 * Must use TX4WD for transmitter - it permits
	 * working on rates up to 25 Mbps. */
	ARM_HCON (c->port) =
		ARM_HCON_BRGEN |	/* baud rate gen. enable */
		ARM_HCON_TX4WD |	/* Tx 4 word mode */
		ARM_HCON_DTXSTSK |	/* DMA Tx stop or skip */
		ARM_HCON_DRXSTSK |	/* DMA Rx stop or skip */
		ARM_HCON_TXFLAG;	/* Tx flag idle */

	hdlc_set_brg (c, c->netif.bps);

	ARM_HMASK (c->port) = 0;	/* ignore HDLC address */
	ARM_HMFLR (c->port) = HDLC_MTU+2;	/* maximum frame length */
	ARM_HRBSR (c->port) = HDLC_MTU+2;	/* receive buffer size */
	ARM_HSTAT (c->port) = ~0;	/* clear status */

	ARM_HINTEN (c->port) =
		ARM_HINTEN_TXUIE |	/* Tx under-run */
		ARM_HINTEN_RXABTIE |	/* Abort detected */
		ARM_HINTEN_RXCRCEIE |	/* CRC error frame */
		ARM_HINTEN_RXNOIE |	/* Non-octet aligned frame */
		ARM_HINTEN_RXOVIE |	/* Rx overrun */
		ARM_HINTEN_RXMOVIE |	/* Rx memory overflow */
		ARM_HINTEN_DTXABTIE |	/* DMA Tx abort */
		ARM_HINTEN_RXIERRIE |	/* Rx internal error */
		ARM_HINTEN_DRXFDIE |	/* DMA Rx frame done */
		ARM_HINTEN_DRXNLIE |	/* DMA Rx null list */
		ARM_HINTEN_DRXNOIE |	/* DMA Rx not owner */
		ARM_HINTEN_DTXFDIE |	/* DMA Tx frame done */
		ARM_HINTEN_DTXNLIE |	/* DMA Tx null list */
		ARM_HINTEN_DTXNOIE;	/* DMA Tx not owner */

	ARM_TCON (c->port) = 0;		/* clear RTS */

	/*
	 * Initialize descriptors.
	 */
	c->rdesc = (hdlc_desc_t *) ARM_NONCACHED (c->rdesc_mem);
	c->tdesc = (hdlc_desc_t *) ARM_NONCACHED (c->tdesc_mem);
	for (i=0; i<HDLC_NRBUF; ++i) {
		q = buf_alloc (c->pool, HDLC_MTU + HDLC_EXTRA, 16);
		if (! q) {
			debug_printf ("hdlc_receiver: out of memory\n");
			uos_halt (1);
		}
		q = (buf_t*) ARM_NONCACHED (q);
		q->payload = (unsigned char*) ARM_NONCACHED (q->payload);

		c->rbuf[i] = q;
		c->rdesc[i].data = (long) q->payload | HOWNER_DMA;
		c->rdesc[i].control = 0;
		c->rdesc[i].reserved = 0;
		c->rdesc[i].length = HDLC_MTU+2;
		c->rdesc[i].status = 0;
		c->rdesc[i].next = c->rdesc + (i + 1) % HDLC_NRBUF;
	}
	for (i=0; i<HDLC_NTBUF; ++i) {
		c->tbuf[i] = 0;
		c->tdesc[i].data = 0;
		c->tdesc[i].control = HCONTROL_E | HCONTROL_L;
		c->tdesc[i].reserved = 0;
		c->tdesc[i].length = 0;
		c->tdesc[i].status = 0;
		c->tdesc[i].next = c->tdesc + (i + 1) % HDLC_NTBUF;
	}

	/* DMA Rx enable */
	ARM_HDMARXPTR (c->port) = (long) c->rdesc;
	ARM_HCON (c->port) |= ARM_HCON_DRXEN;
	ARM_HCON (c->port) |= ARM_HCON_RXEN;

	/* Enable transmitter */
	ARM_HCON (c->port) |= ARM_HCON_TXEN;
}

/*
 * Receive interrupt task.
 */
static void
hdlc_receiver (void *arg)
{
	hdlc_t *c = arg;

	lock_take_irq (&c->netif.lock, HDLC_RECEIVE_IRQ (c->port), 0, 0);
	hdlc_prepare (c);

	for (;;) {
		/* Wait for the receive interrupt. */
		lock_wait (&c->netif.lock);

		/* Process all receive interrupts. */
		++c->rintr;
		hdlc_handle_receive (c, HDLC_NRBUF / 2);
	}
}

void hdlc_receiver_enable_irq (hdlc_t *c, int on)
{
	if (on) {
		/* Enable Rx interrupts. */
		ARM_HINTEN (c->port) |= (ARM_HINTEN_RXABTIE |
			ARM_HINTEN_RXCRCEIE | ARM_HINTEN_RXNOIE |
			ARM_HINTEN_RXOVIE | ARM_HINTEN_RXMOVIE |
			ARM_HINTEN_DRXFDIE);
	} else {
		/* Disable Rx interrupts. */
		ARM_HINTEN (c->port) &= ~(ARM_HINTEN_RXABTIE |
			ARM_HINTEN_RXCRCEIE | ARM_HINTEN_RXNOIE |
			ARM_HINTEN_RXOVIE | ARM_HINTEN_RXMOVIE |
			ARM_HINTEN_DRXFDIE);
	}
}

/*
 * Transmit interrupt task.
 */
static void
hdlc_transmitter (void *arg)
{
	hdlc_t *c = arg;

	lock_take_irq (&c->transmitter, HDLC_TRANSMIT_IRQ (c->port), 0, 0);
	for (;;) {
		/* Wait for the transmit interrupt. */
		lock_wait (&c->transmitter);

		/* Process all transmit interrupts. */
		++c->tintr;
		hdlc_handle_transmit (c, HDLC_NTBUF / 2);
	}
}

void hdlc_poll_rx (hdlc_t *c)
{
	lock_take (&c->netif.lock);
	hdlc_handle_receive (c, 1);
	lock_release (&c->netif.lock);
}

void hdlc_poll_tx (hdlc_t *c)
{
	lock_take (&c->transmitter);
	hdlc_handle_transmit (c, 1);
	lock_release (&c->transmitter);
}

/*
 * Set DTR.
 */
void
hdlc_set_dtr (hdlc_t *c, int on)
{
	lock_take (&c->netif.lock);
	if (on)
		ARM_HCON (c->port) |= ARM_HCON_TXDTR;
	else
		ARM_HCON (c->port) &= ~ARM_HCON_TXDTR;
	lock_release (&c->netif.lock);
}

/*
 * Set RTS.
 */
void
hdlc_set_rts (hdlc_t *c, int on)
{
	lock_take (&c->netif.lock);
	if (on)
		ARM_TCON (c->port) |= ARM_TCON_RTS;
	else
		ARM_TCON (c->port) &= ~ARM_TCON_RTS;
	lock_release (&c->netif.lock);
}

/*
 * Get CTS.
 */
int
hdlc_get_cts (hdlc_t *c)
{
	unsigned long hstat;

	lock_take (&c->netif.lock);
	hstat = ARM_HSTAT (c->port);
	lock_release (&c->netif.lock);
	return (hstat & ARM_HSTAT_TXCTS) != 0;
}

/*
 * Get DTR.
 */
int
hdlc_get_dtr (hdlc_t *c)
{
	unsigned long hcon;

	lock_take (&c->netif.lock);
	hcon = ARM_HCON (c->port);
	lock_release (&c->netif.lock);
	return (hcon & ARM_HCON_TXDTR) != 0;
}

/*
 * Get RTS.
 */
int
hdlc_get_rts (hdlc_t *c)
{
	unsigned long tcon;

	lock_take (&c->netif.lock);
	tcon = ARM_TCON (c->port);
	lock_release (&c->netif.lock);
	return (tcon & ARM_TCON_RTS) != 0;
}

/*
 * Get DCD.
 */
int
hdlc_get_dcd (hdlc_t *c)
{
	unsigned long hstat;

	lock_take (&c->netif.lock);
	hstat = ARM_HSTAT (c->port);
	lock_release (&c->netif.lock);
	return (hstat & ARM_HSTAT_RXDCD) != 0;
}

static netif_interface_t hdlc_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						hdlc_output,
	(buf_t *(*) (netif_t*)) 		hdlc_input,
};

/*
 * Set up the network interface.
 */
void
hdlc_init (hdlc_t *c, unsigned char port, const char *name, int rprio, int tprio,
	mem_pool_t *pool, unsigned long hz)
{
	c->netif.interface = &hdlc_interface;
	c->netif.name = name;
	c->netif.mtu = 1500;
	c->netif.type = NETIF_SDLC;
	c->netif.bps = 0;
	c->pool = pool;
	c->port = port;
	c->hz = hz;
	c->rn = 0;
	c->tn = 0;
	c->te = c->tn;
	buf_queue_init (&c->inq, c->inqdata, sizeof (c->inqdata));
	buf_prio_queue_init (&c->outq, c->outqdata, sizeof (c->outqdata));

	if (rprio || tprio) {
		/* Create hdlc tasks. */
		task_create (hdlc_receiver, c, "hdlcr", rprio,
			c->rstack, sizeof (c->rstack));
		task_create (hdlc_transmitter, c, "hdlct", tprio,
			c->tstack, sizeof (c->tstack));
	} else {
		/* poll mode */
		hdlc_prepare (c);
	}
}
