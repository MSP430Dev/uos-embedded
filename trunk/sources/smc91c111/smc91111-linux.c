/*------------------------------------------------------------------------
 . smc91111.c
 . This is a driver for SMSC's 91C111 single-chip Ethernet device.
 .
 . Copyright (C) 2001 Standard Microsystems Corporation (SMSC)
 .       Developed by Simple Network Magic Corporation (SNMC)
 . Copyright (C) 1996 by Erik Stahlman (ES)
 .
 . This program is free software; you can redistribute it and/or modify
 . it under the terms of the GNU General Public License as published by
 . the Free Software Foundation; either version 2 of the License, or
 . (at your option) any later version.
 .
 . This program is distributed in the hope that it will be useful,
 . but WITHOUT ANY WARRANTY; without even the implied warranty of
 . MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 . GNU General Public License for more details.
 .
 . You should have received a copy of the GNU General Public License
 . along with this program; if not, write to the Free Software
 . Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 .
 . Information contained in this file was obtained from the LAN91C111
 . manual from SMC.  To get a copy, if you really want one, you can find 
 . information under www.smsc.com.
 . 
 . "Features" of the SMC chip:
 .   Integrated PHY/MAC for 10/100BaseT Operation
 .   Supports internal and external MII
 .   Integrated 8K packet memory
 .   EEPROM interface for configuration
 .
 . Module arguments:
 . 	io	= for the base address
 .	irq	= for the IRQ
 .	nowait	= 0 for normal wait states, 1 eliminates additional wait states
 . author:
 . 	Erik Stahlman				( erik@vt.edu )
 . 	Daris A Nevil				( dnevil@snmc.com )
 .  	Pramod B Bhardwaj   			(pramod.bhardwaj@smsc.com)
 . Hardware multicast code from Peter Cammaert ( pc@denkart.be )
 .
 . Sources:
 .    o   SMSC LAN91C111 databook (www.smsc.com)
 .    o   smc9194.c by Erik Stahlman
 .    o   skeleton.c by Donald Becker ( becker@cesdis.gsfc.nasa.gov )
 .
 . History:
 .    09/24/01  Pramod B Bhardwaj, Added the changes for Kernel 2.4
 .    08/21/01  Pramod B Bhardwaj Added support for RevB of LAN91C111
 .	04/25/01  Daris A Nevil  Initial public release through SMSC
 .	03/16/01  Daris A Nevil  Modified smc9194.c for use with LAN91C111
 ----------------------------------------------------------------------------*/

#define POWER_DOWN	1	/* Use power-down feature of the chip */
#undef NO_AUTOPROBE

static const char version[] = "SMSC LAN91C111 v2.1, 05/24/05\n";
#define DRV_NAME "SMSC LAN91C111"
#define DRV_VERSION "v2.1, 05/24/05"

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include "smc91111.h"
#include "smc91111_core.h"

/* The LAN91C111 can be at any of the following port addresses.  To change,
   for a slightly different card, you can add it to the array.  Keep in
   mind that the array must be zero-terminated */
__attribute__ ((unused)) static unsigned int smc_portlist[] __initdata =
    { 0x200, 0x220, 0x240, 0x260, 0x280, 0x2A0, 0x2C0, 0x2E0,
	0x300, 0x320, 0x340, 0x360, 0x380, 0x3A0, 0x3C0, 0x3E0, 0
};

/* Wait time for memory to be free.  This probably shouldn't be
   tuned that much, as waiting for this means nothing else happens
   in the system */
#define MEMORY_WAIT_TIME 16

#define EXPLICITLY_MEANINGLESS_VALUE 0xfeed

/*
 . DEBUGGING LEVELS
 .
 . 0 for normal operation
 . 1 for slightly more details
 . >2 for various levels of increasingly useless information
 .    2 for interrupt tracking, status flags
 .    3 for packet info
 .    4 for complete packet dumps
*/
#define SMC_DEBUG 0

#if (SMC_DEBUG > 2 )
#define PRINTK3(args...) printk(args)
#else
#define PRINTK3(args...)
#endif

#if SMC_DEBUG > 1
#define PRINTK2(args...) printk(args)
#else
#define PRINTK2(args...)
#endif

#ifdef SMC_DEBUG
#define PRINTK(args...) printk(args)
#else
#define PRINTK(args...)
#endif

#undef _inb
#undef _outb
#undef _inw
#undef _outw
#undef _inl
#undef _outl
#undef inb
#undef outb
#undef inw
#undef outw
#undef inl
#undef outl
#undef insw
#undef insl
#undef outsw
#undef outsl

#define SMC_MAXDEVICES 8

static struct net_device *smc_netdev[SMC_NUMDEVICES];

static inline uint8_t smc91111__read_interrupt_mask(int ioaddr, int devno)
{
	return smc_inb(ioaddr + IM_REG, devno);
}

static inline uint8_t smc91111__read_interrupt_status(int ioaddr, int devno)
{
	return smc_inb(ioaddr + INT_REG, devno);
}

static inline uint8_t smc91111_read_interrupt_mask(int ioaddr, int devno)
{
	SMC_SELECT_BANK(2, devno);
	return smc91111__read_interrupt_mask(ioaddr, devno);
}

static inline uint8_t smc91111_read_interrupt_status(int ioaddr, int devno)
{
	SMC_SELECT_BANK(2, devno);
	return smc91111__read_interrupt_status(ioaddr, devno);
}

static void smc_set_hwaddr(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	u_int16_t *addr = (u_int16_t *) dev->dev_addr;

	SMC_SELECT_BANK(1, devno);
	smc_outw(addr[0], ioaddr + ADDR0_REG, devno);
	smc_outw(addr[1], ioaddr + ADDR1_REG, devno);
	smc_outw(addr[2], ioaddr + ADDR2_REG, devno);
}

__attribute__ ((unused)) static void smc_get_hwaddr(uint16_t *addr, struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;

	SMC_SELECT_BANK(1, devno);
	addr[0] = smc_inw(ioaddr + ADDR0_REG, devno);
	addr[1] = smc_inw(ioaddr + ADDR1_REG, devno);
	addr[2] = smc_inw(ioaddr + ADDR2_REG, devno);
}

/* assumes smc register bank #2 active */
static void smc__busywait_on_mmu(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	while (smc_inw(dev->base_addr + MMU_CMD_REG, lp->devno) & MC_BUSY)
		udelay(1);
}

/* assumes smc register bank #2 active */
static void smc__reset_mmu(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	smc_outw(MC_RESET, dev->base_addr + MMU_CMD_REG, lp->devno);
	smc__busywait_on_mmu(dev);
}

static void smc_clear_rxfifo(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno, rxfifo;
	
	SMC_SELECT_BANK(2, devno);
	while (!((rxfifo = smc_inw(dev->base_addr + RXFIFO_REG, devno)) & RXFIFO_REMPTY)) {
		smc_outw(MC_RELEASE, dev->base_addr + MMU_CMD_REG, devno);
		smc__busywait_on_mmu(dev);
	}
}

static void smc_kill_outstanding_tx(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);

	if (lp->saved_skb != NULL) {
		dev_kfree_skb_any(lp->saved_skb);
		lp->saved_skb = NULL;
	}
}

static void smc_restart_with_clean_buffers(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	int ioaddr = dev->base_addr;

	smc_kill_outstanding_tx(dev);
	
	SMC_SELECT_BANK(2, lp->devno);
	smc__reset_mmu(dev);
}

/*
 * The internal workings of the driver.  If you are changing anything
 * here with the SMC stuff, you should have the datasheet and know
 * what you are doing.
 */
#define CARDNAME "LAN91C111"

#define LAN91C111_MEMORY_MULTIPLIER	(1024*2)

/*-----------------------------------------------------------------
 .
 .  The driver can be entered at any of the following entry points.
 .
 .------------------------------------------------------------------  */

/* This is called by  register_netdev().  It is responsible for
 . checking the portlist for the SMC9000 series chipset.  If it finds
 . one, then it will initialize the device, find the hardware information,
 . and sets up the appropriate device parameters.
 . NOTE: Interrupts are *OFF* when this procedure is called.
 .
 . NB:This shouldn't be static since it is referred to externally  */
int smc_init(struct net_device *dev);

/* This is called by  unregister_netdev().  It is responsible for
 . cleaning up before the driver is finally unregistered and discarded */
void smc_destructor(struct net_device *dev);

/* The kernel calls this function when someone wants to use the net_device,
 . typically 'ifconfig ethX up' */
static int smc_open(struct net_device *dev);
/* This is called by the kernel to send a packet out into the net.  it's
 . responsible for doing a best-effort send, but if it's simply not possible
 . to send it, the packet gets dropped */
static void smc_timeout(struct net_device *dev);
/* This is called by the kernel in response to 'ifconfig ethX down'.  It
 . is responsible for cleaning up everything that the open routine
 . does, and maybe putting the card into a powerdown state */
static int smc_close(struct net_device *dev);
/* This routine allows the proc file system to query the driver's statistics */
static struct net_device_stats *smc_query_statistics(struct net_device *dev);
/* Finally, a call to set promiscuous mode ( for TCPDUMP and related
 . programs ) and multicast modes */
static void smc_set_multicast_list(struct net_device *dev);
/* Configures the PHY through the MII Management interface */
static void smc_phy_configure(struct net_device *dev);

/* Interrupt level calls */
irqreturn_t smc_interrupt(int irq, void *);
/* This is a separate procedure to handle the receipt of a packet, to
 . leave the interrupt code looking slightly cleaner */
inline static void smc_rcv(struct net_device *dev);
/* This handles a TX interrupt, which is only called when an error
 . relating to a packet is sent */
inline static void smc_tx(struct net_device *dev);
/* This handles interrupts generated from PHY register 18 */
static void smc_phy_interrupt(struct net_device *dev);

/* Internal routines */
/* Test if a given location contains a chip, trying to cause as
 . little damage as possible if it's not a SMC chip */
static int smc_probe(struct net_device *dev, int ioaddr, int devno);

/* A rather simple routine to print out a packet for debugging purposes */
#if SMC_DEBUG > 2
static void print_packet(u_int8_t *, int);
#endif

#define tx_done(dev) 1

/* since I am not sure if I will have enough room in the chip's ram
 . to store the packet, I call this routine, which either sends it
 . now, or generates an interrupt when the card is ready for the
 . packet */
static int smc_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);
/* this is called to actually send the packet to the chip */
static int smc_hardware_send_packet(struct net_device *dev);
/* soft reset the device */
static void smc_reset(struct net_device *dev);
/* enable Interrupts, Receive, and Transmit */
static void smc_enable(struct net_device *dev);
/* put the device in an inactive state */
static void smc_shutdown(struct net_device *dev);
#ifndef NO_AUTOPROBE
/* This routine will find the IRQ of the driver if one is not
 . specified in the input to the device.  */
static int smc_findirq(int ioaddr, int devno);
#endif

/* this routine will set the hardware multicast table to the specified
   values given it by the higher level routines */
static void smc_setmulticast(int ioaddr, int count, struct dev_mc_list *, int devno);
static int crc32(char *, int);

/* Initilizes our device's sysctl proc filesystem */

#ifdef CONFIG_SYSCTL
static void smc_sysctl_register(struct net_device *);
static void smc_sysctl_unregister(struct net_device *);
#endif				/* CONFIG_SYSCTL */

/* Function: smc_reset( struct device* dev )
 . Purpose:
 .   This sets the SMC91111 chip to its normal state, hopefully from whatever
 .   mess that any other DOS driver has put it in.
 . Maybe I should reset more registers to defaults in here?  SOFTRST  should
 . do that for me.
 . Method:
 .	1.  send a SOFT RESET
 .	2.  wait for it to finish
 .	3.  enable autorelease mode
 .	4.  reset the MMU
 .	5.  mask out all interrupts
*/
static void smc_reset(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;

	PRINTK2("%s:smc_reset\n", dev->name);

	/* This resets the registers mostly to defaults, but doesn't
	   affect EEPROM.  That seems unnecessary */
	SMC_SELECT_BANK(0, devno);
	smc_outw(RCR_SOFTRST, ioaddr + RCR_REG, devno);

	/* Setup the Configuration Register */
	/* This is necessary because the CONFIG_REG is not affected */
	/* by a soft reset */
	SMC_SELECT_BANK(1, devno);
	smc_outw(CONFIG_DEFAULT, ioaddr + CONFIG_REG, devno);

	/* Setup for fast accesses if requested */
	/* If the card/system can't handle it then there will */
	/* be no recovery except for a hard reset or power cycle */
/* 	if (dev->dma) */
	smc_outw(smc_inw(ioaddr + CONFIG_REG, devno) | CONFIG_NO_WAIT,
		 ioaddr + CONFIG_REG, devno);

#ifdef POWER_DOWN
	/* Release from possible power-down state */
	/* Configuration register is not affected by Soft Reset */
	SMC_SELECT_BANK(1, devno);
	smc_outw(smc_inw(ioaddr + CONFIG_REG, devno) | CONFIG_EPH_POWER_EN,
		 ioaddr + CONFIG_REG, devno);
#endif

	SMC_SELECT_BANK(0, devno);

	/* this should pause enough for the chip to be happy */
	mdelay(10);

	/* Disable transmit and receive functionality */
	smc_outw(RCR_CLEAR, ioaddr + RCR_REG, devno);
	smc_outw(TCR_CLEAR, ioaddr + TCR_REG, devno);

	/* set the control register to automatically
	   release successfully transmitted packets, to make the best
	   use out of our limited memory */
	SMC_SELECT_BANK(1, devno);
	smc_outw(smc_inw(ioaddr + CTL_REG, devno) | CTL_AUTO_RELEASE, ioaddr + CTL_REG, devno);

	SMC_SELECT_BANK(2, devno);
	smc__reset_mmu(dev);

	/* Note:  It doesn't seem that waiting for the MMU busy is needed here,
	   but this is a place where future chipsets _COULD_ break.  Be wary
	   of issuing another MMU command right after this */

	/* Disable all interrupts */
	smc_outb(0, ioaddr + IM_REG, devno);
}

static void smc_enable(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	unsigned short ioaddr = dev->base_addr, devno = lp->devno;

	/* see the header file for options in TCR/RCR DEFAULT */
	SMC_SELECT_BANK(0, devno);
	smc_outw(lp->tcr_cur_mode, ioaddr + TCR_REG, devno);
	smc_outw(lp->rcr_cur_mode, ioaddr + RCR_REG, devno);

        SMC_SELECT_BANK(2, devno);
	/* now, enable interrupts:
	 *  IM_EPH_INT, for nasty errors
	 *  IM_RCV_INT, for happy received packets
	 *  IM_RX_OVRN_INT, because I have to kick the receiver
	 *  IM_MDINT, for PHY Register 18 Status Changes
	 */
        smc_outb(IM_EPH_INT | IM_RX_OVRN_INT | IM_RCV_INT | IM_MDINT,
	     ioaddr + IM_REG, devno);
	enable_irq(dev->irq);
}

/*
 . Function: smc_shutdown
 . Purpose:  closes down the SMC91xxx chip.
 . Method:
 .	1. zero the interrupt mask
 .	2. clear the enable receive flag
 .	3. clear the enable xmit flags
 .
 . TODO:
 .   (1) maybe utilize power down mode.
 .	Why not yet?  Because while the chip will go into power down mode,
 .	the manual says that it will wake up in response to any I/O requests
 .	in the register space.   Empirical results do not show this working.
*/
static void smc_shutdown(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	unsigned short ioaddr = dev->base_addr, devno = lp->devno;

	disable_irq(dev->irq);

	/* no more interrupts for me */
	SMC_SELECT_BANK(2, devno);
	smc_outb(0, ioaddr + IM_REG, devno);

	/* and tell the card to stay away from that nasty outside world */
	SMC_SELECT_BANK(0, devno);
	smc_outb(RCR_CLEAR, ioaddr + RCR_REG, devno);
	smc_outb(TCR_CLEAR, ioaddr + TCR_REG, devno);

#ifdef POWER_DOWN
	/* finally, shut the chip down */
	SMC_SELECT_BANK(1, devno);
	smc_outw(smc_inw(ioaddr + CONFIG_REG, devno) & ~CONFIG_EPH_POWER_EN,
	     ioaddr + CONFIG_REG, devno);
#endif
}

/*
 . Function: smc_setmulticast( int ioaddr, int count, dev_mc_list * adds )
 . Purpose:
 .    This sets the internal hardware table to filter out unwanted multicast
 .    packets before they take up memory.
 .
 .    The SMC chip uses a hash table where the high 6 bits of the CRC of
 .    address are the offset into the table.  If that bit is 1, then the
 .    multicast packet is accepted.  Otherwise, it's dropped silently.
 .
 .    To use the 6 bits as an offset into the table, the high 3 bits are the
 .    number of the 8 bit register, while the low 3 bits are the bit within
 .    that register.
 .
 . This routine is based very heavily on the one provided by Peter Cammaert.
*/

static void smc_setmulticast(int ioaddr, int count, struct dev_mc_list *addrs, int devno)
{
	int i;
	unsigned char multicast_table[8];
	struct dev_mc_list *cur_addr;
	/* table for flipping the order of 3 bits */
	unsigned char invert3[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

	PRINTK2("CARDNAME:smc_setmulticast\n");

	/* start with a table of all zeros: reject all */
	memset(multicast_table, 0, sizeof(multicast_table));

	cur_addr = addrs;
	for (i = 0; i < count; i++, cur_addr = cur_addr->next) {
		int position;

		/* do we have a pointer here? */
		if (!cur_addr)
			break;
		/* make sure this is a multicast address - shouldn't this
		   be a given if we have it here ? */
		if (!(*cur_addr->dmi_addr & 1))
			continue;

		/* only use the low order bits */
		position = crc32(cur_addr->dmi_addr, 6) & 0x3f;

		/* do some messy swapping to put the bit in the right spot */
		multicast_table[invert3[position & 7]] |=
		    (1 << invert3[(position >> 3) & 7]);

	}
	/* now, the table can be loaded into the chipset */
	SMC_SELECT_BANK(3, devno);

	for (i = 0; i < 8; i++) {
		smc_outb(multicast_table[i], ioaddr + MCAST_REG1 + i, devno);
	}
}

/* have to switch to the library crc32 code */
static int crc32(char *s, int length)
{
	/* indices */
	int perByte;
	int perBit;
	/* crc polynomial for Ethernet */
	const unsigned long poly = 0xedb88320;
	/* crc value - preinitialized to all 1's */
	unsigned long crc_value = 0xffffffff;

	for (perByte = 0; perByte < length; perByte++) {
		unsigned char c;

		c = *(s++);
		for (perBit = 0; perBit < 8; perBit++) {
			crc_value = (crc_value >> 1) ^
			    (((crc_value ^ c) & 0x01) ? poly : 0);
			c >>= 1;
		}
	}
	return crc_value;
}

/*
 . Function: smc_hard_start_xmit( struct sk_buff * skb, struct device * )
 . Purpose:
 .    Attempt to allocate memory for a packet, if chip-memory is not
 .    available, then tell the card to generate an interrupt when it
 .    is available.
 .
 . Algorithm:
 .
 . o	if the saved_skb is not currently null, then drop this packet
 .	on the floor.  This should never happen, because of TBUSY.
 . o	if the saved_skb is null, then replace it with the current packet,
 . o	See if I can sending it now.
 . o 	(NO): Enable interrupts and let the interrupt handler deal with it.
 . o	(YES):Send it now.
*/
static int smc_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	unsigned short ioaddr = dev->base_addr, devno = lp->devno;
	u_int16_t length, status, timeout, timeout2;
	unsigned short numPages;

	netif_stop_queue(dev);

/* 	if (lp->saved_skb) /\* THIS SHOULD NEVER HAPPEN. *\/ */
/* 		BUG(); */

	lp->saved_skb = skb;

	length = ETH_ZLEN < skb->len ? skb->len : ETH_ZLEN;
	/*
	 * The MMU wants the number of pages to be the number of 256 bytes
	 * 'pages', minus 1 ( since a packet can't ever have 0 pages :) )
	 *
	 * The 91C111 ignores the size bits, but the code is left intact
	 * for backwards and future compatibility.
	 *
	 * Pkt size for allocating is data length +6 (for additional status
	 * length and ctl words)
	 *
	 * If odd size then last byte is included in this header.
	 */
	numPages = (((length & 0xfffe) + 6) >> 8);
	if (numPages > 7) {
		printk("%s: packet too big\n", dev->name);
		dev_kfree_skb(skb);
		lp->saved_skb = NULL;
		/* this IS an error, but no use in saving a broken skb */
		goto leave_success;
	}
	/* either way, a packet is waiting now */
	lp->packets_waiting++;

	/* now, try to allocate the memory */
	SMC_SELECT_BANK(2, devno);
	smc_outw(MC_ALLOC | numPages, ioaddr + MMU_CMD_REG, devno);
	/*
	 . Performance Hack
	 .
	 . wait a short amount of time.. if I can send a packet now, I send
	 . it now.  Otherwise, I enable an interrupt and wait for one to be
	 . available.
	 .
	 . I could have handled this a slightly different way, by checking to
	 . see if any memory was available in the FREE MEMORY register.  However,
	 . either way, I need to generate an allocation, and the allocation works
	 . no matter what, so I saw no point in checking free memory.
	 */
	timeout2 = 3;
	do {
		timeout = MEMORY_WAIT_TIME;
		do {
/* 			unsigned int mask = smc_inw(ioaddr + INT_REG, devno); */
			status = smc_inb(ioaddr + INT_REG, devno);
/* 			mask = smc_inb(ioaddr + IM_REG, devno); */
/* 			printk("asked devno %d to alloc mem, BSR %04x, %04x, %04x\n", */
/* 			       devno, smc_inw(ioaddr + BSR_REG, devno), mask, mask); */
			if (status & IM_ALLOC_INT) { /* ack the interrupt */
				smc_outb(IM_ALLOC_INT, ioaddr + INT_REG, devno);
				goto do_send;
			}
		} while (--timeout);
/* 			printk("%s: clearing RX FIFO\n", dev->name); */
		smc_clear_rxfifo(dev);
	} while (--timeout2 != 0);

	/* if we got here, things are indeed grim --  buffers are seriously
	   overflowed, and we have no other choice but to defer the send until
	   the alloc int tells us it`s got some memory */
	SMC_ENABLE_INT(IM_ALLOC_INT, devno);
	status = smc_inb(ioaddr + INT_REG, devno);
	if (!(status & IM_ALLOC_INT))
		return 0;
	SMC_DISABLE_INT(IM_ALLOC_INT, devno);

 do_send:
	smc_hardware_send_packet(dev);
	lp->saved_skb = NULL;
	dev_kfree_skb(skb);

 leave_success:
	netif_wake_queue(dev);
	return 0;
}

/*
 . Function:  smc_hardware_send_packet(struct device * )
 . Purpose:
 .	This sends the actual packet to the SMC9xxx chip.
 .
 . Algorithm:
 . 	First, see if a saved_skb is available.
 .		( this should NOT be called if there is no 'saved_skb'
 .	Now, find the packet number that the chip allocated
 .	Point the data pointers at it in memory
 .	Set the length u_int16_t in the chip's memory
 .	Dump the packet to chip memory
 .	Check if a last byte is needed ( odd length packet )
 .		if so, set the control flag right
 . 	Tell the card to send it
 .	Enable the transmit interrupt, so I know if it failed
 . 	Free the kernel data if I actually sent it.
*/
static int smc_hardware_send_packet(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	u_int8_t packet_no;
	struct sk_buff *skb = lp->saved_skb;
	u_int16_t length;
	unsigned short ioaddr = dev->base_addr, devno = lp->devno;
	u_int8_t *buf;
	u_int16_t *buf16;
	u_int32_t buf_i, buf_wo;

	SMC_SELECT_BANK(2, devno);
	if (!skb) {
		PRINTK("%s: In XMIT with no packet to send \n", dev->name);
		return -1;
	}
	length = ETH_ZLEN < skb->len ? skb->len : ETH_ZLEN;
	buf_i = (u_int32_t)   skb->data;
	buf16 = (u_int16_t *) skb->data;
	buf =   (u_int8_t *)  skb->data;
	buf_wo = buf_i & 0x2;	/* partial compensation for nonalignedness */

	/* If I get here, I _know_ there is a packet slot waiting for me */
	packet_no = smc_inb(ioaddr + AR_REG, devno);
	if (packet_no & AR_FAILED) {
		/* or isn't there?  BAD CHIP! */
		printk(KERN_DEBUG "%s: AR_FAILED: Memory alloc failed. \n",
		       dev->name);
		return 0;
	}

	/* we have a packet address, so tell the card to use it */
	smc_outb(packet_no, ioaddr + PN_REG, devno);
	/* point to the beginning of the packet */
	smc_outw(PTR_AUTOINC, ioaddr + PTR_REG, devno);

	/* status word + packet length (+6 for status, length and ctl words) */
	smc_writedata((length + 6) << 16, ioaddr, devno);
	/* send the actual data
	 . I _think_ it's faster to send the longs first, and then
	 . mop up by sending the last u_int16_t.  It depends heavily
	 . on alignment, at least on the 486.  Maybe it would be
	 . a good idea to check which is optimal?  But that could take
	 . almost as much time as is saved?
	 */
	if (buf_wo != 0)
		smc_outw(buf16[0], ioaddr + DATA16_REG, devno);
	smc_writedata_str(ioaddr, buf + buf_wo, (length - buf_wo) >> 2, devno);

	/* trailing word */
	if ((length - buf_wo) & 2)
		smc_outw(buf16[(buf_wo + ((length - buf_wo) & 0xfffc)) >> 1], ioaddr + DATA16_REG, devno);
	smc_outw(length & 1 ? ((buf[length - 1]) | 0x2000) : 0,
		 ioaddr + DATA16_REG, devno);

	SMC_ENABLE_INT(IM_TX_INT | IM_TX_EMPTY_INT, devno);
	/* and let the chipset deal with it */
	smc_outw(MC_ENQUEUE, ioaddr + MMU_CMD_REG, devno);

	dev->trans_start = jiffies;

	return 0;
}

static void smc_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strcpy(info->driver, DRV_NAME);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->bus_info, "wooty smc thingie");
}

static int smc_get_regs_len(struct net_device *dev)
{
	return SMC_REGS_SIZE;
}

static int smc_get_stats_count(struct net_device *dev)
{
	return 0;
}

static int smc_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct smc_dev *priv = netdev_priv(dev);
	int rc;

	spin_lock_irq(&priv->lock);
	rc = mii_ethtool_gset(&priv->mii_if, cmd);
	spin_unlock_irq(&priv->lock);

	return rc;
}

static int smc_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct smc_dev *priv = netdev_priv(dev);
	int rc;

	spin_lock_irq(&priv->lock);
	rc = mii_ethtool_sset(&priv->mii_if, cmd);
	spin_unlock_irq(&priv->lock);

	return rc;
}

static int smc_nway_reset(struct net_device *dev)
{
	struct smc_dev *priv = netdev_priv(dev);

	return mii_nway_restart(&priv->mii_if);
}

static int smc_mdio_read(struct net_device *dev, int phy_id, int location)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	return smc_read_mii_register(dev->base_addr, phy_id, location, lp->devno);
}

static void smc_mdio_write(struct net_device *dev, int phy_id, int location, int value)
{
	struct smc_dev *lp = (struct smc_dev *) netdev_priv(dev);
	smc_write_mii_register(dev->base_addr, phy_id, location, value, lp->devno);
}

static struct ethtool_ops smc_ethtool_ops = {
	.get_drvinfo = smc_get_drvinfo,
	.get_regs_len = smc_get_regs_len,
	.get_stats_count = smc_get_stats_count,
	.get_settings = smc_get_settings,
	.set_settings = smc_set_settings,
	.nway_reset = smc_nway_reset,
	.get_link = ethtool_op_get_link,
/* 	.get_msglevel		= cp_get_msglevel, */
/* 	.set_msglevel		= cp_set_msglevel, */
/* 	.get_regs		= cp_get_regs, */
/* 	.get_strings		= cp_get_strings, */
/* 	.get_ethtool_stats	= cp_get_ethtool_stats, */
};

static int smc_set_mac_address(struct net_device *dev, void *data)
{
	struct sockaddr *sa = (struct sockaddr *) data;

	memcpy(dev->dev_addr, sa->sa_data, 6);
	smc_set_hwaddr(dev);

	return 0;
}

static int smc_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct smc_dev *priv = netdev_priv(dev);
	struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&rq->ifr_data;
	int rc;

	if (!netif_running(dev))
		return -EINVAL;

	spin_lock_irq(&priv->lock);
	rc = generic_mii_ioctl(&priv->mii_if, mii, cmd, NULL);
	spin_unlock_irq(&priv->lock);
	return 0;
}

/*-------------------------------------------------------------------------
 |
 | smc_init( struct device * dev )
 |   Input parameters:
 |	dev->base_addr == 0, try to find all possible locations
 |	dev->base_addr == 1, return failure code
 |	dev->base_addr == 2, always allocate space,  and return success
 |	dev->base_addr == <anything else>   this is the address to check
 |
 |   Output:
 |	0 --> there is a device
 |	anything else, error
 |
 ---------------------------------------------------------------------------
*/
int __init smc_init(struct net_device *dev)
{
	struct smc_dev *priv;
	int base_addr = dev ? dev->base_addr : 0;
	int devno = dev->dma; /* devno passing is a dirty trick */

	PRINTK2("CARDNAME:smc_init\n");

	SET_MODULE_OWNER(dev);

	priv = netdev_priv(dev);
	memset(priv, 0, sizeof(struct smc_dev));

	if (smc_probe(dev, base_addr, devno) == 0)
		goto found;

	return -ENODEV;
      found:
	priv->devno = devno;

	dev->open = smc_open;
	dev->stop = smc_close;
	dev->hard_start_xmit = smc_hard_start_xmit;
	dev->tx_timeout = smc_timeout;
	dev->get_stats = smc_query_statistics;
#ifdef	HAVE_MULTICAST
	dev->set_multicast_list = &smc_set_multicast_list;
#endif
	dev->set_mac_address = smc_set_mac_address;
	dev->ethtool_ops = &smc_ethtool_ops;
	dev->do_ioctl = smc_ioctl;

	priv->mii_if.dev = dev;
	priv->mii_if.mdio_read = smc_mdio_read;
	priv->mii_if.mdio_write = smc_mdio_write;
	priv->mii_if.phy_id = 0;
	priv->mii_if.phy_id_mask = 0x1f;
	priv->mii_if.reg_num_mask = 0x1f;

	return 0;
}

/*-------------------------------------------------------------------------
 |
 | smc_destructor( struct device * dev )
 |   Input parameters:
 |	dev, pointer to the device structure
 |
 |   Output:
 |	None.
 |
 ---------------------------------------------------------------------------
*/
void smc_destructor(struct net_device *dev)
{
	PRINTK2("CARDNAME:smc_destructor\n");
}

#ifndef NO_AUTOPROBE
/*----------------------------------------------------------------------
 . smc_findirq
 .
 . This routine has a simple purpose -- make the SMC chip generate an
 . interrupt, so an auto-detect routine can detect it, and find the IRQ,
 ------------------------------------------------------------------------
*/
int __init smc_findirq(int ioaddr, int devno)
{
	int timeout = 20;
	unsigned long cookie;

	PRINTK2("CARDNAME:smc_findirq\n");

	/* I have to do a STI() here, because this is called from
	   a routine that does an CLI during this process, making it
	   rather difficult to get interrupts for auto detection */
	local_irq_enable();

	cookie = probe_irq_on();

	/*
	 * What I try to do here is trigger an ALLOC_INT. This is done
	 * by allocating a small chunk of memory, which will give an interrupt
	 * when done.
	 */

	SMC_SELECT_BANK(2, devno);
	/* enable ALLOCation interrupts ONLY */
	smc_outb(IM_ALLOC_INT, ioaddr + IM_REG, devno);

	/*
	   . Allocate 512 bytes of memory.  Note that the chip was just
	   . reset so all the memory is available
	 */
	smc_outw(MC_ALLOC | 1, ioaddr + MMU_CMD_REG, devno);

	/*
	   . Wait until positive that the interrupt has been generated
	 */
	while (timeout) {
		u_int8_t int_status;

		int_status = smc_inb(ioaddr + INT_REG, devno);

		if (int_status & IM_ALLOC_INT)
			break;	/* got the interrupt */
		timeout--;
	}

	/* there is really nothing that I can do here if timeout fails,
	   as autoirq_report will return a 0 anyway, which is what I
	   want in this case.   Plus, the clean up is needed in both
	   cases.  */

	/* DELAY HERE!
	   On a fast machine, the status might change before the interrupt
	   is given to the processor.  This means that the interrupt was
	   never detected, and autoirq_report fails to report anything.
	   This should fix autoirq_* problems.
	 */
	mdelay(10);

	/* and disable all interrupts again */
	smc_outb(0, ioaddr + IM_REG, devno);

	/* clear hardware interrupts again, because that's how it
	   was when I was called... */
	local_irq_disable();

	/* and return what I found */
	return probe_irq_off(cookie);
}
#endif

/*----------------------------------------------------------------------
 . Function: smc_probe( int ioaddr )
 .
 . Purpose:
 .	Tests to see if a given ioaddr points to an SMC91111 chip.
 .	Returns a 0 on success
 .
 . Algorithm:
 .	(1) see if the high byte of BANK_SELECT is 0x33
 . 	(2) compare the ioaddr with the base register's address
 .	(3) see if I recognize the chip ID in the appropriate register
 .
 .---------------------------------------------------------------------
 */
/*---------------------------------------------------------------
 . Here I do typical initialization tasks.
 .
 . o  Initialize the structure if needed
 . o  print out my vanity message if not done so already
 . o  print out what type of hardware is detected
 . o  print out the ethernet address
 . o  find the IRQ
 . o  set up my private data
 . o  configure the dev structure with my subroutines
 . o  actually GRAB the irq.
 . o  GRAB the region
 .-----------------------------------------------------------------*/

static int __init smc_probe(struct net_device *dev, int ioaddr, int devno)
{
	int i, memory, retval;
	static unsigned int version_printed = 0;
	volatile unsigned int bank;

	const char *version_string;

/*registers */
	u_int16_t revision_register;
	u_int16_t base_address_register;
	u_int16_t memory_info_register;
	/*=> Pramod */
	struct smc_dev *lp;
	/*<= Pramod */

	PRINTK2("CARDNAME:smc_probe\n");

	/* Grab the region so that no one else tries to probe our ioports. */

	/* First, see if the high byte is 0x33 */
	bank = smc_inw(ioaddr + BANK_SELECT, devno);
	if ((bank & 0xFF00) != 0x3300) {
		printk("tested BSR initial value, read %04x\n", bank);
		return -ENODEV;
	}

	/* The above MIGHT indicate a device, but I need to write to further test this.  */
	smc_outw(0x0, ioaddr + BANK_SELECT, devno);
	bank = smc_inw(ioaddr + BANK_SELECT, devno);
	if ((bank & 0xFF00) != 0x3300) {
		printk("tested BSR for stability, read %04x\n", bank);
		retval = -ENODEV;
		goto err_out;
	}

	/* well, we've already written once, so hopefully another time won't
	   hurt.  This time, I need to switch the bank register to bank 1,
	   so I can access the base address register */
	SMC_SELECT_BANK(1, devno);
	base_address_register = smc_inw(ioaddr + BASE_REG, devno);
	if (ioaddr != (base_address_register >> 3 & 0x3E0)) {
		printk("CARDNAME: IOADDR %x doesn't match configuration (%x)."
		       "Probably not a SMC chip\n",
		       ioaddr, base_address_register >> 3 & 0x3E0);
		/* well, the base address register didn't match.  Must not have
		   been a SMC chip after all. */
		retval = -ENODEV;
		goto err_out;
	}

	/*  check if the revision register is something that I recognize.
	   These might need to be added to later, as future revisions
	   could be added.  */
	SMC_SELECT_BANK(3, devno);
	revision_register = smc_inw(ioaddr + REV_REG, devno);
	if (!chip_ids[(revision_register >> 4) & 0xF]) {
		/* I don't recognize this chip, so... */
		printk("CARDNAME: IO %x: Unrecognized revision register:"
		       " %x, Contact author. \n", ioaddr, revision_register);
		retval = -ENODEV;
		goto err_out;
	}

	/* at this point I'll assume that the chip is an SMC9xxx.
	   It might be prudent to check a listing of MAC addresses
	   against the hardware address, or do some other tests. */

	if (version_printed++ == 0)
		printk("%s", version);

	/* fill in some of the fields */
	dev->base_addr = ioaddr;

	/*
	   . Get the MAC address ( bank 1, regs 4 - 9 )
	 */
	SMC_SELECT_BANK(1, devno);
	for (i = 0; i < 6; i += 2) {
		u_int16_t address;

		address = smc_inw(ioaddr + ADDR0_REG + i, devno);
		dev->dev_addr[i + 1] = address >> 8;
		dev->dev_addr[i] = address & 0xFF;
	}

	/* get the memory information */

	SMC_SELECT_BANK(0, devno);
	memory_info_register = smc_inw(ioaddr + MIR_REG, devno);
	memory = memory_info_register & (u_int16_t) 0x00ff;
	memory *= LAN91C111_MEMORY_MULTIPLIER;

	/*
	   Now, I want to find out more about the chip.  This is sort of
	   redundant, but it's cleaner to have it in both, rather than having
	   one VERY long probe procedure.
	 */
	SMC_SELECT_BANK(3, devno);
	revision_register = smc_inw(ioaddr + REV_REG, devno);
	version_string = chip_ids[(revision_register >> 4) & 0xF];
	if (!version_string) {
		/* I shouldn't get here because this call was done before.... */
		printk("SMC91C111: unknown revision %04x\n", revision_register);
		retval = -ENODEV;
		goto err_out;
	}

	/* now, reset the chip, and put it into a known state */
	smc_reset(dev);

	/*
	   . If dev->irq is 0, then the device has to be banged on to see
	   . what the IRQ is.
	   .
	   . This banging doesn't always detect the IRQ, for unknown reasons.
	   . a workaround is to reset the chip and try again.
	   .
	   . Interestingly, the DOS packet driver *SETS* the IRQ on the card to
	   . be what is requested on the command line.   I don't do that, mostly
	   . because the card that I have uses a non-standard method of accessing
	   . the IRQs, and because this _should_ work in most configurations.
	   .
	   . Specifying an IRQ is done with the assumption that the user knows
	   . what (s)he is doing.  No checking is done!!!!
	   .
	 */
	if (dev->irq < 2) {
		int trials;

		trials = 3;
		while (trials--) {
			dev->irq = smc_findirq(ioaddr, devno);
			if (dev->irq)
				break;
			/* kick the card and try again */
			smc_reset(dev);
		}
	}
	if (dev->irq == 0) {
		printk("%s: Couldn't autodetect your IRQ. Use irq=xx.\n",
		       dev->name);
		retval = -ENODEV;
		goto err_out;
	}

/* 	if (dev->irq == 2) { */
/* 		/\* Fixup for users that don't know that IRQ 2 is really IRQ 9, */
/* 		 * or don't know which one to set. */
/* 		 *\/ */
/* 		dev->irq = 9; */
/* 	} */

	/* now, print out the card info, in a short format.. */
	printk("%s: %s(rev:%d) at %#3x IRQ:%d MEMSIZE:%db NOWAIT:%d ",
	       dev->name,
	       version_string, revision_register & 0xF, ioaddr, dev->irq,
	       memory, dev->dma);
	printk("ADDR: ");
	for (i = 0; i < 5; i++)
		printk("%2.2x:", dev->dev_addr[i]);
	printk("%2.2x \n", dev->dev_addr[5]);

	/* Grab the IRQ */
	retval = request_irq(dev->irq, smc_interrupt, IRQF_DISABLED, dev->name, dev);
	irq_desc[dev->irq].depth = 1;
	if (retval) {
		printk("%s: unable to get IRQ %d (irqval=%d).\n",
		       dev->name, dev->irq, retval);
		goto err_out;
	}

	/* => Store the ChipRevision and ChipID, to be used in resolving the Odd-Byte issue in RevB of LAN91C111; Pramod */
	SMC_SELECT_BANK(3, devno);
	revision_register = smc_inw(ioaddr + REV_REG, devno);
	lp = (struct smc_dev *)netdev_priv(dev);
	lp->ChipID = (revision_register >> 4) & 0xF;
	lp->ChipRev = revision_register & 0xF;

	return 0;

      err_out:
	return retval;
}

#if SMC_DEBUG > 2
static void print_packet(u_int8_t * buf, int length)
{
#if 1
	int i;
	int remainder;
	int lines;

	printk("Packet of length %d \n", length);

#if SMC_DEBUG > 3
	lines = length / 16;
	remainder = length % 16;

	for (i = 0; i < lines; i++) {
		int cur;

		for (cur = 0; cur < 8; cur++) {
			u_int8_t a, b;

			a = *(buf++);
			b = *(buf++);
			printk("%02x%02x ", a, b);
		}
		printk("\n");
	}
	for (i = 0; i < remainder / 2; i++) {
		u_int8_t a, b;

		a = *(buf++);
		b = *(buf++);
		printk("%02x%02x ", a, b);
	}
	printk("\n");
#endif
#endif
}
#endif

/*
 * Open and Initialize the board
 * Set up everything, reset the card, etc ..
 */
static int smc_open(struct net_device *dev)
{
	struct smc_dev *priv = (struct smc_dev *)netdev_priv(dev);

	PRINTK2("%s:smc_open\n", dev->name);

	/* clear out all the junk that was put here before... */
	/* JUNK?!? */
/* 	memset(priv, 0, sizeof(struct smc_dev)); */

	// Setup the default Register Modes
	priv->tcr_cur_mode = TCR_DEFAULT;
	priv->rcr_cur_mode = RCR_DEFAULT;
	priv->rpc_cur_mode = RPC_DEFAULT;

	// Set default parameters (files)
	priv->ctl_swfdup = 0;
	priv->ctl_ephloop = 0;
	priv->ctl_miiop = 0;
	priv->ctl_autoneg = 1;
	priv->ctl_rfduplx = 1;
	priv->ctl_rspeed = 100;
	priv->ctl_afduplx = 1;
	priv->ctl_aspeed = 100;
	priv->ctl_lnkfail = 0;
	priv->ctl_forcol = 0;
	priv->ctl_filtcar = 0;

	/* reset the hardware */
	smc_reset(dev);
	smc_enable(dev);

	smc_phy_configure(dev);

	/*
	 * According to Becker, I have to set the hardware address
	 * at this point, because the (l)user can set it with an
	 * ioctl.  Easily done...
	 */
	dev->dev_addr[0] = 0;
	dev->dev_addr[1] = 0;
	dev->dev_addr[2] = 0x22;
	dev->dev_addr[3] = 0x33;
	dev->dev_addr[4] = 0x44;
	dev->dev_addr[5] = 0x55;
	smc_set_hwaddr(dev);

#ifdef CONFIG_SYSCTL
	smc_sysctl_register(dev);
#endif				/* CONFIG_SYSCTL */

	netif_start_queue(dev);
	return 0;
}

static void smc_kick(struct net_device *dev)
{
	smc_reset(dev);
	smc_enable(dev);

	printk("%s called\n", __FUNCTION__);
	/* Reconfigure the PHY */
	smc_phy_configure(dev);

	netif_wake_queue(dev);
	dev->trans_start = jiffies;
	/* clear anything saved */
	((struct smc_dev *)netdev_priv(dev))->saved_skb = NULL;
}

static void smc_timeout(struct net_device *dev)
{
	printk(KERN_WARNING "%s: transmit timed out, %s?\n", dev->name,
	       tx_done(dev) ? "IRQ conflict" : "network cable problem");

	smc_kick(dev);
}

/*--------------------------------------------------------------------
 . This is the main routine of the driver, to handle the net_device when
 . it needs some attention.
 .   first, save state of the chipset
 .   branch off into routines to handle each case, and acknowledge
 .	    each to the interrupt register
 .   and finally restore state.
 ---------------------------------------------------------------------*/
irqreturn_t smc_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;

	u_int8_t status;
	u_int16_t card_stats;
	u_int8_t mask;
	int timeout;
	/* state registers */
	u_int16_t saved_bank;
	u_int16_t saved_pointer;

	if (dev == NULL) {
		printk(KERN_WARNING "%s: irq %d for unknown device.\n",
		       dev->name, irq);
		return IRQ_NONE;
	}
/* will Linux let this happen ??  If not, this costs some speed
	if ( dev->interrupt ) {
		printk(KERN_WARNING "%s: interrupt inside interrupt.\n",
			dev->name);
		return;
	}

	dev->interrupt = 1; */

	saved_bank = smc_inw(ioaddr + BANK_SELECT, devno);

	SMC_SELECT_BANK(0, devno);

	/* read the interrupt status register */
	SMC_SELECT_BANK(2, devno);
	mask = smc91111__read_interrupt_mask(ioaddr, devno);
	
	saved_pointer = smc_inw(ioaddr + PTR_REG, devno);

	/* disable all interrupts */
	smc_outb(0, ioaddr + IM_REG, devno);

	/* set a timeout value, so I don't stay here forever */
	timeout = 4;

/* 	printk(KERN_WARNING "%s: MASK IS %x \n", dev->name, mask); */
	do {
		/* read the status flag, and mask it */
		status = smc91111_read_interrupt_status(ioaddr, devno) & mask;
/* 		printk("smc_interrupt: intr status/mask %04x/%04x\n", */
/* 		       status, mask); */
		if (!status)
			break;

		PRINTK3(KERN_WARNING "%s: Handling interrupt status %x \n",
			dev->name, status);

		if (status & IM_RCV_INT) {
/* 			printk (KERN_WARNING "%s: Receive Interrupt\n", */
/* 				dev->name); */
			smc_rcv(dev);
		} else if (status & IM_TX_INT) {
/* 			printk (KERN_WARNING "%s: TX ERROR handled\n", */
/* 				dev->name); */
			smc_tx(dev);
			/* Acknowledge this interrupt */
			smc_outb(IM_TX_INT, ioaddr + INT_REG, devno);
		} else if (status & IM_TX_EMPTY_INT) {
/* 			printk (KERN_WARNING "%s: TX empty\n", dev->name); */
/* 			update stats */
			SMC_SELECT_BANK(0, devno);
			card_stats = smc_inw(ioaddr + COUNTER_REG, devno);
			lp->stats.collisions += card_stats & 0xF; /* 1 coll */
			card_stats >>= 4;
			lp->stats.collisions += card_stats & 0xF; /* multcoll*/
			lp->stats.tx_packets += lp->packets_waiting;
			lp->packets_waiting = 0;

			/* Acknowledge this interrupt */
			SMC_SELECT_BANK(2, devno);
			smc_outb(IM_TX_EMPTY_INT, ioaddr + INT_REG, devno);

			mask &= ~IM_TX_EMPTY_INT;
		} else if (status & IM_ALLOC_INT) {
/* 			printk (KERN_DEBUG "%s: Allocation interrupt \n", */
/* 			dev->name); */
			/* clear this interrupt so it doesn't happen again */
			mask &= ~IM_ALLOC_INT;

			if (lp->saved_skb != NULL) {
				smc_hardware_send_packet(dev);
				dev_kfree_skb_irq(lp->saved_skb);
				lp->saved_skb = NULL;

				/* enable xmit interrupts based on this */
				mask |= (IM_TX_EMPTY_INT | IM_TX_INT);
				/* and let the card send more packets to me */
				netif_wake_queue(dev);
				PRINTK2("%s: Handoff done successfully\n",
					dev->name);
			}
		} else if (status & IM_RX_OVRN_INT) {
/* 			printk (KERN_WARNING "%s: Receive overrun\n", */
/* 				dev->name); */
			lp->stats.rx_errors++;
			lp->stats.rx_fifo_errors++;

			/* unless we clean out stuff the chip stalls */
			smc_restart_with_clean_buffers(dev);

			/* ack this interrupt */
			smc_outb(IM_RX_OVRN_INT, ioaddr + INT_REG, devno);
		} else if (status & IM_EPH_INT) {
/* 			printk("%s: UNSUPPORTED: EPH INTERRUPT \n", dev->name); */
		} else if (status & IM_MDINT) {
/* 			printk (KERN_WARNING "%s: PHY int\n", dev->name); */
			smc_phy_interrupt(dev);
			/* ack this interrupt */
			smc_outb(IM_MDINT, ioaddr + INT_REG, devno);
		} else if (status & IM_ERCV_INT) {
/* 			printk("%s: UNSUPPORTED: ERCV INTERRUPT \n", dev->name); */
			/* ack this interrupt */
			smc_outb(IM_ERCV_INT, ioaddr + INT_REG, devno);
		}
	}
	while (timeout--);

	/* restore register states */

	SMC_SELECT_BANK(2, devno);

	smc_outb(mask, ioaddr + IM_REG, devno);

	PRINTK3(KERN_WARNING "%s: MASK is now %x \n", dev->name, mask);
	smc_outw(saved_pointer, ioaddr + PTR_REG, devno);

	SMC_SELECT_BANK(saved_bank, devno);

	//dev->interrupt = 0;
	PRINTK3("%s: Interrupt done\n", dev->name);
	return IRQ_HANDLED;
}

/*-------------------------------------------------------------
 . smc_rcv -  receive a packet from the card
 .
 . There is (at least) a packet waiting to be read from
 . chip-memory.
 . o Read the status
 . o If an error, record it
 . o otherwise, read in the packet
 --------------------------------------------------------------
*/
__attribute__ ((unused)) static void print_rxframe_status(u_int16_t status)
{
	printk("RXFRAME: ");
	if (status & 0x8000) printk("ALIGN_ERR ");
	if (status & 0x4000) printk("BROADCAST ");
	if (status & 0x2000) printk("BAD_CRC ");
	if (status & 0x1000) printk("ODD_FRM ");
	if (status & 0x0800) printk("TOO_LNG ");
	if (status & 0x0400) printk("TOO_SHORT ");
	if (status & 0x0001) printk("MULTICAST ");
	printk("\n");
}

static void smc_rcv(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	int packet_number;
	u_int16_t status;
	u_int16_t packet_length;

	PRINTK3("%s:smc_rcv\n", dev->name);

	SMC_SELECT_BANK(2, devno);
	packet_number = smc_inw(ioaddr + RXFIFO_REG, devno);

	if (packet_number & RXFIFO_REMPTY) {
		/* we got called , but nothing was on the FIFO */
		PRINTK("%s: WARNING: smc_rcv with nothing on FIFO. \n",
		       dev->name);
		return;
	}

	/*  start reading from the start of the packet */
	smc_outw(PTR_READ | PTR_RCV | PTR_AUTOINC, ioaddr + PTR_REG, devno);

	/* First two u_int16_ts are status and packet_length */
/* 	status = packet_length = smc_readdata(ioaddr, devno); */
/* 	status >>= 16; */
	status = smc_inw(ioaddr + DATA16_REG, devno);
	packet_length = smc_inw(ioaddr + DATA16_REG, devno);

	packet_length &= 0x07ff;	/* mask off top bits */

/* 	printk("RCV: STATUS %04x LENGTH %04x\n", status, packet_length); */
/* 	print_rxframe_status(status); */
	if (!(status & RS_ERRORS)) {
		/* do stuff to make a new packet */
		struct sk_buff *skb;
		u_int8_t *data, *tail;
		u_int16_t *data16;

		/* set multicast stats */
		if (status & RS_MULTICAST)
			lp->stats.multicast++;
		/* Adjust for having already read the nonwire header */
		skb = dev_alloc_skb(packet_length);
		packet_length -= 4;
		/* Allocate enough memory for entire receive frame */

		if (skb == NULL) {
			if (printk_ratelimit())
				printk(KERN_NOTICE "%s: low on memory, packet dropped\n", dev->name);
			lp->stats.rx_dropped++;
			goto done;
		}

		/*
		   ! This should work without alignment, but it could be
		   ! in the worse case
		 */
		/* TODO: Should I use 32bit alignment here ? */
		skb_reserve(skb, 2);	/* 16 bit alignment */

		skb->dev = dev;

		/* =>
		   ODD-BYTE ISSUE : The odd byte problem has been fixed in the LAN91C111 Rev B.
		   So we check if the Chip Revision, stored in smsc_local->ChipRev, is = 1.
		   If so then we increment the packet length only if RS_ODDFRAME is set.
		   If the Chip's revision is equal to 0, then we blindly increment the packet length
		   by 1, thus always assuming that the packet is odd length, leaving the higher layer
		   to decide the actual length.
		   -- Pramod
		   <= */
		
/* 		if ((9 == lp->ChipID) && (1 == lp->ChipRev)) { */
/* 			if (status & RS_ODDFRAME) */
/* 				data = skb_put(skb, packet_length + 1); */
/* 			else */
/* 				data = skb_put(skb, packet_length); */
/* 		} else { */
/* 			// set odd length for bug in LAN91C111, REV A */
/* 			// which never sets RS_ODDFRAME */
/* 			data = skb_put(skb, packet_length + 1); */
/* 		} */
		if ((9 != lp->ChipID) || (1 != lp->ChipRev) ||
		    (status & RS_ODDFRAME))
			packet_length++;
		data = skb_put(skb, packet_length);
		data16 = (u_int16_t *) data;
		
		/* skb data is 16bit aligned */
		data16[0] = smc_inw(ioaddr + DATA16_REG, devno);
		smc_readdata_str(ioaddr, data + 2, (packet_length - 2) >> 2, devno);

		tail = data + 2 + ((packet_length - 2) & 0xfffc);
		if (tail - data != packet_length) {
			u_int16_t *ptr = (u_int16_t *) tail;
			u_int32_t taildata;
			taildata = smc_readdata(ioaddr, devno);
			*ptr = taildata & 0xffff;
			if (packet_length - (tail - data) > 2)
				*(ptr + 2) = taildata >> 16;
		}
			
#if	SMC_DEBUG > 2
		printk("Receiving Packet\n");
		print_packet(data, packet_length);
#endif
		skb->protocol = eth_type_trans(skb, dev);
		skb->ip_summed = CHECKSUM_NONE;
		/* enqueues to be later picked up by: net_rx_action -> process_backlog -> netif_receive_skb -> call recv routine for every packet_type -> ip_rcv -> ip_local_deliver -> tcp_v4_rcv */
		netif_rx(skb);
		lp->stats.rx_packets++;
	} else {
		/* error ... */
		lp->stats.rx_errors++;

		if (status & RS_ALGNERR)
			lp->stats.rx_frame_errors++;
		if (status & (RS_TOOSHORT | RS_TOOLONG))
			lp->stats.rx_length_errors++;
		if (status & RS_BADCRC)
			lp->stats.rx_crc_errors++;
	}

	smc__busywait_on_mmu(dev);	/* wait for mmu to become free */
      done:
	/*  error or good, tell the card to get rid of this packet */
	smc_outw(MC_RELEASE, ioaddr + MMU_CMD_REG, devno);

	return;
}

/*************************************************************************
 . smc_tx
 .
 . Purpose:  Handle a transmit error message.   This will only be called
 .   when an error, because of the AUTO_RELEASE mode.
 .
 . Algorithm:
 .	Save pointer and packet no
 .	Get the packet no from the top of the queue
 .	check if it's valid ( if not, is this an error??? )
 .	read the status u_int16_t
 .	record the error
 .	( resend?  Not really, since we don't want old packets around )
 .	Restore saved values
 ************************************************************************/
static void smc_tx(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	u_int8_t saved_packet;
	u_int8_t packet_no;
	u_int16_t tx_status;

	printk("%s:smc_tx\n", dev->name);

	/* assume bank 2  */

	saved_packet = smc_inb(ioaddr + PN_REG, devno);
	packet_no = smc_inw(ioaddr + RXFIFO_REG, devno);
	packet_no &= 0x7F;

	/* If the TX FIFO is empty then nothing to do */
	if (packet_no & TXFIFO_TEMPTY)
		return;

	/* select this as the packet to read from */
	smc_outb(packet_no, ioaddr + PN_REG, devno);

	/* read the first u_int16_t (status u_int16_t) from this packet */
	smc_outw(PTR_AUTOINC | PTR_READ, ioaddr + PTR_REG, devno);

	tx_status = smc_inw(ioaddr + DATA_REG, devno);
	PRINTK3("%s: TX DONE STATUS: %4x \n", dev->name, tx_status);

	lp->stats.tx_errors++;
	if (tx_status & TS_LOSTCAR)
		lp->stats.tx_carrier_errors++;
	if (tx_status & TS_LATCOL) {
		printk(KERN_DEBUG
		       "%s: Late collision occurred on last xmit.\n",
		       dev->name);
		lp->stats.tx_window_errors++;
		lp->ctl_forcol = 0;	// Reset forced collsion
	}
#if 0
	if (tx_status & TS_16COL) {
	...}
#endif

	if (tx_status & TS_SUCCESS) {
		printk("%s: Successful packet caused interrupt \n", dev->name);
	}
	/* re-enable transmit */
	SMC_SELECT_BANK(0, devno);
	smc_outw(smc_inw(ioaddr + TCR_REG, devno) | TCR_ENABLE, ioaddr + TCR_REG, devno);

	/* kill the packet */
	SMC_SELECT_BANK(2, devno);
	smc_outw(MC_FREEPKT, ioaddr + MMU_CMD_REG, devno);

	/* one less packet waiting for me */
	lp->packets_waiting--;

	/* Don't change Packet Number Reg until busy bit is cleared */
	/* Per LAN91C111 Spec, Page 50 */
	while (smc_inw(ioaddr + MMU_CMD_REG, devno) & MC_BUSY, devno)
		;

	smc_outb(saved_packet, ioaddr + PN_REG, devno);
	return;
}

/*----------------------------------------------------
 . smc_close
 .
 . this makes the board clean up everything that it can
 . and not talk to the outside world.   Caused by
 . an 'ifconfig ethX down'
 .
 -----------------------------------------------------*/
static int smc_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	//dev->start = 0;

	PRINTK2("%s:smc_close\n", dev->name);

#ifdef CONFIG_SYSCTL
	smc_sysctl_unregister(dev);
#endif				/* CONFIG_SYSCTL */

	/* clear everything */
	smc_shutdown(dev);

	/* Update the statistics here. */

	return 0;
}

/*------------------------------------------------------------
 . Get the current statistics.
 . This may be called with the card open or closed.
 .-------------------------------------------------------------*/
static struct net_device_stats *smc_query_statistics(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);

	PRINTK2("%s:smc_query_statistics\n", dev->name);

	return &lp->stats;
}

/*-----------------------------------------------------------
 . smc_set_multicast_list
 .
 . This routine will, depending on the values passed to it,
 . either make it accept multicast packets, go into
 . promiscuous mode ( for TCPDUMP and cousins ) or accept
 . a select set of multicast packets
*/
static void smc_set_multicast_list(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	short ioaddr = dev->base_addr, devno = lp->devno;

	PRINTK2("%s:smc_set_multicast_list\n", dev->name);

	SMC_SELECT_BANK(0, devno);
	if (dev->flags & IFF_PROMISC) {
		PRINTK2("%s:smc_set_multicast_list:RCR_PRMS\n", dev->name);
		smc_outw(smc_inw(ioaddr + RCR_REG, devno) | RCR_PRMS, ioaddr + RCR_REG, devno);
	}

/* BUG?  I never disable promiscuous mode if multicasting was turned on.
   Now, I turn off promiscuous mode, but I don't do anything to multicasting
   when promiscuous mode is turned on.
*/

	/* Here, I am setting this to accept all multicast packets.
	   I don't need to zero the multicast table, because the flag is
	   checked before the table is
	 */
	else if (dev->flags & IFF_ALLMULTI) {
		smc_outw(smc_inw(ioaddr + RCR_REG, devno) | RCR_ALMUL, ioaddr + RCR_REG, devno);
		PRINTK2("%s:smc_set_multicast_list:RCR_ALMUL\n", dev->name);
	}

	/* We just get all multicast packets even if we only want them
	   . from one source.  This will be changed at some future
	   . point. */
	else if (dev->mc_count) {
		/* support hardware multicasting */

		/* be sure I get rid of flags I might have set */
		smc_outw(smc_inw(ioaddr + RCR_REG, devno) & ~(RCR_PRMS | RCR_ALMUL),
		     ioaddr + RCR_REG, devno);
		/* NOTE: this has to set the bank, so make sure it is the
		   last thing called.  The bank is set to zero at the top */
		smc_setmulticast(ioaddr, dev->mc_count, dev->mc_list, devno);
	} else {
		PRINTK2("%s:smc_set_multicast_list:~(RCR_PRMS|RCR_ALMUL)\n",
			dev->name);
		smc_outw(smc_inw(ioaddr + RCR_REG, devno) & ~(RCR_PRMS | RCR_ALMUL),
			 ioaddr + RCR_REG, devno);

		/*
		   since I'm disabling all multicast entirely, I need to
		   clear the multicast list
		 */
		SMC_SELECT_BANK(3, devno);
		smc_outw(0, ioaddr + MCAST_REG1, devno);
		smc_outw(0, ioaddr + MCAST_REG2, devno);
		smc_outw(0, ioaddr + MCAST_REG3, devno);
		smc_outw(0, ioaddr + MCAST_REG4, devno);
	}
}

int smc91111_io = MC_SMC_IO;
int smc91111_irq = MC_SMC_IRQ;
int smc91111_nowait = 0;

#ifndef MODULE
static int __init smc91111_decode_io(char *str)
{
	smc91111_io = simple_strtol(str, NULL, 0);
	return 1;
}
static int __init smc91111_decode_irq(char *str)
{
	smc91111_irq = simple_strtol(str, NULL, 0);
	return 1;
}
static int __init smc91111_decode_nowait(char *str)
{
	smc91111_nowait = simple_strtol(str, NULL, 0);
	return 1;
}

__setup("smc91111_io=", smc91111_decode_io);
__setup("smc91111_irq=", smc91111_decode_irq);
__setup("smc91111_nowait=", smc91111_decode_nowait);
#endif

/* options - modular */
/* MODULE_PARM(smc91111_io, "i"); */
/* MODULE_PARM(smc91111_irq, "i"); */
/* MODULE_PARM(smc91111_nowait, "i"); */
/* MODULE_LICENSE("GPL"); */

/*------------------------------------------------------------
 . Module initialization function
 .-------------------------------------------------------------*/
int smc91c111_init(void)
{
	const u_int32_t cscon_config = 0x001f17ff;
	int acc_result, result, i;
	struct net_device *dev;

	PRINTK2("CARDNAME:init_module\n");
	if (smc91111_io == 0)
		printk(KERN_WARNING
		       CARDNAME
		       ": You shouldn't use auto-probing with insmod!\n");

	/* map the appropriate CSCON */
	mc_mport->cscon[MC_SMC_CS] = cscon_config;
	printk("mc_setup_nic: setting up CSCON %08x to %08x\n", MC_SMC_CS, cscon_config);

	if (!request_region(smc91111_io, SMC_IO_EXTENT, "SMC91C111 block"))
		return -EBUSY;

	acc_result = 0;
	for (i = 0; i < SMC_NUMDEVICES; i++) {
		dev = smc_netdev[i] = alloc_etherdev(sizeof(struct smc_dev));
	/* copy the parameters from insmod into the device structure */
		dev->base_addr = smc91111_io;
		dev->irq = SMC_IRQBASE + i;
		dev->dma = i;	/* Use DMA field for init-time cheat devno */
		dev->init = smc_init;
		result = register_netdev(dev);
		if (result != 0) {
			free_netdev(dev);
			printk("smc91c111: device #%d was not found.\n", i);
		}
		acc_result |= result;
	}

	return acc_result;
}

/*------------------------------------------------------------
 . Cleanup when module is removed with rmmod
 .-------------------------------------------------------------*/
void smc91c111_cleanup(void)
{
	struct net_device *dev;
	int i;

	for (i = 0; i < SMC_NUMDEVICES; i++) {
		dev = smc_netdev[i];
		unregister_netdev(dev);

		free_irq(dev->irq, dev);

		free_netdev(dev);
	}
	release_region(smc_netdev[0]->base_addr, SMC_IO_EXTENT);
}

module_init(smc91c111_init);
module_exit(smc91c111_cleanup);

#ifdef CONFIG_SYSCTL
static u_int16_t
smc__write_bit(int bank, int ioaddr, int reg, unsigned int bit, int val, int devno)
{
	u_int16_t regval;

	SMC_SELECT_BANK(bank, devno);

	regval = smc_inw(ioaddr + reg, devno);
	if (val)
		regval |= bit;
	else
		regval &= ~bit;

	smc_outw(regval, ioaddr, devno);
	return (regval);
}

static int smc__read_bit(int bank, int ioaddr, int reg, unsigned int bit, int devno)
{
	SMC_SELECT_BANK(bank, devno);
	return smc_inw(ioaddr + reg, devno) & bit ? 1 : 0;
}

static void smc__write(int bank, int ioaddr, int reg, u_int16_t val, int devno)
{
	SMC_SELECT_BANK(bank, devno);
	smc_outw(val, ioaddr + reg, devno);
}

static int smc__read(int bank, int ioaddr, int reg, int devno)
{
	SMC_SELECT_BANK(bank, devno);
	return (smc_inw(ioaddr + reg, devno));
}

static const char smc_info_string[] =
    "\n"
    "info           Provides this information blurb\n"
    "swver          Prints the software version information of this driver\n"
    "autoneg        Auto-negotiate Mode = 1\n"
    "rspeed         Requested Speed, 100=100Mbps, 10=10Mpbs\n"
    "rfduplx        Requested Full Duplex Operation\n"
    "aspeed         Actual Speed, 100=100Mbps, 10=10Mpbs\n"
    "afduplx        Actual Full Duplex Operation\n"
    "lnkfail        PHY Link Failure when 1\n"
    "miiop          External MII when 1, Internal PHY when 0\n"
    "swfdup         Switched Full Duplex Mode (allowed only in MII operation)\n"
    "ephloop        EPH Block Loopback\n"
    "forcol         Force a collision\n"
    "filtcar        Filter leading edge of carrier sense for 12 bit times\n"
    "freemem        Free buffer memory in bytes\n"
    "totmem         Total buffer memory in bytes\n"
    "leda           Output of LED-A (green)\n"
    "ledb           Output of LED-B (yellow)\n"
    "chiprev        Revision ID of the LAN91C111 chip\n" "";

/*------------------------------------------------------------
 . /proc/net/ sysctl handler for all integer parameters
 .-------------------------------------------------------------*/
static int smc_sysctl_handler(ctl_table *ctl, int write, struct file *filp, void *buffer, size_t *lenp, loff_t *ppos)
{
	struct net_device *dev = (struct net_device *)ctl->extra1;
	struct smc_dev *lp = (struct smc_dev *)ctl->extra2;
	int devno = lp->devno, val, ret;
	int ioaddr = dev->base_addr;
	int *valp = ctl->data;

	// Update parameters from the real registers
	switch (ctl->ctl_name) {
	case CTL_SMC_FORCOL:
		*valp = smc__read_bit(0, ioaddr, TCR_REG, TCR_FORCOL, devno);
		break;

	case CTL_SMC_FREEMEM:
		*valp = ((u_int16_t) smc__read(0, ioaddr, MIR_REG, devno) >> 8)
		    * LAN91C111_MEMORY_MULTIPLIER;
		break;

	case CTL_SMC_TOTMEM:
		*valp = (smc__read(0, ioaddr, MIR_REG, devno) & (u_int16_t) 0x00ff)
		    * LAN91C111_MEMORY_MULTIPLIER;
		break;

	case CTL_SMC_CHIPREV:
		*valp = smc__read(3, ioaddr, REV_REG, devno);
		break;

	case CTL_SMC_AFDUPLX:
		*valp = (lp->lastPhy18 & PHY_INT_DPLXDET) ? 1 : 0;
		break;

	case CTL_SMC_ASPEED:
		*valp = (lp->lastPhy18 & PHY_INT_SPDDET) ? 100 : 10;
		break;

	case CTL_SMC_LNKFAIL:
		*valp = (lp->lastPhy18 & PHY_INT_LNKFAIL) ? 1 : 0;
		break;

	case CTL_SMC_LEDA:
		*valp =
		    (lp->rpc_cur_mode >> RPC_LSXA_SHFT) & (u_int16_t) 0x0007;
		break;

	case CTL_SMC_LEDB:
		*valp =
		    (lp->rpc_cur_mode >> RPC_LSXB_SHFT) & (u_int16_t) 0x0007;
		break;

	case CTL_SMC_MIIOP:
		*valp = smc__read_bit(1, ioaddr, CONFIG_REG, CONFIG_EXT_PHY, devno);
		break;
#if 0
#ifdef SMC_DEBUG
	case CTL_SMC_REG_BSR:	// Bank Select
		*valp = smc__read(0, ioaddr, BSR_REG);
		break;

	case CTL_SMC_REG_TCR:	// Transmit Control
		*valp = smc__read(0, ioaddr, TCR_REG);
		break;

	case CTL_SMC_REG_ESR:	// EPH Status
		*valp = smc__read(0, ioaddr, EPH_STATUS_REG);
		break;

	case CTL_SMC_REG_RCR:	// Receive Control
		*valp = smc__read(0, ioaddr, RCR_REG);
		break;

	case CTL_SMC_REG_CTRR:	// Counter
		*valp = smc__read(0, ioaddr, COUNTER_REG);
		break;

	case CTL_SMC_REG_MIR:	// Memory Information
		*valp = smc__read(0, ioaddr, MIR_REG);
		break;

	case CTL_SMC_REG_RPCR:	// Receive/Phy Control
		*valp = smc__read(0, ioaddr, RPC_REG);
		break;

	case CTL_SMC_REG_CFGR:	// Configuration
		*valp = smc__read(1, ioaddr, CONFIG_REG);
		break;

	case CTL_SMC_REG_BAR:	// Base Address
		*valp = smc__read(1, ioaddr, BASE_REG);
		break;

	case CTL_SMC_REG_IAR0:	// Individual Address
		*valp = smc__read(1, ioaddr, ADDR0_REG);
		break;

	case CTL_SMC_REG_IAR1:	// Individual Address
		*valp = smc__read(1, ioaddr, ADDR1_REG);
		break;

	case CTL_SMC_REG_IAR2:	// Individual Address
		*valp = smc__read(1, ioaddr, ADDR2_REG);
		break;

	case CTL_SMC_REG_GPR:	// General Purpose
		*valp = smc__read(1, ioaddr, GP_REG);
		break;

	case CTL_SMC_REG_CTLR:	// Control
		*valp = smc__read(1, ioaddr, CTL_REG);
		break;

	case CTL_SMC_REG_MCR:	// MMU Command
		*valp = smc__read(2, ioaddr, MMU_CMD_REG);
		break;

	case CTL_SMC_REG_PNR:	// Packet Number
		*valp = smc__read(2, ioaddr, PN_REG);
		break;

	case CTL_SMC_REG_FPR:	// Allocation Result/FIFO Ports
		*valp = smc__read(2, ioaddr, RXFIFO_REG);
		break;

	case CTL_SMC_REG_PTR:	// Pointer
		*valp = smc__read(2, ioaddr, PTR_REG);
		break;

	case CTL_SMC_REG_DR:	// Data 
		*valp = smc__read(2, ioaddr, DATA_REG);
		break;

	case CTL_SMC_REG_ISR:	// Interrupt Status/Mask
		*valp = smc__read(2, ioaddr, INT_REG);
		break;

	case CTL_SMC_REG_MTR1:	// Multicast Table Entry 1
		*valp = smc__read(3, ioaddr, MCAST_REG1);
		break;

	case CTL_SMC_REG_MTR2:	// Multicast Table Entry 2
		*valp = smc__read(3, ioaddr, MCAST_REG2);
		break;

	case CTL_SMC_REG_MTR3:	// Multicast Table Entry 3
		*valp = smc__read(3, ioaddr, MCAST_REG3);
		break;

	case CTL_SMC_REG_MTR4:	// Multicast Table Entry 4
		*valp = smc__read(3, ioaddr, MCAST_REG4);
		break;

	case CTL_SMC_REG_MIIR:	// Management Interface
		*valp = smc__read(3, ioaddr, MII_REG);
		break;

	case CTL_SMC_REG_REVR:	// Revision
		*valp = smc__read(3, ioaddr, REV_REG);
		break;

	case CTL_SMC_REG_ERCVR:	// Early RCV
		*valp = smc__read(3, ioaddr, ERCV_REG);
		break;

	case CTL_SMC_REG_EXTR:	// External
		*valp = smc__read(7, ioaddr, EXT_REG);
		break;

	case CTL_SMC_PHY_CTRL:
		*valp =
		    smc_read_mii_register(ioaddr, lp->phyaddr, PHY_CNTL_REG);
		break;

	case CTL_SMC_PHY_STAT:
		*valp =
		    smc_read_mii_register(ioaddr, lp->phyaddr, PHY_STAT_REG);
		break;

	case CTL_SMC_PHY_ID1:
		*valp = smc_read_mii_register(ioaddr, lp->phyaddr, PHY_ID1_REG);
		break;

	case CTL_SMC_PHY_ID2:
		*valp = smc_read_mii_register(ioaddr, lp->phyaddr, PHY_ID2_REG);
		break;

	case CTL_SMC_PHY_ADC:
		*valp = smc_read_mii_register(ioaddr, lp->phyaddr, PHY_AD_REG);
		break;

	case CTL_SMC_PHY_REMC:
		*valp = smc_read_mii_register(ioaddr, lp->phyaddr, PHY_RMT_REG);
		break;

	case CTL_SMC_PHY_CFG1:
		*valp =
		    smc_read_mii_register(ioaddr, lp->phyaddr, PHY_CFG1_REG);
		break;

	case CTL_SMC_PHY_CFG2:
		*valp =
		    smc_read_mii_register(ioaddr, lp->phyaddr, PHY_CFG2_REG);
		break;

	case CTL_SMC_PHY_INT:
		*valp = smc_read_mii_register(ioaddr, lp->phyaddr, PHY_INT_REG);
		break;

	case CTL_SMC_PHY_MASK:
		*valp =
		    smc_read_mii_register(ioaddr, lp->phyaddr, PHY_MASK_REG);
		break;

#endif				// SMC_DEBUG
#endif
	default:
		// Just ignore unsupported parameters
		break;
	}

	// Save old state
	val = *valp;

	// Perform the generic integer operation        
	if ((ret = proc_dointvec(ctl, write, filp, buffer, lenp, ppos)) != 0)
		return (ret);

	// Write changes out to the registers
	if (write && *valp != val) {

		val = *valp;
		switch (ctl->ctl_name) {

		case CTL_SMC_SWFDUP:
			if (val)
				lp->tcr_cur_mode |= TCR_SWFDUP;
			else
				lp->tcr_cur_mode &= ~TCR_SWFDUP;

			smc__write_bit(0, ioaddr, TCR_REG, TCR_SWFDUP, val, devno);
			break;

		case CTL_SMC_EPHLOOP:
			if (val)
				lp->tcr_cur_mode |= TCR_EPH_LOOP;
			else
				lp->tcr_cur_mode &= ~TCR_EPH_LOOP;

			smc__write_bit(0, ioaddr, TCR_REG, TCR_EPH_LOOP, val, devno);
			break;

		case CTL_SMC_FORCOL:
			if (val)
				lp->tcr_cur_mode |= TCR_FORCOL;
			else
				lp->tcr_cur_mode &= ~TCR_FORCOL;

			// Update the EPH block
			smc__write_bit(0, ioaddr, TCR_REG, TCR_FORCOL, val, devno);
			break;

		case CTL_SMC_FILTCAR:
			if (val)
				lp->rcr_cur_mode |= RCR_FILT_CAR;
			else
				lp->rcr_cur_mode &= ~RCR_FILT_CAR;

			// Update the EPH block
			smc__write_bit(0, ioaddr, RCR_REG, RCR_FILT_CAR, val, devno);
			break;

		case CTL_SMC_RFDUPLX:
			// Disallow changes if in auto-negotiation mode
			if (lp->ctl_autoneg)
				break;

			if (val) {
				lp->rpc_cur_mode |= RPC_DPLX;
			} else {
				lp->rpc_cur_mode &= ~RPC_DPLX;
			}

			// Reconfigure the PHY
			smc_phy_configure(dev);

			break;

		case CTL_SMC_RSPEED:
			// Disallow changes if in auto-negotiation mode
			if (lp->ctl_autoneg)
				break;

			if (val > 10)
				lp->rpc_cur_mode |= RPC_SPEED;
			else
				lp->rpc_cur_mode &= ~RPC_SPEED;

			// Reconfigure the PHY
			smc_phy_configure(dev);

			break;

		case CTL_SMC_AUTONEG:
			if (val)
				lp->rpc_cur_mode |= RPC_ANEG;
			else
				lp->rpc_cur_mode &= ~RPC_ANEG;

			// Reconfigure the PHY
			smc_phy_configure(dev);

			break;

		case CTL_SMC_LEDA:
			val &= 0x07;	// Restrict to 3 ls bits
			lp->rpc_cur_mode &=
			    ~(u_int16_t) (0x07 << RPC_LSXA_SHFT);
			lp->rpc_cur_mode |= (u_int16_t) (val << RPC_LSXA_SHFT);

			// Update the Internal PHY block
			smc__write(0, ioaddr, RPC_REG, lp->rpc_cur_mode, devno);
			break;

		case CTL_SMC_LEDB:
			val &= 0x07;	// Restrict to 3 ls bits
			lp->rpc_cur_mode &=
			    ~(u_int16_t) (0x07 << RPC_LSXB_SHFT);
			lp->rpc_cur_mode |= (u_int16_t) (val << RPC_LSXB_SHFT);

			// Update the Internal PHY block
			smc__write(0, ioaddr, RPC_REG, lp->rpc_cur_mode, devno);
			break;

		case CTL_SMC_MIIOP:
			// Update the Internal PHY block
			smc__write_bit(1, ioaddr, CONFIG_REG, CONFIG_EXT_PHY,
				       val, devno);
			break;
#if 0
#ifdef SMC_DEBUG
		case CTL_SMC_REG_BSR:	// Bank Select
			smc__write(0, ioaddr, BSR_REG, val);
			break;

		case CTL_SMC_REG_TCR:	// Transmit Control
			smc__write(0, ioaddr, TCR_REG, val);
			break;

		case CTL_SMC_REG_ESR:	// EPH Status
			smc__write(0, ioaddr, EPH_STATUS_REG, val);
			break;

		case CTL_SMC_REG_RCR:	// Receive Control
			smc__write(0, ioaddr, RCR_REG, val);
			break;

		case CTL_SMC_REG_CTRR:	// Counter
			smc__write(0, ioaddr, COUNTER_REG, val);
			break;

		case CTL_SMC_REG_MIR:	// Memory Information
			smc__write(0, ioaddr, MIR_REG, val);
			break;

		case CTL_SMC_REG_RPCR:	// Receive/Phy Control
			smc__write(0, ioaddr, RPC_REG, val);
			break;

		case CTL_SMC_REG_CFGR:	// Configuration
			smc__write(1, ioaddr, CONFIG_REG, val);
			break;

		case CTL_SMC_REG_BAR:	// Base Address
			smc__write(1, ioaddr, BASE_REG, val);
			break;

		case CTL_SMC_REG_IAR0:	// Individual Address
			smc__write(1, ioaddr, ADDR0_REG, val);
			break;

		case CTL_SMC_REG_IAR1:	// Individual Address
			smc__write(1, ioaddr, ADDR1_REG, val);
			break;

		case CTL_SMC_REG_IAR2:	// Individual Address
			smc__write(1, ioaddr, ADDR2_REG, val);
			break;

		case CTL_SMC_REG_GPR:	// General Purpose
			smc__write(1, ioaddr, GP_REG, val);
			break;

		case CTL_SMC_REG_CTLR:	// Control
			smc__write(1, ioaddr, CTL_REG, val);
			break;

		case CTL_SMC_REG_MCR:	// MMU Command
			smc__write(2, ioaddr, MMU_CMD_REG, val);
			break;

		case CTL_SMC_REG_PNR:	// Packet Number
			smc__write(2, ioaddr, PN_REG, val);
			break;

		case CTL_SMC_REG_FPR:	// Allocation Result/FIFO Ports
			smc__write(2, ioaddr, RXFIFO_REG, val);
			break;

		case CTL_SMC_REG_PTR:	// Pointer
			smc__write(2, ioaddr, PTR_REG, val);
			break;

		case CTL_SMC_REG_DR:	// Data 
			smc__write(2, ioaddr, DATA_REG, val);
			break;

		case CTL_SMC_REG_ISR:	// Interrupt Status/Mask
			smc__write(2, ioaddr, INT_REG, val);
			break;

		case CTL_SMC_REG_MTR1:	// Multicast Table Entry 1
			smc__write(3, ioaddr, MCAST_REG1, val);
			break;

		case CTL_SMC_REG_MTR2:	// Multicast Table Entry 2
			smc__write(3, ioaddr, MCAST_REG2, val);
			break;

		case CTL_SMC_REG_MTR3:	// Multicast Table Entry 3
			smc__write(3, ioaddr, MCAST_REG3, val);
			break;

		case CTL_SMC_REG_MTR4:	// Multicast Table Entry 4
			smc__write(3, ioaddr, MCAST_REG4, val);
			break;

		case CTL_SMC_REG_MIIR:	// Management Interface
			smc__write(3, ioaddr, MII_REG, val);
			break;

		case CTL_SMC_REG_REVR:	// Revision
			smc__write(3, ioaddr, REV_REG, val);
			break;

		case CTL_SMC_REG_ERCVR:	// Early RCV
			smc__write(3, ioaddr, ERCV_REG, val);
			break;

		case CTL_SMC_REG_EXTR:	// External
			smc__write(7, ioaddr, EXT_REG, val);
			break;

		case CTL_SMC_PHY_CTRL:
			smc_write_mii_register(ioaddr, lp->phyaddr,
					       PHY_CNTL_REG, val);
			break;

		case CTL_SMC_PHY_STAT:
			smc_write_mii_register(ioaddr, lp->phyaddr,
					       PHY_STAT_REG, val);
			break;

		case CTL_SMC_PHY_ID1:
			smc_write_mii_register(ioaddr, lp->phyaddr, PHY_ID1_REG,
					       val);
			break;

		case CTL_SMC_PHY_ID2:
			smc_write_mii_register(ioaddr, lp->phyaddr, PHY_ID2_REG,
					       val);
			break;

		case CTL_SMC_PHY_ADC:
			smc_write_mii_register(ioaddr, lp->phyaddr, PHY_AD_REG,
					       val);
			break;

		case CTL_SMC_PHY_REMC:
			smc_write_mii_register(ioaddr, lp->phyaddr, PHY_RMT_REG,
					       val);
			break;

		case CTL_SMC_PHY_CFG1:
			smc_write_mii_register(ioaddr, lp->phyaddr,
					       PHY_CFG1_REG, val);
			break;

		case CTL_SMC_PHY_CFG2:
			smc_write_mii_register(ioaddr, lp->phyaddr,
					       PHY_CFG2_REG, val);
			break;

		case CTL_SMC_PHY_INT:
			smc_write_mii_register(ioaddr, lp->phyaddr, PHY_INT_REG,
					       val);
			break;

		case CTL_SMC_PHY_MASK:
			smc_write_mii_register(ioaddr, lp->phyaddr,
					       PHY_MASK_REG, val);
			break;

#endif				// SMC_DEBUG
#endif
		default:
			// Just ignore unsupported parameters
			break;
		}		// end switch

	}			// end if

	return ret;
}

/*------------------------------------------------------------
 . Sysctl registration function for all parameters (files)
 .-------------------------------------------------------------*/
static void smc_sysctl_register(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	static int ctl_name = CTL_SMC;
	ctl_table *ct;
	int i;

	// Make sure the ctl_tables start out as all zeros
	memset(lp->root_table, 0, sizeof lp->root_table);
	memset(lp->eth_table, 0, sizeof lp->eth_table);
	memset(lp->param_table, 0, sizeof lp->param_table);

	// Initialize the root table
	ct = lp->root_table;
	ct->ctl_name = CTL_DEV;
	ct->procname = "dev";
	ct->maxlen = 0;
	ct->mode = 0555;
	ct->child = lp->eth_table;
	// remaining fields are zero

	// Initialize the ethX table (this device's table)
	ct = lp->eth_table;
	ct->ctl_name = ctl_name++;	// Must be unique
	ct->procname = dev->name;
	ct->maxlen = 0;
	ct->mode = 0555;
	ct->child = lp->param_table;
	// remaining fields are zero

	// Initialize the parameter (files) table
	// Make sure the last entry remains null
	ct = lp->param_table;
	for (i = 0; i < (CTL_SMC_LAST_ENTRY - 1); ++i) {
		// Initialize fields common to all table entries
		ct[i].proc_handler = smc_sysctl_handler;
		ct[i].extra1 = (void *)dev;	// Save our device pointer
		ct[i].extra2 = (void *)lp;	// Save our smc_local data pointer
	}

	// INFO - this is our only string parameter
	i = 0;
	ct[i].proc_handler = proc_dostring;	// use default handler
	ct[i].ctl_name = CTL_SMC_INFO;
	ct[i].procname = "info";
	ct[i].data = (void *)smc_info_string;
	ct[i].maxlen = sizeof smc_info_string;
	ct[i].mode = 0444;	// Read only

	// SWVER
	++i;
	ct[i].proc_handler = proc_dostring;	// use default handler
	ct[i].ctl_name = CTL_SMC_SWVER;
	ct[i].procname = "swver";
	ct[i].data = (void *)version;
	ct[i].maxlen = sizeof version;
	ct[i].mode = 0444;	// Read only

	// SWFDUP
	++i;
	ct[i].ctl_name = CTL_SMC_SWFDUP;
	ct[i].procname = "swfdup";
	ct[i].data = (void *)&(lp->ctl_swfdup);
	ct[i].maxlen = sizeof lp->ctl_swfdup;
	ct[i].mode = 0644;	// Read by all, write by root

	// EPHLOOP
	++i;
	ct[i].ctl_name = CTL_SMC_EPHLOOP;
	ct[i].procname = "ephloop";
	ct[i].data = (void *)&(lp->ctl_ephloop);
	ct[i].maxlen = sizeof lp->ctl_ephloop;
	ct[i].mode = 0644;	// Read by all, write by root

	// MIIOP
	++i;
	ct[i].ctl_name = CTL_SMC_MIIOP;
	ct[i].procname = "miiop";
	ct[i].data = (void *)&(lp->ctl_miiop);
	ct[i].maxlen = sizeof lp->ctl_miiop;
	ct[i].mode = 0644;	// Read by all, write by root

	// AUTONEG
	++i;
	ct[i].ctl_name = CTL_SMC_AUTONEG;
	ct[i].procname = "autoneg";
	ct[i].data = (void *)&(lp->ctl_autoneg);
	ct[i].maxlen = sizeof lp->ctl_autoneg;
	ct[i].mode = 0644;	// Read by all, write by root

	// RFDUPLX
	++i;
	ct[i].ctl_name = CTL_SMC_RFDUPLX;
	ct[i].procname = "rfduplx";
	ct[i].data = (void *)&(lp->ctl_rfduplx);
	ct[i].maxlen = sizeof lp->ctl_rfduplx;
	ct[i].mode = 0644;	// Read by all, write by root

	// RSPEED
	++i;
	ct[i].ctl_name = CTL_SMC_RSPEED;
	ct[i].procname = "rspeed";
	ct[i].data = (void *)&(lp->ctl_rspeed);
	ct[i].maxlen = sizeof lp->ctl_rspeed;
	ct[i].mode = 0644;	// Read by all, write by root

	// AFDUPLX
	++i;
	ct[i].ctl_name = CTL_SMC_AFDUPLX;
	ct[i].procname = "afduplx";
	ct[i].data = (void *)&(lp->ctl_afduplx);
	ct[i].maxlen = sizeof lp->ctl_afduplx;
	ct[i].mode = 0444;	// Read only

	// ASPEED
	++i;
	ct[i].ctl_name = CTL_SMC_ASPEED;
	ct[i].procname = "aspeed";
	ct[i].data = (void *)&(lp->ctl_aspeed);
	ct[i].maxlen = sizeof lp->ctl_aspeed;
	ct[i].mode = 0444;	// Read only

	// LNKFAIL
	++i;
	ct[i].ctl_name = CTL_SMC_LNKFAIL;
	ct[i].procname = "lnkfail";
	ct[i].data = (void *)&(lp->ctl_lnkfail);
	ct[i].maxlen = sizeof lp->ctl_lnkfail;
	ct[i].mode = 0444;	// Read only

	// FORCOL
	++i;
	ct[i].ctl_name = CTL_SMC_FORCOL;
	ct[i].procname = "forcol";
	ct[i].data = (void *)&(lp->ctl_forcol);
	ct[i].maxlen = sizeof lp->ctl_forcol;
	ct[i].mode = 0644;	// Read by all, write by root

	// FILTCAR
	++i;
	ct[i].ctl_name = CTL_SMC_FILTCAR;
	ct[i].procname = "filtcar";
	ct[i].data = (void *)&(lp->ctl_filtcar);
	ct[i].maxlen = sizeof lp->ctl_filtcar;
	ct[i].mode = 0644;	// Read by all, write by root

	// FREEMEM
	++i;
	ct[i].ctl_name = CTL_SMC_FREEMEM;
	ct[i].procname = "freemem";
	ct[i].data = (void *)&(lp->ctl_freemem);
	ct[i].maxlen = sizeof lp->ctl_freemem;
	ct[i].mode = 0444;	// Read only

	// TOTMEM
	++i;
	ct[i].ctl_name = CTL_SMC_TOTMEM;
	ct[i].procname = "totmem";
	ct[i].data = (void *)&(lp->ctl_totmem);
	ct[i].maxlen = sizeof lp->ctl_totmem;
	ct[i].mode = 0444;	// Read only

	// LEDA
	++i;
	ct[i].ctl_name = CTL_SMC_LEDA;
	ct[i].procname = "leda";
	ct[i].data = (void *)&(lp->ctl_leda);
	ct[i].maxlen = sizeof lp->ctl_leda;
	ct[i].mode = 0644;	// Read by all, write by root

	// LEDB
	++i;
	ct[i].ctl_name = CTL_SMC_LEDB;
	ct[i].procname = "ledb";
	ct[i].data = (void *)&(lp->ctl_ledb);
	ct[i].maxlen = sizeof lp->ctl_ledb;
	ct[i].mode = 0644;	// Read by all, write by root

	// CHIPREV
	++i;
	ct[i].ctl_name = CTL_SMC_CHIPREV;
	ct[i].procname = "chiprev";
	ct[i].data = (void *)&(lp->ctl_chiprev);
	ct[i].maxlen = sizeof lp->ctl_chiprev;
	ct[i].mode = 0444;	// Read only

#if 0
#ifdef SMC_DEBUG
	// REG_BSR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_BSR;
	ct[i].procname = "reg_bsr";
	ct[i].data = (void *)&(lp->ctl_reg_bsr);
	ct[i].maxlen = sizeof lp->ctl_reg_bsr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_TCR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_TCR;
	ct[i].procname = "reg_tcr";
	ct[i].data = (void *)&(lp->ctl_reg_tcr);
	ct[i].maxlen = sizeof lp->ctl_reg_tcr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_ESR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_ESR;
	ct[i].procname = "reg_esr";
	ct[i].data = (void *)&(lp->ctl_reg_esr);
	ct[i].maxlen = sizeof lp->ctl_reg_esr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_RCR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_RCR;
	ct[i].procname = "reg_rcr";
	ct[i].data = (void *)&(lp->ctl_reg_rcr);
	ct[i].maxlen = sizeof lp->ctl_reg_rcr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_CTRR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_CTRR;
	ct[i].procname = "reg_ctrr";
	ct[i].data = (void *)&(lp->ctl_reg_ctrr);
	ct[i].maxlen = sizeof lp->ctl_reg_ctrr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MIR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MIR;
	ct[i].procname = "reg_mir";
	ct[i].data = (void *)&(lp->ctl_reg_mir);
	ct[i].maxlen = sizeof lp->ctl_reg_mir;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_RPCR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_RPCR;
	ct[i].procname = "reg_rpcr";
	ct[i].data = (void *)&(lp->ctl_reg_rpcr);
	ct[i].maxlen = sizeof lp->ctl_reg_rpcr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_CFGR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_CFGR;
	ct[i].procname = "reg_cfgr";
	ct[i].data = (void *)&(lp->ctl_reg_cfgr);
	ct[i].maxlen = sizeof lp->ctl_reg_cfgr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_BAR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_BAR;
	ct[i].procname = "reg_bar";
	ct[i].data = (void *)&(lp->ctl_reg_bar);
	ct[i].maxlen = sizeof lp->ctl_reg_bar;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_IAR0
	++i;
	ct[i].ctl_name = CTL_SMC_REG_IAR0;
	ct[i].procname = "reg_iar0";
	ct[i].data = (void *)&(lp->ctl_reg_iar0);
	ct[i].maxlen = sizeof lp->ctl_reg_iar0;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_IAR1
	++i;
	ct[i].ctl_name = CTL_SMC_REG_IAR1;
	ct[i].procname = "reg_iar1";
	ct[i].data = (void *)&(lp->ctl_reg_iar1);
	ct[i].maxlen = sizeof lp->ctl_reg_iar1;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_IAR2
	++i;
	ct[i].ctl_name = CTL_SMC_REG_IAR2;
	ct[i].procname = "reg_iar2";
	ct[i].data = (void *)&(lp->ctl_reg_iar2);
	ct[i].maxlen = sizeof lp->ctl_reg_iar2;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_GPR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_GPR;
	ct[i].procname = "reg_gpr";
	ct[i].data = (void *)&(lp->ctl_reg_gpr);
	ct[i].maxlen = sizeof lp->ctl_reg_gpr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_CTLR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_CTLR;
	ct[i].procname = "reg_ctlr";
	ct[i].data = (void *)&(lp->ctl_reg_ctlr);
	ct[i].maxlen = sizeof lp->ctl_reg_ctlr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MCR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MCR;
	ct[i].procname = "reg_mcr";
	ct[i].data = (void *)&(lp->ctl_reg_mcr);
	ct[i].maxlen = sizeof lp->ctl_reg_mcr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_PNR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_PNR;
	ct[i].procname = "reg_pnr";
	ct[i].data = (void *)&(lp->ctl_reg_pnr);
	ct[i].maxlen = sizeof lp->ctl_reg_pnr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_FPR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_FPR;
	ct[i].procname = "reg_fpr";
	ct[i].data = (void *)&(lp->ctl_reg_fpr);
	ct[i].maxlen = sizeof lp->ctl_reg_fpr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_PTR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_PTR;
	ct[i].procname = "reg_ptr";
	ct[i].data = (void *)&(lp->ctl_reg_ptr);
	ct[i].maxlen = sizeof lp->ctl_reg_ptr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_DR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_DR;
	ct[i].procname = "reg_dr";
	ct[i].data = (void *)&(lp->ctl_reg_dr);
	ct[i].maxlen = sizeof lp->ctl_reg_dr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_ISR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_ISR;
	ct[i].procname = "reg_isr";
	ct[i].data = (void *)&(lp->ctl_reg_isr);
	ct[i].maxlen = sizeof lp->ctl_reg_isr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MTR1
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MTR1;
	ct[i].procname = "reg_mtr1";
	ct[i].data = (void *)&(lp->ctl_reg_mtr1);
	ct[i].maxlen = sizeof lp->ctl_reg_mtr1;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MTR2
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MTR2;
	ct[i].procname = "reg_mtr2";
	ct[i].data = (void *)&(lp->ctl_reg_mtr2);
	ct[i].maxlen = sizeof lp->ctl_reg_mtr2;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MTR3
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MTR3;
	ct[i].procname = "reg_mtr3";
	ct[i].data = (void *)&(lp->ctl_reg_mtr3);
	ct[i].maxlen = sizeof lp->ctl_reg_mtr3;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MTR4
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MTR4;
	ct[i].procname = "reg_mtr4";
	ct[i].data = (void *)&(lp->ctl_reg_mtr4);
	ct[i].maxlen = sizeof lp->ctl_reg_mtr4;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_MIIR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_MIIR;
	ct[i].procname = "reg_miir";
	ct[i].data = (void *)&(lp->ctl_reg_miir);
	ct[i].maxlen = sizeof lp->ctl_reg_miir;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_REVR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_REVR;
	ct[i].procname = "reg_revr";
	ct[i].data = (void *)&(lp->ctl_reg_revr);
	ct[i].maxlen = sizeof lp->ctl_reg_revr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_ERCVR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_ERCVR;
	ct[i].procname = "reg_ercvr";
	ct[i].data = (void *)&(lp->ctl_reg_ercvr);
	ct[i].maxlen = sizeof lp->ctl_reg_ercvr;
	ct[i].mode = 0644;	// Read by all, write by root

	// REG_EXTR
	++i;
	ct[i].ctl_name = CTL_SMC_REG_EXTR;
	ct[i].procname = "reg_extr";
	ct[i].data = (void *)&(lp->ctl_reg_extr);
	ct[i].maxlen = sizeof lp->ctl_reg_extr;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Control
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_CTRL;
	ct[i].procname = "phy_ctrl";
	ct[i].data = (void *)&(lp->ctl_phy_ctrl);
	ct[i].maxlen = sizeof lp->ctl_phy_ctrl;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Status
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_STAT;
	ct[i].procname = "phy_stat";
	ct[i].data = (void *)&(lp->ctl_phy_stat);
	ct[i].maxlen = sizeof lp->ctl_phy_stat;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY ID1
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_ID1;
	ct[i].procname = "phy_id1";
	ct[i].data = (void *)&(lp->ctl_phy_id1);
	ct[i].maxlen = sizeof lp->ctl_phy_id1;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY ID2
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_ID2;
	ct[i].procname = "phy_id2";
	ct[i].data = (void *)&(lp->ctl_phy_id2);
	ct[i].maxlen = sizeof lp->ctl_phy_id2;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Advertise Capabilities
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_ADC;
	ct[i].procname = "phy_adc";
	ct[i].data = (void *)&(lp->ctl_phy_adc);
	ct[i].maxlen = sizeof lp->ctl_phy_adc;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Remote Capabilities
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_REMC;
	ct[i].procname = "phy_remc";
	ct[i].data = (void *)&(lp->ctl_phy_remc);
	ct[i].maxlen = sizeof lp->ctl_phy_remc;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Configuration 1
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_CFG1;
	ct[i].procname = "phy_cfg1";
	ct[i].data = (void *)&(lp->ctl_phy_cfg1);
	ct[i].maxlen = sizeof lp->ctl_phy_cfg1;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Configuration 2
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_CFG2;
	ct[i].procname = "phy_cfg2";
	ct[i].data = (void *)&(lp->ctl_phy_cfg2);
	ct[i].maxlen = sizeof lp->ctl_phy_cfg2;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Interrupt/Status Output
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_INT;
	ct[i].procname = "phy_int";
	ct[i].data = (void *)&(lp->ctl_phy_int);
	ct[i].maxlen = sizeof lp->ctl_phy_int;
	ct[i].mode = 0644;	// Read by all, write by root

	// PHY Interrupt/Status Mask
	++i;
	ct[i].ctl_name = CTL_SMC_PHY_MASK;
	ct[i].procname = "phy_mask";
	ct[i].data = (void *)&(lp->ctl_phy_mask);
	ct[i].maxlen = sizeof lp->ctl_phy_mask;
	ct[i].mode = 0644;	// Read by all, write by root

#endif				// SMC_DEBUG
#endif

	// Register /proc/sys/dev/ethX
	lp->sysctl_header = register_sysctl_table(lp->root_table, 1);
}

/*------------------------------------------------------------
 . Sysctl unregistration when driver is closed
 .-------------------------------------------------------------*/
static void smc_sysctl_unregister(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);

	unregister_sysctl_table(lp->sysctl_header);
}

#endif				/* endif CONFIG_SYSCTL */

//---PHY CONTROL AND CONFIGURATION-----------------------------------------

#if (SMC_DEBUG > 2 )

/*------------------------------------------------------------
 . Debugging function for viewing MII Management serial bitstream
 .-------------------------------------------------------------*/
static void smc_dump_mii_stream(u_int8_t * bits, int size)
{
	int i;

	printk("BIT#:");
	for (i = 0; i < size; ++i) {
		printk("%d", i % 10);
	}

	printk("\nMDOE:");
	for (i = 0; i < size; ++i) {
		if (bits[i] & MII_MDOE)
			printk("1");
		else
			printk("0");
	}

	printk("\nMDO :");
	for (i = 0; i < size; ++i) {
		if (bits[i] & MII_MDO)
			printk("1");
		else
			printk("0");
	}

	printk("\nMDI :");
	for (i = 0; i < size; ++i) {
		if (bits[i] & MII_MDI)
			printk("1");
		else
			printk("0");
	}

	printk("\n");
}
#endif

/*------------------------------------------------------------
 . Finds and reports the PHY address
 .-------------------------------------------------------------*/
static int smc_detect_phy(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	u_int16_t phy_id1;
	u_int16_t phy_id2;
	int phyaddr;
	int found = 0;

	PRINTK3("%s:smc_detect_phy()\n", dev->name);

	// Scan all 32 PHY addresses if necessary
	for (phyaddr = 0; phyaddr < 32; ++phyaddr) {
		// Read the PHY identifiers
		phy_id1 = smc_read_mii_register(ioaddr, phyaddr, PHY_ID1_REG, devno);
		phy_id2 = smc_read_mii_register(ioaddr, phyaddr, PHY_ID2_REG, devno);

		PRINTK3("%s: phy_id1=%x, phy_id2=%x\n", dev->name, phy_id1,
			phy_id2);

		// Make sure it is a valid identifier   
		if ((phy_id2 > 0x0000) && (phy_id2 < 0xffff) &&
		    (phy_id1 > 0x0000) && (phy_id1 < 0xffff)) {
			if ((phy_id1 != 0x8000) && (phy_id2 != 0x8000)) {
				// Save the PHY's address
				lp->phyaddr = phyaddr;
				found = 1;
				break;
			}
		}
	}

	if (!found) {
		PRINTK("%s: No PHY found\n", dev->name);
		return (0);
	}
	// Set the PHY type
	if ((phy_id1 == 0x0016) && ((phy_id2 & 0xFFF0) == 0xF840)) {
		lp->phytype = PHY_LAN83C183;
		printk("%s: PHY=LAN83C183 (LAN91C111 Internal)\n", dev->name);
	}

	if ((phy_id1 == 0x0282) && ((phy_id2 & 0xFFF0) == 0x1C50)) {
		lp->phytype = PHY_LAN83C180;
		PRINTK("%s: PHY=LAN83C180\n", dev->name);
	}

	return (1);
}

/*------------------------------------------------------------
 . Waits the specified number of milliseconds - kernel friendly
 .-------------------------------------------------------------*/
static void smc_wait_ms(unsigned int ms)
{
	if (in_atomic())
		udelay(ms * 1000);
	else if (!in_interrupt()) {
		current->state = TASK_UNINTERRUPTIBLE;
		schedule_timeout(1 + ms * HZ / 1000);
	} else {
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(1 + ms * HZ / 1000);
		current->state = TASK_RUNNING;
	}
}

/*------------------------------------------------------------
 . Sets the PHY to a configuration as determined by the user
 .-------------------------------------------------------------*/
static int smc_phy_fixed(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	u_int8_t phyaddr = lp->phyaddr;
	u_int16_t my_fixed_caps;
	u_int16_t cfg1;

	printk("%s:smc_phy_fixed()\n", dev->name);

	// Enter Link Disable state
	cfg1 = smc_read_mii_register(ioaddr, phyaddr, PHY_CFG1_REG, devno);
	cfg1 |= PHY_CFG1_LNKDIS;
	smc_write_mii_register(ioaddr, phyaddr, PHY_CFG1_REG, cfg1, devno);

	// Set our fixed capabilities
	// Disable auto-negotiation
	my_fixed_caps = 0;

	if (lp->ctl_rfduplx)
		my_fixed_caps |= PHY_CNTL_DPLX;

	if (lp->ctl_rspeed == 100)
		my_fixed_caps |= PHY_CNTL_SPEED;

	// Write our capabilities to the phy control register
	smc_write_mii_register(ioaddr, phyaddr, PHY_CNTL_REG, my_fixed_caps, devno);

	// Re-Configure the Receive/Phy Control register
	smc_outw(lp->rpc_cur_mode, ioaddr + RPC_REG, devno);

	// Success
	return (1);
}

/* nonzero -- timeout or signal during reset */
static int smc_reset_phy(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int devno = lp->devno, timeout;

	/* Reset the PHY, setting all other bits to zero */
	smc_write_mii_register(dev->base_addr, lp->phyaddr, PHY_CNTL_REG, PHY_CNTL_RST, devno);

	/* Wait for the reset to complete, or time out after 3 seconds */
	timeout = 6;
	while (timeout--) {
		if (!(smc_read_mii_register(dev->base_addr, lp->phyaddr,
					    PHY_CNTL_REG, devno)
		      & PHY_CNTL_RST))
			goto success;

		smc_wait_ms(500);
		if (signal_pending(current))	/* exit anyway if signaled */
		{
			PRINTK2("%s:PHY reset interrupted by signal\n",
				dev->name);
			timeout = 0;
			goto timed_out;
		}
	}
	if (timeout > 0)
		goto success;

 timed_out:
	return -1;
 success:
	return 0;
}

/*------------------------------------------------------------
 . Configures the specified PHY using Autonegotiation. Calls
 . smc_phy_fixed() if the user has requested a certain config.
 .-------------------------------------------------------------*/
static void smc_phy_configure(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	int timeout;
	u_int8_t phyaddr;
	u_int16_t my_phy_caps;	// My PHY capabilities
	u_int16_t my_ad_caps;	// My Advertised capabilities
	u_int16_t status;
	int failed = 0;

	PRINTK3("%s:smc_program_phy()\n", dev->name);

	// Set the blocking flag
	lp->autoneg_active = 1;

	/* find the address and type of our phy */
	if (!smc_detect_phy(dev)) {
		goto smc_phy_configure_exit;
	}
	/* get the detected phy address */
	phyaddr = lp->phyaddr;

	if (smc_reset_phy(dev) != 0) {
		PRINTK2("%s:PHY reset timed out\n", dev->name);
		goto smc_phy_configure_exit;
	}
	// Read PHY Register 18, Status Output
	lp->lastPhy18 = smc_read_mii_register(ioaddr, phyaddr, PHY_INT_REG, devno);

	// Enable PHY Interrupts (for register 18)
	// Interrupts listed here are disabled
	smc_write_mii_register(ioaddr, phyaddr, PHY_MASK_REG,
			       PHY_INT_LOSSSYNC | PHY_INT_CWRD | PHY_INT_SSD |
			       PHY_INT_ESD | PHY_INT_RPOL | PHY_INT_JAB |
			       PHY_INT_SPDDET | PHY_INT_DPLXDET, devno);

	/* Configure the Receive/Phy Control register */
	SMC_SELECT_BANK(0, devno);
	smc_outw(lp->rpc_cur_mode, ioaddr + RPC_REG, devno);

	// Copy our capabilities from PHY_STAT_REG to PHY_AD_REG
	my_phy_caps = smc_read_mii_register(ioaddr, phyaddr, PHY_STAT_REG, devno);
	my_ad_caps = PHY_AD_CSMA;	// I am CSMA capable

	if (my_phy_caps & PHY_STAT_CAP_T4)
		my_ad_caps |= PHY_AD_T4;

	if (my_phy_caps & PHY_STAT_CAP_TXF)
		my_ad_caps |= PHY_AD_TX_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TXH)
		my_ad_caps |= PHY_AD_TX_HDX;

	if (my_phy_caps & PHY_STAT_CAP_TF)
		my_ad_caps |= PHY_AD_10_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TH)
		my_ad_caps |= PHY_AD_10_HDX;

	// Disable capabilities not selected by our user
	if (lp->ctl_rspeed != 100)
		my_ad_caps &= ~(PHY_AD_T4 | PHY_AD_TX_FDX | PHY_AD_TX_HDX);

	if (!lp->ctl_rfduplx)
		my_ad_caps &= ~(PHY_AD_TX_FDX | PHY_AD_10_FDX);
	// Update our Auto-Neg Advertisement Register
	smc_write_mii_register(ioaddr, phyaddr, PHY_AD_REG, my_ad_caps, devno);

	PRINTK3("%s:phy caps=%04x\n", dev->name, my_phy_caps);
	PRINTK3("%s:phy advertised caps=%04x\n", dev->name, my_ad_caps);

	// If the user requested no auto neg, then go set his request
	if (!(lp->ctl_autoneg)) {
		smc_phy_fixed(dev);
		goto smc_phy_configure_exit;
	}
	// Restart auto-negotiation process in order to advertise my caps
	smc_write_mii_register(ioaddr, phyaddr, PHY_CNTL_REG,
			       PHY_CNTL_ANEG_EN | PHY_CNTL_ANEG_RST, devno);

	// Wait for the auto-negotiation to complete.  This may take from
	// 2 to 3 seconds.
	// Wait for the reset to complete, or time out
	timeout = 10;		/* wait up to 5 seconds */
	while (timeout--) {
		status = smc_read_mii_register(ioaddr, phyaddr, PHY_STAT_REG, devno);
		if (status & PHY_STAT_ANEG_ACK) {
			// auto-negotiate complete
			break;
		}

		smc_wait_ms(50);	// wait 50 millisecs
		if (signal_pending(current))	// Exit anyway if signaled
		{
			printk(KERN_DEBUG
			       "%s:PHY auto-negotiate interrupted by signal\n",
			       dev->name);
			timeout = 0;
			break;
		}
		// Restart auto-negotiation if remote fault
		if (status & PHY_STAT_REM_FLT) {
			PRINTK2("%s:PHY remote fault detected\n", dev->name);

			// Restart auto-negotiation
			PRINTK2("%s:PHY restarting auto-negotiation\n",
				dev->name);
			smc_write_mii_register(ioaddr, phyaddr, PHY_CNTL_REG,
					       PHY_CNTL_ANEG_EN |
					       PHY_CNTL_ANEG_RST |
					       PHY_CNTL_SPEED | PHY_CNTL_DPLX,
					       devno);
		}
	}

	if (timeout < 1) {
		printk(KERN_DEBUG "%s:PHY auto-negotiate timed out\n",
		       dev->name);
		PRINTK2("%s:PHY auto-negotiate timed out\n", dev->name);
		failed = 1;
	}
	// Fail if we detected an auto-negotiate remote fault
	if (status & PHY_STAT_REM_FLT) {
		PRINTK2("%s:PHY remote fault detected\n", dev->name);
		failed = 1;
	}
	// The smc_phy_interrupt() routine will be called to update lastPhy18

	// Set our sysctl parameters to match auto-negotiation results
	if (lp->lastPhy18 & PHY_INT_SPDDET) {
		PRINTK2("%s:PHY 100BaseT\n", dev->name);
		lp->rpc_cur_mode |= RPC_SPEED;
	} else {
		PRINTK2("%s:PHY 10BaseT\n", dev->name);
		lp->rpc_cur_mode &= ~RPC_SPEED;
	}

	if (lp->lastPhy18 & PHY_INT_DPLXDET) {
		PRINTK2("%s:PHY Full Duplex\n", dev->name);
		lp->rpc_cur_mode |= RPC_DPLX;
	} else {
		PRINTK2("%s:PHY Half Duplex\n", dev->name);
		lp->rpc_cur_mode &= ~RPC_DPLX;
	}

	// Re-Configure the Receive/Phy Control register
	smc_outw(lp->rpc_cur_mode, ioaddr + RPC_REG, devno);

      smc_phy_configure_exit:

	// Exit auto-negotiation
	lp->autoneg_active = 0;
}

/*************************************************************************
 . smc_phy_interrupt
 .
 . Purpose:  Handle interrupts relating to PHY register 18. This is
 .  called from the "hard" interrupt handler.
 .
 ************************************************************************/
static void smc_phy_interrupt(struct net_device *dev)
{
	struct smc_dev *lp = (struct smc_dev *)netdev_priv(dev);
	int ioaddr = dev->base_addr, devno = lp->devno;
	u_int8_t phyaddr = lp->phyaddr;
	u_int16_t phy18;

	printk("%s: smc_phy_interrupt\n", dev->name);
	while (1) {
		// Read PHY Register 18, Status Output
		phy18 = smc_read_mii_register(ioaddr, phyaddr, PHY_INT_REG, devno);

		// Exit if not more changes
		if (phy18 == lp->lastPhy18)
			break;

#if (SMC_DEBUG > 1 )

		PRINTK2("%s:     phy18=0x%x\n", dev->name, phy18);
		PRINTK2("%s: lastPhy18=0x%x\n", dev->name, lp->lastPhy18);

		// Handle events
		if ((phy18 & PHY_INT_LNKFAIL) !=
		    (lp->lastPhy18 & PHY_INT_LNKFAIL)) {
			PRINTK2("%s: PHY Link Fail=%x\n", dev->name,
				phy18 & PHY_INT_LNKFAIL);
		}

		if ((phy18 & PHY_INT_LOSSSYNC) !=
		    (lp->lastPhy18 & PHY_INT_LOSSSYNC)) {
			PRINTK2("%s: PHY LOSS SYNC=%x\n", dev->name,
				phy18 & PHY_INT_LOSSSYNC);
		}

		if ((phy18 & PHY_INT_CWRD) != (lp->lastPhy18 & PHY_INT_CWRD)) {
			PRINTK2("%s: PHY INVALID 4B5B code=%x\n", dev->name,
				phy18 & PHY_INT_CWRD);
		}

		if ((phy18 & PHY_INT_SSD) != (lp->lastPhy18 & PHY_INT_SSD)) {
			PRINTK2("%s: PHY No Start Of Stream=%x\n", dev->name,
				phy18 & PHY_INT_SSD);
		}

		if ((phy18 & PHY_INT_ESD) != (lp->lastPhy18 & PHY_INT_ESD)) {
			PRINTK2("%s: PHY No End Of Stream=%x\n", dev->name,
				phy18 & PHY_INT_ESD);
		}

		if ((phy18 & PHY_INT_RPOL) != (lp->lastPhy18 & PHY_INT_RPOL)) {
			PRINTK2("%s: PHY Reverse Polarity Detected=%x\n",
				dev->name, phy18 & PHY_INT_RPOL);
		}

		if ((phy18 & PHY_INT_JAB) != (lp->lastPhy18 & PHY_INT_JAB)) {
			PRINTK2("%s: PHY Jabber Detected=%x\n", dev->name,
				phy18 & PHY_INT_JAB);
		}

		if ((phy18 & PHY_INT_SPDDET) !=
		    (lp->lastPhy18 & PHY_INT_SPDDET)) {
			PRINTK2("%s: PHY Speed Detect=%x\n", dev->name,
				phy18 & PHY_INT_SPDDET);
		}

		if ((phy18 & PHY_INT_DPLXDET) !=
		    (lp->lastPhy18 & PHY_INT_DPLXDET)) {
			PRINTK2("%s: PHY Duplex Detect=%x\n", dev->name,
				phy18 & PHY_INT_DPLXDET);
		}
#endif

		// Update the last phy 18 variable
		lp->lastPhy18 = phy18;

	}			// end while
}
