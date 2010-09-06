/*
 * Ethernet controller driver for ELVEES MULTICORE NVCom
 * Writed by Ildar F Kaibyshev skif@elvees.com
 * Copyright (c) 2010 Elvees
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <linux/dma-mapping.h>


#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/platform_device.h>
#include <asm/mach-multicore/platform.h>
#include <asm/mach-multicore/platform_eth.h>
#include "nvcom_eth_phy.h"
#include "nvcom_eth.h"

/* We ignore QSTR/MASKR 1,2 */
#define MC_NR_IRQS	(1 + 8 + 32)  /* ONCD + MIPS + QSTR */

#define IRQ_QSTR0_0_TO_22       3
#define IRQ_MEMCH_DMA           4
#define IRQ_MFBSP               5
#define IRQ_DSP                 7
#define IRQ_COMPARE             8

#define IRQMASK_QSTR0_0_TO_22   mips_irqmask(IRQMASK_QSTR0_0_TO_22)
#define IRQMASK_MEMCH_DMA       mips_irqmask(IRQ_MEMCH_DMA)
#define IRQMASK_MFBSP           mips_irqmask(IRQ_MFBSP)
#define IRQMASK_DSP             mips_irqmask(IRQ_DSP)
#define IRQMASK_COMPARE         mips_irqmask(IRQ_COMPARE)

/* QSTR0 */
#define	IRQ_nIRQ0		QSTR_IRQNO(0)
#define	IRQ_nIRQ1		QSTR_IRQNO(1)
#define	IRQ_nIRQ2		QSTR_IRQNO(2)
#define	IRQ_nIRQ3		QSTR_IRQNO(3)
#define	IRQ_UART0		QSTR_IRQNO(4)
#define	IRQ_UART1		QSTR_IRQNO(5)
#define	IRQ_USB			QSTR_IRQNO(7)
#define	IRQ_USB_EP4		QSTR_IRQNO(8)
#define	IRQ_USB_EP3		QSTR_IRQNO(9)
#define	IRQ_USB_EP2		QSTR_IRQNO(10)
#define	IRQ_USB_EP1		QSTR_IRQNO(11)
#define	IRQ_ETH_RXFRAME		QSTR_IRQNO(12)
#define	IRQ_ETH_TXFRAME		QSTR_IRQNO(13)
#define	IRQ_ETH_DMA_RX		QSTR_IRQNO(14)
#define	IRQ_ETH_DMA_TX		QSTR_IRQNO(15)
#define	IRQ_VPIN		QSTR_IRQNO(16)
#define	IRQ_VPIN_RX		QSTR_IRQNO(17)
#define	IRQ_VPOUT		QSTR_IRQNO(18)
#define	IRQ_VPOUT_TX		QSTR_IRQNO(19)
#define	IRQ_WDT			QSTR_IRQNO(20)
#define	IRQ_RTT			QSTR_IRQNO(21)
#define	IRQ_IT			QSTR_IRQNO(22)
#define	IRQ_I2C			QSTR_IRQNO(23)
#define	IRQ_MCC			QSTR_IRQNO(31)

#define	QSTR_nIRQ0		QSTR_IRQMASK(0)
#define	QSTR_nIRQ1		QSTR_IRQMASK(1)
#define	QSTR_nIRQ2		QSTR_IRQMASK(2)
#define	QSTR_nIRQ3		QSTR_IRQMASK(3)
#define	QSTR_UART0		QSTR_IRQMASK(4)
#define	QSTR_UART1		QSTR_IRQMASK(5)
#define	QSTR_USB		QSTR_IRQMASK(7)
#define	QSTR_USB_EP4		QSTR_IRQMASK(8)
#define	QSTR_USB_EP3		QSTR_IRQMASK(9)
#define	QSTR_USB_EP2		QSTR_IRQMASK(10)
#define	QSTR_USB_EP1		QSTR_IRQMASK(11)
#define	QSTR_ETH_RXFRAME	QSTR_IRQMASK(12)
#define	QSTR_ETH_TXFRAME	QSTR_IRQMASK(13)
#define	QSTR_ETH_DMA_RX		QSTR_IRQMASK(14)
#define	QSTR_ETH_DMA_TX		QSTR_IRQMASK(15)
#define	QSTR_VPIN		QSTR_IRQMASK(16)
#define	QSTR_VPIN_RX		QSTR_IRQMASK(17)
#define	QSTR_VPOUT		QSTR_IRQMASK(18)
#define	QSTR_VPOUT_TX		QSTR_IRQMASK(19)
#define	QSTR_WDT		QSTR_IRQMASK(20)
#define	QSTR_RTT		QSTR_IRQMASK(21)
#define	QSTR_IT			QSTR_IRQMASK(22)
#define	QSTR_I2C		QSTR_IRQMASK(23)
#define	QSTR_MCC		QSTR_IRQMASK(31)

#define IRQ_TIMER		IRQ_IT
#define QSTR_TIMER		QSTR_IT

#define CSR_VMMODE		(1 << 0)
#define CSR_FLUSH_I		(1 << 12)
#define CSR_FLUSH_D		(1 << 14)

#define CR_PLL_CLKMASK		0x7f0000
#define CR_PLL_CLKSHIFT		16
#define mc_frequency_multiplier() ((mc_regs->cr_pll & CR_PLL_CLKMASK) >> CR_PLL_CLKSHIFT)

#define MC_CRAM_BASE		0xb8000000	/* 64k bytes there */
#define MC_LPORTCH_BASE		0xb82f0400	/* ADSP21160-compatible */
#define MC_MPORT_BASE		0xb82f1000
#define MC_UART0_BASE		0xb82f3000
#define MC_UART1_BASE		0xb82f3800
#define MC_REGS_BASE		0xb82f4000
#define MC_LPORT_BASE		0xb82f7000
#define MC_TIMER_BASE		0xb82fd000

#define MC_LPORT_STEP		0x1000
#define MC_LPORTCH_STEP		0x0100
#define UART0_BASE		MC_UART0_BASE
#define UART1_BASE		MC_UART1_BASE
#define MC_UART_SCLR		5

#define MC_MEMSIZE		(1048576 * CONFIG_MULTICORE_MEMORY)

/*
 * Define MAC_PHY ID.
 * Micrel KS8721BL PHY, see datasheet.
 */
#define MII_KS8721_PHY_ID_1		0x0022
#define MII_KS8721_PHY_ID_2		0x5
#define MII_KS8721_PHY_MODEL_NUMBER	0x21
#define MII_KS8721_PHY_REVISION_NUMBER	0x9
#define MII_KS8721_PHY_REVISION_MASK	0xf
#define MII_KS8721_PHY_ID_MASK		0xfffffff0

/* Assemble the ID of KS8721BL PHY  */

#define MII_KS8721BL_ID		(((MII_KS8721_PHY_REVISION_NUMBER | \
				(MII_KS8721_PHY_MODEL_NUMBER << 4) | \
				(MII_KS8721_PHY_ID_2 << 10)) & 0xffff) | \
				(MII_KS8721_PHY_ID_1 << 16))

//#define MII_KS8721BL_ID   0x00221619

/*
 * Define the value of MAC adress.
 * We have to get VENDOR ID from IEEE Registration Authority
 * There is the MAC adress, it is defined for test purpose
 */
#define MAC_ADR_1  0x00
#define MAC_ADR_2  0x01
#define MAC_ADR_3  0x02
#define MAC_ADR_4  0x13
#define MAC_ADR_5  0x24
#define MAC_ADR_6  0x35

/*
* Define Ethernet controller registers
*/
/*
0x182fe000 - is phisical address
*/

#define MC_NVCOM_ETHER_BASE	0xb82fe000
#define MC_NVCOM_ETHER_ADR_RANGE 0x70

#define MC_NVCOM_ETHER_IRQ 12
#define MC_NVCOM_ETHER_IRQ_RANGE 3

#define REG_MD_MODE	MC_NVCOM_ETHER_BASE + 0x060
  #define MD_MODE_FR_DIVIDER_MASK	0xff
  #define MD_MODE_FR_DIVIDER_SHFT	0x0
  #define MD_MODE_RST_MASK		0x01
  #define MD_MODE_RST_SHFT		0x08

#define REG_MD_CTRL		MC_NVCOM_ETHER_BASE + 0x058
  #define MD_CTRL_DATA_WR_MASK		0xffff
  #define MD_CTRL_DATA_WR_SHFT		0x0
  #define MD_CTRL_PHYREG_ADR_MASK	0x1f
  #define MD_CTRL_PHYREG_ADR_SHFT	16
  #define MD_CTRL_PHY_ADR_MASK		0x1f
  #define MD_CTRL_PHY_ADR_SHFT		24
  #define MD_CTRL_PHY_IRQ_EN_MASK	0x1
  #define MD_CTRL_PHY_IRQ_EN_SHFT	29
  #define MD_CTRL_OPCOD_MASK		0x3
  #define MD_CTRL_OPCOD_SHFT		30

  #define MD_OP_READ	0x1
  #define MD_OP_WRITE	0x2
  #define MD_OP_IDLE	0x0
  #define MD_OP_FBDN	0x3 /* error, forbiden combination */


#define REG_MD_STAT		MC_NVCOM_ETHER_BASE + 0x05c
  #define MD_STAT_DATA_RD_MASK		0xffff
  #define MD_STAT_DATA_RD_SHFT		0x0
  #define MD_STAT_BUSY_MASK		0x1
  #define MD_STAT_BUSY_SHFT		29
  #define MD_STAT_OP_END_MASK		0x3
  #define MD_STAT_OP_END_SHFT		30


/* This definition need to discribe mdio resorses*/
#define MC_NVCOM_MDIO_BASE REG_MD_CTRL
#define MC_NVCOM_MDIO_RANGE (4+4+4)


#define REG_MAC_CTRL	 MC_NVCOM_ETHER_BASE+0
  #define MAC_CTRL_FULLD_SHFT		0
  #define MAC_CTRL_EN_TX_DMA_SHFT	1
  #define MAC_CTRL_EN_TX_SHFT		2
  #define MAC_CTRL_MASK_TX_DONE_SHFT	3
  #define MAC_CTRL_EN_RX_SHFT		4
  #define MAC_CTRL_LOOPBACK_SHFT	5
  #define MAC_CTRL_FULLD_RX_SHFT	6
  #define MAC_CTRL_RX_DONE		7
  #define MAC_CTRL_MASK_RX_FIFO_OVR_ERR_SHFT	8
  #define MAC_CTRL_CP_TX_SHFT		9
  #define MAC_CTRL_RST_TX_SHFT		10
  #define MAC_CTRL_CP_RX_SHFT		11
  #define MAC_CTRL_RST_RX_SHFT		12
  #define CMD_MAC_RST_TX	(1<<MAC_CTRL_RST_TX_SHFT)
  #define CMD_MAC_RST_P_TX	(1<<MAC_CTRL_CP_TX_SHFT)
  #define CMD_MAC_RST_RX        (1<<MAC_CTRL_RST_RX_SHFT)
  #define CMD_MAC_RST_P_RX      (1<<MAC_CTRL_CP_RX_SHFT)


#define REG_TX_FRM_CTRL		 MC_NVCOM_ETHER_BASE+0x20
  #define TX_FRM_CTRL_LENGTH_SHFT	0
  #define TX_FRM_CTRL_LENGTH_MASK	0x3ff
  #define TX_FRM_CTRL_TYPE_EN_SHFT	12
  #define TX_FRM_CTRL_FCS_CLT_EN_SHFT	13
  #define TX_FRM_CTRL_DIS_MK_FRM_SHFT	14
  #define TX_FRM_CTRL_DISPAD_SHFT	15
  #define TX_FRM_CTRL_TX_REQ_SHFT	16
  #define CMD_MAC_SEND_PKG 	(1<<TX_FRM_CTRL_TX_REQ_SHFT)

#define REG_STATUS_TX		MC_NVCOM_ETHER_BASE+0x24

#define REG_UCADDR_L 	MC_NVCOM_ETHER_BASE+0x28

#define REG_UCADDR_H	MC_NVCOM_ETHER_BASE+0x2c

#define REG_SRC_ADDR_L 	MC_NVCOM_ETHER_BASE+0x04

#define REG_SRC_ADDR_H 	MC_NVCOM_ETHER_BASE+0x08

#define REG_DST_ADDR_L 	MC_NVCOM_ETHER_BASE+0x0c

#define REG_DST_ADDR_H 	MC_NVCOM_ETHER_BASE+0x10

/* Used 16 bit of REG_TYPE_FRAME */
#define REG_TYPE_FRAME	MC_NVCOM_ETHER_BASE+0x18



#define REG_RX_FRM_CTRL MC_NVCOM_ETHER_BASE+0x48
  #define RX_FRM_CTRL_EN_ALL_SHFT	9
  #define CMD_RX_ALL_PKG_EN	(1<<RX_FRM_CTRL_EN_ALL_SHFT)


#define REG_STATUS_RX	MC_NVCOM_ETHER_BASE+0x50
  #define STATUS_RX_DONE_SHFT		3
  #define STATUS_RX_NUM_FR_SHFT		4
  #define STATUS_RX_NUM_FR_MASK		0x7f
  #define STATUS_RXW_SHFT		12
  #define STATUS_RXW_MASK		0x3ff
  #define STATUS_RX_DONE_BIT		(1<<STATUS_RX_DONE_SHFT)


#define REG_RX_FRAME_STATUS_FIFO MC_NVCOM_ETHER_BASE+0x54
  #define RX_FRAME_STATUS_LEN_SHFT 0
  #define RX_FRAME_STATUS_LEN_MASK 0xfff
  #define RX_FRAME_STATUS_OK_SHFT 12
  #define RX_FRAME_STATUS_OK_BIT  (1<<RX_FRAME_STATUS_OK_SHFT )

/* Define EMAC DMA stuff*/
//0xB82FE800
#define REG_MAC_CSR_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x800
#define REG_MAC_CP_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x804
#define REG_MAC_IR_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x808
//#define REG_MAC_OR_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x80C
//#define REG_MAC_Y_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x810
#define REG_MAC_RUN_DMA_0_RX	MC_NVCOM_ETHER_BASE+0x80C

//0xB82FE840
#define REG_MAC_CSR_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x840
#define REG_MAC_CP_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x844
#define REG_MAC_IR_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x848
//#define REG_MAC_OR_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x84C
//#define REG_MAC_Y_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x850
#define REG_MAC_RUN_DMA_1_TX	MC_NVCOM_ETHER_BASE+0x84c //54



#define CSR_DMA_WCX_MASK	0xffff0000
#define CSR_DMA_WCX_SHFT	16
#define CSR_DMA_DONE_SHFT	15
#define CMD_DMA_RUN		1
#define CMD_DMA_DONE		(1<<CSR_DMA_DONE_SHFT)



//The macro convert virtual to phisical address
#define _v2ph(a) ( 0xffffffff & ((a) - 0xa0000000) )

//The macro convert phisical to virtual address
#define _ph2v(a) ( 0xffffffff & ((a) + 0xa0000000) )

#undef _inl
#undef _outl

#define _inl(port)      (*(volatile u32 *)( (port) ))
#define _outl(port, val) *(volatile u32 *)( (port) ) = val

#define PDEBUG printk
//#define PDEBUG(...)




static int nvcom_eth_close(struct net_device *dev);
static int nvcom_eth_open(struct net_device *dev);
static int nvcom_eth_probe(struct platform_device *pdev);
static int nvcom_eth_remove(struct platform_device *pdev);
static void init_registers(struct net_device *dev);
int nvcom_set_mac_address(struct net_device *dev);

static int init_nvcom_phy(struct net_device *dev);

static void adjust_link(struct net_device *dev);

static int nvcom_start_xmit(struct sk_buff *skb, struct net_device *dev);

static int nvcom_dump_frame(unsigned char * head, int len);




static void init_registers(struct net_device *dev)
{
/*Set default values to Ethernet controller registers */
/* Let use the controller in mode when the packet is
made in transmit block*/
	u_int32_t val;

/*Set full duplex mode */
/*Reset TX and RX blocks and pointers*/

//_outl(REG_MAC_CTRL ,CMD_MAC_RST_TX | CMD_MAC_RST_P_TX |
//		    CMD_MAC_RST_RX | CMD_MAC_RST_P_RX );

	val=_inl(REG_MAC_CTRL);
	PDEBUG(KERN_ERR "REG_MAC_CTRL:read 0x%08x\n",val);

 	val= val |
/* TX Block enable */
		   (1<<MAC_CTRL_FULLD_SHFT) |
		   (1<<MAC_CTRL_EN_TX_SHFT) |
 		   (1<<MAC_CTRL_EN_TX_DMA_SHFT)|
/* RX block enable */
 		   (1<<MAC_CTRL_EN_RX_SHFT)|
/*RX interrupt enable */
		   (1<<MAC_CTRL_RX_DONE) |
		   (1<<MAC_CTRL_MASK_RX_FIFO_OVR_ERR_SHFT);
        PDEBUG(KERN_ERR "REG_MAC_CTRL:w 0x%08x\n",val);

	_outl(REG_MAC_CTRL, val);
/* Let check value from cfg port*/
        val=_inl(REG_MAC_CTRL);
        PDEBUG(KERN_ERR "REG_MAC_CTRL:cr 0x%08x\n",val);

/* Set enable RX all type of packets*/
        val=_inl(REG_RX_FRM_CTRL);
        PDEBUG(KERN_ERR "REG_RX_FRM_CTRL:r 0x%08x\n",val);

	val=val| CMD_RX_ALL_PKG_EN;
        _outl(REG_RX_FRM_CTRL,val);
        PDEBUG(KERN_ERR "REG_RX_FRM_CTRL:w 0x%08x\n",val);

        val=_inl(REG_RX_FRM_CTRL);
        PDEBUG(KERN_ERR "REG_RX_FRM_CTRL:cr 0x%08x\n",val);


//#define MK_PKG_IN_MAC 1
#undef MK_PKG_IN_MAC

#ifdef MK_PKG_IN_MAC
/*Let form the packet in TX block =0 */
	val=_inl(REG_TX_FRM_CTRL);
       PDEBUG(KERN_ERR "!!! trace from 0x%08x read 0x%08x\n",REG_TX_FRM_CTRL ,val);

	val=val & (~(1<<TX_FRM_CTRL_DIS_MK_FRM_SHFT));
	_outl(REG_TX_FRM_CTRL,val);

/* Let check value from cfg port*/
        val=_inl(REG_TX_FRM_CTRL );
        PDEBUG(KERN_ERR "!!! trace from 0x%08x read 0x%08x\n",REG_TX_FRM_CTRL ,val);

#else


/*Let send prepared packet */
	val=_inl(REG_TX_FRM_CTRL);
       PDEBUG(KERN_ERR "REG_TX_FRM_CTRL:r 0x%08x\n",val);

	val=val | (1<<TX_FRM_CTRL_DIS_MK_FRM_SHFT);
	_outl(REG_TX_FRM_CTRL,val);

       PDEBUG(KERN_ERR "REG_TX_FRM_CTRL:w 0x%08x\n",val);
/* Let check value from cfg port*/
        val=_inl(REG_TX_FRM_CTRL );

       PDEBUG(KERN_ERR "REG_TX_FRM_CTRL:cr 0x%08x\n",val);

#endif /*MK_PKG_IN_NAC*/

#ifdef MK_PKG_IN_MAC

/* Set the type/length of packet*/
/*Set type enable for all packets*/
        val=_inl(REG_TX_FRM_CTRL);
        printk(KERN_ERR "read (REG_TX_FRM_CTR)=0x%08x\n",val);

/* This code is best to set in open function */
        val=val | (1<<TX_FRM_CTRL_TYPE_EN_SHFT);
        printk(KERN_ERR "new val to write=0x%08x\n",val);
        _outl(REG_TX_FRM_CTRL,val);
//      _inl(REG_TX_FRM_CTRL);
//        printk(KERN_ERR "read (REG_TX_FRM_CTR)=0x%08x\n",val);

        /* control read register REG_TX_FRM_CTRL */
//        val=_inl(REG_TX_FRM_CTRL);
//        printk(KERN_ERR "read (REG_TX_FRM_CTR)=0x%08x\n",val);

#endif /*MK_PKG_IN_NAC*/


}

/* Set the MAC addres for RX */
int nvcom_set_mac_address(struct net_device *dev)
{
	u_int32_t mac_adr_part =0;

/* I do it how I can, may be improved later*/
	mac_adr_part=dev->dev_addr[0] |
	(dev->dev_addr[1]<<8) |
	(dev->dev_addr[2]<<16)|
	(dev->dev_addr[3]<<24);
       _outl(REG_UCADDR_L,mac_adr_part );
//       _outl(REG_UCADDR_L, 0xffffffff);
	mac_adr_part=0;
	mac_adr_part=dev->dev_addr[4] |
	(dev->dev_addr[5] << 8 );
       _outl(REG_UCADDR_H,mac_adr_part );
//       _outl(REG_UCADDR_H,0xffffffff);

        PDEBUG(KERN_ERR "!!! trace from 0x%08x read 0x%08x\n",REG_UCADDR_L ,_inl(REG_UCADDR_L));

       PDEBUG(KERN_ERR "!!! trace from 0x%08x read 0x%08x\n",REG_UCADDR_H ,_inl(REG_UCADDR_H));


	return 0;
}

/* Called every time the controller might need to be made
 * aware of new link state.  The PHY code conveys this
 * information through variables in the phydev structure, and this
 * function converts those variables into the appropriate
 * register values, and can bring down the device if needed.
 */

static void adjust_link(struct net_device *dev)
{
	printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

	return;
}

//the register controlls clk to divices in SOC
#define REG_CLK_EN 0x182F4004
//define the bit number to controll clk to ethernet PHY device
#define CLK_EN_ENET_SHFT 20
#define CLK_EN_ENET_MASK 0x1
#define GET_CLK_ENET(d) 	(((d) >> CLK_EN_ENET_SHFT) & CLK_EN_ENET_MASK)
#define SET_CLK_ENET(d)		 ( (d) | (1 << CLK_EN_ENET_SHFT) )
#define UNSET_CLK_ENET(d)	 ( (d) & (~(1 << CLK_EN_ENET_SHFT))  )

//The macro convert phisical address to virtual
#define PHY_TO_VIRT(a) ( 0xffffffff & ((a) + 0xa0000000) )
//#define PHY_TO_VIRT(a) (a)


#undef _inl
#undef _outl

#define _inl(port)      (*(volatile u32 *)( (port) ))
#define _outl(port, val) *(volatile u32 *)( (port) ) = val

#define PDEBUG printk
//#define PDEBUG(...)

/* Index to functions, as function prototypes. */
//static int nvcom_eth_init(void);

/*
static int __init nvcom_eth_init(void)
{
	pr_info("NVCom ethernet driver version 0.0\n");
	return 0;
}


*/
/*
* Write value to the PHY at mii_id at register regnum,
* on the bus, waiting until the write is done before returning.
*/
int nvcom_mdio_write(struct mii_bus *bus, int mii_id, int regnum, u16 value)
{
        u_int32_t dr; /* data to read*/
        u_int32_t dw; /*data to write*/
//probably need to lock  the mutex_lock(&dev)
// if the port is busy wate the time out, if time out exceed - error
         while( (dr=_inl(REG_MD_STAT)) & (MD_STAT_BUSY_MASK<<MD_STAT_BUSY_SHFT) )
        {
                PDEBUG(KERN_ERR "!!! %s:%d\n",__FUNCTION__,__LINE__);
                udelay(10);
        }
//set adress of the PHY, the register number,
// the opcode  to write , the data to write
//      dr=_inl(REG_MD_CTRL);
        dw= ( (mii_id & MD_CTRL_PHY_ADR_MASK) << MD_CTRL_PHY_ADR_SHFT ) |
            ( (regnum & MD_CTRL_PHYREG_ADR_MASK) << MD_CTRL_PHYREG_ADR_SHFT )|
	    ( (value & MD_CTRL_DATA_WR_MASK) << MD_CTRL_DATA_WR_SHFT  )|
            ( (MD_OP_WRITE & MD_CTRL_OPCOD_MASK) << MD_CTRL_OPCOD_SHFT );

//isue the command to write
        _outl(PHY_TO_VIRT(REG_MD_CTRL),dw );

// I supose that data are writting correct,
//and I do not check the correctnes of writting data

        return 0;
}

/*
*  Read the bus for PHY at addr mii_id, register regnum, and
* return the value.
*/
static int nvcom_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	u_int32_t dr; /* data to read*/
	u_int32_t dw; /*data to write*/

	static int call_count=0;

//       PDEBUG(KERN_ERR "!!! %d trace %s:%d mii_id=%d; regnum=%d\n",call_count++,__FUNCTION__,__LINE__,mii_id,regnum);

//probably need to lock  the mutex_lock(&dev)
// Is port busy wate the time out, if time out exceed - error
	 while( (dr=_inl(REG_MD_STAT)) & (MD_STAT_BUSY_MASK<<MD_STAT_BUSY_SHFT) )
	{
	        PDEBUG(KERN_ERR "!!! %s:%d\n",__FUNCTION__,__LINE__);
		cpu_relax();
//	udelay(10);
	}

//set adress of the PHY, the register number, the opcode  to read
//	dr=_inl(REG_MD_CTRL);
	dw= (( mii_id & MD_CTRL_PHY_ADR_MASK) << MD_CTRL_PHY_ADR_SHFT) |
	    (( regnum & MD_CTRL_PHYREG_ADR_MASK) << MD_CTRL_PHYREG_ADR_SHFT)| 		(( MD_OP_READ & MD_CTRL_OPCOD_MASK) << MD_CTRL_OPCOD_SHFT);

//isue the command to PHY
         _outl(REG_MD_CTRL,dw );

//Read data from PHY
         while( (dr=_inl(REG_MD_STAT)) & (MD_STAT_BUSY_MASK<<MD_STAT_BUSY_SHFT) )
        {
//                PDEBUG(KERN_ERR "!!! trace %s:%d\n",__FUNCTION__,__LINE__);
		cpu_relax();
//                udelay(10);
        }

//here needs to unlock mutex

//    PDEBUG(KERN_ERR "!!! trace %s:%d return 0x%x \n",__FUNCTION__,__LINE__,(dr & MD_STAT_DATA_RD_MASK) );
	return (dr & MD_STAT_DATA_RD_MASK);
}



/* Reset the MIIM registers, and wait for the bus to free */
int nvcom_mdio_reset(struct mii_bus *bus)
{
	u_int32_t t;
        u_int32_t timeout = NV_PHY_INIT_TIMEOUT ;

	PDEBUG(KERN_ERR "!!!trace: %s:%d\n",__FUNCTION__,__LINE__);

	spin_lock_bh(&bus->mdio_lock);

	/*Check if clock to MII & PHY is not enable
	then switch on this one*/
        t=_inl(PHY_TO_VIRT(REG_CLK_EN));
//  	PDEBUG(KERN_ERR "RESET_01 0x%8x := 0x%08x\n",REG_CLK_EN,t);

	/* check is bit 20 is not set to 1 then set
	this one  to enble clk to ether PHY*/
        if(! (t & (CLK_EN_ENET_MASK<<CLK_EN_ENET_SHFT) ) )
        {
                t=t |(0x1 << CLK_EN_ENET_SHFT );
                _outl(PHY_TO_VIRT(REG_CLK_EN),t );
  		PDEBUG(KERN_ERR "Clock to PHY switched ON\n");

	}

	/* CLK divider, the output friquency
	should not exeed 2,5 MHg see Data sheet to ks8721*/

	/* Reset the PHY */

	spin_unlock_bh(&bus->mdio_lock);


#if 0
//Check is enable the friquency to ethernet PHY
//read clk enabling controll register
        printk(KERN_ERR "read from addr 0x%lx\n",PHY_TO_VIRT(REG_CLK_EN));
        tt=_inl(PHY_TO_VIRT(REG_CLK_EN));
        printk(KERN_ERR "!!! 0x%lx := 0x%lx\n",REG_CLK_EN,tt);
//check is bit 20 set to 1 to enble clk to ether PHY


         printk (KERN_ERR "MASK=0x%lx\n", (~(1 << CLK_EN_ENET_SHFT))  );

        tt=UNSET_CLK_ENET(tt);
        printk(KERN_ERR "!!!_2 0x%lx := 0x%lx\n",REG_CLK_EN,tt);
        if(!(GET_CLK_ENET(tt)))
        {
                printk(KERN_ERR "Try to set enable clk  to ethernet PHY\n");
                tt=SET_CLK_ENET(tt);
                _outl(PHY_TO_VIRT(REG_CLK_EN),tt );
        }

//read MD_MODE
        tt=_inl(PHY_TO_VIRT(REG_MD_MODE));
        printk(KERN_ERR "REG_MD_MODE=0x%08x\n",tt);

#endif /* 0 */
        return 0;
}

int nvcom_mdio_probe(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct  nvcom_mdio_platform_data *pdata;
	struct mii_bus *new_bus;
	int err = 0;
	int i;

        PDEBUG(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

        if (NULL == dev)
                return -EINVAL;

        new_bus = kzalloc(sizeof(struct mii_bus), GFP_KERNEL);

        if (NULL == new_bus)
                return -ENOMEM;

        new_bus->name = "NVCom MII Bus",
        new_bus->read = &nvcom_mdio_read,
        new_bus->write = &nvcom_mdio_write,
        new_bus->reset = &nvcom_mdio_reset,
        new_bus->id = pdev->id;

        pdata = (struct nvcom_mdio_platform_data *)pdev->dev.platform_data;

        if (NULL == pdata) {
                printk(KERN_ERR "MNCom  mdio %d: Missing platform data!\n", pdev->id);
                return -ENODEV;
        }

	new_bus->irq = pdata->irq;
	new_bus->dev = dev;
	dev_set_drvdata(dev, new_bus);

	err = mdiobus_register(new_bus);
        if (0 != err) {
                printk (KERN_ERR "%s: Cannot register as MDIO bus\n",
                                new_bus->name);
                goto bus_register_fail;
        }

/* Put consol the adress of PHY device on MII bus*/
	for(i=0; i<PHY_MAX_ADDR; i++){
		if (new_bus->phy_map[i]) {

                        if ( (MII_KS8721BL_ID & MII_KS8721_PHY_ID_MASK) ==
       		 (new_bus->phy_map[i]->phy_id & MII_KS8721_PHY_ID_MASK) ){
                         pr_info("%s: PHY KS8721 Rev.%d is founded on adress 0x%x\n", new_bus->name,new_bus->phy_map[i]->phy_id & MII_KS8721_PHY_REVISION_MASK,i);
			 }
			else
                       pr_info("%s:Unknown PHY ID: 0x%x is founded on adress 0x%x\n",new_bus->name,new_bus->phy_map[i]->phy_id,i );
		}
	}

        return 0;

bus_register_fail:
        kfree(new_bus);
	return err;
}

static int  init_nvcom_phy(struct net_device *dev)
{
/*
* The problem is that tha address of phy device has a bug
* To make phy_connection I has to discover this divice on mio bus
* by scanning this bus instead of getting this adress(phy_id)
* from struct platform_data
*/
	int i;
	int phy_is_found=0;
        struct nvcom_eth_private  *priv = netdev_priv(dev);

        struct phy_device *phydev;
        char phy_id[BUS_ID_SIZE];
        phy_interface_t interface;

//	interface= PHY_INTERFACE_MODE_MII;
	interface = priv->einfo->interface;

/*Scan the mdio_bus again*/
       for(i=0; i<PHY_MAX_ADDR; i++){

	        snprintf(phy_id, BUS_ID_SIZE, PHY_ID_FMT, priv->einfo->bus_id,i /*priv->einfo->phy_id*/);

	        phydev = phy_connect(dev, phy_id, &adjust_link, 0, interface);

	        if (! (IS_ERR(phydev)) ) {
			phy_is_found=1;
			pr_info("%s: attach to PHY %s\n",dev->name, phy_id);
			break;
		}
	}

//        if (IS_ERR(phydev)) {
        if (phy_is_found==0) {

                printk(KERN_ERR "%s: Could not attach to PHY\n", dev->name);
                return PTR_ERR(phydev);
        }

	priv->phydev = phydev;

	return 0;
}

int startup_nvcom(struct net_device *dev)
{
        struct nvcom_eth_private  *priv = netdev_priv(dev);
//	unsigned char* t;
	int err=0;
//	u_int32_t tt;
	dma_addr_t dma_addr;
	u_int32_t virt_addr;
/*Set TX enable */
/*Set TX_DMA enable*/
/* Set mode 'Make frame in MAC'  */
/*allocate memory buffer aligned to 64 bytes */
/*
	priv->tx_b_64 =
	(u_int64_t *) kmalloc(sizeof(u_int64_t) * TX_BUF_SIZE_64, GFP_KERNEL);

	if(NULL==priv->tx_b_64)
	{
		printk(KERN_ERR "%s: Could not allocate tx buffer",dev->name	);
	err = -ENOMEM;
	goto fail;
	}
*/

        virt_addr = (unsigned long) dma_alloc_coherent(NULL,
		sizeof(unsigned char) * (TX_BUF_BYTE_SIZE+TX_ALIGN_ADD),
                &dma_addr, GFP_KERNEL);

      	 if (virt_addr == 0) {
//             if (netif_msg_ifup(priv))
                 printk(KERN_ERR "%s: Could not allocate buffer for DMA to tx fifo!\n",
                dev->name);
//                return -ENOMEM;
		err=-ENOMEM;
		goto tx_alloc_err;

        }

	priv->dma_addr_tx=dma_addr;
	priv->virt_addr_tx=virt_addr;

printk(KERN_ERR "TX dma_addr=0x%08x;  virt_addr=0x%08x\n",dma_addr,virt_addr);


/* Allocate buffer for STATUS_RX_FIFO*/



       priv->rx_status_buf = (unsigned long *) kmalloc(
	sizeof(unsigned char) * (RX_STATUS_BUF_BYTE_SIZE), GFP_KERNEL);

      	 if ( priv->rx_status_buf == 0) {
//             if (netif_msg_ifup(priv))
                 printk(KERN_ERR "%s: Could not allocate buffer for DMA to rx status fifo!\n",dev->name);

//                return -ENOMEM;
		err=-ENOMEM;
		goto status_rx_alloc_err;
        }
printk(KERN_ERR "RX_STATUS priv->rx_status_buf=0x%08x\n",priv->rx_status_buf);


/* Allocate buffer for Data of packets _RX_FIFO*/

        virt_addr = (unsigned long) dma_alloc_coherent(NULL,
		sizeof(unsigned char) * (RX_BUF_BYTE_SIZE+RX_ALIGN_ADD),
                &dma_addr, GFP_KERNEL);

      	 if (virt_addr == 0) {
//             if (netif_msg_ifup(priv))
                 printk(KERN_ERR "%s: Could not allocate buffer for DMA!\n",
                               dev->name);

//                return -ENOMEM;
		goto rx_alloc_err;
        }
printk(KERN_ERR "RX dma_addr=0x%08x;  virt_addr=0x%08x\n",dma_addr,virt_addr);

	priv->dma_addr_rx=dma_addr;
	priv->virt_addr_rx=virt_addr;


//	t =
//	(unsigned char *) kmalloc(sizeof(unsigned char) * (TX_BUF_BYTE_SIZE+TX_ALIGN_ADD)  , GFP_KERNEL);

//	if(NULL==t)
//	{
//		printk(KERN_ERR "%s: Could not allocate tx buffer",dev->name	);
//	err = -ENOMEM;
//	goto fail;
//	}
//	printk(KERN_ERR "t = 0x%x\n", t);

	/*Make align to 8 bytes boundary */
//	tt=(u_int32_t ) t;

//	tt= (( (tt+7) >> 3 )<<3);

//	priv->tx_b_64=(u_int64_t*)tt;

//	printk(KERN_ERR "priv->tx_b_64 = 0x%x\n", priv->tx_b_64);

//fail:

	/* Here settings about RX*/
	/* Let register the RX inerrupt handler*/

	if (request_irq(IRQ_ETH_RXFRAME , /* int interrupt line allocate*/
                        nvcom_receive, /* function handler*/
                        IRQF_DISABLED,     /* irq flags */
                        "enet_rx", /*asci name of claiming dev*/
                        dev) < 0)
	{
//		if (netif_msg_intr(priv))
                     printk(KERN_ERR "%s: Can't get IRQ %d (receive)\n",                                       dev->name, IRQ_ETH_RXFRAME);

                        err = -1;
                        goto rx_irq_fail;
         }
	disable_irq(IRQ_ETH_RXFRAME);

	return 0;



rx_irq_fail:
        dma_free_coherent(NULL,
		sizeof(unsigned char) * (RX_BUF_BYTE_SIZE+RX_ALIGN_ADD),        	    priv->virt_addr_rx,
                priv->dma_addr_rx);

rx_alloc_err:

	kfree(priv->rx_status_buf);


status_rx_alloc_err:
        dma_free_coherent(NULL,
		sizeof(unsigned char) * (TX_BUF_BYTE_SIZE+TX_ALIGN_ADD),        	    priv->virt_addr_tx,
                priv->dma_addr_tx);

tx_alloc_err:

	return err;
}



static int nvcom_eth_open(struct net_device *dev)
{
	int err;
	err=0;
printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

	init_registers(dev);
	nvcom_set_mac_address(dev);
	err=init_nvcom_phy(dev);
        if(err)
                return err;
	err = startup_nvcom(dev);

/*Now hardware is ready to transmit/recive packets */

//printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);
        netif_start_queue(dev);

	enable_irq(IRQ_ETH_RXFRAME);
//	printk("handler: irq_desc[IRQ_ETH_RXFRAME]: %x, want %x\n",
//	       irq_desc[IRQ_ETH_RXFRAME].action,
//               );

//printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);
	return err;

}


static int nvcom_eth_close(struct net_device *dev)
{

printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

        netif_stop_queue(dev);

	return 0;


}


static int  nvcom_dump_frame(unsigned char * head, int len)
{
	int jkj;
	int jlenj;

	jlenj=len;
	for(jkj=0;jkj<jlenj;jkj++)
	{
		if ( ((jkj & 0xf)==0)&&(jkj != 0))
			printk("\n");

 		printk("%02x ",*( head+jkj) ) ;
	}

	printk("\n");

	return jkj;
}


/*
* This called by kernerl to transmit a frame
*/
static int nvcom_start_xmit(struct sk_buff *skb, struct net_device *dev) {
	int i;
	unsigned long flags;
	unsigned char *pkg_data;
	u_int32_t val;
	struct nvcom_eth_private *priv = netdev_priv(dev);



	printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);


	pkg_data=skb->data;
        /* Lock priv now */
        spin_lock_irqsave(&priv->txlock, flags);

	/* Dump sk_buff */
//	printk(KERN_ERR "data_len=0x%x\n",skb->data_len);
//	printk(KERN_ERR "mac_len=0x%x\n",skb->mac_len);
	printk(KERN_ERR "skb->len=0x%x\n",skb->len);
	printk(KERN_ERR "Dump *data of sk_buff len=0x%x (%d)\n",skb->len,skb->len ) ;

	i=nvcom_dump_frame(skb->data,skb->len);
//	printk(KERN_ERR "dumped %d\n",i);

#ifdef MK_PKG_IN_MAC
	/* Here I need to put dest/src addres of packet*/
	/* Set high 16 bit part of dst adr */
	val=(u_int32_t) *( (u_int16_t* )&(*(pkg_data+4)));
	_outl(REG_DST_ADDR_H,val);
	printk(KERN_ERR "dst_h=0x%08x\n",val);

	/* Set low 32 bit part of dst adr */
        val= (u_int32_t) *( (u_int32_t* )&(*(pkg_data+0)));
        _outl(REG_DST_ADDR_L,val);
	printk(KERN_ERR "dst_l=0x%08x\n",val);

       /* Set high 16 bit part of dst adr */
        val=(u_int32_t) *( (u_int16_t* )&(*(pkg_data+10)));
        _outl(REG_SRC_ADDR_H,val);
	printk(KERN_ERR "src_h=0x%08x\n",val);

        /* Set low 32 bit part of dst adr */
        val= (u_int32_t) *( (u_int32_t* )&(*(pkg_data+6)));
        _outl(REG_SRC_ADDR_L,val);
	printk(KERN_ERR "src_l=0x%08x\n",val);

	/* Set the type/length of packet*/
	/*
	The multicore has litle endian (intel endian)
	that is why it's need to change the orde of bytes in regs
	*/
	val= ((*( (u_int8_t* )&(*(pkg_data+12))) )<<8)|
 			*( (u_int8_t* )&(*(pkg_data+13)));

	printk(KERN_ERR "type=0x%08x\n",val);
	_outl(REG_TYPE_FRAME,val);
	/* control read register REG_TX_FRM_CTRL */
//        val=_inl(REG_TYPE_FRAME);
//        printk(KERN_ERR "read (REG_TX_FRM_CTR)=0x%08x\n",val);

	/* Set the length of LOAD of packet */
	priv->load_len_bytes=(skb->len) - ETH_PKG_HEADER_LEN;
        val=_inl(REG_TX_FRM_CTRL);
	val= (val & (~TX_FRM_CTRL_LENGTH_MASK)) |
	  (TX_FRM_CTRL_LENGTH_MASK & ( priv->load_len_bytes  ) ) ;
        _outl(REG_TX_FRM_CTRL,val);

        printk(KERN_ERR "REG_TX_FRM_CTR:wr 0x%08x\n",val);
//        val=_inl(REG_TX_FRM_CTRL);
//        printk(KERN_ERR "read (REG_TX_FRM_CTR)=0x%08x\n",val);


	/*Load the load of packet to MAC controller */

	/* adjust the lengt of buffer in long*/
	priv->load_len64=(priv->load_len_bytes+7)>>3;

       	printk(KERN_ERR "load_len_in_long=%d\n",priv->load_len64);
//	printk(KERN_ERR "sizeof(long)=%d\n",sizeof(u_int64_t));

	/*Copy the load of packet to `64 byte` aligned buffer*/
	/* Here need to correct the 8 bytes boundary aligment*/
	memcpy((unsigned char *)priv->virt_addr_tx,
		(pkg_data + ETH_PKG_HEADER_LEN), priv->load_len_bytes );

#else

	priv->load_len_bytes=skb->len;
        val=_inl(REG_TX_FRM_CTRL);
	val= (val & (~TX_FRM_CTRL_LENGTH_MASK)) |
	  (TX_FRM_CTRL_LENGTH_MASK & ( priv->load_len_bytes  ) ) ;
        _outl(REG_TX_FRM_CTRL,val);
        printk(KERN_ERR "REG_TX_FRM_CTR:wr 0x%08x\n",val);

	priv->load_len64=(priv->load_len_bytes+7)>>3;
       	printk(KERN_ERR "load_len_in_long=%d\n",priv->load_len64);

	/*Copy the load of packet to `64 byte` aligned buffer*/
	/* Here need to correct the 8 bytes boundary aligment*/
	memcpy((unsigned char *)priv->virt_addr_tx,
		(pkg_data ), priv->load_len_bytes );

#endif/* MK_PKG_IN_MAC */

	/*Copy the load of packet to controller tx buffer by DMA */
	/* TO DO Check the DMA status before programm DMA*/
	/*Set DMA modes */
	/* Set DMA source */

	val=priv->dma_addr_tx;
	printk (KERN_ERR "REG_MAC_IR_DMA_1_TX:wr 0x%08x\n",val);
	_outl(REG_MAC_IR_DMA_1_TX,val);

	/*Set the length for DMA */
	val= ( (priv->load_len64-1) << CSR_DMA_WCX_SHFT) & CSR_DMA_WCX_MASK;

        printk(KERN_ERR "REG_MAC_CSR_DMA_1_TX:wr 0x%08x\n",val);
	_outl(REG_MAC_CSR_DMA_1_TX,val);

	/*Control read */
//	val=0;
//	val=_inl(REG_MAC_CSR_DMA_1_TX);
//	printk(KERN_ERR "REG_MAC_CSR_DMA_1_TX before DMA start=0x%08x\n",val);





	printk(KERN_ERR "TX: Start DMA\n");
	/* Run the DMA*/
	val=_inl(REG_MAC_CSR_DMA_1_TX);
	val=val|CMD_DMA_RUN;
	printk(KERN_ERR "TX: Set DMA run:0x%08x\n ",val);
       _outl(REG_MAC_CSR_DMA_1_TX,val);

//	_outl(REG_MAC_RUN_DMA_1_TX , CMD_DMA_RUN);

	while ( _inl (REG_MAC_RUN_DMA_1_TX)& CMD_DMA_RUN )
	{
		;
		printk(KERN_ERR "TX: DMA running 0x%08x\n",_inl (REG_MAC_RUN_DMA_1_TX) );
	}
	printk (KERN_ERR "TX: DMA finished\n");
	val=_inl( REG_MAC_CSR_DMA_1_TX );
        printk(KERN_ERR "REG_MAC_CSR_DMA_1_TX after DMA stop=0x%08x\n",val);

	/* Send the packet */
	printk(KERN_ERR "MAC status TX =0x%08x\n",_inl( REG_STATUS_TX ));

	val=_inl(REG_TX_FRM_CTRL);

        printk(KERN_ERR "REG_TX_FRM_CTRL:r 0x%08x\n",val);

	val=val| CMD_MAC_SEND_PKG;

        printk(KERN_ERR "REG_TX_FRM_CTRL:w 0x%08x\n",val);

	_outl(REG_TX_FRM_CTRL, val);

	val=_inl(REG_TX_FRM_CTRL);
       printk(KERN_ERR "control read (REG_TX_FRM_CTRL)=0x%08x\n",val);

       printk(KERN_ERR "MAC status TX =0x%08x\n",_inl( REG_STATUS_TX ));

	/*Here  */



	/* Update statistic*/



        /* Update transmit stats */
        priv->stats.tx_bytes += skb->len;



        /* Unlock priv */
        spin_unlock_irqrestore(&priv->txlock, flags);

	return 0;
}




static int nvcom_eth_probe(struct platform_device *pdev)
{
        struct net_device *dev = NULL;
        struct nvcom_eth_private *priv = NULL;
        struct nvcom_eth_platform_data *einfo;
        int err = 0;
	int idx;

printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

/* Get data about device from platform device data*/
        einfo = (struct nvcom_eth_platform_data *) pdev->dev.platform_data;

        if (NULL == einfo) {
                printk(KERN_ERR "nvcom_eth %d: Missing additional data!\n", pdev->id);

                return -ENODEV;
        }


        dev = alloc_etherdev(sizeof (*priv));

        if (NULL == dev)
                return -ENOMEM;


        priv = netdev_priv(dev);

/* Set the info in the priv to the current info */
        priv->einfo = einfo;

//Get platform info and fill private data

        spin_lock_init(&priv->txlock);
        spin_lock_init(&priv->rxlock);
        platform_set_drvdata(pdev, dev);

/* Copy the MAC address into the dev structure, */
        memcpy(dev->dev_addr, einfo->mac_addr, MAC_ADDR_LEN);

        SET_MODULE_OWNER(dev);
        SET_NETDEV_DEV(dev, &pdev->dev);
        /* Fill in the dev structure */
	dev->open	=nvcom_eth_open;
	dev->stop	=nvcom_eth_close;
	dev->hard_start_xmit = nvcom_start_xmit;
//	dev->tx_timeout = nvcom_timeout;


	/* Fill in the fields of the device structure
	with Ethernet-generic values.*/
        ether_setup(dev);

        err = register_netdev(dev);

        if (err) {
                printk(KERN_ERR "%s: Cannot register net device, aborting.\n",  dev->name);

                goto register_fail;
        }

        /* Print out the device info */
        printk(KERN_INFO DEVICE_NAME, dev->name);
        for (idx = 0; idx < 6; idx++)
                printk("%2.2x%c", dev->dev_addr[idx], idx == 5 ? ' ' : ':');
        printk("\n");

//printk(KERN_INFO "%s: start \n", dev->name);

	return 0;

register_fail:
        free_netdev(dev);
        return err;

}


//struct sk_buff






irqreturn_t nvcom_receive(int irq, void *dev_id)
{
        struct net_device *dev = (struct net_device *) dev_id;
        struct nvcom_eth_private *priv = netdev_priv(dev);

	struct sk_buff *skb;

	int i;
	u_int32_t val;
	u_int32_t num_frame=0 ;
	u_int32_t* buffer_rx_status;
	u_int32_t frame_rxw_64;
	u_int32_t rx_frame_len;
	u_int32_t rx_frame_len_64;

	printk(KERN_ERR "RX IRQ\n");
//Let check the status register to discover the cause of interrupt
	val=_inl(REG_STATUS_RX);
	num_frame=(val>>STATUS_RX_NUM_FR_SHFT)&STATUS_RX_NUM_FR_MASK;
	frame_rxw_64=(val>>STATUS_RXW_SHFT )&STATUS_RXW_MASK;

	printk(KERN_ERR "RX: Status=0x%08x\n",val );

	printk(KERN_ERR "RX: %d frames in fifo allocated %d 64bit words\n ",num_frame,	frame_rxw_64 );

	if( ((val & STATUS_RX_DONE_BIT)==0) && (num_frame==0))
	{
		printk(KERN_ERR "There is no frame in RX fifo\n");
	}
	else
	{

//Let get SATUS RX values from fifo via CPU

	buffer_rx_status=priv->rx_status_buf;

	for(i=0;i<num_frame;i++)
	{
		val=_inl(REG_RX_FRAME_STATUS_FIFO);
		rx_frame_len=val & RX_FRAME_STATUS_LEN_MASK ;
		rx_frame_len_64=(rx_frame_len+7)>>3;


	printk(KERN_ERR "RX: from fifo rx_status=0x%08x\n",val );
		*(buffer_rx_status++)=val;
	printk(KERN_ERR "RX: frame_len_byte=%d; len_64=%d\n",rx_frame_len,rx_frame_len_64);

/* Is frame received OK ?*/
	if ( (RX_FRAME_STATUS_OK_BIT & val) != 0)
	{
/* Let get the load of packet via dma */
/* TO DO Check the DMA status before programm DMA*/
/*Set DMA modes */
/* Set DMA destination */

	val=priv->dma_addr_rx;
	printk (KERN_ERR "REG_MAC_IR_DMA_0_RX:wr 0x%08x\n",val);
	_outl(REG_MAC_IR_DMA_0_RX,val);

	/*Set the length for DMA */
	val= ( 	(rx_frame_len_64-1) << CSR_DMA_WCX_SHFT) & CSR_DMA_WCX_MASK;

        printk(KERN_ERR "REG_MAC_CSR_DMA_0_RX:wr 0x%08x\n",val);
	_outl(REG_MAC_CSR_DMA_0_RX,val);

	/*Control read */
//	val=0;
//	val=_inl(REG_MAC_CSR_DMA_0_RX);
//	printk(KERN_ERR "REG_MAC_CSR_DMA_0_RX before DMA start=0x%08x\n",val);

	printk(KERN_ERR "RX: Start DMA\n");
	/* Run the DMA*/
	val=_inl(REG_MAC_CSR_DMA_0_RX);
	val=val|CMD_DMA_RUN;
	printk(KERN_ERR "RX: Set DMA run:0x%08x\n ",val);
       _outl(REG_MAC_CSR_DMA_0_RX,val);

//	_outl(REG_MAC_RUN_DMA_1_TX , CMD_DMA_RUN);

	while ( _inl (REG_MAC_RUN_DMA_0_RX)& CMD_DMA_RUN )
	{
		;
		printk(KERN_ERR "RX: DMA running 0x%08x\n",_inl (REG_MAC_RUN_DMA_0_RX) );
	}
	printk (KERN_ERR "RX: DMA finished\n");
	val=_inl( REG_MAC_CSR_DMA_0_RX );
        printk(KERN_ERR "RX: REG_MAC_CSR_DMA_0_RX after DMA stop=0x%08x\n",val);

/* Dump the RX frame */
        printk(KERN_ERR "Dump RX buf len=0x%x (%d)\n",rx_frame_len,rx_frame_len ) ;

	nvcom_dump_frame((unsigned char*)priv->virt_addr_rx,rx_frame_len );


	}
//???? +2 ??????
	skb=dev_alloc_skb(rx_frame_len+2);
	if(!skb)
	{
		printk(KERN_ERR "RX: nvcom low on mem - packet dropped\n");
//TO DO:  add to statictics dropped packet counter
		continue;
//		goto out;
	}

	memcpy(skb_put(skb,rx_frame_len),priv->virt_addr_rx,rx_frame_len );
	skb->dev=dev;
	skb->protocol = eth_type_trans(skb,dev);
//	skb->ipsummed
//TO DO: add statistic received packet and length
	netif_rx(skb);


	} /*For()  */

/*Check number of rx_status records in fifo rx_status after read */
//	val=_inl(REG_STATUS_RX);
//	num_frame=(val>>STATUS_RX_NUM_FR_SHFT)&STATUS_RX_NUM_FR_MASK;
//	printk(KERN_ERR "RX: %d frames in fifo after read\n ",num_frame );
//	buffer_rx_status=priv->rx_status_buf;
//	for(i=0;i<num_frame;i++)
//		printk(KERN_ERR "RX: rx_status=0x%08x\n",*(buffer_rx_status++));

	}

//	_outl(REG_MAC_CTRL, _inl(REG_MAC_CTRL) & (~(1 << MAC_CTRL_RX_DONE)));
	// (setc (devbit dev :mac-ctrl :ctrl-rx-done) nil)

//	_outl(REG_MAC_CTRL, _inl(REG_MAC_CTRL) & (~(1 << MAC_CTRL_RX_DONE)));

	return IRQ_HANDLED;

}






static int nvcom_eth_remove(struct platform_device *pdev)
{
        struct net_device *dev = platform_get_drvdata(pdev);
//        struct nvcom_eth_private *priv = netdev_priv(dev);

        platform_set_drvdata(pdev, NULL);

        free_netdev(dev);

        return 0;
}


static struct platform_driver nvcom_eth_driver = {
        .probe = nvcom_eth_probe,
        .remove = nvcom_eth_remove,
        .driver = {
		    .name = "nvcom-eth",
        },
};


static int __init nvcom_eth_init(void)
{
       int err;
printk(KERN_ERR "#### %s:%d\n",__FUNCTION__,__LINE__);

	err = nvcom_mdio_init();

        if (err)
                return err;

        err = platform_driver_register(&nvcom_eth_driver);

//printk(KERN_ERR "#### platform_driver_register(&nvcom_eth_driver)=%d\n",err);
        if (err)
                nvcom_mdio_exit();

        return err;
}


static void __exit nvcom_eth_exit(void)
{
        platform_driver_unregister(&nvcom_eth_driver);
        nvcom_mdio_exit();
}


module_init(nvcom_eth_init);

module_exit(nvcom_eth_exit);
