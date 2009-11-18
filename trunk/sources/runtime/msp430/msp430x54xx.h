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

#define __MSP430_HAS_T0A5__
#define __MSP430_HAS_T1A3__
#define __MSP430_HAS_UCS__
#define __MSP430_HAS_USCI0_5__
#define __MSP430_HAS_USCI1_5__
#define __MSP430_HAS_USCI2_5__
#define __MSP430_HAS_USCI3_5__
#define __MSP430_HAS_T0A5__
#define __MSP430_HAS_T0B7__
#define __MSP430_HAS_T1A3__
#define __MSP430_SYS_BASE__     0x180
#define __MSP430_WDT_A_BASE__   0x150
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

sfrb (UCB0CTL0,	 __MSP430_USCI0_BASE__+0x20);	/* USCI B0 synchronous control 0 */
sfrb (UCB0CTL1,	 __MSP430_USCI0_BASE__+0x21);	/* USCI B0 synchronous control 1 */
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

sfrb (UCB1CTL0,	 __MSP430_USCI1_BASE__+0x20);	/* USCI B1 synchronous control 0 */
sfrb (UCB1CTL1,	 __MSP430_USCI1_BASE__+0x21);	/* USCI B1 synchronous control 1 */
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

sfrb (UCB2CTL0,	 __MSP430_USCI2_BASE__+0x20);	/* USCI B2 synchronous control 0 */
sfrb (UCB2CTL1,	 __MSP430_USCI2_BASE__+0x21);	/* USCI B2 synchronous control 1 */
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

sfrb (UCB3CTL0,	 __MSP430_USCI3_BASE__+0x20);	/* USCI B3 synchronous control 0 */
sfrb (UCB3CTL1,	 __MSP430_USCI3_BASE__+0x21);	/* USCI B3 synchronous control 1 */
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
