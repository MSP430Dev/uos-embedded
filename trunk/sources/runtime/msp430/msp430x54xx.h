#if !defined(__msp430x54xx)
#define __msp430x54xx

/* msp430x54xx.h
 *
 * mspgcc project: MSP430 device headers
 * MSP430x54xx family header
 *
 * (c) 2006 by Steve Underwood <steveu@coppice.org>
 * Originally based in part on work by Texas Instruments Inc.
 *
 * 2008-10-08 - sb-sf (sb-sf@users.sf.net)
 * - created, based on msp430xG461x.h
 *
 * $Id: msp430x54xx.h,v 1.5 2009/06/04 21:55:18 cliechti Exp $
 */

#include <runtime/msp430/iomacros.h>

#define __MSP430_HAS_UCS__
#define __MSP430_HAS_USCI__
#define __MSP430_HAS_USCI_AB0__
#define __MSP430_HAS_USCI_AB1__
#define __MSP430_HAS_USCI_AB2__
#define __MSP430_HAS_USCI_AB3__
#define __MSP430_HAS_T0A5__
#define __MSP430_HAS_T0B7__
#define __MSP430_HAS_T1A3__
#define __MSP430_PMM_BASE__	0x120
#define __MSP430_FLASH_BASE__   0x140
#define __MSP430_WDT_A_BASE__   0x150
#define __MSP430_SYS_BASE__     0x180
#define __MSP430_PORT1_BASE__   0x200
#define __MSP430_PORT2_BASE__   0x200
#define __MSP430_PORT3_BASE__   0x220
#define __MSP430_PORT4_BASE__   0x220
#define __MSP430_PORT5_BASE__   0x240
#define __MSP430_PORT6_BASE__   0x240
#define __MSP430_PORT7_BASE__   0x260
#define __MSP430_PORT8_BASE__   0x260
#define __MSP430_PORT9_BASE__   0x280
#define __MSP430_PORT10_BASE__  0x280
#define __MSP430_PORT11_BASE__  0x2A0
#define __MSP430_PORTJ_BASE__   0x320

#define __MSP430_TA0_BASE__	0x340
#define __MSP430_TA1_BASE__	0x380
#define __MSP430_TB0_BASE__	0x3C0

#define __MSP430_MPY32_BASE__   0x4C0
#define __MSP430_DMA_BASE__     0x500
#define __MSP430_DMA0_BASE__    0x510
#define __MSP430_DMA1_BASE__    0x520
#define __MSP430_DMA2_BASE__    0x530
#define __MSP430_USCI0_BASE__	0x5C0
#define __MSP430_USCI1_BASE__	0x600
#define __MSP430_USCI2_BASE__	0x640
#define __MSP430_USCI3_BASE__	0x680

#include <runtime/msp430/wdt_a.h>
#include <runtime/msp430/sys.h>
#include <runtime/msp430/gpio_5xxx.h>
#include <runtime/msp430/mpy32.h>
#include <runtime/msp430/timera.h>
#include <runtime/msp430/timerb.h>
#include <runtime/msp430/unified_clock_system.h>
#include <runtime/msp430/usci.h>
#include <runtime/msp430/dma.h>
#include <runtime/msp430/flash.h>

#include <runtime/msp430/common.h>


#define GIE                 0x0008

#define SFRIE1_             0x0100  /* Interrupt Enable 1 */
#define SFRIE1_L_           SFRIE1_
#define SFRIE1_H_           SFRIE1_ + 0x01
sfrw(SFRIE1, SFRIE1_);
sfrb(SFRIE1_L, SFRIE1_L_);
sfrb(IE1, SFRIE1_L_);
sfrb(SFRIE1_H, SFRIE1_H_);
sfrb(IE2, SFRIE1_H_);
#define WDTIE               (1<<0)
#define OFIE                (1<<1)
/* RESERVED                 (1<<2)*/
#define VMAIE               (1<<3)
#define NMIIE               (1<<4)
#define ACCVIE              (1<<5)
#define JMBINIE             (1<<6)
#define JMBOUTIE            (1<<7)

#define SFRIFG1_            0x0102  /* Interrupt Flag 1 */
#define SFRIFG1_L_          SFRIFG1_
#define SFRIFG1_H_          SFRIFG1_ + 0x01
sfrw(SFRIFG1, SFRIFG1_);
sfrb(SFRIFG1_L, SFRIFG1_L_);
sfrb(IFG1, SFRIFG1_L_);
sfrb(SFRIFG1_H, SFRIFG1_H_);
sfrb(IFG2, SFRIFG1_H_);
#define WDTIFG              (1<<0)
#define OFIFG               (1<<1)
/* RESERVED                 (1<<2)*/
#define VMAIFG              (1<<3)
#define NMIIFG              (1<<4)
/* RESERVED                 (1<<5)*/
#define JMBINIFG            (1<<6)
#define JMBOUTIFG           (1<<7)

#define SFRRPCR_            0x0104  /* Reset pin control */
sfrw(SFRRPCR, SFRRPCR_);
#define SFRRPCR_L_          SFRRPCR_
#define SFRRPCR_H_          SFRRPCR_ + 0x01
sfrb(SFRRPCR_L, SFRRPCR_L_);
sfrb(SFRRPCR_H, SFRRPCR_H_);
#define SYSNMI              (1<<0)  /* RST/NMI pin (0:Reset, 1: NMI) */
#define SYSNMIIES           (1<<1)  /* NMI edge select (0:rising edge). Can trigger NMI */
#define SYSRSTUP            (1<<2)  /* Reset resistor pin pullup (0: pulldown, 1: pullup) */
#define SYSRSTRE            (1<<3)  /* Reset pin resistor enable (0: disabled, 1: enabled) */

/*
 * Universal Serial Communication Interface
 */
sfrb (UCA0CTL1,	 __MSP430_USCI0_BASE__+0x00);	/* USCI A0 control 1 */
sfrb (UCA0CTL0,	 __MSP430_USCI0_BASE__+0x01);	/* USCI A0 control 0 */
sfrb (UCA0BR0,	 __MSP430_USCI0_BASE__+0x06);	/* USCI A0 baud rate 0 */
sfrb (UCA0BR1,	 __MSP430_USCI0_BASE__+0x07);	/* USCI A0 baud rate 1 */
sfrb (UCA0MCTL,	 __MSP430_USCI0_BASE__+0x08);	/* USCI A0 modulation control */
sfrb (UCA0STAT,	 __MSP430_USCI0_BASE__+0x0A);	/* USCI A0 status */
sfrb (UCA0RXBUF, __MSP430_USCI0_BASE__+0x0C);	/* USCI A0 receive buffer */
sfrb (UCA0TXBUF, __MSP430_USCI0_BASE__+0x0E);	/* USCI A0 transmit buffer */
sfrb (UCA0ABCTL, __MSP430_USCI0_BASE__+0x10);	/* USCI A0 LIN control */
sfrb (UCA0IRTCTL,__MSP430_USCI0_BASE__+0x12);	/* USCI A0 IrDA transmit control */
sfrb (UCA0IRRCTL,__MSP430_USCI0_BASE__+0x13);	/* USCI A0 IrDA receive control */
sfrb (UCA0IE,	 __MSP430_USCI0_BASE__+0x1C);	/* USCI A0 interrupt enable */
sfrb (UCA0IFG,	 __MSP430_USCI0_BASE__+0x1D);	/* USCI A0 interrupt flags */
sfrw (UCA0IV,	 __MSP430_USCI0_BASE__+0x1E);	/* USCI A0 interrupt vector word */

sfrb (UCB0CTL1,	 __MSP430_USCI0_BASE__+0x20);	/* USCI B0 synchronous control 1 */
sfrb (UCB0CTL0,	 __MSP430_USCI0_BASE__+0x21);	/* USCI B0 synchronous control 0 */
sfrb (UCB0BR0,	 __MSP430_USCI0_BASE__+0x26);	/* USCI B0 synchronous bit rate 0 */
sfrb (UCB0BR1,	 __MSP430_USCI0_BASE__+0x27);	/* USCI B0 synchronous bit rate 1 */
sfrb (UCB0I2CIE, __MSP430_USCI0_BASE__+0x28);	/* USCI B0 I2C interrupt enable */
sfrb (UCB0STAT,	 __MSP430_USCI0_BASE__+0x2A);	/* USCI B0 synchronous status */
sfrb (UCB0RXBUF, __MSP430_USCI0_BASE__+0x2C);	/* USCI B0 synchronous receive buffer */
sfrb (UCB0TXBUF, __MSP430_USCI0_BASE__+0x2E);	/* USCI B0 synchronous transmit buffer */
sfrb (UCB0I2COA, __MSP430_USCI0_BASE__+0x30);	/* USCI B0 I2C own address */
sfrb (UCB0I2CSA, __MSP430_USCI0_BASE__+0x32);	/* USCI B0 I2C slave address */
sfrb (UCB0IE,	 __MSP430_USCI0_BASE__+0x3C);	/* USCI B0 interrupt enable */
sfrb (UCB0IFG,	 __MSP430_USCI0_BASE__+0x3D);	/* USCI B0 interrupt flags */
sfrw (UCB0IV,	 __MSP430_USCI0_BASE__+0x3E);	/* USCI B0 interrupt vector word */

sfrb (UCA1CTL1,	 __MSP430_USCI1_BASE__+0x00);	/* USCI A1 control 1 */
sfrb (UCA1CTL0,	 __MSP430_USCI1_BASE__+0x01);	/* USCI A1 control 0 */
sfrb (UCA1BR0,	 __MSP430_USCI1_BASE__+0x06);	/* USCI A1 baud rate 0 */
sfrb (UCA1BR1,	 __MSP430_USCI1_BASE__+0x07);	/* USCI A1 baud rate 1 */
sfrb (UCA1MCTL,	 __MSP430_USCI1_BASE__+0x08);	/* USCI A1 modulation control */
sfrb (UCA1STAT,	 __MSP430_USCI1_BASE__+0x0A);	/* USCI A1 status */
sfrb (UCA1RXBUF, __MSP430_USCI1_BASE__+0x0C);	/* USCI A1 receive buffer */
sfrb (UCA1TXBUF, __MSP430_USCI1_BASE__+0x0E);	/* USCI A1 transmit buffer */
sfrb (UCA1ABCTL, __MSP430_USCI1_BASE__+0x10);	/* USCI A1 LIN control */
sfrb (UCA1IRTCTL,__MSP430_USCI1_BASE__+0x12);	/* USCI A1 IrDA transmit control */
sfrb (UCA1IRRCTL,__MSP430_USCI1_BASE__+0x13);	/* USCI A1 IrDA receive control */
sfrb (UCA1IE,	 __MSP430_USCI1_BASE__+0x1C);	/* USCI A1 interrupt enable */
sfrb (UCA1IFG,	 __MSP430_USCI1_BASE__+0x1D);	/* USCI A1 interrupt flags */
sfrw (UCA1IV,	 __MSP430_USCI1_BASE__+0x1E);	/* USCI A1 interrupt vector word */

sfrb (UCB1CTL1,	 __MSP430_USCI1_BASE__+0x20);	/* USCI B1 synchronous control 1 */
sfrb (UCB1CTL0,	 __MSP430_USCI1_BASE__+0x21);	/* USCI B1 synchronous control 0 */
sfrb (UCB1BR0,	 __MSP430_USCI1_BASE__+0x26);	/* USCI B1 synchronous bit rate 0 */
sfrb (UCB1BR1,	 __MSP430_USCI1_BASE__+0x27);	/* USCI B1 synchronous bit rate 1 */
sfrb (UCB1I2CIE, __MSP430_USCI1_BASE__+0x28);	/* USCI B1 I2C interrupt enable */
sfrb (UCB1STAT,	 __MSP430_USCI1_BASE__+0x2A);	/* USCI B1 synchronous status */
sfrb (UCB1RXBUF, __MSP430_USCI1_BASE__+0x2C);	/* USCI B1 synchronous receive buffer */
sfrb (UCB1TXBUF, __MSP430_USCI1_BASE__+0x2E);	/* USCI B1 synchronous transmit buffer */
sfrb (UCB1I2COA, __MSP430_USCI1_BASE__+0x30);	/* USCI B1 I2C own address */
sfrb (UCB1I2CSA, __MSP430_USCI1_BASE__+0x32);	/* USCI B1 I2C slave address */
sfrb (UCB1IE,	 __MSP430_USCI1_BASE__+0x3C);	/* USCI B1 interrupt enable */
sfrb (UCB1IFG,	 __MSP430_USCI1_BASE__+0x3D);	/* USCI B1 interrupt flags */
sfrw (UCB1IV,	 __MSP430_USCI1_BASE__+0x3E);	/* USCI B1 interrupt vector word */

sfrb (UCA2CTL1,	 __MSP430_USCI2_BASE__+0x00);	/* USCI A2 control 1 */
sfrb (UCA2CTL0,	 __MSP430_USCI2_BASE__+0x01);	/* USCI A2 control 0 */
sfrb (UCA2BR0,	 __MSP430_USCI2_BASE__+0x06);	/* USCI A2 baud rate 0 */
sfrb (UCA2BR1,	 __MSP430_USCI2_BASE__+0x07);	/* USCI A2 baud rate 1 */
sfrb (UCA2MCTL,	 __MSP430_USCI2_BASE__+0x08);	/* USCI A2 modulation control */
sfrb (UCA2STAT,	 __MSP430_USCI2_BASE__+0x0A);	/* USCI A2 status */
sfrb (UCA2RXBUF, __MSP430_USCI2_BASE__+0x0C);	/* USCI A2 receive buffer */
sfrb (UCA2TXBUF, __MSP430_USCI2_BASE__+0x0E);	/* USCI A2 transmit buffer */
sfrb (UCA2ABCTL, __MSP430_USCI2_BASE__+0x10);	/* USCI A2 LIN control */
sfrb (UCA2IRTCTL,__MSP430_USCI2_BASE__+0x12);	/* USCI A2 IrDA transmit control */
sfrb (UCA2IRRCTL,__MSP430_USCI2_BASE__+0x13);	/* USCI A2 IrDA receive control */
sfrb (UCA2IE,	 __MSP430_USCI2_BASE__+0x1C);	/* USCI A2 interrupt enable */
sfrb (UCA2IFG,	 __MSP430_USCI2_BASE__+0x1D);	/* USCI A2 interrupt flags */
sfrw (UCA2IV,	 __MSP430_USCI2_BASE__+0x1E);	/* USCI A2 interrupt vector word */

sfrb (UCB2CTL1,	 __MSP430_USCI2_BASE__+0x20);	/* USCI B2 synchronous control 1 */
sfrb (UCB2CTL0,	 __MSP430_USCI2_BASE__+0x21);	/* USCI B2 synchronous control 0 */
sfrb (UCB2BR0,	 __MSP430_USCI2_BASE__+0x26);	/* USCI B2 synchronous bit rate 0 */
sfrb (UCB2BR1,	 __MSP430_USCI2_BASE__+0x27);	/* USCI B2 synchronous bit rate 1 */
sfrb (UCB2I2CIE, __MSP430_USCI2_BASE__+0x28);	/* USCI B2 I2C interrupt enable */
sfrb (UCB2STAT,	 __MSP430_USCI2_BASE__+0x2A);	/* USCI B2 synchronous status */
sfrb (UCB2RXBUF, __MSP430_USCI2_BASE__+0x2C);	/* USCI B2 synchronous receive buffer */
sfrb (UCB2TXBUF, __MSP430_USCI2_BASE__+0x2E);	/* USCI B2 synchronous transmit buffer */
sfrb (UCB2I2COA, __MSP430_USCI2_BASE__+0x30);	/* USCI B2 I2C own address */
sfrb (UCB2I2CSA, __MSP430_USCI2_BASE__+0x32);	/* USCI B2 I2C slave address */
sfrb (UCB2IE,	 __MSP430_USCI2_BASE__+0x3C);	/* USCI B2 interrupt enable */
sfrb (UCB2IFG,	 __MSP430_USCI2_BASE__+0x3D);	/* USCI B2 interrupt flags */
sfrw (UCB2IV,	 __MSP430_USCI2_BASE__+0x3E);	/* USCI B2 interrupt vector word */

sfrb (UCA3CTL1,	 __MSP430_USCI3_BASE__+0x00);	/* USCI A3 control 1 */
sfrb (UCA3CTL0,	 __MSP430_USCI3_BASE__+0x01);	/* USCI A3 control 0 */
sfrb (UCA3BR0,	 __MSP430_USCI3_BASE__+0x06);	/* USCI A3 baud rate 0 */
sfrb (UCA3BR1,	 __MSP430_USCI3_BASE__+0x07);	/* USCI A3 baud rate 1 */
sfrb (UCA3MCTL,	 __MSP430_USCI3_BASE__+0x08);	/* USCI A3 modulation control */
sfrb (UCA3STAT,	 __MSP430_USCI3_BASE__+0x0A);	/* USCI A3 status */
sfrb (UCA3RXBUF, __MSP430_USCI3_BASE__+0x0C);	/* USCI A3 receive buffer */
sfrb (UCA3TXBUF, __MSP430_USCI3_BASE__+0x0E);	/* USCI A3 transmit buffer */
sfrb (UCA3ABCTL, __MSP430_USCI3_BASE__+0x10);	/* USCI A3 LIN control */
sfrb (UCA3IRTCTL,__MSP430_USCI3_BASE__+0x12);	/* USCI A3 IrDA transmit control */
sfrb (UCA3IRRCTL,__MSP430_USCI3_BASE__+0x13);	/* USCI A3 IrDA receive control */
sfrb (UCA3IE,	 __MSP430_USCI3_BASE__+0x1C);	/* USCI A3 interrupt enable */
sfrb (UCA3IFG,	 __MSP430_USCI3_BASE__+0x1D);	/* USCI A3 interrupt flags */
sfrw (UCA3IV,	 __MSP430_USCI3_BASE__+0x1E);	/* USCI A3 interrupt vector word */

sfrb (UCB3CTL1,	 __MSP430_USCI3_BASE__+0x20);	/* USCI B3 synchronous control 1 */
sfrb (UCB3CTL0,	 __MSP430_USCI3_BASE__+0x21);	/* USCI B3 synchronous control 0 */
sfrb (UCB3BR0,	 __MSP430_USCI3_BASE__+0x26);	/* USCI B3 synchronous bit rate 0 */
sfrb (UCB3BR1,	 __MSP430_USCI3_BASE__+0x27);	/* USCI B3 synchronous bit rate 1 */
sfrb (UCB3I2CIE, __MSP430_USCI3_BASE__+0x28);	/* USCI B3 I2C interrupt enable */
sfrb (UCB3STAT,	 __MSP430_USCI3_BASE__+0x2A);	/* USCI B3 synchronous status */
sfrb (UCB3RXBUF, __MSP430_USCI3_BASE__+0x2C);	/* USCI B3 synchronous receive buffer */
sfrb (UCB3TXBUF, __MSP430_USCI3_BASE__+0x2E);	/* USCI B3 synchronous transmit buffer */
sfrb (UCB3I2COA, __MSP430_USCI3_BASE__+0x30);	/* USCI B3 I2C own address */
sfrb (UCB3I2CSA, __MSP430_USCI3_BASE__+0x32);	/* USCI B3 I2C slave address */
sfrb (UCB3IE,	 __MSP430_USCI3_BASE__+0x3C);	/* USCI B3 interrupt enable */
sfrb (UCB3IFG,	 __MSP430_USCI3_BASE__+0x3D);	/* USCI B3 interrupt flags */
sfrw (UCB3IV,	 __MSP430_USCI3_BASE__+0x3E);	/* USCI B3 interrupt vector word */

#define UCRXIFG			(1<<0)
#define UCTXIFG			(1<<1)

/* UCAxIE Control Bits */
#define UCRXIE			(1<<0)	/* USCI Receive Interrupt Enable */
#define UCTXIE			(1<<1)	/* USCI Transmit Interrupt Enable */

/*
 * DMA controller
 */
sfrw (DMACTL0,	__MSP430_DMA_BASE__+0x00);	/* DMA module control 0 */
sfrw (DMACTL1,	__MSP430_DMA_BASE__+0x02);	/* DMA module control 1 */
sfrw (DMACTL2,	__MSP430_DMA_BASE__+0x04);	/* DMA module control 2 */
sfrw (DMACTL3,	__MSP430_DMA_BASE__+0x06);	/* DMA module control 3 */
sfrw (DMACTL4,	__MSP430_DMA_BASE__+0x08);	/* DMA module control 4 */
sfrw (DMAIV,	__MSP430_DMA_BASE__+0x0E);	/* DMA interrupt vector */

sfrw (DMA0CTL,	__MSP430_DMA0_BASE__+0x00);	/* DMA channel 0 control */
sfrw (DMA0SAL,	__MSP430_DMA0_BASE__+0x02);	/* DMA channel 0 source address low */
sfrw (DMA0SAH,	__MSP430_DMA0_BASE__+0x04);	/* DMA channel 0 source address high */
sfrw (DMA0DAL,	__MSP430_DMA0_BASE__+0x06);	/* DMA channel 0 destination address low */
sfrw (DMA0DAH,	__MSP430_DMA0_BASE__+0x08);	/* DMA channel 0 destination address high */
sfrw (DMA0SZ,	__MSP430_DMA0_BASE__+0x0A);	/* DMA channel 0 transfer size */

sfrw (DMA1CTL,	__MSP430_DMA1_BASE__+0x00);	/* DMA channel 1 control */
sfrw (DMA1SAL,	__MSP430_DMA1_BASE__+0x02);	/* DMA channel 1 source address low */
sfrw (DMA1SAH,	__MSP430_DMA1_BASE__+0x04);	/* DMA channel 1 source address high */
sfrw (DMA1DAL,	__MSP430_DMA1_BASE__+0x06);	/* DMA channel 1 destination address low */
sfrw (DMA1DAH,	__MSP430_DMA1_BASE__+0x08);	/* DMA channel 1 destination address high */
sfrw (DMA1SZ,	__MSP430_DMA1_BASE__+0x0A);	/* DMA channel 1 transfer size */

sfrw (DMA2CTL,	__MSP430_DMA2_BASE__+0x00);	/* DMA channel 2 control */
sfrw (DMA2SAL,	__MSP430_DMA2_BASE__+0x02);	/* DMA channel 2 source address low */
sfrw (DMA2SAH,	__MSP430_DMA2_BASE__+0x04);	/* DMA channel 2 source address high */
sfrw (DMA2DAL,	__MSP430_DMA2_BASE__+0x06);	/* DMA channel 2 destination address low */
sfrw (DMA2DAH,	__MSP430_DMA2_BASE__+0x08);	/* DMA channel 2 destination address high */
sfrw (DMA2SZ,	__MSP430_DMA2_BASE__+0x0A);	/* DMA channel 2 transfer size */

#define DMA0TSEL__DMA_REQ      (0*0x0001u)    /* DMA channel 0 transfer select 0:  DMA_REQ (sw) */
#define DMA0TSEL__TA0CCR0      (1*0x0001u)    /* DMA channel 0 transfer select 1:  Timer0_A (TA0CCR0.IFG) */
#define DMA0TSEL__TA0CCR2      (2*0x0001u)    /* DMA channel 0 transfer select 2:  Timer0_A (TA0CCR2.IFG) */
#define DMA0TSEL__TA1CCR0      (3*0x0001u)    /* DMA channel 0 transfer select 3:  Timer1_A (TA1CCR0.IFG) */
#define DMA0TSEL__TA1CCR2      (4*0x0001u)    /* DMA channel 0 transfer select 4:  Timer1_A (TA1CCR2.IFG) */
#define DMA0TSEL__TB0CCR0      (5*0x0001u)    /* DMA channel 0 transfer select 5:  TimerB (TB0CCR0.IFG) */
#define DMA0TSEL__TB0CCR2      (6*0x0001u)    /* DMA channel 0 transfer select 6:  TimerB (TB0CCR2.IFG) */
#define DMA0TSEL__RES7         (7*0x0001u)    /* DMA channel 0 transfer select 7:  Reserved */
#define DMA0TSEL__RES8         (8*0x0001u)    /* DMA channel 0 transfer select 8:  Reserved */
#define DMA0TSEL__RES9         (9*0x0001u)    /* DMA channel 0 transfer select 9:  Reserved */
#define DMA0TSEL__RES10        (10*0x0001u)   /* DMA channel 0 transfer select 10: Reserved */
#define DMA0TSEL__RES11        (11*0x0001u)   /* DMA channel 0 transfer select 11: Reserved */
#define DMA0TSEL__RES12        (12*0x0001u)   /* DMA channel 0 transfer select 12: Reserved */
#define DMA0TSEL__RES13        (13*0x0001u)   /* DMA channel 0 transfer select 13: Reserved */
#define DMA0TSEL__RES14        (14*0x0001u)   /* DMA channel 0 transfer select 14: Reserved */
#define DMA0TSEL__RES15        (15*0x0001u)   /* DMA channel 0 transfer select 15: Reserved */
#define DMA0TSEL__USCIA0RX     (16*0x0001u)   /* DMA channel 0 transfer select 16: USCIA0 receive */
#define DMA0TSEL__USCIA0TX     (17*0x0001u)   /* DMA channel 0 transfer select 17: USCIA0 transmit */
#define DMA0TSEL__USCIB0RX     (18*0x0001u)   /* DMA channel 0 transfer select 18: USCIB0 receive */
#define DMA0TSEL__USCIB0TX     (19*0x0001u)   /* DMA channel 0 transfer select 19: USCIB0 transmit */
#define DMA0TSEL__USCIA1RX     (20*0x0001u)   /* DMA channel 0 transfer select 20: USCIA1 receive */
#define DMA0TSEL__USCIA1TX     (21*0x0001u)   /* DMA channel 0 transfer select 21: USCIA1 transmit */
#define DMA0TSEL__USCIB1RX     (22*0x0001u)   /* DMA channel 0 transfer select 22: USCIB1 receive */
#define DMA0TSEL__USCIB1TX     (23*0x0001u)   /* DMA channel 0 transfer select 23: USCIB1 transmit */
#define DMA0TSEL__ADC12IFG     (24*0x0001u)   /* DMA channel 0 transfer select 24: ADC12IFGx */
#define DMA0TSEL__RES25        (25*0x0001u)   /* DMA channel 0 transfer select 25: Reserved */
#define DMA0TSEL__RES26        (26*0x0001u)   /* DMA channel 0 transfer select 26: Reserved */
#define DMA0TSEL__RES27        (27*0x0001u)   /* DMA channel 0 transfer select 27: Reserved */
#define DMA0TSEL__RES28        (28*0x0001u)   /* DMA channel 0 transfer select 28: Reserved */
#define DMA0TSEL__MPY          (29*0x0001u)   /* DMA channel 0 transfer select 29: Multiplier ready */
#define DMA0TSEL__DMA2IFG      (30*0x0001u)   /* DMA channel 0 transfer select 30: previous DMA channel DMA2IFG */
#define DMA0TSEL__DMAE0        (31*0x0001u)   /* DMA channel 0 transfer select 31: ext. Trigger (DMAE0) */

#define DMA1TSEL__DMA_REQ      (0*0x0100u)    /* DMA channel 1 transfer select 0:  DMA_REQ (sw) */
#define DMA1TSEL__TA0CCR0      (1*0x0100u)    /* DMA channel 1 transfer select 1:  Timer0_A (TA0CCR0.IFG) */
#define DMA1TSEL__TA0CCR2      (2*0x0100u)    /* DMA channel 1 transfer select 2:  Timer0_A (TA0CCR2.IFG) */
#define DMA1TSEL__TA1CCR0      (3*0x0100u)    /* DMA channel 1 transfer select 3:  Timer1_A (TA1CCR0.IFG) */
#define DMA1TSEL__TA1CCR2      (4*0x0100u)    /* DMA channel 1 transfer select 4:  Timer1_A (TA1CCR2.IFG) */
#define DMA1TSEL__TB0CCR0      (5*0x0100u)    /* DMA channel 1 transfer select 5:  TimerB (TB0CCR0.IFG) */
#define DMA1TSEL__TB0CCR2      (6*0x0100u)    /* DMA channel 1 transfer select 6:  TimerB (TB0CCR2.IFG) */
#define DMA1TSEL__RES7         (7*0x0100u)    /* DMA channel 1 transfer select 7:  Reserved */
#define DMA1TSEL__RES8         (8*0x0100u)    /* DMA channel 1 transfer select 8:  Reserved */
#define DMA1TSEL__RES9         (9*0x0100u)    /* DMA channel 1 transfer select 9:  Reserved */
#define DMA1TSEL__RES10        (10*0x0100u)   /* DMA channel 1 transfer select 10: Reserved */
#define DMA1TSEL__RES11        (11*0x0100u)   /* DMA channel 1 transfer select 11: Reserved */
#define DMA1TSEL__RES12        (12*0x0100u)   /* DMA channel 1 transfer select 12: Reserved */
#define DMA1TSEL__RES13        (13*0x0100u)   /* DMA channel 1 transfer select 13: Reserved */
#define DMA1TSEL__RES14        (14*0x0100u)   /* DMA channel 1 transfer select 14: Reserved */
#define DMA1TSEL__RES15        (15*0x0100u)   /* DMA channel 1 transfer select 15: Reserved */
#define DMA1TSEL__USCIA0RX     (16*0x0100u)   /* DMA channel 1 transfer select 16: USCIA0 receive */
#define DMA1TSEL__USCIA0TX     (17*0x0100u)   /* DMA channel 1 transfer select 17: USCIA0 transmit */
#define DMA1TSEL__USCIB0RX     (18*0x0100u)   /* DMA channel 1 transfer select 18: USCIB0 receive */
#define DMA1TSEL__USCIB0TX     (19*0x0100u)   /* DMA channel 1 transfer select 19: USCIB0 transmit */
#define DMA1TSEL__USCIA1RX     (20*0x0100u)   /* DMA channel 1 transfer select 20: USCIA1 receive */
#define DMA1TSEL__USCIA1TX     (21*0x0100u)   /* DMA channel 1 transfer select 21: USCIA1 transmit */
#define DMA1TSEL__USCIB1RX     (22*0x0100u)   /* DMA channel 1 transfer select 22: USCIB1 receive */
#define DMA1TSEL__USCIB1TX     (23*0x0100u)   /* DMA channel 1 transfer select 23: USCIB1 transmit */
#define DMA1TSEL__ADC12IFG     (24*0x0100u)   /* DMA channel 1 transfer select 24: ADC12IFGx */
#define DMA1TSEL__RES25        (25*0x0100u)   /* DMA channel 1 transfer select 25: Reserved */
#define DMA1TSEL__RES26        (26*0x0100u)   /* DMA channel 1 transfer select 26: Reserved */
#define DMA1TSEL__RES27        (27*0x0100u)   /* DMA channel 1 transfer select 27: Reserved */
#define DMA1TSEL__RES28        (28*0x0100u)   /* DMA channel 1 transfer select 28: Reserved */
#define DMA1TSEL__MPY          (29*0x0100u)   /* DMA channel 1 transfer select 29: Multiplier ready */
#define DMA1TSEL__DMA0IFG      (30*0x0100u)   /* DMA channel 1 transfer select 30: previous DMA channel DMA0IFG */
#define DMA1TSEL__DMAE0        (31*0x0100u)   /* DMA channel 1 transfer select 31: ext. Trigger (DMAE0) */

#define DMA2TSEL__DMA_REQ      (0*0x0001u)    /* DMA channel 2 transfer select 0:  DMA_REQ (sw) */
#define DMA2TSEL__TA0CCR0      (1*0x0001u)    /* DMA channel 2 transfer select 1:  Timer0_A (TA0CCR0.IFG) */
#define DMA2TSEL__TA0CCR2      (2*0x0001u)    /* DMA channel 2 transfer select 2:  Timer0_A (TA0CCR2.IFG) */
#define DMA2TSEL__TA1CCR0      (3*0x0001u)    /* DMA channel 2 transfer select 3:  Timer1_A (TA1CCR0.IFG) */
#define DMA2TSEL__TA1CCR2      (4*0x0001u)    /* DMA channel 2 transfer select 4:  Timer1_A (TA1CCR2.IFG) */
#define DMA2TSEL__TB0CCR0      (5*0x0001u)    /* DMA channel 2 transfer select 5:  TimerB (TB0CCR0.IFG) */
#define DMA2TSEL__TB0CCR2      (6*0x0001u)    /* DMA channel 2 transfer select 6:  TimerB (TB0CCR2.IFG) */
#define DMA2TSEL__RES7         (7*0x0001u)    /* DMA channel 2 transfer select 7:  Reserved */
#define DMA2TSEL__RES8         (8*0x0001u)    /* DMA channel 2 transfer select 8:  Reserved */
#define DMA2TSEL__RES9         (9*0x0001u)    /* DMA channel 2 transfer select 9:  Reserved */
#define DMA2TSEL__RES10        (10*0x0001u)   /* DMA channel 2 transfer select 10: Reserved */
#define DMA2TSEL__RES11        (11*0x0001u)   /* DMA channel 2 transfer select 11: Reserved */
#define DMA2TSEL__RES12        (12*0x0001u)   /* DMA channel 2 transfer select 12: Reserved */
#define DMA2TSEL__RES13        (13*0x0001u)   /* DMA channel 2 transfer select 13: Reserved */
#define DMA2TSEL__RES14        (14*0x0001u)   /* DMA channel 2 transfer select 14: Reserved */
#define DMA2TSEL__RES15        (15*0x0001u)   /* DMA channel 2 transfer select 15: Reserved */
#define DMA2TSEL__USCIA0RX     (16*0x0001u)   /* DMA channel 2 transfer select 16: USCIA0 receive */
#define DMA2TSEL__USCIA0TX     (17*0x0001u)   /* DMA channel 2 transfer select 17: USCIA0 transmit */
#define DMA2TSEL__USCIB0RX     (18*0x0001u)   /* DMA channel 2 transfer select 18: USCIB0 receive */
#define DMA2TSEL__USCIB0TX     (19*0x0001u)   /* DMA channel 2 transfer select 19: USCIB0 transmit */
#define DMA2TSEL__USCIA1RX     (20*0x0001u)   /* DMA channel 2 transfer select 20: USCIA1 receive */
#define DMA2TSEL__USCIA1TX     (21*0x0001u)   /* DMA channel 2 transfer select 21: USCIA1 transmit */
#define DMA2TSEL__USCIB1RX     (22*0x0001u)   /* DMA channel 2 transfer select 22: USCIB1 receive */
#define DMA2TSEL__USCIB1TX     (23*0x0001u)   /* DMA channel 2 transfer select 23: USCIB1 transmit */
#define DMA2TSEL__ADC12IFG     (24*0x0001u)   /* DMA channel 2 transfer select 24: ADC12IFGx */
#define DMA2TSEL__RES25        (25*0x0001u)   /* DMA channel 2 transfer select 25: Reserved */
#define DMA2TSEL__RES26        (26*0x0001u)   /* DMA channel 2 transfer select 26: Reserved */
#define DMA2TSEL__RES27        (27*0x0001u)   /* DMA channel 2 transfer select 27: Reserved */
#define DMA2TSEL__RES28        (28*0x0001u)   /* DMA channel 2 transfer select 28: Reserved */
#define DMA2TSEL__MPY          (29*0x0001u)   /* DMA channel 2 transfer select 29: Multiplier ready */
#define DMA2TSEL__DMA1IFG      (30*0x0001u)   /* DMA channel 2 transfer select 30: previous DMA channel DMA1IFG */
#define DMA2TSEL__DMAE0        (31*0x0001u)   /* DMA channel 2 transfer select 31: ext. Trigger (DMAE0) */

/*
 * PMM - Power Management System
 */
#define __MSP430_HAS_PMM__			/* Definition to show that Module is available */

sfrw (PMMCTL0,	__MSP430_PMM_BASE__+0x00);	/* PMM Control 0 */
sfrw (PMMCTL1,  __MSP430_PMM_BASE__+0x02);	/* PMM Control 1 */
sfrw (SVSMHCTL, __MSP430_PMM_BASE__+0x04);	/* SVS and SVM high side control register */
sfrw (SVSMLCTL, __MSP430_PMM_BASE__+0x06);	/* SVS and SVM low side control register */
sfrw (PMMIFG,   __MSP430_PMM_BASE__+0x0C);	/* PMM Interrupt Flag */
sfrw (PMMRIE,   __MSP430_PMM_BASE__+0x0E);	/* PMM and RESET Interrupt Enable */

#define PMMPW			(0xA500)	/* PMM Register Write Password */

/* PMMCTL0 Control Bits */
#define PMMCOREV0		(0x0001)	/* PMM Core Voltage Bit: 0 */
#define PMMCOREV1		(0x0002)	/* PMM Core Voltage Bit: 1 */
#define PMMSWBOR		(0x0004)	/* PMM Software BOR */
#define PMMSWPOR		(0x0008)	/* PMM Software POR */
#define PMMREGOFF		(0x0010)	/* PMM Turn Regulator off */
#define PMMHPMRE		(0x0080)	/* PMM Global High Power Module Request Enable */

/* PMMCTL0 Control Bits */

#define PMMCOREV_0		(0x0000)	/* PMM Core Voltage 0 (1.35V) */
#define PMMCOREV_1		(0x0001)	/* PMM Core Voltage 1 (1.55V) */
#define PMMCOREV_2		(0x0002)	/* PMM Core Voltage 2 (1.75V) */
#define PMMCOREV_3		(0x0003)	/* PMM Core Voltage 3 (1.85V) */

/* PMMCTL1 Control Bits */
#define PMMREFMD		(0x0001)	/* PMM Reference Mode */
#define PMMCMD0			(0x0010)	/* PMM Voltage Regulator Current Mode Bit: 0 */
#define PMMCMD1			(0x0020)	/* PMM Voltage Regulator Current Mode Bit: 1 */

/* SVSMHCTL Control Bits */
#define SVSMHRRL0		(0x0001)	/* SVS and SVM high side Reset Release Voltage Level Bit: 0 */
#define SVSMHRRL1		(0x0002)	/* SVS and SVM high side Reset Release Voltage Level Bit: 1 */
#define SVSMHRRL2		(0x0004)	/* SVS and SVM high side Reset Release Voltage Level Bit: 2 */
#define SVSMHDLYST		(0x0008)	/* SVS and SVM high side delay status */
#define SVSHMD			(0x0010)	/* SVS high side mode */
#define SVSMHEVM		(0x0040)	/* SVS and SVM high side event mask */
#define SVSMHACE		(0x0080)	/* SVS and SVM high side auto control enable */
#define SVSHRVL0		(0x0100)	/* SVS high side reset voltage level Bit: 0 */
#define SVSHRVL1		(0x0200)	/* SVS high side reset voltage level Bit: 1 */
#define SVSHE			(0x0400)	/* SVS high side enable */
#define SVSHFP			(0x0800)	/* SVS high side full performace mode */
#define SVMHOVPE		(0x1000)	/* SVM high side over-voltage enable */
#define SVMHE			(0x4000)	/* SVM high side enable */
#define SVMHFP			(0x8000)	/* SVM high side full performace mode */

#define SVSMHRRL_0		(0x0000)	/* SVS and SVM high side Reset Release Voltage Level 0 */
#define SVSMHRRL_1		(0x0001)	/* SVS and SVM high side Reset Release Voltage Level 1 */
#define SVSMHRRL_2		(0x0002)	/* SVS and SVM high side Reset Release Voltage Level 2 */
#define SVSMHRRL_3		(0x0003)	/* SVS and SVM high side Reset Release Voltage Level 3 */
#define SVSMHRRL_4		(0x0004)	/* SVS and SVM high side Reset Release Voltage Level 4 */
#define SVSMHRRL_5		(0x0005)	/* SVS and SVM high side Reset Release Voltage Level 5 */
#define SVSMHRRL_6		(0x0006)	/* SVS and SVM high side Reset Release Voltage Level 6 */
#define SVSMHRRL_7		(0x0007)	/* SVS and SVM high side Reset Release Voltage Level 7 */

#define SVSHRVL_0		(0x0000)	/* SVS high side Reset Release Voltage Level 0 */
#define SVSHRVL_1		(0x0100)	/* SVS high side Reset Release Voltage Level 1 */
#define SVSHRVL_2		(0x0200)	/* SVS high side Reset Release Voltage Level 2 */
#define SVSHRVL_3		(0x0300)	/* SVS high side Reset Release Voltage Level 3 */

/* SVSMLCTL Control Bits */
#define SVSMLRRL0		(0x0001)	/* SVS and SVM low side Reset Release Voltage Level Bit: 0 */
#define SVSMLRRL1		(0x0002)	/* SVS and SVM low side Reset Release Voltage Level Bit: 1 */
#define SVSMLRRL2		(0x0004)	/* SVS and SVM low side Reset Release Voltage Level Bit: 2 */
#define SVSMLDLYST		(0x0008)	/* SVS and SVM low side delay status */
#define SVSLMD			(0x0010)	/* SVS low side mode */
#define SVSMLEVM		(0x0040)	/* SVS and SVM low side event mask */
#define SVSMLACE		(0x0080)	/* SVS and SVM low side auto control enable */
#define SVSLRVL0		(0x0100)	/* SVS low side reset voltage level Bit: 0 */
#define SVSLRVL1		(0x0200)	/* SVS low side reset voltage level Bit: 1 */
#define SVSLE			(0x0400)	/* SVS low side enable */
#define SVSLFP			(0x0800)	/* SVS low side full performace mode */
#define SVMLOVPE		(0x1000)	/* SVM low side over-voltage enable */
#define SVMLE			(0x4000)	/* SVM low side enable */
#define SVMLFP			(0x8000)	/* SVM low side full performace mode */

#define SVSMLRRL_0		(0x0000)	/* SVS and SVM low side Reset Release Voltage Level 0 */
#define SVSMLRRL_1		(0x0001)	/* SVS and SVM low side Reset Release Voltage Level 1 */
#define SVSMLRRL_2		(0x0002)	/* SVS and SVM low side Reset Release Voltage Level 2 */
#define SVSMLRRL_3		(0x0003)	/* SVS and SVM low side Reset Release Voltage Level 3 */
#define SVSMLRRL_4		(0x0004)	/* SVS and SVM low side Reset Release Voltage Level 4 */
#define SVSMLRRL_5		(0x0005)	/* SVS and SVM low side Reset Release Voltage Level 5 */
#define SVSMLRRL_6		(0x0006)	/* SVS and SVM low side Reset Release Voltage Level 6 */
#define SVSMLRRL_7		(0x0007)	/* SVS and SVM low side Reset Release Voltage Level 7 */

#define SVSLRVL_0		(0x0000)	/* SVS low side Reset Release Voltage Level 0 */
#define SVSLRVL_1		(0x0100)	/* SVS low side Reset Release Voltage Level 1 */
#define SVSLRVL_2		(0x0200)	/* SVS low side Reset Release Voltage Level 2 */
#define SVSLRVL_3		(0x0300)	/* SVS low side Reset Release Voltage Level 3 */

/* SVSMIO Control Bits */
#define SVMLOE			(0x0008)	/* SVM low side output enable */
#define SVMLVLROE		(0x0010)	/* SVM low side voltage level reached output enable */
#define SVMOUTPOL		(0x0020)	/* SVMOUT pin polarity */
#define SVMHOE			(0x0800)	/* SVM high side output enable */
#define SVMHVLROE		(0x1000)	/* SVM high side voltage level reached output enable */

/* PMMIFG Control Bits */
#define SVSMLDLYIFG		(0x0001)	/* SVS and SVM low side Delay expired interrupt flag */
#define SVMLIFG			(0x0002)	/* SVM low side interrupt flag */
#define SVMLVLRIFG		(0x0004)	/* SVM low side Voltage Level Reached interrupt flag */
#define SVSMHDLYIFG		(0x0010)	/* SVS and SVM high side Delay expired interrupt flag */
#define SVMHIFG			(0x0020)	/* SVM high side interrupt flag */
#define SVMHVLRIFG		(0x0040)	/* SVM high side Voltage Level Reached interrupt flag */
#define PMMBORIFG		(0x0100)	/* PMM Software BOR interrupt flag */
#define PMMRSTIFG		(0x0200)	/* PMM RESET pin interrupt flag */
#define PMMPORIFG		(0x0400)	/* PMM Software POR interrupt flag */
#define SVSHIFG			(0x1000)	/* SVS low side interrupt flag */
#define SVSLIFG			(0x2000)	/* SVS high side interrupt flag */
#define PMMLPM5IFG		(0x8000)	/* LPM5 indication Flag */

#define PMMRSTLPM5IFG		PMMLPM5IFG	/* LPM5 indication Flag */

/* PMMIE and RESET Control Bits */
#define SVSMLDLYIE		(0x0001)	/* SVS and SVM low side Delay expired interrupt enable */
#define SVMLIE			(0x0002)	/* SVM low side interrupt enable */
#define SVMLVLRIE		(0x0004)	/* SVM low side Voltage Level Reached interrupt enable */
#define SVSMHDLYIE		(0x0010)	/* SVS and SVM high side Delay expired interrupt enable */
#define SVMHIE			(0x0020)	/* SVM high side interrupt enable */
#define SVMHVLRIE		(0x0040)	/* SVM high side Voltage Level Reached interrupt enable */
#define SVSLPE			(0x0100)	/* SVS low side POR enable */
#define SVMLVLRPE		(0x0200)	/* SVM low side Voltage Level reached POR enable */
#define SVSHPE			(0x1000)	/* SVS high side POR enable */
#define SVMHVLRPE		(0x2000)	/* SVM high side Voltage Level reached POR enable */


#define RTC_A_VECTOR        0x52    /* 0xFFD2 Basic Timer / RTC */
#define PORT2_VECTOR        0x54    /* 0xFFD4 Port 2 */
#define USCIB3_RXTX_VECTOR  0x56    /* 0xFFD6 USCI B3 RX/TX */
#define USCIA3_RXTX_VECTOR  0x58    /* 0xFFD8 USCI A3 RX/TX */
#define USCIB1_RXTX_VECTOR  0x5A    /* 0xFFDA USCI B1 RX/TX */
#define USCIA1_RXTX_VECTOR  0x5C    /* 0xFFDC USCI A1 RX/TX */
#define PORT1_VECTOR        0x5E    /* 0xFFDE Port 1 */
#define TIMER1_A1_VECTOR    0x60    /* 0xFFE0 Timer1_A3 CC1-2, TA1 */
#define TIMER1_A0_VECTOR    0x62    /* 0xFFE2 Timer1_A3 CC0 */
#define DMA_VECTOR          0x64    /* 0xFFE4 DMA */
#define USCIB2_RXTX_VECTOR  0x66    /* 0xFFE6 USCI B2 RX/TX */
#define USCIA2_RXTX_VECTOR  0x68    /* 0xFFE8 USCI A2 RX/TX */
#define TIMER0_A1_VECTOR    0x6A    /* 0xFFEA Timer0_A5 CC1-4, TA0 */
#define TIMER0_A0_VECTOR    0x6C    /* 0xFFEC Timer0_A5 CC0 */
#define AD12_A_VECTOR       0x6E    /* 0xFFEE ADC */
#define USCIB0_RXTX_VECTOR  0x70    /* 0xFFF0 USCI B0 RX/TX */
#define USCIA0_RXTX_VECTOR  0x72    /* 0xFFF2 USCI A0 RX/TX */
#define WDT_VECTOR          0x74    /* 0xFFF4 Watchdog Timer */
#define TIMER0_B1_VECTOR    0x76    /* 0xFFF6 Timer_B7 CC1-6, TB */
#define TIMER0_B0_VECTOR    0x78    /* 0xFFF8 Timer_B7 CC0 */
#define USER_NMI_VECTOR     0x7A    /* 0xFFFA Non-maskable */
#define NMI_VECTOR          0x7C    /* 0xFFFC Non-maskable */

#endif /* #ifndef __msp430x54xx */
