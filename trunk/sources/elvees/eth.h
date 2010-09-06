/*
 * Ethernet controller driver for ELVEES MULTICORE NVCom
 * Writed by Ildar F Kaibyshev skif@elvees.com
 * Copyright (c) 2010 Elvees
 */

#ifndef __NVCOM_ETH_H
#define __NVCOM_ETH_H

#define MAC_ADDR_LEN 6

#define DEVICE_NAME "%s: NVCom Ethernet Controller Version 1.0, "

/* The size (2048 bytes) in 64 bytes of TX buffer for dma use */
#define TX_BUF_SIZE_64		256
#define TX_BUF_BYTE_SIZE	2048
#define TX_ALIGN_ADD		8
#define ETH_PKG_HEADER_LEN	14

/* The size of RXframe buffer fifo is 64 x 32 bit */
#define RX_STATUS_BUF_SIZE_32	64
#define RX_STATUS_BUF_BYTE_SIZE 64*4

/* The size of RX FIFO is 512 x 64 bit */
#define RX_BUF_SIZE_64		512
#define RX_BUF_BYTE_SIZE	4096
#define RX_ALIGN_ADD		8

#define NV_PHY_INIT_TIMEOUT	1000000

struct nvcom_eth_platform_data
{
        u32     board_id;
	u32	bus_id;
	u32	phy_id;
	phy_interface_t interface;
	u8	mac_addr[6];
};

struct nvcom_mdio_platform_data
{
        u32     board_id;
        u32     phy_id;
	int	irq[32];
};

struct nvcom_eth_private {

	/* Fields controlled by TX lock */
	spinlock_t txlock;

	/* Pointer to the array of skbuffs */
	struct sk_buff ** tx_skbuff;

	/* TX Phisical addres for DMA TX buffer */
	dma_addr_t dma_addr_tx;

	/* TX Virtual addres of DMA TX buffer */
	u_int32_t virt_addr_tx;


	/* RX_STATUS buffer */
	u_int32_t* rx_status_buf;

	/* RX Phisical addres for DMA RX buffer */
	dma_addr_t dma_addr_rx;

	/* Virtual addres of DMA RX buffer */
	u_int32_t virt_addr_rx;


	/* RX Locked fields */
	spinlock_t rxlock;

	/* info structure initialized by platform code */
	struct nvcom_eth_platform_data *einfo;

	/* PHY stuff */
	struct mii_if_info mii_if;
	struct phy_device *phydev;

	/* Network Statistics */
	struct net_device_stats stats;

	/* the buffer to TX aligned to 64 bytes boundary for dma use */
//	u_int64_t *tx_b_64;

	u_int32_t load_len_bytes;
	u_int32_t load_len64;
};

extern irqreturn_t nvcom_receive (int irq, void *dev_id);

int nvcom_mdio_read (struct mii_bus *bus, int mii_id, int regnum);
int nvcom_mdio_write (struct mii_bus *bus, int mii_id, int regnum, u16 value);
int __init nvcom_mdio_init (void);
void __exit nvcom_mdio_exit (void);

#endif /* __NVCOM_ETH_H */
