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
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <enc28j60/eth.h>
#include <enc28j60/regs.h>

#define ETH_MTU		1518		/* maximum ethernet frame length */

/*
 * There are 8 kbytes of internal RAM.
 * Receive buffer must start at 0 because of chip bug (see errata).
 * Give TX buffer space for one full ethernet frame (~1500 bytes).
 */
#define MEM_SIZE	0x2000
#define MEM_TXSIZE	0x0600
#define MEM_RXSIZE	(MEM_SIZE - MEM_TXSIZE)
#define MEM_TXSTART	MEM_RXSIZE

/*
 * Configure I/O pins.
 */
static void inline
chip_init_spi (void)
{
#ifdef ENC28J60_SEISMONET_E10
/*
 * Microcontroller MSP430F5438
 */
#define ETH_IRQ		PORT2_VECTOR/2	/* Interrupt vector */

#define P2_ENIND	BIT1		/* Enable LEDs */
#define P2_ENCLK	BIT4		/* Enable clock */
#define P2_INT		BIT6		/* IRQ from ethernet */
#define P2_NRES		BIT7		/* /RESET */

#define P3_NCS		BIT6		/* UCB1STE */
#define P3_SDO		BIT7		/* UCB1SIMO */

#define P5_SDI		BIT4		/* UCB1SOMI */
#define P5_SCK		BIT5		/* UCB1CLK */

	P2DIR |= P2_ENIND | P2_ENCLK | P2_NRES;	/* Set outputs */
	P2DIR &= ~P2_INT;		/* Set INT as input */
	P2SEL &= ~(P2_ENIND | P2_ENCLK | P2_INT | P2_NRES);
	P2OUT &= ~P2_INT;
	P2OUT |= P2_ENIND | P2_ENCLK | P2_NRES;	/* Enable clock and indication */
	P2IES |= P2_INT;		/* Interrupt on falling edge */
	_msp430_p2ie |= P2_INT;		/* Interrupt enable */

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
	UCB1CTL0 = UCSYNC | UCMST |	/* 3-pin, 8-bit master SPI */
		UCMSB | UCCKPH;		/* MSB first, data changed on falling SCK */
	UCB1CTL1 |= UCSSEL_SMCLK;	/* Clock source SMCLK */
	UCB1BR0 = KHZ / 1000;		/* Set clock 1 MHz */
	UCB1BR1 = 0;
	UCB1CTL1 &= ~UCSWRST;		/* Clear reset */
#endif

#ifdef ENC28J60_SEISMONET_M10
/*
 * Microcontroller MSP430F2272
 */
#define ETH_IRQ		PORT1_VECTOR/2	/* Interrupt vector */

#define P1_INT		BIT3		/* IRQ from ethernet */

#define P3_SDO		BIT1		/* UCB0SIMO */
#define P3_SDI		BIT2		/* UCB1SOMI */
#define P3_SCK		BIT3		/* UCB0CLK */

#define P4_NRES		BIT1		/* /RESET for ethernet */
#define P4_NCS		BIT2		/* /CS for ethernet */

	P1DIR &= ~P1_INT;		/* Set INT as input */
	P1SEL &= ~P1_INT;
	P1OUT &= ~P1_INT;
	P1IES |= P1_INT;		/* Interrupt on falling edge */
	_msp430_p1ie |= P1_INT;		/* Interrupt enable */

	P3DIR |= P3_SDO | P3_SCK;	/* Set SCK and MOSI as outputs */
	P3DIR &= ~P3_SDI;		/* Set MISO as input */
	P3SEL |= P3_SDO | P3_SDI | P3_SCK; /* Special functions for SPI pins */
	P3OUT &= ~(P3_SDO | P3_SCK |	/* Clear SDO, SDI, SCK and TEST. */
		P3_SDI);

	P4DIR |= P4_NRES | P4_NCS;	/* Set /RESET and /CS as outputs */
	P4SEL &= ~(P4_NRES | P4_NCS);
	P4OUT |= P4_NRES | P4_NCS;	/* Set /RESET and /CS */

	/* Use USCI_B0 as SPI interface. */
	UCB0CTL1 = UCSWRST;		/* Reset */
	UCB0CTL0 = UCSYNC | UCMST |	/* 3-pin, 8-bit master SPI */
		UCMSB | UCCKPH;		/* MSB first, data changed on falling SCK */
	UCB0CTL1 |= UCSSEL_SMCLK;	/* SMCLK, max 10 MHz */
	UCB0BR0 = KHZ / 1000;		/* Set clock 1 MHz */
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST;		/* Clear reset */
#endif
}

static void inline
chip_select (int on)
{
#ifdef ENC28J60_SEISMONET_E10
	if (on)
		P3OUT &= ~P3_NCS;	/* assert CS */
	else
		P3OUT |= P3_NCS;	/* release CS */
#endif
#ifdef ENC28J60_SEISMONET_M10
	if (on)
		P4OUT &= ~P4_NCS;	/* assert CS */
	else
		P4OUT |= P4_NCS;	/* release CS */
#endif
}

static unsigned
chip_io (unsigned data)
{
	/*debug_printf ("chip_io (%02x)", data);*/
#ifdef ENC28J60_SEISMONET_E10
	while (! (UCB1IFG & UCTXIFG))	/* wait until TX buffer empty */
		continue;
	UCB1TXBUF = data;		/* send byte */
  	while (! (UCB1IFG & UCRXIFG))	/* data present in RX buffer? */
		continue;
	data = UCB1RXBUF;		/* read data */
#endif
#ifdef ENC28J60_SEISMONET_M10
	while (! (IFG2 & UCB0TXIFG))	/* wait until TX buffer empty */
		continue;
	UCB0TXBUF = data;		/* send byte */
  	while (! (IFG2 & UCB0RXIFG))	/* data present in RX buffer? */
		continue;
	data = UCB0RXBUF;		/* read data */
#endif
	/*debug_printf (" returned %02x\n", data);*/
	return data;
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
chip_write_op (unsigned op, unsigned address, unsigned char data)
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
chip_write (enc28j60_t *u, unsigned address, unsigned char data)
{
	chip_set_bank (u, address);
	chip_write_op (ENC28J60_WRITE_CTRL_REG, address, data);
}

static void
chip_write16 (enc28j60_t *u, unsigned address, unsigned data)
{
	chip_set_bank (u, address);
	chip_write_op (ENC28J60_WRITE_CTRL_REG, address, data);
	chip_write_op (ENC28J60_WRITE_CTRL_REG, address + 1, data >> 8);
}

static void
chip_set_bits (enc28j60_t *u, unsigned address, unsigned char data)
{
	chip_set_bank (u, address);
	chip_write_op (ENC28J60_BIT_FIELD_SET, address, data);
}

static void
chip_clear_bits (enc28j60_t *u, unsigned address, unsigned char data)
{
	chip_set_bank (u, address);
	chip_write_op (ENC28J60_BIT_FIELD_CLR, address, data);
}

/*
 * PHY register write
 */
static void
chip_phy_write (enc28j60_t *u, unsigned address, unsigned data)
{
	chip_write (u, MIREGADR, address);	/* set the PHY register address */
	chip_write16 (u, MIWRL, data);		/* write the PHY data */

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
	data = chip_read (u, MIRDL);
	data |= chip_read (u, MIRDH) << 8;
	return data;
}

void
enc28j60_debug (enc28j60_t *u, struct _stream_t *stream)
{
	unsigned short eir, estat, phstat1, phstat2;

	mutex_lock (&u->netif.lock);
	eir = chip_read (u, EIR);
	estat = chip_read (u, ESTAT);
	phstat1 = chip_phy_read (u, PHSTAT1);
	phstat2 = chip_phy_read (u, PHSTAT2);
	mutex_unlock (&u->netif.lock);

	printf (stream, "EIR=%b\n", eir, EIR_BITS);
	printf (stream, "ESTAT=%b\n", estat, ESTAT_BITS);
	printf (stream, "PHSTAT1=%b\n", phstat1, PHSTAT1_BITS);
	printf (stream, "PHSTAT2=%b\n", phstat2, PHSTAT2_BITS);
}

void enc28j60_start_negotiation (enc28j60_t *u)
{
	mutex_lock (&u->netif.lock);
/*TODO*/
	mutex_unlock (&u->netif.lock);
}

int enc28j60_get_carrier (enc28j60_t *u)
{
	unsigned phstat2;

	mutex_lock (&u->netif.lock);
	phstat2 = chip_phy_read (u, PHSTAT2);
	mutex_unlock (&u->netif.lock);
	return (phstat2 & PHSTAT2_LSTAT) != 0;
}

long enc28j60_get_speed (enc28j60_t *u, int *duplex)
{
	unsigned phstat2;

	mutex_lock (&u->netif.lock);
	phstat2 = chip_phy_read (u, PHSTAT2);
	mutex_unlock (&u->netif.lock);
	if (! (phstat2 & PHSTAT2_LSTAT))
		return 0;
	if (phstat2 & PHSTAT2_DPXSTAT) {
                if (duplex)
                        *duplex = 1;
        } else {
                if (duplex)
                        *duplex = 0;
        }
        return u->netif.bps;
}

void enc28j60_set_loop (enc28j60_t *u, int on)
{
	mutex_lock (&u->netif.lock);
	/* MAC loopback. */
	if (on)
		chip_write (u, MACON1, MACON1_LOOPBK |
			MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	else
		chip_write (u, MACON1,
			MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	mutex_unlock (&u->netif.lock);
}

void enc28j60_set_promisc (enc28j60_t *u, int station, int group)
{
	unsigned erxfcon;

	mutex_lock (&u->netif.lock);
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
	mutex_unlock (&u->netif.lock);
}

/*
 * Write the packet to chip memory and start transmission.
 * Deallocate the packet.
 */
static void
chip_transmit_packet (enc28j60_t *u, buf_t *p)
{
	buf_t *q;
	unsigned len, i;

	/* Set the write pointer to start of transmit buffer area. */
	chip_write16 (u, EWRPTL, MEM_TXSTART);

	/* Set the TXND pointer to correspond to the packet size given. */
	len = p->tot_len;
	if (len < 60)
		len = 60;
	chip_write16 (u, ETXNDL, MEM_TXSTART + len);

	/* Write per-packet control byte. */
	chip_write_op (ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	/* Send the data from the buf chain to the interface,
	 * one buf at a time. The size of the data in each
	 * buf is kept in the ->len variable. */
	for (q=p; q; q=q->next) {
		/* Copy the packet into the transmit buffer. */
		assert (q->len > 0);
		chip_write_buffer (q->len, q->payload);
	}
	/* Pad to minimal ethernet packet size (64 bytes). */
	for (i=p->tot_len; i<len; i++)
		chip_write_op (ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	/* Send the contents of the transmit buffer onto the network. */
	chip_set_bits (u, ECON1, ECON1_TXRTS);

	++u->netif.out_packets;
	u->netif.out_bytes += len;
	buf_free (p);
}

/*
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */
static bool_t
enc28j60_output (enc28j60_t *u, buf_t *p, small_uint_t prio)
{
	mutex_lock (&u->netif.lock);

	/* Exit if link has failed */
	if (p->tot_len < 4 || p->tot_len > ETH_MTU ||
	    ! (chip_phy_read (u, PHSTAT2) & PHSTAT2_LSTAT)) {
/*debug_printf ("enc28j60_output: transmit %d bytes, link failed\n", p->tot_len);*/
		++u->netif.out_errors;
		mutex_unlock (&u->netif.lock);
		buf_free (p);
		return 0;
	}
/*debug_printf ("enc28j60_output: transmit %d bytes\n", p->tot_len);*/

	if (chip_read (u, ECON1) & ECON1_TXRTS) {
		/* Занято, ставим в очередь. */
		if (buf_queue_is_full (&u->outq)) {
			++u->netif.out_discards;
			mutex_unlock (&u->netif.lock);
/*			debug_printf ("enc28j60_output: overflow\n");*/
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

/*
 * ERXRDPT need to be set always at odd addresses, refer to errata datasheet
 */
static void
set_erxrdpt (enc28j60_t *u, unsigned ptr)
{
	--ptr;
	if (ptr < 0 || ptr >= MEM_RXSIZE)
		ptr = MEM_RXSIZE - 1;
	chip_write16 (u, ERXRDPTL, ptr);
/*debug_printf (" set ERXRDPT to %#04x\n", ptr);*/
}


/*
 * Fetch and process the received packet from the network controller.
 */
static void
enc28j60_receive_data (enc28j60_t *u)
{
	unsigned char buf[6];
	unsigned len, rxstat;
	buf_t *p;

	/* Set the read pointer to the start of the received packet. */
/*debug_printf ("enc28j60_receive_data:\n set ERDPT to %#04x\n", u->next_packet_ptr);*/
	chip_write16 (u, ERDPTL, u->next_packet_ptr);

	/* Read next packet pointer, packet length and receive status. */
	chip_read_buffer (6, buf);
	u->next_packet_ptr = buf[0] | buf[1] << 8;
	len = buf[2] | buf[3] << 8;
	rxstat = buf[4] | buf[5] << 8;
/*debug_printf (" next packet pointer = %#04x\n", u->next_packet_ptr);*/

	if (! (rxstat & RSV_RXOK) || len < 4 || len > ETH_MTU) {
		/* Skip this frame */
/*debug_printf ("enc28j60_receive_data: failed, rxstat=%#04x, length %d bytes\n", rxstat, len);*/
		/*TODO: if next_packet_ptr or len is incorrect, reset the receiver.*/
		++u->netif.in_errors;
		goto skip;
	}
	++u->netif.in_packets;
	u->netif.in_bytes += len;
/*debug_printf ("enc28j60_receive_data: ok, rxstat=%#04x, length %d bytes\n", rxstat, len);*/

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

	/* Copy the packet from the receive buffer. */
	chip_read_buffer (len, p->payload);
	buf_queue_put (&u->inq, p);
skip:
	/* Move the RX read pointer to the start of the next received packet
	 * This frees the memory we just read out */
	set_erxrdpt (u, u->next_packet_ptr);

	/* decrement the packet counter indicate we are done with this packet */
	chip_set_bits (u, ECON2, ECON2_PKTDEC);
}

static unsigned
handle_interrupts (enc28j60_t *u)
{
	unsigned active, eir;
	buf_t *p;

	active = 0;
	for (;;) {
		/* Process all pending interrupts. */
		eir = chip_read (u, EIR);
/*debug_printf ("enc28j60 irq: EIR = %b\n", eir, EIR_BITS);*/
		if (eir & EIR_RXERIF) {
			/* Count lost incoming packets. */
			++u->netif.in_discards;
			chip_clear_bits (u, EIR, EIR_RXERIF);
			chip_clear_bits (u, ESTAT, ESTAT_BUFER);
		}
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
		if (! (eir & (EIR_PKTIF | EIR_TXIF))) {
			/* All interrupts processed. */
			return active;
		}
		++active;

		/* Check if a packet has been received and buffered. */
		while (chip_read (u, EPKTCNT) != 0) {
			enc28j60_receive_data (u);
		}
		if (eir & EIR_TXIF) {
			chip_clear_bits (u, EIR, EIR_TXIF);

			/* Transmit a packet from queue. */
			p = buf_queue_get (&u->outq);
			if (p)
				chip_transmit_packet (u, p);
		}
	}
}

/*
 * Receive interrupt task.
 */
void
enc28j60_poll (enc28j60_t *u)
{
	mutex_lock (&u->netif.lock);
	if (handle_interrupts (u))
		mutex_signal (&u->netif.lock, 0);
	mutex_unlock (&u->netif.lock);
}

/*
 * Receive interrupt task.
 */
static void
enc28j60_receiver (void *arg)
{
	enc28j60_t *u = arg;

	mutex_lock_irq (&u->netif.lock, ETH_IRQ, 0, 0);

	/* Enable interrupts. */
	chip_write (u, EIE, EIE_INTIE | EIE_PKTIE | EIE_TXIE);

	for (;;) {
		/* Wait for the interrupt. */
		mutex_wait (&u->netif.lock);
		++u->intr;
		handle_interrupts (u);
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
	buf_queue_init (&u->outq, u->outqdata, sizeof (u->outqdata));

	/* Initialize hardware. */
	mutex_lock (&u->netif.lock);
	chip_init_spi ();
	u->bank = 0;

	/* Perform system reset */
	chip_write_op (ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	mdelay (10);

	/* Check CLKRDY bit to see if reset is complete */
	while (! (chip_read (u, ESTAT) & ESTAT_CLKRDY))
		continue;

	/* Disable CLKOUT. */
	chip_write (u, ECOCON, 0);

	/* Set LED configuration.
	 * LEDA is yellow: duplex status and collision activity.
	 * LEDB is green: link status and transmit/receive activity. */
	chip_phy_write (u, PHLCON, 0x3ed6);

	/* Do bank 0 stuff.
	 * 16-bit transfers, must write low byte first.
	 * Set transmit buffer start.
	 * ETXST defaults to 0x0000 (beginnging of RAM). */
	chip_write16 (u, ETXSTL, MEM_TXSTART);

	/* Initialize receive buffer. */
	u->next_packet_ptr = 0;
	chip_write16 (u, ERXSTL, 0);

	/* Set receive end pointer. */
	set_erxrdpt (u, 0);

	/* Set receive buffer end.
	 * ERXND defaults to 0x1FFF (end of RAM). */
	chip_write16 (u, ERXNDL, MEM_RXSIZE - 1);

	/* Do bank 1 stuff.
	 * Enable CRC, unicast and broadcast filters. */
	chip_write (u, ERXFCON,
		ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

	/* Do bank 2 stuff.
	 * Enable MAC receive and pause control. */
	chip_write (u, MACON1,
		MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);

	/* Half duplex, automatic padding and CRC operations. */
	chip_set_bits (u, MACON3,
		MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);

	/* set MACON 4 bits (necessary for half-duplex only) */
	chip_set_bits (u, MACON4, MACON4_DEFER);

	/* set inter-frame gap (non-back-to-back) */
	chip_write16 (u, MAIPGL, 0x0C12);

	/* set inter-frame gap (back-to-back) */
	chip_write (u, MABBIPG, 0x12);

	/* Set the maximum packet size which the controller will accept */
	chip_write16 (u, MAMXFLL, ETH_MTU);

	/* Half duplex, disable loopback. */
	chip_phy_write (u, PHCON1, 0);

	/* No loopback of transmitted frames. */
	chip_phy_write (u, PHCON2, PHCON2_HDLDIS);

	chip_write (u, ECON2, ECON2_AUTOINC | ECON2_VRPS);

	/* Enable packet reception. */
	chip_set_bits (u, ECON1, ECON1_RXEN);
	mutex_unlock (&u->netif.lock);

	/* Create receive task. */
	task_create (enc28j60_receiver, u, name, prio,
		u->stack, sizeof (u->stack));
}
