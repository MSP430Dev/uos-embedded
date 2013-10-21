/*
 * Intel LXT971A PHY transceiver registers.
 *
 * Copyright (C) 2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
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
#define PHY_CTL                 0       /* Control Register */
#define PHY_STS                 1       /* Status Register #1 */
#define PHY_ID1                 2       /* PHY Identifier 1 */
#define PHY_ID2                 3       /* PHY Identifier 2 */
#define PHY_ADVRT               4       /* Auto-Negotiation Advertisement */
#define PHY_RMT_ADVRT           5       /* Auto-Negotiation Link Partner Ability */
#define PHY_ANE                 6       /* Auto-Negotiation Expansion */
#define PHY_ANNP                7       /* Auto-Negotiation Next Page */
#define PHY_LPNPA               8       /* Link Partner Next Page Ability */
#define PHY_PCR                 16      /* Port Configuration Register */
#define PHY_STS2                17      /* Status Register #2 */
#define PHY_IER                 18      /* Interrupt Enable Register */
#define PHY_ISR                 19      /* Interrupt Status Register */
#define PHY_LED                 20      /* LED Configuration Register */
#define PHY_DCR                 26      /* Digital Config Register */
#define PHY_TCR                 30      /* Transmit Control Register */

/*
 * Control Register
 */
#define PHY_CTL_COLTST          0x0080  /* collision test enable */
#define PHY_CTL_DPLX            0x0100  /* full duplex */
#define PHY_CTL_ANEG_RST        0x0200  /* write 1 to restart autoneg */
#define PHY_CTL_MII_DIS         0x0400  /* MII interface disable */
#define PHY_CTL_PDN             0x0800  /* powerdown enable */
#define PHY_CTL_ANEG_EN         0x1000  /* auto-negotiation enable */
#define PHY_CTL_LPBK            0x4000  /* loopback enable */
#define PHY_CTL_RST             0x8000  /* reset, bit self cleared */
#define PHY_CTL_SPEED_10        0x0000  /* select 10 Mbps speed */
#define PHY_CTL_SPEED_100       0x2000  /* select 100 Mbps speed */
#define PHY_CTL_SPEED_1000      0x0040  /* select 1000 Mbps speed */
#define PHY_CTL_SPEED_MASK      0x2040  /* speed mask */

/*
 * Status register
 */
#define PHY_STS_EXREG           0x0001  /* extended regs enabled */
#define PHY_STS_JAB             0x0002  /* jabber detected */
#define PHY_STS_LINK            0x0004  /* link valid */
#define PHY_STS_CAP_ANEG        0x0008  /* auto-negotiation available */
#define PHY_STS_REM_FLT         0x0010  /* remote fault detected */
#define PHY_STS_ANEG_ACK        0x0020  /* auto-negotiation acknowledge */
#define PHY_STS_CAP_SUPR        0x0040  /* MII preamble suppression capable */
#define PHY_STS_EXT_STAT        0x0100  /* Extended status in register 15 */
#define PHY_STS_CAP_T2_HDX      0x0200  /* can do 100BASE-T2 half duplex */
#define PHY_STS_CAP_T2_FDX      0x0200  /* can do 100BASE-T2 full duplex */
#define PHY_STS_CAP_10_HDX      0x0800  /* can do 10Base-TX half duplex */
#define PHY_STS_CAP_10_FDX      0x1000  /* can do 10Base-TX full duplex */
#define PHY_STS_CAP_100_HDX     0x2000  /* can do 100Base-TX half duplex */
#define PHY_STS_CAP_100_FDX     0x4000  /* can do 100Base-TX full duplex */
#define PHY_STS_CAP_100_T4      0x8000  /* can do 100Base-T4 */

/*
 * ID1-ID2 registers
 */
#define PHY_ID_MASK             0xfffffff0
#define PHY_ID_LXT971A          0x001378e0

/*
 * Auto negotiation advertisement,
 * Auto negotiation remote end capability
 */
#define PHY_ADVRT_CSMA          0x0001  /* capable of 802.3 operation */
#define PHY_ADVRT_802_9         0x0002  /* capable of 802.9 ISLAN-16T operation */
#define PHY_ADVRT_10_HDX        0x0020  /* can do 10Base-TX half duplex */
#define PHY_ADVRT_10_FDX        0x0040  /* can do 10Base-TX full duplex */
#define PHY_ADVRT_100_HDX       0x0080  /* can do 100Base-TX half duplex */
#define PHY_ADVRT_100_FDX       0x0100  /* can do 100Base-TX full duplex */
#define PHY_ADVRT_100_T4        0x0200  /* can do 100Base-T4 */
#define PHY_ADVRT_PAUSE         0x0400  /* pause function supported */
#define PHY_ADVRT_ASYM_PAUSE    0x0800  /* pause operation defined in Clause 40 and 27 */
#define PHY_ADVRT_RF            0x2000  /* remote fault */
#define PHY_ADVRT_NP            0x8000  /* next page exists */

/*
 * Configuration Register
 */
#define PHY_PCR_FIBER           0x0001  /* Fiber mode, else TP mode */
#define PHY_PCR_ALT_NP          0x0002  /* Enable alternate auto negotiate NP */
#define PHY_PCR_FLT_CODE        0x0004  /* Enable FEFI transmission */
#define PHY_PCR_SLEEP_3_04      0x0000  /* Sleep 3.04 seconds */
#define PHY_PCR_SLEEP_2_00      0x0008  /* Sleep 2.00 seconds */
#define PHY_PCR_SLEEP_1_04      0x0010  /* Sleep 1.04 seconds */
#define PHY_PCR_PRE_EN          0x0020  /* Preamble enable */
#define PHY_PCR_SLEEP_MODE      0x0040  /* Enable Sleep Mode */
#define PHY_PCR_CRS             0x0080  /* CRS deassert extends to RX_DV deassert */
#define PHY_PCR_TP_LPBK_DIS     0x0100  /* Disable TP loopback during half-duplex */
#define PHY_PCR_SQE             0x0200  /* Enable Heart Beat */
#define PHY_PCR_JABBER_DIS      0x0400  /* Disable Jabber Correction */
#define PHY_PCR_SCR_DIS         0x1000  /* Bypass Scrambler and Descrambler */
#define PHY_PCR_TX_DIS          0x2000  /* Disable twisted pair transmitter */
#define PHY_PCR_FORCE_LP        0x4000  /* Force Link pass */       

/*
 * Status Register #2
 */
#define PHY_STS2_ERR            0x0008  /* Error Occurred (Remote Fault, X, Y, Z) */
#define PHY_STS2_PAUSE          0x0010  /* Device Pause capable */
#define PHY_STS2_INV_POL        0x0020  /* Polarity is reversed */
#define PHY_STS2_ANEG_DONE      0x0080  /* Auto-negotiation complete */
#define PHY_STS2_ANEG           0x0100  /* LXT971A is in auto-negotiation */
#define PHY_STS2_FDX            0x0200  /* Full-duplex mode */
#define PHY_STS2_LINK           0x0400  /* Link is up */
#define PHY_STS2_COL            0x0800  /* Collision is occuring */
#define PHY_STS2_RX             0x1000  /* Receiving a packet */
#define PHY_STS2_TX             0x2000  /* Transmitting a packet */
#define PHY_STS2_100BASE_TX     0x4000  /* LXT971A is operating in 100BASE-TX mode */
