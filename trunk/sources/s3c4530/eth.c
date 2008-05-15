#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "s3c4530/eth.h"
#include "s3c4530/seeq80225.h"

#define ETH_BDMA_RECEIVE_IRQ	17	/* BDMA receive interrupt */
#define ETH_MAC_TRANSMIT_IRQ	18	/* MAC transmit interrupt */

/*
 * Maximum packet size.
 * 1514 - traditional value: 1500 for IP and 14 for Ethernet header.
 * 1518 - for VLAN support: extra 4 bytes for VLAN tag.
 * 4224 - for NetWare 5.x
 * 9216 - Cisco "Jumbo" packets
 */
#define ETH_MTU			4224
#define ETH_EXTRA		16	/* extra space for Rx DMA */

/*
 * Access to PHY registers.
 */
static void
phy_write (eth_t *c, unsigned long addr, unsigned long data)
{
	int i;

	ARM_STADATA = data;
	ARM_STACON = addr | c->phy_addr | ARM_STACON_BUSY | ARM_STACON_WR;
	for (i = 0; i < 100000; ++i)
		if (! (ARM_STACON & ARM_STACON_BUSY))
			break;
}

static unsigned long
phy_read (eth_t *c, unsigned long addr)
{
	int i;

	ARM_STACON = addr | c->phy_addr | ARM_STACON_BUSY;
	for (i=0; i<100000; ++i)
		if (! (ARM_STACON & ARM_STACON_BUSY))
			break;
	return ARM_STADATA;
}

/*
 * Start auto-negotiation.
 * This function must be called from user level only.
 */
void
eth_start_negotiation (eth_t *c)
{
	lock_take (&c->netif.lock);
	ARM_MACON = ARM_MACON_FULLDUP;
	phy_write (c, PHY_ADVRT, PHY_ADVRT_CSMA | PHY_ADVRT_10_HDX |
		PHY_ADVRT_10_FDX | PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX);
	phy_write (c, PHY_CTL, PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST);
	lock_release (&c->netif.lock);
}

/*
 * Set mode.
 * This function must be called from user level only.
 */
void
eth_set_mode (eth_t *c, int speed100, int fdx, int disneg)
{
	unsigned short advrt = PHY_ADVRT_CSMA, ctl = 0;

	lock_take (&c->netif.lock);
	if (disneg) {
		/* Hardware mode, no negotiation. */
		if (speed100)
			ctl |= PHY_CTL_SPEED_100;
		if (fdx)
			ctl |= PHY_CTL_DPLX;
	} else {
		/* Negotiation with limited capability list. */
		if (fdx)
			advrt |= speed100 ? PHY_ADVRT_100_FDX : PHY_ADVRT_10_FDX;
		else
			advrt |= speed100 ? PHY_ADVRT_100_HDX : PHY_ADVRT_10_HDX;

		ctl |= PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST;
	}
	if (fdx)
		ARM_MACON = ARM_MACON_FULLDUP;
	else
		ARM_MACON = 0;

	phy_write (c, PHY_ADVRT, advrt);
	phy_write (c, PHY_CTL, ctl);
	lock_release (&c->netif.lock);
}

void
eth_debug (eth_t *c, struct _stream_t *stream)
{
	unsigned short ctl, advrt, sts, stsout;

	lock_take (&c->netif.lock);
	ctl = phy_read (c, PHY_CTL);
	sts = phy_read (c, PHY_STS);
	advrt = phy_read (c, PHY_ADVRT);
	stsout = phy_read (c, PHY_STSOUT);
	lock_release (&c->netif.lock);

	printf (stream, "CTL=%b\n", ctl, PHY_CTL_BITS);
	printf (stream, "STS=%b\n", sts, PHY_STS_BITS);
	printf (stream, "ADVRT=%b\n", advrt, PHY_ADVRT_BITS);
	printf (stream, "STSOUT=%b\n", stsout, PHY_STSOUT_BITS);
}

/*
 * Query carrier.
 * This function must be called from user level only.
 */
int
eth_get_carrier (eth_t *c)
{
	int status;

	lock_take (&c->netif.lock);
	status = phy_read (c, PHY_STS);
	lock_release (&c->netif.lock);

	return (status & PHY_STS_LINK) != 0;
}

/*
 * Query speed and duplex mode.
 * Return 0 if negotiation is not yet finished.
 * This function must be called from user level only.
 */
int
eth_get_speed (eth_t *c, int *duplex)
{
	int status, output;

	lock_take (&c->netif.lock);
	status = phy_read (c, PHY_STS);
	output = phy_read (c, PHY_STSOUT);
	lock_release (&c->netif.lock);

	if (! (status & PHY_STS_ANEG_ACK))
		return 0;

	c->netif.bps = (output & PHY_STSOUT_SPEED_100) ?
		100000000 : 10000000;

	if (output & PHY_STSOUT_DPLX) {
		ARM_MACON = ARM_MACON_FULLDUP;
		if (duplex)
			*duplex = 1;
	} else {
		ARM_MACON = 0;
		if (duplex)
			*duplex = 0;
	}
	return c->netif.bps;
}

/*
 * Set loopback mode.
 * This function must be called from user level only.
 */
void
eth_set_loop (eth_t *c, int on)
{
	unsigned long control;

	lock_take (&c->netif.lock);

	/* Set loop-back mode. */
	control = phy_read (c, PHY_CTL);
	if (on) {
		control |= PHY_CTL_LPBK;
	} else {
		control &= ~PHY_CTL_LPBK;
	}
	phy_write (c, PHY_CTL, control);

	lock_release (&c->netif.lock);
}

/*
 * Enable receiving all packets (promiscuous mode).
 * This function must be called from user level only.
 */
void
eth_set_promisc (eth_t *c, int station, int group)
{
	unsigned long camcon;

	lock_take (&c->netif.lock);

	/* Set promiscuous mode. */
	camcon = ARM_CAMCON;
	if (station) {
		/* Accept any unicast. */
		camcon |= ARM_CAMCON_STATIONACC;
		camcon &= ~ARM_CAMCON_COMPEN;
	} else {
		/* Compare incoming address with CAM memory. */
		camcon |= ARM_CAMCON_COMPEN;
		camcon &= ~ARM_CAMCON_STATIONACC;
	}
	if (group) {
		/* Accept any multicast. */
		camcon |= ARM_CAMCON_GROUPACC;
	} else {
		camcon &= ~ARM_CAMCON_GROUPACC;
	}
	ARM_CAMCON = camcon;

	lock_release (&c->netif.lock);
}

int
eth_transmit_space (eth_t *c)
{
	int space;

	lock_take (&c->transmitter);
	space = (ETH_NTBUF + c->tn - c->te - 1) % ETH_NTBUF;

	/* Transmitter could hang up -- kick it. */
	if (space == 0 && ! (c->tdesc[c->tn].data & EOWNER_DMA)) {
		/*debug_printf ("\n* kick tx\n");*/
		lock_signal (&c->transmitter, 0);
	}
	lock_release (&c->transmitter);
	return space;
}

static void
transmit_enqueue (eth_t *c, buf_t *p)
{
	volatile eth_desc_t *desc;

	c->tbuf[c->te] = p;

	/* Set up the tx descriptor. */
	desc = c->tdesc + c->te;
	desc->length = p->tot_len;
	desc->status = 0;

	/* When source data are not aligned on word boundary,
	 * we must skip 1, 2 or 3 heading bytes. */
	desc->control = ECONTROL_L | ECONTROL_T | ECONTROL_A;
	switch ((unsigned) p->payload & 3) {
	case 1:	desc->control |= ECONTROL_WA_1; break;
	case 2:	desc->control |= ECONTROL_WA_2; break;
	case 3:	desc->control |= ECONTROL_WA_3; break;
	}

	/* vich: только в самом конце говорим, что DMA может работать */
	desc->data = ((long) p->payload & ~3ul) | EOWNER_DMA;
	arm_bus_yield ();
	/*debug_printf ("eth_output: enqueue #%d, tn=%d, %d bytes\n",
		c->te, c->tn, desc->length);*/

	c->te = (c->te + 1) % ETH_NTBUF;
}

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
bool_t
eth_output (eth_t *c, buf_t *p, uint_t prio)
{
	lock_take (&c->transmitter);

	if (p->tot_len < 14 || p->tot_len > ETH_MTU) {
		lock_release (&c->transmitter);
		/* invalid packet length */
		/*debug_printf ("eth_output: invalid packet length %d bytes\n",
			p->tot_len);*/
		++c->netif.out_errors;
		++c->tx_big;
		buf_free (p);
		if (c->callback_error)
			c->callback_error (c);
		return 0;
	}

	/* Convert to a single buffer */
	p = buf_make_continuous (p);
	if (! p) {
		lock_release (&c->transmitter);
		/*debug_printf ("eth_output: cannot make continuous packet\n");*/
		++c->netif.out_errors;
		++c->tx_nomem;
		if (c->callback_error)
			c->callback_error (c);
		return 0;
	}

	if ((c->te + 1) % ETH_NTBUF == c->tn) {
		/* Нет места в очереди устройства. */
		lock_release (&c->transmitter);
		/* no free space */
		/*debug_printf ("eth_output: no free space, te=%d, tn=%d\n",
			c->te, c->tn);*/
		++c->netif.out_errors;
		++c->tx_qo;
		buf_free (p);
		if (c->callback_error)
			c->callback_error (c);
		return 0;
	}

	/* Есть место в очереди устройства. */
	transmit_enqueue (c, p);

	if (! (ARM_BDMATXCON & ARM_BDMATXCON_EN)) {
		/* Start the transmitter. */
		ARM_BDMATXPTR = (long) (c->tdesc + c->tn);
		ARM_BDMATXCON |= ARM_BDMATXCON_EN;	/* BDMA Tx enable */
/*ARM_MACTXCON = EnComp | TxEn;*/
		/*debug_printf ("eth_output: start tx\n");*/
		arm_bus_yield ();
	}
	lock_release (&c->transmitter);
	return 1;
}

static buf_t *
eth_input (eth_t *c)
{
	buf_t *p;

	lock_take (&c->netif.lock);
	p = buf_queue_get (&c->inq);
	lock_release (&c->netif.lock);
	return p;
}

static void
eth_set_address (eth_t *c, unsigned char *addr)
{
	lock_take (&c->netif.lock);
	memcpy (&c->netif.ethaddr, addr, 6);

	/* Set MAC address. */
	ARM_CAM(0) = c->netif.ethaddr[0] << 24 | c->netif.ethaddr[1] << 16 |
		c->netif.ethaddr[2] << 8 | c->netif.ethaddr[3];
	ARM_CAM(1) = c->netif.ethaddr[4] << 24 | c->netif.ethaddr[5] << 16;
	lock_release (&c->netif.lock);
}

/*
 * Fetch and process the received packet from the network controller.
 */
static buf_t *
eth_receive_data (eth_t *c, buf_t *p, int len)
{
	buf_t *q;

	if (buf_queue_is_full (&c->inq)) {
		/*debug_printf ("eth_receive_data: input overflow\n");*/
		++c->netif.in_discards;
		return p;
	}

	/* Allocate a buf chain with total length 'len' */
	q = buf_alloc (c->pool, ETH_MTU + ETH_EXTRA, 16);
	if (! q) {
		/* Could not allocate a buf - skip received frame */
		/*debug_printf ("eth_receive_data: ignore packet - out of memory\n");*/
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
eth_receive_done (eth_t *c)
{
	unsigned long st;
	int len;

	st = c->rdesc[c->rn].status;
	len = c->rdesc[c->rn].length - 4;
	/*debug_printf ("* received frame #%d, %d bytes, status 0x%x\n",
		c->rn, len, st);*/

	if (st & ESTAT_RX_OVERFLOW) {
		/* Receive overrun error */
		++c->overrun;
		++c->netif.in_errors;
		if (c->callback_error)
			c->callback_error (c);
	} else if ((st & ESTAT_RX_ALIGNERR) ||	/* non-octet aligned */
	    (st & ESTAT_RX_PAR) ||		/* parity error */
	    (st & ESTAT_RX_OVMAX)) {		/* too long frame */
		/* Receive frame error */
		++c->frame;
		++c->netif.in_errors;
		if (c->callback_error)
			c->callback_error (c);
	} else if (st & ESTAT_RX_CRCERR) {
		/* Receive CRC error */
		++c->crc;
		++c->netif.in_errors;
		if (c->callback_error)
			c->callback_error (c);
	} else if (len > ETH_MTU) {
		/* Too long frame - skip it */
		++c->rx_big;
		++c->netif.in_errors;
		if (c->callback_error)
			c->callback_error (c);
	} else {
		/* Valid packet */
		++c->netif.in_packets;
		c->netif.in_bytes += len;
		c->rbuf[c->rn] = eth_receive_data (c, c->rbuf[c->rn], len);
	}
	c->rdesc[c->rn].length = ETH_MTU+4;
	c->rdesc[c->rn].status = 0;

	/* vich: только в самом конце говорим, что DMA может работать. */
	c->rdesc[c->rn].data = (long) c->rbuf[c->rn]->payload | EOWNER_DMA;
	arm_bus_yield ();

	if (! (c->rdesc[(c->rn - 1) % ETH_NRBUF].data & EOWNER_DMA))
		c->rx_qo++; /* LY: считаем переполнением ситуацию,
				   когда в rx-очереди нет пустых элементов. */
	c->rn = (c->rn + 1) % ETH_NRBUF;
}

/*
 * Process a pending interrupt.
 */
static void
eth_handle_receive (eth_t *c, unsigned long st)
{
	/* if (st & ARM_BDMASTAT_RX_NO)
		debug_printf ("* DMA Rx not owner\n"); */

	if (st & ARM_BDMASTAT_RX_RDF) {
		/* debug_printf ("* DMA Rx frame done, %d bytes, rn=%d, status=%04x\n",
			c->rdesc[c->rn].length, c->rn, c->rdesc[c->rn].status); */
		while (! (c->rdesc[c->rn].data & EOWNER_DMA))
			eth_receive_done (c);
	}

	//if (ARM_MAXRXSTAT & ARM_MACRXCON_ENRXPAR) {
	//	/* LY: re-enable after the parity error */
	//	ARM_BDMARXCON |= ARM_BDMARXCON_EN; /* DMA Rx enable */
	//}

	/* If the receive DMA halted - force it to continue. */
	if (! (ARM_BDMARXCON & ARM_BDMARXCON_EN)) {
		ARM_BDMARXPTR = (long) (c->rdesc + c->rn);
		ARM_BDMARXCON |= ARM_BDMARXCON_EN; /* DMA Rx enable */
		arm_bus_yield ();
		/*debug_printf ("\n* start rx\n");*/
	}
}

static void
eth_handle_transmit (eth_t *c, int_t limit)
{
	volatile eth_desc_t *desc;
	unsigned long st = 0;
	int len, ncoll;

	/* Use owner bit to find finished descriptors.
	 * Do not use status T bit here, because it could fail
	 * on high data rates. */
	for (; limit > 0 && c->te != c->tn; --limit) {
		desc = c->tdesc + c->tn;
		if (desc->data & EOWNER_DMA) {
			arm_bus_yield ();
			if (desc->data & EOWNER_DMA)
				break;
		}

		st |= desc->status;
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
		c->tn = (c->tn + 1) % ETH_NTBUF;
	}

	ncoll = st & (ESTAT_TX_COLLCNT | ESTAT_TX_EXCOLL);
	if (ncoll) {
		c->netif.out_collisions += ncoll;
		if (c->callback_collision)
			c->callback_collision (c, ncoll);
	}

	if (st & ESTAT_TX_UNDER) {
		/*debug_printf ("* Tx underrun\n");*/
		++c->underrun;
		++c->netif.out_errors;
		if (c->callback_error)
			c->callback_error (c);
	}
        if (st & ESTAT_TX_LATECOLL) {
		/*debug_printf ("* late collision\n");*/
		++c->tx_lc;
		++c->netif.out_errors;
		if (c->callback_error)
			c->callback_error (c);
	}
	/*if (st & ESTAT_TX_PAUSED)
		debug_printf ("* pause received\n");*/
	/*if (st & ESTAT_TX_DEFER)
		debug_printf ("* transfer deferred\n");*/
	/*if (st & ESTAT_TX_SQE)
		debug_printf ("* SQE fault\n");*/
	/*if (st & ESTAT_TX_PAR)
		debug_printf ("* transmit parity error\n");*/
	/*if (st & ESTAT_TX_HALTED)
		debug_printf ("* transmission halted\n");*/

	/* On high data rates, the transmit DMA could halt.
	 * Here, we force it to continue. */
	if (c->te != c->tn && ! (ARM_BDMATXCON & ARM_BDMATXCON_EN)) {
		/* Start the transmitter. */
		ARM_BDMATXPTR = (long) (c->tdesc + c->tn);
		ARM_BDMATXCON |= ARM_BDMATXCON_EN; /* DMA Tx enable */
		arm_bus_yield ();
		/*debug_printf ("\n* start tx\n");*/
	}
}

void eth_kick_tx (eth_t *c, bool_t force)
{
	lock_take (&c->transmitter);

	if (c->te != c->tn) {
		if (force)
			ARM_BDMATXCON &= ~ARM_BDMATXCON_EN;
		eth_handle_transmit (c, 1);
	}

	lock_release (&c->transmitter);
}

void eth_poll_rx (eth_t *c)
{
	// LY: выбираем из очереди по одному пакету за раз.
	lock_take (&c->netif.lock);
	if (! (c->rdesc[c->rn].data & EOWNER_DMA))
		eth_receive_done (c);

	//if (ARM_MAXRXSTAT & ARM_MACRXCON_ENRXPAR) {
	//	/* LY: re-enable after the parity error */
	//	ARM_BDMARXCON |= ARM_BDMARXCON_EN; /* DMA Rx enable */
	//}

	/* If the receive DMA halted - force it to continue. */
	if (! (ARM_BDMARXCON & ARM_BDMARXCON_EN)) {
		ARM_BDMARXPTR = (long) (c->rdesc + c->rn);
		ARM_BDMARXCON |= ARM_BDMARXCON_EN; /* DMA Rx enable */
		arm_bus_yield ();
		/*debug_printf ("\n* start rx\n");*/
	}
	lock_release (&c->netif.lock);
}

void eth_poll_tx (eth_t *c)
{
	lock_take (&c->transmitter);
	if (c->te != c->tn)
		eth_handle_transmit (c, 1);
	lock_release (&c->transmitter);
}

/*
 * Transmit interrupt task.
 */
static void
eth_transmitter (void *arg)
{
	eth_t *c = arg;

	lock_take_irq (&c->transmitter, ETH_MAC_TRANSMIT_IRQ, 0, 0);
	for (;;) {
		/* Wait for the transmit interrupt. */
		lock_wait (&c->transmitter);

		/* Process all transmit interrupts. */
		++c->tintr;
		eth_handle_transmit (c, ETH_NTBUF / 2);
	}
}

static void eth_prepare (eth_t *c)
{
	unsigned i;
	buf_t *q;

	/* Reset transceiver. */
	phy_write (c, PHY_CTL, PHY_CTL_RST);
	while (phy_read (c, PHY_CTL) & PHY_CTL_RST);

	/* Perform auto-negotiation. */
	phy_write (c, PHY_ADVRT, PHY_ADVRT_CSMA | PHY_ADVRT_10_HDX |
		PHY_ADVRT_10_FDX | PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX);
	phy_write (c, PHY_CTL, PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST);

	/*
	 * Reset hardware.
	 */
	ARM_BDMARXCON = ARM_BDMARXCON_RS;	/* reset receive BDMA */
	ARM_BDMATXCON = ARM_BDMATXCON_RS;	/* reset transmit BDMA */
	ARM_MACON = ARM_MACON_RESET;		/* reset MAC */

	ARM_BDMARXLSZ = ETH_MTU+4;		/* maximum frame length */
	ARM_MACON = ARM_MACON_FULLDUP;		/* enable full duplex */

	/*
	 * Initialize descriptors.
	 */
	c->rdesc = (eth_desc_t *) ARM_NONCACHED (c->rdesc_mem);
	c->tdesc = (eth_desc_t *) ARM_NONCACHED (c->tdesc_mem);
	for (i=0; i<ETH_NRBUF; ++i) {
		q = buf_alloc (c->pool, ETH_MTU + ETH_EXTRA, 16);
		if (! q) {
			debug_printf ("eth_receiver: out of memory\n");
			abort ();
		}
		q = (buf_t*) ARM_NONCACHED (q);
		q->payload = (unsigned char*) ARM_NONCACHED (q->payload);

		c->rbuf[i] = q;
		c->rdesc[i].data = (long) q->payload | EOWNER_DMA;
		c->rdesc[i].control = 0;
		c->rdesc[i].length = ETH_MTU+4;
		c->rdesc[i].status = 0;
		c->rdesc[i].next = c->rdesc + (i + 1) % ETH_NRBUF;
	}
	for (i=0; i<ETH_NTBUF; ++i) {
		c->tbuf[i] = 0;
		c->tdesc[i].data = 0;
		/* Little-endian mode, enable interrupt, increment pointer. */
		c->tdesc[i].control = ECONTROL_L | ECONTROL_T | ECONTROL_A;
		c->tdesc[i].length = 0;
		c->tdesc[i].status = 0;
		c->tdesc[i].next = c->tdesc + (i + 1) % ETH_NTBUF;
	}
	ARM_BDMATXPTR = (long) c->tdesc;
	ARM_BDMARXPTR = (long) c->rdesc;

	/*
	 * Set MAC address, enable broadcasts.
	 */
	ARM_CAM(0) = c->netif.ethaddr[0] << 24 | c->netif.ethaddr[1] << 16 |
		c->netif.ethaddr[2] << 8 | c->netif.ethaddr[3];
	ARM_CAM(1) = c->netif.ethaddr[4] << 24 | c->netif.ethaddr[5] << 16;
	ARM_CAMEN = 0x0001;
	ARM_CAMCON = ARM_CAMCON_COMPEN |	/* enable compare mode */
		ARM_CAMCON_BROADACC;		/* accept broadcasts */

	/*
	 * Enable BDMA and MAC.
	 */
	ARM_BDMATXCON = ARM_BDMATXCON_BRST_32 |	/* burst 32 words - A MUST! */
		ARM_BDMATXCON_MSL_6_8 |		/* level 6/8 */
		ARM_BDMATXCON_STSKO |		/* stop on owner bit */
		ARM_BDMATXCON_EN;		/* BDMA Tx enable */

	ARM_MACTXCON = ARM_MACTXCON_TXEN |	/* enable transmittion */
		ARM_MACTXCON_ENCOMP;		/* en.irq on frame completion */

	ARM_BDMARXCON = ARM_BDMARXCON_DIE |	/* every pkt intr enable */
		ARM_BDMARXCON_EN |		/* BDMA Rx enable */
		ARM_BDMARXCON_LITTLE |		/* little endian */
		ARM_BDMARXCON_MAINC |		/* memory address increment */
		ARM_BDMARXCON_BRST_32 |		/* 32 words - A MUST! */
		ARM_BDMARXCON_STSKO |		/* stop/skip by owner bit */
		ARM_BDMARXCON_NOIE;		/* not owner intr enable */

	ARM_MACRXCON = ARM_MACRXCON_RXEN |	/* receive enable */
		ARM_MACRXCON_SHORTEN |		/* short enable (<64 bytes) */
		ARM_MACRXCON_LONGEN;		/* long enable (>1518 bytes) */
}

/*
 * Receive interrupt task.
 */
static void
eth_receiver (void *arg)
{
	eth_t *c = arg;
	unsigned long st;

	lock_take_irq (&c->netif.lock, ETH_BDMA_RECEIVE_IRQ, 0, 0);
	eth_prepare (c);

	for (;;) {
		/* Wait for the receive interrupt. */
		lock_wait (&c->netif.lock);

		++c->rintr;

		st = ARM_BDMASTAT;
		ARM_BDMASTAT = st;

		/* Process all receive interrupts. */
		eth_handle_receive (c, st);
	}
}

static netif_interface_t eth_interface = {
	(bool_t (*) (netif_t*, buf_t*, uint_t))
						eth_output,
	(buf_t *(*) (netif_t*)) 		eth_input,
	(void (*) (netif_t*, unsigned char*))	eth_set_address,
};

/*
 * Set up the network interface.
 */
void
eth_init (eth_t *c, const char *name, int rprio, int tprio, mem_pool_t *pool, arp_t *arp)
{
	unsigned long id;
	int i;

	lock_init (&c->transmitter);
	lock_init (&c->netif.lock);
	c->netif.interface = &eth_interface;
	c->netif.name = name;
	c->netif.arp = arp;
	c->netif.mtu = 1500;
	c->netif.type = NETIF_ETHERNET_CSMACD;
	c->netif.bps = 10000000;
	c->pool = pool;
	c->rn = 0;
	c->tn = 0;
	c->te = c->tn;
	buf_queue_init (&c->inq, c->inqdata, sizeof (c->inqdata));

	/* Obtain MAC address from network interface.
	 * We just fake an address... */
	memcpy (&c->netif.ethaddr, "\x00\x09\x94\xff\xff\xff", 6);

	/* Find a device address of PHY transceiver.
	 * For Seeq 80225, it is determined my resistors on MDA (LED) pins. */
	for (i = 0; i < 10000; ++i) {
		/* It seems that PHY address cannot be 0.
		 * When address is made 0, the transceiver Seeq 80225
		 * stops detecting media. */
		for (c->phy_addr = 1<<5; c->phy_addr < (16 << 5); c->phy_addr += 1 << 5) {
			id = phy_read (c, PHY_ID1) << 16 |
				phy_read (c, PHY_ID2);
			if (id != 0 && id != 0xffffffff)
				goto ok;
		}
	}
	debug_printf ("eth_init: no PHY detected\n");
	abort ();
ok:
#ifndef NDEBUG
	debug_printf ("eth_init: transceiver `%s' detected at address %d\n",
		((id & PHY_ID_MASK) == PHY_ID_80225) ? "Seeq 80225" : "Unknown",
		c->phy_addr >> 5);
#endif
	if (rprio || tprio) {
		/* Create eth tasks. */
		task_create (eth_receiver, c, "ethr", rprio,
			c->rstack, sizeof (c->rstack));
		task_create (eth_transmitter, c, "etht", tprio,
			c->tstack, sizeof (c->tstack));
	} else {
		/* poll mode */
		eth_prepare (c);
	}
}
