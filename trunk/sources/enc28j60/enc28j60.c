/*
 * Microchip ENC28J60 Ethernet Interface Driver.
 *
 * Ported to uOS by Serge Vakulenko (c) 2009.
 * Based on sources from Andreas Reinhardt (c) 2007,
 * and Pascal Stang (c) 2005.
 *
 * This driver provides initialization and transmit/receive
 * functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
 * This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
 * chip, using an SPI interface to the host processor.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <enc28j60/eth.h>
#include <enc28j60/regs.h>

#ifdef UIP_CONF_BUFFER_SIZE
#   define MAX_FRAMELEN		UIP_CONF_BUFFER_SIZE
#else
#   define MAX_FRAMELEN		1518	/* maximum ethernet frame length */
#endif

#define P2_ENIND		BIT1	/* Enable LEDs */
#define P2_ENCLK		BIT4	/* Enable clock */
#define P2_INT			BIT6	/* IRQ from ethernet */
#define P2_NRES			BIT7	/* /RESET */

#define P3_NCS			BIT6 	/* UCB1STE */
#define P3_SDO			BIT7	/* UCB1SIMO */

#define P5_SDI			BIT4	/* UCB1SOMI */
#define P5_SCK			BIT5	/* UCB1CLK */

/*
 * Configure I/O pins.
 */
static void inline
chip_init_spi (void)
{
	P2DIR |= P2_ENIND | P2_ENCLK | P2_NRES;	/* Set outputs */
	P2DIR &= ~P2_INT;		/* Set INT as input */
	P2SEL &= ~(P2_ENIND | P2_ENCLK | P2_INT | P2_NRES);
	P2OUT &= ~P2_INT;
	P2OUT |= P2_ENIND | P2_ENCLK | P2_NRES;	/* Enable clock and indication */

	P3DIR |= P3_SDO | P3_NCS;	/* Set nCS and MOSI as outputs */
	P3SEL |= P3_SDO;		/* Special functions for SPI pins */
	P3OUT &= ~P3_SDO;		/* Clear SDO */
	P3OUT |= P3_NCS;		/* Release CS */

	P5DIR |= P5_SCK;		/* Set SCK as output */
	P5DIR &= ~P5_SDI;		/* Set MISO as input */
	P5SEL |= P5_SCK | P5_SDI;	/* Special functions for SPI pins */
	P5OUT &= ~(P5_SCK | P5_SDI);	/* Clear SCK */

	/* Use USCI_B1 as SPI interface. */
	UCB1CTL1 = UCSWRST;		/* Reset */
	UCB1CTL0 = UCSYNC | UCMST | UCMSB; /* Master 3-bit SPI, 8-bit data, MSB first */
	UCB1CTL1 |= UCSSEL_ACLK;	/* Clock source ACLK */
	UCB1BR0 = 2;			/* Baud rate UCLK/2 (maximum rate) */
	UCB1BR1 = 0;
	UCB1CTL1 &= ~UCSWRST;		/* Clear reset */
	/* UCB1IE |= UCRXIE; */		/* Enable USCI_A0 RX interrupt */
}

static void inline
chip_select (int on)
{
	if (on)
		P3OUT &= ~P3_NCS;	/* assert CS */
	else
		P3OUT |= P3_NCS;	/* release CS */
}

static unsigned
chip_io (unsigned data)
{
	while (! (UCB1IFG & UCTXIFG))	/* wait until TX buffer empty */
		continue;
	UCB1TXBUF = data;		/* send byte */
  	while (! (UCB1IFG & UCRXIFG))	/* data present in RX buffer? */
		continue;
	return UCB1RXBUF;		/* return read data */
}

static unsigned
chip_read_op (unsigned op, unsigned address)
{
	unsigned data = 0;

	chip_select (1);			/* assert CS signal */
	chip_io (op | (address & ADDR_MASK));	/* issue read command */
	data = chip_io (0);			/* read data (send zeroes) */
	if (address & 0x80) {			/* do dummy read if needed */
		data = chip_io (0);		/* read data (send zeroes) */
	}
	chip_select (0);			/* release CS signal */
	return data;
}

static void
chip_write_op (unsigned op, unsigned address, unsigned data)
{
	chip_select (1);
	chip_io (op | (address & ADDR_MASK));	/* issue write command */
	if (op != ENC28J60_SOFT_RESET)
		chip_io (data);			/* send data */
	chip_select (0);
}

static void
chip_read_buffer (unsigned len, unsigned char *data)
{
	chip_select (1);
	chip_io (ENC28J60_READ_BUF_MEM);	/* issue read command */
	while (len--) {
		*data++ = chip_io (0);
	}
	chip_select (0);
}

static void
chip_write_buffer (unsigned len, unsigned char *data)
{
	chip_select (1);
	chip_io (ENC28J60_WRITE_BUF_MEM);	/* issue write command */
	while (len--) {
		chip_io (*data++);
	}
	chip_select (0);
}

static void
chip_set_bank (enc28j60_t *u, unsigned address)
{
	if ((address & BANK_MASK) != u->bank) {
		/* set the bank (if needed) */
		chip_write_op (ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
		chip_write_op (ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
		u->bank = address & BANK_MASK;
	}
}

static unsigned
chip_read (enc28j60_t *u, unsigned address)
{
	chip_set_bank (u, address);
	return chip_read_op (ENC28J60_READ_CTRL_REG, address);
}

static void
chip_write (enc28j60_t *u, unsigned address, unsigned data)
{
	chip_set_bank (u, address);
	chip_write_op (ENC28J60_WRITE_CTRL_REG, address, data);
}

/*
 * PHY register write
 */
static void
chip_phy_write (enc28j60_t *u, unsigned address, unsigned data)
{
	chip_write (u, MIREGADR, address);	/* set the PHY register address */
	chip_write (u, MIWRL, data);		/* write the PHY data */
	chip_write (u, MIWRH, data >> 8);

	/* wait until the PHY write completes */
	while (chip_read (u, MISTAT) & MISTAT_BUSY)
		continue;
}

/*
 * PHY register read
 */
static unsigned
chip_phy_read (enc28j60_t *u, unsigned address)
{
	unsigned data;

	chip_write (u, MIREGADR, address);
	chip_write (u, MICMD, MICMD_MIIRD);

	/* wait until the PHY read completes */
	while (chip_read (u, MISTAT) & MISTAT_BUSY)
		continue;

	chip_write (u, MICMD, 0);
	data = chip_read (priv, MIRDL);
	data |= chip_read (priv, MIRDH) << 8;
	return data;
}


static void
chip_packet_send (enc28j60_t *u, unsigned len, unsigned char *packet)
{
	/* Set the write pointer to start of transmit buffer area */
	chip_write (u, EWRPTL, TXSTART_INIT);
	chip_write (u, EWRPTH, TXSTART_INIT>>8);
	/* Set the TXND pointer to correspond to the packet size given */
	chip_write (u, ETXNDL, (TXSTART_INIT+len));
	chip_write (u, ETXNDH, (TXSTART_INIT+len)>>8);

	/* write per-packet control byte */
	chip_write_op (ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	/* copy the packet into the transmit buffer */
	chip_write_buffer (len, packet);

	/* send the contents of the transmit buffer onto the network */
	chip_write_op (ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

static unsigned
chip_packet_receive (enc28j60_t *u, unsigned maxlen, unsigned char *packet)
{
	unsigned rxstat, len;

	/* check if a packet has been received and buffered */
	if (! chip_read (u, EPKTCNT))
		return 0;

	/* Set the read pointer to the start of the received packet */
	chip_write (u, ERDPTL, (u->next_packet_ptr));
	chip_write (u, ERDPTH, (u->next_packet_ptr)>>8);

	/* read the next packet pointer */
	u->next_packet_ptr  = chip_read_op (ENC28J60_READ_BUF_MEM, 0);
	u->next_packet_ptr |= chip_read_op (ENC28J60_READ_BUF_MEM, 0)<<8;

	/* read the packet length */
	len  = chip_read_op (ENC28J60_READ_BUF_MEM, 0);
	len |= chip_read_op (ENC28J60_READ_BUF_MEM, 0)<<8;

	/* read the receive status */
	rxstat  = chip_read_op (ENC28J60_READ_BUF_MEM, 0);
	rxstat |= chip_read_op (ENC28J60_READ_BUF_MEM, 0)<<8;

	/* limit retrieve length
	 * (we reduce the MAC-reported length by 4 to remove the CRC) */
	if (len > maxlen)
		len = maxlen;

	/* copy the packet from the receive buffer */
	chip_read_buffer (len, packet);

	/* Move the RX read pointer to the start of the next received packet
	 * This frees the memory we just read out */
	chip_write (u, ERXRDPTL, (u->next_packet_ptr));
	chip_write (u, ERXRDPTH, (u->next_packet_ptr)>>8);

	/* decrement the packet counter indicate we are done with this packet */
	chip_write_op (ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

	return len;
}

static buf_t *
enc28j60_input (enc28j60_t *u)
{
	buf_t *p;

	mutex_lock (&u->netif.lock);
	p = buf_queue_get (&u->inq);
	mutex_unlock (&u->netif.lock);
	return p;
}

static void
enc28j60_set_address (enc28j60_t *u, unsigned char *addr)
{
	mutex_lock (&u->netif.lock);
	memcpy (&u->netif.ethaddr, addr, 6);

	/* Set MAC address.
	 * Note: MAC address in ENC28J60 is byte-backward. */
	chip_write (u, MAADR6, u->netif.ethaddr[5]);
	chip_write (u, MAADR5, u->netif.ethaddr[4]);
	chip_write (u, MAADR4, u->netif.ethaddr[3]);
	chip_write (u, MAADR3, u->netif.ethaddr[2]);
	chip_write (u, MAADR2, u->netif.ethaddr[1]);
	chip_write (u, MAADR1, u->netif.ethaddr[0]);

	mutex_unlock (&u->netif.lock);
}

#if 0
/*
 * Fetch and process the received packet from the network controller.
 */
static void
enc28j60_receive_data (enc28j60_t *u)
{
	unsigned len, event;
	unsigned char *b, h;
	buf_t *p;

	/* Read and check RxEvent, expecting correctly received frame,
	 * either broadcast or individual address */
	event = cs_in (DATAREG);
	len = cs_in (DATAREG);
	if (! (event & RXE_OK) || len < 4 || len > ETH_MTU) {
		/* Skip this frame */
/*debug_printf ("enc28j60_receive_data: failed, event=%#04x, length %d bytes\n", event, len);*/
		++u->netif.in_errors;
skip:
		/* Throw away the last committed received frame */
		cs_out (PACKETPP, PP_RXCFG);
		OUT_RXCFG (SKIP_1 | RX_OK_ENBL | RX_RUNT_ENBL);
		return;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
/*debug_printf ("enc28j60_receive_data: ok, event=%#04x, length %d bytes\n", event, len);*/

	if (buf_queue_is_full (&u->inq)) {
/*debug_printf ("enc28j60_receive_data: input overflow\n");*/
		++u->netif.in_discards;
		goto skip;
	}

	/* Allocate a buf chain with total length 'len' */
	p = buf_alloc (u->pool, len, 2);
	if (! p) {
		/* Could not allocate a buf - skip received frame */
/*debug_printf ("enc28j60_receive_data: ignore packet - out of memory\n");*/
		++u->netif.in_discards;
		goto skip;
	}

	/* Get received packet. */
	for (b = p->payload; ;) {
		b[0] = cs_inb (DATAREG);
		h = cs_inb (DATAREG + 1);
		if (len == 1)
			break;
		b[1] = h;
		if ((len -= 2) == 0)
			break;
		b += 2;
	}

	/* debug_dump ("enc28j60-RX", p->payload, p->len); */
	buf_queue_put (&u->inq, p);
}
#endif

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
bool_t
enc28j60_output (enc28j60_t *u, buf_t *p, small_uint_t prio)
{
#if 1
	return 0;
#else
	buf_t *q;
	small_uint_t hilo;
	unsigned char *b;
	unsigned len;

/*debug_printf ("enc28j60_output: transmit %d bytes\n", p->tot_len);*/
	mutex_lock (&u->netif.lock);

	/* Exit if link has failed */
	cs_out (PACKETPP, PP_LINEST);
	if (! (cs_in (PPDATA) & LINK_OK) || p->tot_len < 4 || p->tot_len > ETH_MTU) {
/*debug_printf ("enc28j60_output: transmit %d bytes, link failed\n", p->tot_len);*/
failed: 	++u->netif.out_errors;
		mutex_unlock (&u->netif.lock);
		buf_free (p);
		return 0;
	}

	/* Transmit command */
	cs_out (TXCMD, TX_START_ALL_BYTES);
	len = p->tot_len;
	if (len < 60)
		len = 60;
	cs_out (TXLENGTH, len);

#if 1	/* LY: более "умный" вариант, если нет места в tx-буфере, то:
	 *	- пробуем вычитать приемный буфер;
	 *	- считаем сбой если нет несущей;
	 *	- ждем готовности бесконечно, при аварии сработает watchdog;
	 * 	Проблем на момент commit'а не замечено.
	*/
	for (;;) {
		cs_out (PACKETPP, PP_BUSST);
		if (cs_in (PPDATA) & READY_FOR_TX_NOW)
			break;
		task_yield ();
		enc28j60_interrupt (u);
		cs_out (PACKETPP, PP_LINEST);
		if (! (cs_in (PPDATA) & LINK_OK))
			goto failed;
	}
	if (u->inq.count != 0)
		mutex_signal (&u->netif.lock, 0);
#else	/* LY: А так было раньше: */
	cs_out (PACKETPP, PP_BUSST);
	tries = 0;
	/* Wait until ready for transmission, up to 100 retries */
	while (! (cs_in (PPDATA) & READY_FOR_TX_NOW) && (tries++ < 100)) {
		/* Throw away the last committed received frame */
		cs_out (PACKETPP, PP_RXCFG);
		OUT_RXCFG (SKIP_1 | RX_OK_ENBL | RX_RUNT_ENBL);
		cs_out (PACKETPP, PP_BUSST);
	}

	/* Ready to transmit? */
	if (! (cs_in (PPDATA) & READY_FOR_TX_NOW))
		goto failed;
#endif

	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */

	hilo = DATAREG;
	q = p; do {
		/* LY: писать данные в чип нужно всегда словам.
		 * Делаем здесь все правильно, с учетом возможной фрагментации пакета.
		 */
		assert (q->len > 0);
		b = q->payload;
		for (len = q->len; len; --len) {
			cs_outb (hilo, *b++);
			hilo ^= 1;
		}
		if (hilo != DATAREG)
			cs_outb (DATAREG + 1, 0);
		q = q->next;
	} while (q);
	/* debug_dump ("enc28j60-TX", p->payload, p->len); */

	/* LY: pad eth-frame tail, else enc28j60 just repead last byte. */
	if (p->tot_len < 60-1) {
		small_uint_t pad = (60 - p->tot_len) >> 1;
		do {
			cs_out (DATAREG, 0);
		} while (--pad);
	}

	++u->netif.out_packets;
	u->netif.out_bytes += p->tot_len;

	mutex_unlock (&u->netif.lock);
	buf_free (p);
	return 1;
#endif
}

/*
 * Process a pending interrupt.
 */
static void
enc28j60_interrupt (enc28j60_t *u)
{
#if 0
	for (;;) {
		cs_out (PACKETPP, PP_RXEVENT);
		if (0 == (cs_in (PPDATA) & RXE_ALL_BITS))
			break;

		enc28j60_receive_data (u);
	}

	/* Read RxMiss Counter (zeroes itself upon read) */
	cs_out (PACKETPP, PP_RXMISS);
	u->netif.in_discards += cs_in (PPDATA) >> 6;

	/* Read RxCol Counter (zeroes itself upon read) */
	cs_out (PACKETPP, PP_TXCOL);
	u->netif.out_collisions += cs_in (PPDATA) >> 6;
#endif
}

/*
 * Receive interrupt task.
 */
static void
enc28j60_receiver (void *arg)
{
	enc28j60_t *u = arg;

	for (;;) {
		/* Wait for the receive interrupt. */
		mutex_wait (&u->netif.lock);

		/* Process all pending interrupts. */
		enc28j60_interrupt (u);
	}
}

static netif_interface_t enc28j60_interface = {
	(bool_t (*) (netif_t*, buf_t*, small_uint_t))
						enc28j60_output,
	(buf_t *(*) (netif_t*))			enc28j60_input,
	(void (*) (netif_t*, unsigned char*))	enc28j60_set_address,
};

/*
 * Set up the network interface.
 */
void
enc28j60_init (enc28j60_t *u, const char *name, int prio, mem_pool_t *pool,
	arp_t *arp)
{
	u->netif.interface = &enc28j60_interface;
	u->netif.name = name;
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));

	/* Initialize hardware. */
	mutex_lock (&u->netif.lock);
	chip_init_spi ();
	u->bank = 0;

	/* Disable CLKOUT. */
	chip_write (u, ECOCON, 0);

	/* Perform system reset */
	chip_write_op (ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	mdelay (10);

	/* Check CLKRDY bit to see if reset is complete */
	while (! (chip_read (u, ESTAT) & ESTAT_CLKRDY))
		continue;

	/* Set LED configuration.
	 * LEDA is yellow: duplex status and collision activity.
	 * LEDB is green: link status and transmit/receive activity. */
	chip_phy_write (u, PHLCON, 0x3ed6);

	/* Do bank 0 stuff.
	 * 16-bit transfers, must write low byte first.
	 * Set transmit buffer start.
	 * ETXST defaults to 0x0000 (beginnging of RAM). */
	chip_write (u, ETXSTL, TXSTART_INIT & 0xFF);
	chip_write (u, ETXSTH, TXSTART_INIT >> 8);

	/* Initialize receive buffer. */
	u->next_packet_ptr = RXSTART_INIT;
	chip_write (u, ERXSTL, RXSTART_INIT & 0xFF);
	chip_write (u, ERXSTH, RXSTART_INIT >> 8);

	/* Set receive pointer. */
	chip_write (u, ERXRDPTL, RXSTART_INIT & 0xFF);
	chip_write (u, ERXRDPTH, RXSTART_INIT >> 8);

	/* Set receive buffer end.
	 * ERXND defaults to 0x1FFF (end of RAM). */
	chip_write (u, ERXNDL, RXSTOP_INIT & 0xFF);
	chip_write (u, ERXNDH, RXSTOP_INIT >> 8);

	/* do bank 2 stuff
	 * enable MAC receive */
	chip_write (u, MACON1,
		MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);

	/* enable automatic padding and CRC operations */
	chip_write_op (ENC28J60_BIT_FIELD_SET, MACON3,
		MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

	/* set MACON 4 bits (necessary for half-duplex only) */
/*	chip_write_op (ENC28J60_BIT_FIELD_SET, MACON4, MACON4_DEFER);*/

	/* set inter-frame gap (non-back-to-back) */
	chip_write (u, MAIPGL, 0x12);
	chip_write (u, MAIPGH, 0x0C);

	/* set inter-frame gap (back-to-back) */
	chip_write (u, MABBIPG, 0x12);

	/* Set the maximum packet size which the controller will accept */
	chip_write (u, MAMXFLL, MAX_FRAMELEN & 0xFF);
	chip_write (u, MAMXFLH, MAX_FRAMELEN >> 8);

	/* no loopback of transmitted frames */
	chip_phy_write (u, PHCON2, PHCON2_HDLDIS);

	/* enable packet reception */
	chip_set_bank (u, ECON1);
	chip_write_op (ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	mutex_unlock (&u->netif.lock);

	/* Create receive task. */
	task_create (enc28j60_receiver, u, name, prio,
		u->stack, sizeof (u->stack));
}
