/*
 * Ethernet driver for Elvees NVCom.
 * Ported to uOS by Serge Vakulenko (c) 2010.
 * Based on sources from Ildar F Kaibyshev skif@elvees.com.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <elvees/eth.h>
#include <elvees/ks8721bl.h>

#define ETH_IRQ_RECEIVE		12	/* receive interrupt */
#define ETH_IRQ_TRANSMIT	13	/* transmit interrupt */
#define ETH_IRQ_DMA_RX		14	/* receive DMA interrupt */
#define ETH_IRQ_DMA_TX		15	/* transmit DMA interrupt */

/*
 * PHY register write
 */
static void
phy_write (eth_t *u, unsigned address, unsigned data)
{
	unsigned i, status;

	/* Issue the command to PHY. */
	MC_MAC_MD_CONTROL = MD_CONTROL_OP_WRITE |	/* операция записи */
		MD_CONTROL_DATA (data) |		/* данные для записи */
		MD_CONTROL_PHY (u->phy) |		/* адрес PHY */
		MD_CONTROL_REG (address);		/* адрес регистра PHY */

	/* Wait until the PHY write completes. */
	for (i=0; i<100000; ++i) {
		status = MC_MAC_MD_STATUS;
		if (! (status & MD_STATUS_BUSY))
			break;
	}
	/*debug_printf ((status & MD_STATUS_BUSY) ?
		"phy_write(%d, 0x%02x, 0x%04x) TIMEOUT\n" :
		"phy_write(%d, 0x%02x, 0x%04x)\n", u->phy, address, data);*/
}

/*
 * PHY register read
 */
static unsigned
phy_read (eth_t *u, unsigned address)
{
	unsigned status, i;

	/* Issue the command to PHY. */
	MC_MAC_MD_CONTROL = MD_CONTROL_OP_READ |	/* операция чтения */
		MD_CONTROL_PHY (u->phy) |		/* адрес PHY */
		MD_CONTROL_REG (address);		/* адрес регистра PHY */

	/* Wait until the PHY read completes. */
	for (i=0; i<100000; ++i) {
		status = MC_MAC_MD_STATUS;
		if (! (status & MD_STATUS_BUSY))
			break;
	}
	/*debug_printf ((status & MD_STATUS_BUSY) ?
		"phy_read(%d, 0x%02x) TIMEOUT\n" :
		"phy_read(%d, 0x%02x) returned 0x%04x\n",
		u->phy, address, status & MD_STATUS_DATA);*/
	return status & MD_STATUS_DATA;
}

/*
 * Set default values to Ethernet controller registers.
 */
static void chip_init (eth_t *u)
{
	/* Включение тактовой частоты EMAC */
	MC_CLKEN |= MC_CLKEN_EMAC;
	udelay (10);

	/* Find a device address of PHY transceiver.
	 * Count down from 31 to 0, several times. */
	unsigned id, retry = 0;
	u->phy = 31;
	for (;;) {
		id = phy_read (u, PHY_ID1) << 16 |
			phy_read (u, PHY_ID2);
		if (id != 0 && id != 0xffffffff)
			break;
		if (u->phy > 0)
			u->phy--;
		else {
			u->phy = 31;
			retry++;
			if (retry > 3) {
				debug_printf ("eth_init: no PHY detected\n");
				uos_halt (0);
			}
		}
	}
#ifndef NDEBUG
	debug_printf ("eth_init: transceiver `%s' detected at address %d\n",
		((id & PHY_ID_MASK) == PHY_ID_KS8721BL) ? "KS8721" : "Unknown",
		u->phy);
#endif
	/* Reset transceiver. */
	phy_write (u, PHY_CTL, PHY_CTL_RST);
	while (phy_read (u, PHY_CTL) & PHY_CTL_RST)
		continue;
	phy_write (u, PHY_EXTCTL, PHY_EXTCTL_JABBER);

	/* Perform auto-negotiation. */
	phy_write (u, PHY_ADVRT, PHY_ADVRT_CSMA | PHY_ADVRT_10_HDX |
		PHY_ADVRT_10_FDX | PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX);
	phy_write (u, PHY_CTL, PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST);

	/* Reset TX and RX blocks and pointers */
	MC_MAC_CONTROL = MAC_CONTROL_CP_TX | MAC_CONTROL_RST_TX |
			 MAC_CONTROL_CP_RX | MAC_CONTROL_RST_RX;
	udelay (10);
	debug_printf ("MAC_CONTROL: 0x%08x\n", MC_MAC_CONTROL);

	/* Общие режимы. */
	MC_MAC_CONTROL =
		MAC_CONTROL_FULLD |		/* дуплексный режим */
		MAC_CONTROL_EN_TX |		/* разрешение передачи */
		MAC_CONTROL_EN_TX_DMA |		/* разрешение передающего DMА */
		MAC_CONTROL_EN_RX |		/* разрешение приема */
		MAC_CONTROL_IRQ_RX_DONE | 	/* прерывание по приёму */
		MAC_CONTROL_IRQ_RX_OVF; 	/* прерывание по переполнению */
	debug_printf ("MAC_CONTROL: 0x%08x\n", MC_MAC_CONTROL);

	/* Режимы приёма. */
	MC_MAC_RX_FRAME_CONTROL =
		RX_FRAME_CONTROL_DIS_RCV_FCS | 	/* не сохранять контрольную сумму */
		RX_FRAME_CONTROL_ACC_TOOSHORT |	/* прием коротких кадров */
		RX_FRAME_CONTROL_DIS_TOOLONG | 	/* отбрасывание слишком длинных кадров */
		RX_FRAME_CONTROL_DIS_FCSCHERR |	/* отбрасывание кадров с ошибкой контрольной суммы */
		RX_FRAME_CONTROL_DIS_LENGTHERR;	/* отбрасывание кадров с ошибкой длины */
	debug_printf ("RX_FRAME_CONTROL: 0x%08x\n", MC_MAC_RX_FRAME_CONTROL);

	/* Режимы передачи:
	 * запрет формирования кадра в блоке передачи. */
	MC_MAC_TX_FRAME_CONTROL = TX_FRAME_CONTROL_DISENCAPFR;
	debug_printf ("TX_FRAME_CONTROL: 0x%08x\n", MC_MAC_TX_FRAME_CONTROL);

	/* Тактовый сигнал MDC не должен превышать 2.5 МГц. */
	MC_MAC_MD_MODE = MD_MODE_DIVIDER (KHZ / 2000);

	/* Максимальный размер кадра. */
	MC_MAC_RX_FR_MAXSIZE = ETH_MTU;
}

void
eth_debug (eth_t *u, struct _stream_t *stream)
{
	unsigned short ctl, advrt, sts, extctl;

	mutex_lock (&u->netif.lock);
	ctl = phy_read (u, PHY_CTL);
	sts = phy_read (u, PHY_STS);
	advrt = phy_read (u, PHY_ADVRT);
	extctl = phy_read (u, PHY_EXTCTL);
	mutex_unlock (&u->netif.lock);

	printf (stream, "CTL=%b\n", ctl, PHY_CTL_BITS);
	printf (stream, "STS=%b\n", sts, PHY_STS_BITS);
	printf (stream, "ADVRT=%b\n", advrt, PHY_ADVRT_BITS);
	printf (stream, "EXTCTL=%b\n", extctl, PHY_EXTCTL_BITS);
}

void eth_start_negotiation (eth_t *u)
{
	mutex_lock (&u->netif.lock);
	phy_write (u, PHY_ADVRT, PHY_ADVRT_CSMA | PHY_ADVRT_10_HDX |
		PHY_ADVRT_10_FDX | PHY_ADVRT_100_HDX | PHY_ADVRT_100_FDX);
	phy_write (u, PHY_CTL, PHY_CTL_ANEG_EN | PHY_CTL_ANEG_RST);
	mutex_unlock (&u->netif.lock);
}

int eth_get_carrier (eth_t *u)
{
	unsigned status;

	mutex_lock (&u->netif.lock);
	status = phy_read (u, PHY_STS);
	mutex_unlock (&u->netif.lock);

	return (status & PHY_STS_LINK) != 0;
}

long eth_get_speed (eth_t *u, int *duplex)
{
	unsigned extctl;

	mutex_lock (&u->netif.lock);
	extctl = phy_read (u, PHY_EXTCTL);
	mutex_unlock (&u->netif.lock);

	switch (extctl & PHY_EXTCTL_MODE_MASK) {
	case PHY_EXTCTL_MODE_10_HDX:	/* 10Base-T half duplex */
		if (duplex)
			*duplex = 0;
		u->netif.bps = 10 * 1000000;
		break;
	case PHY_EXTCTL_MODE_100_HDX:	/* 100Base-TX half duplex */
		if (duplex)
			*duplex = 0;
		u->netif.bps = 100 * 1000000;
		break;
	case PHY_EXTCTL_MODE_10_FDX:	/* 10Base-T full duplex */
		if (duplex)
			*duplex = 1;
		u->netif.bps = 10 * 1000000;
		break;
	case PHY_EXTCTL_MODE_100_FDX:	/* 100Base-TX full duplex */
		if (duplex)
			*duplex = 1;
		u->netif.bps = 100 * 1000000;
		break;
	default:
		return 0;
	}
	return u->netif.bps;
}

void eth_set_loop (eth_t *u, int on)
{
	unsigned control;

	mutex_lock (&u->netif.lock);

	/* Set PHY loop-back mode. */
	control = phy_read (u, PHY_CTL);
	if (on) {
		control |= PHY_CTL_LPBK;
	} else {
		control &= ~PHY_CTL_LPBK;
	}
	phy_write (u, PHY_CTL, control);
	mutex_unlock (&u->netif.lock);
}

void eth_set_promisc (eth_t *u, int station, int group)
{
	mutex_lock (&u->netif.lock);
#if 0
	erxfcon = ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN;
	if (station) {
		/* Accept any unicast. */
		erxfcon &= ~ERXFCON_UCEN;
	}
	if (group) {
		/* Accept any multicast. */
		erxfcon |= ERXFCON_MCEN;
	}
	chip_write (u, ERXFCON, erxfcon);
#endif
	mutex_unlock (&u->netif.lock);
}

/*
 * Put data to transmit FIFO from dmabuf.
 */
static void
chip_write_txfifo (unsigned long long *addr, unsigned nbytes)
{
	/* Set the address and length for DMA. */
	MC_IR_EMAC(1) = (unsigned) addr & 0x1FFFFFFC;
	MC_CSR_EMAC(1) = MC_DMA_CSR_WCX (((nbytes + 7) >> 3) - 1);

	/* Run the DMA. */
	MC_RUN_EMAC(1) = MC_DMA_RUN;
	while (MC_RUN_EMAC(1) & MC_DMA_RUN) {
		;
	}
}

/*
 * Fetch data from receive FIFO to dmabuf.
 */
static void
chip_read_rxfifo (unsigned long long *addr, unsigned nbytes)
{
	/* Set the address and length for DMA. */
	MC_IR_EMAC(0) = (unsigned) addr & 0x1FFFFFFC;
	MC_CSR_EMAC(0) = MC_DMA_CSR_WCX (((nbytes + 7) >> 3) - 1);

	/* Run the DMA. */
	MC_RUN_EMAC(0) = MC_DMA_RUN;
	while (MC_RUN_EMAC(0) & MC_DMA_RUN) {
		;
	}
}

/*
 * Write the packet to chip memory and start transmission.
 * Deallocate the packet.
 */
static void
chip_transmit_packet (eth_t *u, buf_t *p)
{
	unsigned len = p->tot_len;
	if (len < 60)
		len = 60;

	MC_MAC_TX_FRAME_CONTROL = TX_FRAME_CONTROL_DISENCAPFR |
		TX_FRAME_CONTROL_LENGTH (len);

	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */
	buf_t *q;
	unsigned char *buf = (unsigned char*) u->dmabuf;
	for (q=p; q; q=q->next) {
		/* Copy the packet into the transmit buffer. */
		assert (q->len > 0);
		memcpy (buf, q->payload, q->len);
		buf += q->len;
	}
	chip_write_txfifo (u->dmabuf, len);
	MC_MAC_TX_FRAME_CONTROL |= TX_FRAME_CONTROL_TX_REQ;

	++u->netif.out_packets;
	u->netif.out_bytes += len;
	buf_free (p);
}

/*
 * Do the actual transmission of the packet. The packet is contained
 * in the pbuf that is passed to the function. This pbuf might be chained.
 */
static bool_t
eth_output (eth_t *u, buf_t *p, small_uint_t prio)
{
	mutex_lock (&u->netif.lock);

	/* Exit if link has failed */
	if (p->tot_len < 4 || p->tot_len > ETH_MTU ||
	    ! (phy_read (u, PHY_STS) & PHY_STS_LINK)) {
debug_printf ("eth_output: transmit %d bytes, link failed\n", p->tot_len);
		++u->netif.out_errors;
		mutex_unlock (&u->netif.lock);
		buf_free (p);
		return 0;
	}
debug_printf ("eth_output: transmit %d bytes\n", p->tot_len);

	if (MC_MAC_STATUS_TX & STATUS_TX_ONTX_REQ) {
		/* Занято, ставим в очередь. */
		if (buf_queue_is_full (&u->outq)) {
			++u->netif.out_discards;
			mutex_unlock (&u->netif.lock);
			debug_printf ("eth_output: overflow\n");
			buf_free (p);
			return 0;
		}
		buf_queue_put (&u->outq, p);
	} else {
		chip_transmit_packet (u, p);
	}
	mutex_unlock (&u->netif.lock);
	return 1;
}

/*
 * Get a packet from input queue.
 */
static buf_t *
eth_input (eth_t *u)
{
	buf_t *p;

	mutex_lock (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	mutex_unlock (&u->netif.lock);
	return p;
}

/*
 * Setup MAC address.
 */
static void
eth_set_address (eth_t *u, unsigned char *addr)
{
	mutex_lock (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);

	MC_MAC_UCADDR_L = u->netif.ethaddr[0] |
		(u->netif.ethaddr[1] << 8) |
		(u->netif.ethaddr[2] << 16)|
		(u->netif.ethaddr[3] << 24);
	MC_MAC_UCADDR_H = u->netif.ethaddr[4] |
		(u->netif.ethaddr[5] << 8);

	mutex_unlock (&u->netif.lock);
}

/*
 * Fetch and process the received packet from the network controller.
 */
static void
eth_receive_frame (eth_t *u)
{
	unsigned frame_status = MC_MAC_RX_FRAME_STATUS_FIFO;
	if (! (frame_status & RX_FRAME_STATUS_OK)) {
		/* Invalid frame */
debug_printf ("eth_receive_data: failed, frame_status=%#08x\n", frame_status);
		++u->netif.in_errors;
		return;
	}
	/* Extract data from RX FIFO. */
	unsigned len = RX_FRAME_STATUS_LEN (frame_status);
	chip_read_rxfifo (u->dmabuf, len);

	if (len < 4 || len > ETH_MTU) {
		/* Skip this frame */
debug_printf ("eth_receive_data: bad length %d bytes, frame_status=%#08x\n", len, frame_status);
		++u->netif.in_errors;
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
debug_printf ("eth_receive_data: ok, frame_status=%#08x, length %d bytes\n", frame_status, len);
/*debug_printf ("eth_receive_data: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
((unsigned char*)u->dmabuf)[0], ((unsigned char*)u->dmabuf)[1],
((unsigned char*)u->dmabuf)[2], ((unsigned char*)u->dmabuf)[3],
((unsigned char*)u->dmabuf)[4], ((unsigned char*)u->dmabuf)[5],
((unsigned char*)u->dmabuf)[6], ((unsigned char*)u->dmabuf)[7],
((unsigned char*)u->dmabuf)[8], ((unsigned char*)u->dmabuf)[9],
((unsigned char*)u->dmabuf)[10], ((unsigned char*)u->dmabuf)[11],
((unsigned char*)u->dmabuf)[12], ((unsigned char*)u->dmabuf)[13]);*/

	if (buf_queue_is_full (&u->inq)) {
debug_printf ("eth_receive_data: input overflow\n");
		++u->netif.in_discards;
		return;
	}

	/* Allocate a buf chain with total length 'len' */
	buf_t *p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
debug_printf ("eth_receive_data: ignore packet - out of memory\n");
		++u->netif.in_discards;
		return;
	}

	/* Copy the packet data. */
	memcpy (p->payload, u->dmabuf, len);
	buf_queue_put (&u->inq, p);
}

/*
 * Process an interrupt.
 * Return nonzero when there was some activity.
 */
static unsigned
handle_interrupts (eth_t *u)
{
	unsigned active, status_rx, nframes;

	active = 0;
	for (;;) {
		/* Process all pending interrupts. */
		status_rx = MC_MAC_STATUS_RX;
/*debug_printf ("eth irq: STATUS_RX = %08x\n", status_rx);*/
		if (status_rx & (STATUS_RX_STATUS_OVF | STATUS_RX_FIFO_OVF)) {
			/* Count lost incoming packets. */
			if (STATUS_RX_NUM_MISSED (status_rx))
				u->netif.in_discards += STATUS_RX_NUM_MISSED (status_rx);
			else
				u->netif.in_discards++;
			MC_MAC_STATUS_RX = 0;
		}
#if 0
		if (eir & EIR_TXERIF) {
			/* Count collisions. */
			++u->netif.out_collisions;

			/* Transmitter stopped, reset required. */
			chip_set_bits (u, ECON1, ECON1_TXRST);
			chip_clear_bits (u, ECON1, ECON1_TXRST);
			chip_clear_bits (u, ESTAT, ESTAT_TXABRT);

			/* Retransmit the packet. */
			chip_clear_bits (u, EIR, EIR_TXERIF | EIR_TXIF);
			chip_set_bits (u, ECON1, ECON1_TXRTS);
			eir &= ~EIR_TXIF;
		}
#endif
		/* Check if a packet has been received and buffered. */
		if (status_rx & STATUS_RX_DONE) {
			++active;
			nframes = STATUS_RX_NUM_FR (status_rx);
			while (nframes-- > 0)
				eth_receive_frame (u);
		} else {
			/* All interrupts processed. */
			return active;
		}
#if 0
		if (eir & EIR_TXIF) {
			chip_clear_bits (u, EIR, EIR_TXIF);

			/* Transmit a packet from queue. */
			buf_t *p = buf_queue_get (&u->outq);
			if (p)
				chip_transmit_packet (u, p);
		}
#endif
	}
}

/*
 * Poll for interrupts.
 * Must be called by user in case there is a chance to lose an interrupt.
 */
void
eth_poll (eth_t *u)
{
	mutex_lock (&u->netif.lock);
	if (handle_interrupts (u))
		mutex_signal (&u->netif.lock, 0);
	mutex_unlock (&u->netif.lock);
}

/*
 * Interrupt task.
 */
static void
eth_interrupt (void *arg)
{
	eth_t *u = arg;

	mutex_lock_irq (&u->netif.lock, ETH_IRQ_RECEIVE, 0, 0);

	/* Initialize hardware. */
	chip_init (u);

	for (;;) {
		/* Wait for the interrupt. */
		mutex_wait (&u->netif.lock);
		++u->intr;
		handle_interrupts (u);
	}
}

static netif_interface_t eth_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						eth_output,
	(buf_t *(*) (netif_t*))			eth_input,
	(void (*) (netif_t*, unsigned char*))	eth_set_address,
};

/*
 * Set up the network interface.
 */
void
eth_init (eth_t *u, const char *name, int prio, mem_pool_t *pool,
	arp_t *arp)
{
	u->netif.interface = &eth_interface;
	u->netif.name = name;
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));
	buf_queue_init (&u->outq, u->outqdata, sizeof (u->outqdata));

	/* Create receive task. */
	task_create (eth_interrupt, u, name, prio, u->stack, sizeof (u->stack));
}
