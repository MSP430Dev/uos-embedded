/*
 * Micrel KS8721BL PHY transceiver registers.
 *
 * Copyright (C) 2010 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#define PHY_CTL                 0	/* Basic Control */
#define PHY_STS                 1	/* Basic Status */
#define PHY_ID1                 2	/* PHY Identifier I */
#define PHY_ID2                 3	/* PHY Identifier II */
#define PHY_ADVRT               4	/* Auto-Negotiation Advertisement */
#define PHY_RMT_ADVRT           5	/* Auto-Negotiation Link Partner Ability */
#define PHY_ANE			6	/* Auto-Negotiation Expansion */
#define PHY_ANNP		7	/* Auto-Negotiation Next Page */
#define PHY_LPNPA		8	/* Link Partner Next Page Ability */
#define PHY_RXERC		0x15	/* RXER Counter */
#define PHY_ICS			0x1b	/* Interrupt Control/Status */
#define PHY_EXTCTL		0x1f	/* 100BASE-TX PHY Control */

/*
 * Control Register
 */
#define PHY_CTL_DISTX		0x0001	/* disable transmitter */
#define PHY_CTL_COLTST		0x0080	/* collision test enable */
#define PHY_CTL_DPLX		0x0100	/* full duplex */
#define PHY_CTL_ANEG_RST	0x0200	/* write 1 to restart autoneg */
#define PHY_CTL_MII_DIS		0x0400	/* MII interface disable */
#define PHY_CTL_PDN		0x0800	/* powerdown enable */
#define PHY_CTL_ANEG_EN		0x1000	/* auto-negotiation enable */
#define PHY_CTL_SPEED_100	0x2000	/* select 100 Mbps speed */
#define PHY_CTL_LPBK		0x4000	/* loopback enable */
#define PHY_CTL_RST		0x8000	/* reset, bit self cleared */

#define PHY_CTL_BITS "\20"\
"\1distx\10coltst"\
"\11dplx\12aneg-rst\13mii-dis\14pdn\15aneg-en\16speed100\17lpbk\20rst"

/*
 * Status register
 */
#define PHY_STS_EXREG		0x0001	/* extended regs enabled */
#define PHY_STS_JAB		0x0002	/* jabber detected */
#define PHY_STS_LINK		0x0004	/* link valid */
#define PHY_STS_CAP_ANEG	0x0008	/* auto-negotiation available */
#define PHY_STS_REM_FLT		0x0010	/* remote fault detected */
#define PHY_STS_ANEG_ACK	0x0020	/* auto-negotiation acknowledge */
#define PHY_STS_CAP_SUPR	0x0040	/* MII preamble suppression capable */
#define PHY_STS_CAP_10_HDX	0x0800	/* can do 10Base-TX half duplex */
#define PHY_STS_CAP_10_FDX	0x1000	/* can do 10Base-TX full duplex */
#define PHY_STS_CAP_100_HDX	0x2000	/* can do 100Base-TX half duplex */
#define PHY_STS_CAP_100_FDX	0x4000	/* can do 100Base-TX full duplex */
#define PHY_STS_CAP_100_T4	0x8000	/* can do 100Base-T4 */

#define PHY_STS_BITS "\20"\
"\1exreg\2jab\3link\4cap-aneg\5rem-flt\6aneg-ack\7cap-supr"\
"\14hdx10\15fdx10\16hdx100\17fdx100\20t4-100"

/*
 * ID1-ID2 registers
 */
#define PHY_ID_MASK		0xfffffff0
#define PHY_ID_KS8721BL		0x00221610

/*
 * Auto negotiation advertisement,
 * Auto negotiation remote end capability
 */
#define PHY_ADVRT_CSMA		0x0001	/* capable of 802.3 CSMA operation */
#define PHY_ADVRT_10_HDX	0x0020	/* can do 10Base-TX half duplex */
#define PHY_ADVRT_10_FDX	0x0040	/* can do 10Base-TX full duplex */
#define PHY_ADVRT_100_HDX	0x0080	/* can do 100Base-TX half duplex */
#define PHY_ADVRT_100_FDX	0x0100	/* can do 100Base-TX full duplex */
#define PHY_ADVRT_100_T4	0x0200	/* can do 100Base-T4 */
#define PHY_ADVRT_PAUSE		0x0400	/* pause function supported */
#define PHY_ADVRT_RF		0x2000	/* remote fault */
#define PHY_ADVRT_NP		0x8000	/* next page exists */

#define PHY_ADVRT_BITS "\20"\
"\1csma\6hdx10\7fdx10\10hdx100"\
"\11fdx100\12t4-100\13pause\16rf\20np"

/*
 * 100Base-TX PHY Control Register
 */
#define PHY_EXTCTL_DISSCR	0x0001	/* disable scrambler */
#define PHY_EXTCTL_ENSQET	0x0002	/* enable SQE test */
#define PHY_EXTCTL_MODE_MASK	0x001C	/* operation mode */
#define PHY_EXTCTL_MODE_ANEG	0x0000	/* still in auto-negotiation */
#define PHY_EXTCTL_MODE_10_HDX	0x0004	/* 10Base-T half duplex */
#define PHY_EXTCTL_MODE_100_HDX	0x0008	/* 100Base-TX half duplex */
#define PHY_EXTCTL_MODE_10_FDX	0x0014	/* 10Base-T full duplex */
#define PHY_EXTCTL_MODE_100_FDX	0x0018	/* 100Base-TX full duplex */
#define PHY_EXTCTL_MODE_ISOLATE	0x001C	/* isolate */
#define PHY_EXTCTL_ISOLATE	0x0020	/* isolate mode */
#define PHY_EXTCTL_PAUSE	0x0040	/* enable pause */
#define PHY_EXTCTL_ANEG_DONE	0x0080	/* auto-negotiation complete */
#define PHY_EXTCTL_JABBER	0x0100	/* enable jabber counter */
#define PHY_EXTCTL_IRQHIGH	0x0200	/* 1 - irq pin active high */
#define PHY_EXTCTL_POWERSAVE	0x0400	/* enable power saving */
#define PHY_EXTCTL_FORCELINK	0x0800	/* enable TX even if no link */
#define PHY_EXTCTL_RXDETECT	0x1000	/* RX signal detected */
#define PHY_EXTCTL_DISPSWAP	0x2000	/* disable pair swap */

#define PHY_EXTCTL_BITS "\20"\
"\1disscr\2ensqet\00310\004100\5fdx\6isolate\7pause\10aneg"\
"\11jabber\12irqhigh\13pwrsv\14force\15rxdet\16dpswap"
