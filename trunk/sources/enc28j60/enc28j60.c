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

#define P2_ENIND	BIT1
#define P2_ENCLK	BIT4
#define P2_INT		BIT6
#define P2_RES		BIT7

#define P3_NCS		BIT6 	/* UCB1STE */
#define P3_SDO		BIT7	/* UCB1SIMO */

#define P5_SDI		BIT4	/* UCB1SOMI */
#define P5_SCK		BIT5	/* UCB1CLK */

/*
 * Configure I/O pins.
 */
static void inline
chip_init (void)
{
	P3DIR |= P3_SDO | P3_NCS;	/* Set nCS and MOSI as outputs */
	P3SEL |= P3_SDO;		/* Special functions for SPI pins */
	P3OUT |= P3_NCS;

	P5DIR |= P5_SCK;		/* Set SCK as output */
	P5DIR &= ~P5_SDI;		/* Set MISO as input */
	P5SEL |= P5_SCK | P5_SDI;	/* Special functions for SPI pins */
	P5OUT = 0x00;

	/* Set SPI registers (see slau049f.pdf for reference). */
	U1CTL   = SWRST;		/* Put USART1 in reset mode */
	ME2    |= USPIE1;		/* Enable USART1 SPI mode */
	U1TCTL |= SSEL0 | STC;		/* Transmit control, (ACLK, 3-pin mode) */
	U1BR0   = 0x02;			/* Baud rate 0 (UCLK/2) (maximum rate) */
	U1BR1   = 0x00;			/* Baud rate 1 (upper 16 bit of baud rate divisor) */
	U1MCTL  = 0x00;			/* Modulation control (no modulation in SPI!) */
	U1CTL  |= CHAR | SYNC | MM;	/* USART control (8-bit SPI Master **SWRST**) */
	U1CTL  &= ~SWRST;		/* Deactivate reset state */
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
	while (! (IFG2 & UTXIFG1))	// wait until TX buffer empty
		continue;
	U1TXBUF = data;			// send byte
  	while (! (IFG2 & URXIFG1))	// data present in RX buffer?
		continue;
	return U1RXBUF;			// return read data
}

unsigned chip_ReadOp (unsigned op, unsigned address)
{
	unsigned data = 0;

	chip_select (1);			// assert CS signal
	chip_io (op | (address & ADDR_MASK));	// issue read command
	data = chip_io (0);			// read data (send zeroes)
	if (address & 0x80) {			// do dummy read if needed
		data = chip_io (0);		// read data (send zeroes)
	}
	chip_select (0);			// release CS signal
	return data;
}

void chip_WriteOp (unsigned op, unsigned address, unsigned data)
{
	chip_select (1);
	chip_io (op | (address & ADDR_MASK));	// issue write command
	if (op != ENC28J60_SOFT_RESET)
		chip_io (data);			// send data
	chip_select (0);
}

void chip_ReadBuffer (unsigned len, unsigned char *data)
{
	chip_select (1);
	chip_io (ENC28J60_READ_BUF_MEM);	// issue read command
	while (len--) {
		*data++ = chip_io (0);
	}
	chip_select (0);
}

void chip_WriteBuffer (unsigned len, unsigned char *data)
{
	chip_select (1);
	chip_io (ENC28J60_WRITE_BUF_MEM);	// issue write command
	while (len--) {
		chip_io (*data++);
	}
	chip_select (0);
}

void chip_SetBank (unsigned address)
{
	if ((address & BANK_MASK) != u->bank) {
		// set the bank (if needed)
		chip_WriteOp (ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
		chip_WriteOp (ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
		u->bank = address & BANK_MASK;
	}
}

unsigned chip_Read (unsigned address)
{
	chip_SetBank (address);									// set the bank
	return chip_ReadOp (ENC28J60_READ_CTRL_REG, address);
}

void chip_Write (unsigned address, unsigned data)
{
	chip_SetBank (address);									// set the bank
	chip_WriteOp (ENC28J60_WRITE_CTRL_REG, address, data);
}

void chip_PhyWrite (unsigned address, unsigned data)
{
	chip_Write (MIREGADR, address);		// set the PHY register address
	chip_Write (MIWRL, data);			// write the PHY data
	chip_Write (MIWRH, data >> 8);

	while (chip_Read (MISTAT) & MISTAT_BUSY);	// wait until the PHY write completes
}

void chip_PacketSend (unsigned len, unsigned char *packet)
{
	// Set the write pointer to start of transmit buffer area
	chip_Write (EWRPTL, TXSTART_INIT);
	chip_Write (EWRPTH, TXSTART_INIT>>8);
	// Set the TXND pointer to correspond to the packet size given
	chip_Write (ETXNDL, (TXSTART_INIT+len));
	chip_Write (ETXNDH, (TXSTART_INIT+len)>>8);

	// write per-packet control byte
	chip_WriteOp (ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	// copy the packet into the transmit buffer
	chip_WriteBuffer (len, packet);

	// send the contents of the transmit buffer onto the network
	chip_WriteOp (ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

unsigned chip_PacketReceive (unsigned maxlen, unsigned char *packet)
{
	unsigned rxstat, len;

	// check if a packet has been received and buffered
	if (!chip_Read (EPKTCNT))
		return 0;

	// Set the read pointer to the start of the received packet
	chip_Write (ERDPTL, (u->next_packet_ptr));
	chip_Write (ERDPTH, (u->next_packet_ptr)>>8);
	// read the next packet pointer
	u->next_packet_ptr  = chip_ReadOp (ENC28J60_READ_BUF_MEM, 0);
	u->next_packet_ptr |= chip_ReadOp (ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length
	len  = chip_ReadOp (ENC28J60_READ_BUF_MEM, 0);
	len |= chip_ReadOp (ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the receive status
	rxstat  = chip_ReadOp (ENC28J60_READ_BUF_MEM, 0);
	rxstat |= chip_ReadOp (ENC28J60_READ_BUF_MEM, 0)<<8;

	// limit retrieve length
	// (we reduce the MAC-reported length by 4 to remove the CRC)
	len = MIN(len, maxlen);

	// copy the packet from the receive buffer
	chip_ReadBuffer (len, packet);

	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	chip_Write (ERXRDPTL, (u->next_packet_ptr));
	chip_Write (ERXRDPTH, (u->next_packet_ptr)>>8);

	// decrement the packet counter indicate we are done with this packet
	chip_WriteOp (ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

	return len;
}

// For compatibility with the CS8900A package
void enc28j60_send (void)
{
	chip_PacketSend (uip_len, uip_buf);
}

// For compatibility with the CS8900A package
u16_t enc28j60_poll (void)
{
	return chip_PacketReceive (UIP_BUFSIZE, uip_buf);
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
	arp_t *arp, const char *macaddr)
{
	u->netif.interface = &enc28j60_interface;
	u->netif.name = name;
	u->netif.arp = arp;
	u->netif.mtu = 1500;
	u->netif.type = NETIF_ETHERNET_CSMACD;
	u->netif.bps = 10000000;
	u->pool = pool;
	buf_queue_init (&u->inq, u->inqdata, sizeof (u->inqdata));

	/* Obtain MAC address from network interface.
	 * We just fake an address... */
	u->netif.ethaddr[0] = macaddr[0];
	u->netif.ethaddr[1] = macaddr[1];
	u->netif.ethaddr[2] = macaddr[2];
	u->netif.ethaddr[3] = macaddr[3];
	u->netif.ethaddr[4] = macaddr[4];
	u->netif.ethaddr[5] = macaddr[5];

	/*
	 * Initialize hardware.
	 */
	mutex_lock (&u->netif.lock);
	chip_init ();

	/* Set clock to 8.333MHz */
	chip_Write (ECOCON, 0x03);

	// perform system reset
	chip_WriteOp (ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	mdelay (10);

	// check CLKRDY bit to see if reset is complete
	while (!(chip_Read (ESTAT) & ESTAT_CLKRDY));

	// Set LED configuration
	//chip_PhyWrite (PHLCON, 0x3ba6);

	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	u->next_packet_ptr = RXSTART_INIT;
	chip_Write (ERXSTL, RXSTART_INIT&0xFF);
	chip_Write (ERXSTH, RXSTART_INIT>>8);
	// set receive pointer address
	chip_Write (ERXRDPTL, RXSTART_INIT&0xFF);
	chip_Write (ERXRDPTH, RXSTART_INIT>>8);
	// set receive buffer end
	// ERXND defaults to 0x1FFF (end of ram)
	chip_Write (ERXNDL, RXSTOP_INIT&0xFF);
	chip_Write (ERXNDH, RXSTOP_INIT>>8);
	// set transmit buffer start
	// ETXST defaults to 0x0000 (beginnging of ram)
	chip_Write (ETXSTL, TXSTART_INIT&0xFF);
	chip_Write (ETXSTH, TXSTART_INIT>>8);

	// do bank 2 stuff
	// enable MAC receive
	chip_Write (MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// enable automatic padding and CRC operations
	chip_WriteOp (ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// set MACON 4 bits (necessary for half-duplex only)
	//chip_WriteOp (ENC28J60_BIT_FIELD_SET, MACON4, MACON4_DEFER);

	// set inter-frame gap (non-back-to-back)
	chip_Write (MAIPGL, 0x12);
	chip_Write (MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	chip_Write (MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
	chip_Write (MAMXFLL, MAX_FRAMELEN&0xFF);
	chip_Write (MAMXFLH, MAX_FRAMELEN>>8);

	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	chip_Write (MAADR6, ENC28J60_MAC5);
	chip_Write (MAADR5, ENC28J60_MAC4);
	chip_Write (MAADR4, ENC28J60_MAC3);
	chip_Write (MAADR3, ENC28J60_MAC2);
	chip_Write (MAADR2, ENC28J60_MAC1);
	chip_Write (MAADR1, ENC28J60_MAC0);

	// no loopback of transmitted frames
	chip_PhyWrite (PHCON2, PHCON2_HDLDIS);

	// switch to bank 0
	chip_SetBank (ECON1);
	// enable packet reception
	chip_WriteOp (ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	mutex_unlock (&u->netif.lock);

	/* Create receive task. */
	task_create (enc28j60_receiver, u, name, prio,
		u->stack, sizeof (u->stack));
}
