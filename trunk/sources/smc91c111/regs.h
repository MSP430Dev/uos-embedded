/*
 * SMC91C111 Control Registers.
 *
 * Ported to uOS by Serge Vakulenko (c) 2010.
 * Based on Linux driver sources from Erik Stahlman (c) 1996,
 * and Daris A Nevil (c) 2001.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Registers are organized in 4 banks.
 * Most registers are 16 bit wide.
 */
#define SMC_REG_BASE	0xA8000000		/* Using /CS1 */
#define SMC_DATA_BASE	0xAA000000		/* When A25=1 */

#define SMC_REG32(x)	(*(volatile unsigned long*)  (SMC_REG_BASE + x))
#define SMC_REG16(x)	(*(volatile unsigned short*) (SMC_REG_BASE + x))
#define SMC_REG8(x)	(*(volatile unsigned char*)  (SMC_REG_BASE + x))
#define SMC_DATA32	(*(volatile unsigned long*)  SMC_DATA_BASE)
#define SMC_DATA16	(*(volatile unsigned short*) SMC_DATA_BASE)

/* All-bank register */
#define BS_REG		SMC_REG16 (0xE)		/* Bank select register */

/* Bank 0 registers */
#define	TCR_REG 	SMC_REG16 (0x0)		/* Transmit control */
#define EPH_STATUS_REG	SMC_REG16 (0x2)		/* Transmit status, read only */
#define	RCR_REG		SMC_REG16 (0x4)		/* Receive control */
#define	COUNTER_REG	SMC_REG16 (0x6)		/* Counters, read only */
#define	MIR_REG		SMC_REG16 (0x8)		/* Memory information, read only */
#define	RPC_REG		SMC_REG16 (0xA)		/* Receive/PHY control */

/* Bank 1 registers */
#define CONFIG_REG	SMC_REG16 (0x0)		/* Configuration */
#define	BASE_REG	SMC_REG16 (0x2)		/* Base address */
#define	ADDR0_REG	SMC_REG16 (0x4)		/* Individual... */
#define	ADDR1_REG	SMC_REG16 (0x6)		/* ... */
#define	ADDR2_REG	SMC_REG16 (0x8)		/* ...address */
#define	GP_REG		SMC_REG16 (0xA)		/* General purpose */
#define	CTL_REG		SMC_REG16 (0xC)		/* Control */

/* Bank 2 registers */
#define MMU_CMD_REG	SMC_REG16 (0x0)		/* MMU command */
#define	PN_REG		SMC_REG8 (0x2)		/* Packet number */
#define	AR_REG		SMC_REG8 (0x2 + 1)	/* Allocation result */
#define FIFO_REG	SMC_REG16 (0x4)		/* FIFO ports, read only */
#define PTR_REG		SMC_REG16 (0x6)		/* Pointer */
#define	DATA16_REG	SMC_REG16 (0x8)		/* Data, 16-bit wide */
#define	DATA_REG	SMC_REG32 (0x8)		/* Data, 32-bit wide */
#define	INT_REG		SMC_REG8 (0xC)		/* Interrupt status/acknowledge */
#define IM_REG		SMC_REG8 (0xC + 1)	/* Interrupt mask */

/* Bank 3 registers */
#define	MCASTL_REG	SMC_REG32 (0x0)		/* Multicast... */
#define	MCASTH_REG	SMC_REG32 (0x4)		/* ... */
#define	MII_REG		SMC_REG16 (0x8)		/* Management interface */
#define	REV_REG		SMC_REG16 (0xA)		/* Revision */
#define	ERCV_REG	SMC_REG16 (0xC)		/* Early receive */

/*
 * Transmit Control Register
 */
#define TCR_ENABLE		0x0001	/* When 1 we can transmit */
#define TCR_LOOP		0x0002	/* Controls output pin LBK */
#define TCR_FORCOL		0x0004	/* When 1 will force a collision */
#define TCR_PAD_EN		0x0080	/* When 1 will pad tx frames < 64 bytes w/0 */
#define TCR_NOCRC		0x0100	/* When 1 will not append CRC to tx frames */
#define TCR_MON_CSN		0x0400	/* When 1 tx monitors carrier */
#define TCR_FDUPLX		0x0800	/* When 1 enables full duplex operation */
#define TCR_STP_SQET		0x1000	/* When 1 stops tx if Signal Quality Error */
#define	TCR_EPH_LOOP		0x2000	/* When 1 enables EPH block loopback */
#define	TCR_SWFDUP		0x8000	/* When 1 enables Switched Full Duplex mode */

/*
 * EPH Status Register
 */
#define ES_TX_SUC		0x0001	/* Last TX was successful */
#define ES_SNGL_COL		0x0002	/* Single collision detected for last tx */
#define ES_MUL_COL		0x0004	/* Multiple collisions detected for last tx */
#define ES_LTX_MULT		0x0008	/* Last tx was a multicast */
#define ES_16COL		0x0010	/* 16 Collisions Reached */
#define ES_SQET			0x0020	/* Signal Quality Error Test */
#define ES_LTXBRD		0x0040	/* Last tx was a broadcast */
#define ES_TXDEFR		0x0080	/* Transmit Deferred */
#define ES_LATCOL		0x0200	/* Late collision detected on last tx */
#define ES_LOSTCARR		0x0400	/* Lost Carrier Sense */
#define ES_EXC_DEF		0x0800	/* Excessive Deferral */
#define ES_CTR_ROL		0x1000	/* Counter Roll Over indication */
#define ES_LINK_OK		0x4000	/* Driven by inverted value of nLNK pin */
#define ES_TXUNRN		0x8000	/* Tx Underrun */

/*
 * Receive Control Register
 */
#define	RCR_RX_ABORT		0x0001	/* Set if a rx frame was aborted */
#define	RCR_PRMS		0x0002	/* Enable promiscuous mode */
#define	RCR_ALMUL		0x0004	/* When set accepts all multicast frames */
#define RCR_RXEN		0x0100	/* IFF this is set, we can receive packets */
#define	RCR_STRIP_CRC		0x0200	/* When set strips CRC from rx packets */
#define	RCR_ABORT_ENB		0x0200	/* When set will abort rx on collision */
#define	RCR_FILT_CAR		0x0400	/* When set filters leading 12 bit s of carrier */
#define RCR_SOFTRST		0x8000	/* resets the chip */

/*
 * Receive/Phy Control Register
 */
#define	RPC_SPEED		0x2000	/* When 1 PHY is in 100Mbps mode. */
#define	RPC_DPLX		0x1000	/* When 1 PHY is in Full-Duplex Mode */
#define	RPC_ANEG		0x0800	/* When 1 PHY is in Auto-Negotiate Mode */
#define	RPC_LSXA_SHFT		5	/* Bits to shift LS2A,LS1A,LS0A to lsb */
#define	RPC_LSXB_SHFT		2	/* Bits to get LS2B,LS1B,LS0B to lsb */
#define RPC_LED_100_10		(0x00)	/* LED = 100Mbps OR's with 10Mbps link detect */
#define RPC_LED_RES		(0x01)	/* LED = Reserved */
#define RPC_LED_10		(0x02)	/* LED = 10Mbps link detect */
#define RPC_LED_FD		(0x03)	/* LED = Full Duplex Mode */
#define RPC_LED_TX_RX		(0x04)	/* LED = TX or RX packet occurred */
#define RPC_LED_100		(0x05)	/* LED = 100Mbps link dectect */
#define RPC_LED_TX		(0x06)	/* LED = TX packet occurred */
#define RPC_LED_RX		(0x07)	/* LED = RX packet occurred */

/*
 * Configuration Register
 */
#define CONFIG_EXT_PHY		0x0200	/* 1=external MII, 0=internal Phy */
#define CONFIG_GPCNTRL		0x0400	/* Inverse value drives pin nCNTRL */
#define CONFIG_NO_WAIT		0x1000	/* When 1 no extra wait states on ISA bus */
#define CONFIG_EPH_POWER_EN	0x8000	/* When 0 EPH is placed into low power mode */

/*
 * Control Register
 */
#define CTL_RCV_BAD		0x4000	/* When 1 bad CRC packets are received */
#define CTL_AUTO_RELEASE	0x0800	/* When 1 tx pages are released automatically */
#define	CTL_LE_ENABLE		0x0080	/* When 1 enables Link Error interrupt */
#define	CTL_CR_ENABLE		0x0040	/* When 1 enables Counter Rollover interrupt */
#define	CTL_TE_ENABLE		0x0020	/* When 1 enables Transmit Error interrupt */
#define	CTL_EEPROM_SELECT	0x0004	/* Controls EEPROM reload & store */
#define	CTL_RELOAD		0x0002	/* When set reads EEPROM into registers */
#define	CTL_STORE		0x0001	/* When set stores registers into EEPROM */

/*
 * MMU Command Register
 */
#define MC_BUSY			1	/* When 1 the last release has not completed */
#define MC_NOP			(0<<5)	/* No Op */
#define	MC_ALLOC		(1<<5)	/* OR with number of 256 byte packets */
#define	MC_RESET		(2<<5)	/* Reset MMU to initial state */
#define	MC_REMOVE		(3<<5)	/* Remove the current rx packet */
#define MC_RELEASE		(4<<5)	/* Remove and release the current rx packet */
#define MC_FREEPKT		(5<<5)	/* Release packet in PNR register */
#define MC_ENQUEUE		(6<<5)	/* Enqueue the packet for transmit */
#define MC_RSTTXFIFO		(7<<5)	/* Reset the TX FIFOs */

/*
 * Allocation Result Register
 */
#define AR_FAILED		0x80	/* Allocation Failed */

/*
 * FIFO Ports Register
 */
#define FIFO_REMPTY		0x8000	/* RX FIFO Empty */
#define FIFO_TEMPTY		0x0080	/* TX FIFO Empty */

/*
 * Pointer Register
 */
#define	PTR_RCV			0x8000	/* 1=Receive area, 0=Transmit area */
#define	PTR_AUTOINC		0x4000	/* Auto increment the pointer on each access */
#define PTR_READ		0x2000	/* When 1 the operation is a read */

/*
 * Interrupt Mask Register
 */
#define	IM_MDINT		0x80	/* PHY MI Register 18 Interrupt */
#define	IM_ERCV_INT		0x40	/* Early Receive Interrupt */
#define	IM_EPH_INT		0x20	/* Set by Etheret Protocol Handler section */
#define	IM_RX_OVRN_INT		0x10	/* Set by Receiver Overruns */
#define	IM_ALLOC_INT		0x08	/* Set when allocation request is completed */
#define	IM_TX_EMPTY_INT		0x04	/* Set if the TX FIFO goes empty */
#define	IM_TX_INT		0x02	/* Transmit Interrrupt */
#define IM_RCV_INT		0x01	/* Receive Interrupt */

/*
 * Management Interface Register (MII)
 */
#define MII_MSK_CRS100		0x4000	/* Disables CRS100 detection during tx half dup */
#define MII_MDOE		0x0008	/* MII Output Enable */
#define MII_MCLK		0x0004	/* MII Clock, pin MDCLK */
#define MII_MDI			0x0002	/* MII Input, pin MDI */
#define MII_MDO			0x0001	/* MII Output, pin MDO */

/*
 * Early RCV Register
 */
#define ERCV_RCV_DISCRD		0x0080	/* When 1 discards a packet being received */
#define ERCV_THRESHOLD		0x001F	/* ERCV Threshold Mask */


/*
 * Transmit status bits
 */
#define TS_SUCCESS		0x0001
#define TS_LOSTCAR		0x0400
#define TS_LATCOL		0x0200
#define TS_16COL		0x0010

/*
 * Receive status bits
 */
#define RS_ALGNERR		0x8000
#define RS_BRODCAST		0x4000
#define RS_BADCRC		0x2000
#define RS_ODDFRAME		0x1000	/* bug: the LAN91C111 never sets this on receive */
#define RS_TOOLONG		0x0800
#define RS_TOOSHORT		0x0400
#define RS_MULTICAST		0x0001

/*
 * PHYY Control Register (aka MII_MBCR)
 */
#define PHY_CNTL_REG		0x00
#define PHY_CNTL_RST		0x8000	/* 1=PHY Reset */
#define PHY_CNTL_LPBK		0x4000	/* 1=PHY Loopback */
#define PHY_CNTL_SPEED		0x2000	/* 1=100Mbps, 0=10Mpbs */
#define PHY_CNTL_ANEG_EN	0x1000	/* 1=Enable Auto negotiation */
#define PHY_CNTL_PDN		0x0800	/* 1=PHY Power Down mode */
#define PHY_CNTL_MII_DIS	0x0400	/* 1=MII 4 bit interface disabled */
#define PHY_CNTL_ANEG_RST	0x0200	/* 1=Reset Auto negotiate */
#define PHY_CNTL_DPLX		0x0100	/* 1=Full Duplex, 0=Half Duplex */
#define PHY_CNTL_COLTST		0x0080	/* 1= MII Colision Test */

/*
 * PHY Status Register (aka MII_BMSR)
 */
#define PHY_STAT_REG		0x01
#define PHY_STAT_CAP_T4		0x8000	/* 1=100Base-T4 capable */
#define PHY_STAT_CAP_TXF	0x4000	/* 1=100Base-X full duplex capable */
#define PHY_STAT_CAP_TXH	0x2000	/* 1=100Base-X half duplex capable */
#define PHY_STAT_CAP_TF		0x1000	/* 1=10Mbps full duplex capable */
#define PHY_STAT_CAP_TH		0x0800	/* 1=10Mbps half duplex capable */
#define PHY_STAT_CAP_SUPR	0x0040	/* 1=recv mgmt frames with not preamble */
#define PHY_STAT_ANEG_ACK	0x0020	/* 1=ANEG has completed */
#define PHY_STAT_REM_FLT	0x0010	/* 1=Remote Fault detected */
#define PHY_STAT_CAP_ANEG	0x0008	/* 1=Auto negotiate capable */
#define PHY_STAT_LINK		0x0004	/* 1=valid link */
#define PHY_STAT_JAB		0x0002	/* 1=10Mbps jabber condition */
#define PHY_STAT_EXREG		0x0001	/* 1=extended registers implemented */

/*
 * PHY Identifier Registers
 */
#define PHY_ID1_REG		0x02	/* PHY Identifier 1 */
#define PHY_ID2_REG		0x03	/* PHY Identifier 2 */

/*
 * PHY Auto-Negotiation Advertisement Register
 */
#define PHY_AD_REG		0x04
#define PHY_AD_NP		0x8000	/* 1=PHY requests exchange of Next Page */
#define PHY_AD_ACK		0x4000	/* 1=got link code u_int16_t from remote */
#define PHY_AD_RF		0x2000	/* 1=advertise remote fault */
#define PHY_AD_T4		0x0200	/* 1=PHY is capable of 100Base-T4 */
#define PHY_AD_TX_FDX		0x0100	/* 1=PHY is capable of 100Base-TX FDPLX */
#define PHY_AD_TX_HDX		0x0080	/* 1=PHY is capable of 100Base-TX HDPLX */
#define PHY_AD_10_FDX		0x0040	/* 1=PHY is capable of 10Base-T FDPLX */
#define PHY_AD_10_HDX		0x0020	/* 1=PHY is capable of 10Base-T HDPLX */
#define PHY_AD_CSMA		0x0001	/* 1=PHY is capable of 802.3 CMSA */

/*
 * PHY Auto-negotiation Remote End Capability Register
 */
#define PHY_RMT_REG		0x05	/* Uses same bit definitions as PHY_AD_REG */

/*
 * PHY Configuration Register 1
 */
#define PHY_CFG1_REG		0x10
#define PHY_CFG1_LNKDIS		0x8000	/* 1=Rx Link Detect Function disabled */
#define PHY_CFG1_XMTDIS		0x4000	/* 1=TP Transmitter Disabled */
#define PHY_CFG1_XMTPDN		0x2000	/* 1=TP Transmitter Powered Down */
#define PHY_CFG1_BYPSCR		0x0400	/* 1=Bypass scrambler/descrambler */
#define PHY_CFG1_UNSCDS		0x0200	/* 1=Unscramble Idle Reception Disable */
#define PHY_CFG1_EQLZR		0x0100	/* 1=Rx Equalizer Disabled */
#define PHY_CFG1_CABLE		0x0080	/* 1=STP(150ohm), 0=UTP(100ohm) */
#define PHY_CFG1_RLVL0		0x0040	/* 1=Rx Squelch level reduced by 4.5db */
#define PHY_CFG1_TLVL_SHIFT	2	/* Transmit Output Level Adjust */
#define PHY_CFG1_TLVL_MASK	0x003C
#define PHY_CFG1_TRF_MASK	0x0003	/* Transmitter Rise/Fall time */

/*
 * PHY Configuration Register 2
 */
#define PHY_CFG2_REG		0x11
#define PHY_CFG2_APOLDIS	0x0020	/* 1=Auto Polarity Correction disabled */
#define PHY_CFG2_JABDIS		0x0010	/* 1=Jabber disabled */
#define PHY_CFG2_MREG		0x0008	/* 1=Multiple register access (MII mgt) */
#define PHY_CFG2_INTMDIO	0x0004	/* 1=Interrupt signaled with MDIO pulseo */

/*
 * PHY Status Output (and Interrupt status) Register
 */
#define PHY_INT_REG		0x12	/* Status Output (Interrupt Status) */
#define PHY_INT_INT		0x8000	/* 1=bits have changed since last read */
#define	PHY_INT_LNKFAIL		0x4000	/* 1=Link Not detected */
#define PHY_INT_LOSSSYNC	0x2000	/* 1=Descrambler has lost sync */
#define PHY_INT_CWRD		0x1000	/* 1=Invalid 4B5B code detected on rx */
#define PHY_INT_SSD		0x0800	/* 1=No Start Of Stream detected on rx */
#define PHY_INT_ESD		0x0400	/* 1=No End Of Stream detected on rx */
#define PHY_INT_RPOL		0x0200	/* 1=Reverse Polarity detected */
#define PHY_INT_JAB		0x0100	/* 1=Jabber detected */
#define PHY_INT_SPDDET		0x0080	/* 1=100Base-TX mode, 0=10Base-T mode */
#define PHY_INT_DPLXDET		0x0040	/* 1=Device in Full Duplex */

/*
 * PHY Interrupt/Status Mask Register
 */
#define PHY_MASK_REG		0x13	/* Interrupt Mask */
