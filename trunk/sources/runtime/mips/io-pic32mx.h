/*
 * Hardware register defines for all Microchip PIC32MX microcontrollers.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
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
#ifndef _IO_PIC32MX_H
#define _IO_PIC32MX_H

/*--------------------------------------
 * Coprocessor 0 registers.
 */
#define C0_HWRENA	7	/* Enable RDHWR in non-privileged mode */
#define C0_BADVADDR	8	/* Virtual address of last exception */
#define C0_COUNT	9	/* Processor cycle count */
#define C0_COMPARE	11	/* Timer interrupt control */
#define C0_STATUS	12	/* Processor status and control */
#define C0_CAUSE	13	/* Cause of last exception */
#define C0_EPC		14	/* Program counter at last exception */
#define C0_PRID_EBASE	15	/* Processor identification; exception base address */
#define C0_CONFIG	16	/* Configuration */
#define C0_DEBUG	23	/* Debug control and status */
#define C0_DEPC		24	/* Program counter at last debug exception */
#define C0_ERROREPC	30	/* Program counter at last error */
#define C0_DESAVE	31	/* Debug handler scratchpad register */

/*
 * Status register.
 */
#define ST_CU0		0x10000000	/* Access to coprocessor 0 allowed (in user mode) */
#define ST_RP		0x08000000	/* Enable reduced power mode */
#define ST_RE		0x02000000	/* Reverse endianness (in user mode) */
#define ST_BEV		0x00400000	/* Exception vectors: bootstrap */
#define ST_SR		0x00100000	/* Soft reset */
#define ST_NMI		0x00080000	/* NMI reset */
#define ST_IPL(x)	((x) << 10)	/* Current interrupt priority level */
#define ST_UM		0x00000010	/* User mode */
#define ST_ERL		0x00000004	/* Error level */
#define ST_EXL		0x00000002	/* Exception level */
#define ST_IE		0x00000001	/* Interrupt enable */

/*
 * Сause register.
 */
#define CA_BD		0x80000000	/* Exception occured in delay slot */
#define CA_TI		0x40000000	/* Timer interrupt is pending */
#define CA_CE		0x30000000	/* Coprocessor exception */
#define CA_DC		0x08000000	/* Disable COUNT register */
#define CA_IV		0x00800000	/* Use special interrupt vector 0x200 */
#define CA_RIPL(r)	((r)>>10 & 63)	/* Requested interrupt priority level */
#define CA_IP1		0x00020000	/* Request software interrupt 1 */
#define CA_IP0		0x00010000	/* Request software interrupt 0 */
#define CA_EXC_CODE	0x0000007c	/* Exception code */

#define CA_Int		0		/* Interrupt */
#define CA_AdEL		(4 << 2)	/* Address error, load or instruction fetch */
#define CA_AdES		(5 << 2)	/* Address error, store */
#define CA_IBE		(6 << 2)	/* Bus error, instruction fetch */
#define CA_DBE		(7 << 2)	/* Bus error, load or store */
#define CA_Sys		(8 << 2)	/* Syscall */
#define CA_Bp		(9 << 2)	/* Breakpoint */
#define CA_RI		(10 << 2)	/* Reserved instruction */
#define CA_CPU		(11 << 2)	/* Coprocessor unusable */
#define CA_Ov		(12 << 2)	/* Arithmetic overflow */
#define CA_Tr		(13 << 2)	/* Trap */

/*--------------------------------------
 * Peripheral registers.
 */
#define PIC32_R(a)		*(volatile unsigned*)(0xBF800000 + (a))

/*--------------------------------------
 * UART registers.
 */
#define U1MODE		PIC32_R (0x6000) /* Mode */
#define U1MODECLR	PIC32_R (0x6004)
#define U1MODESET	PIC32_R (0x6008)
#define U1MODEINV	PIC32_R (0x600C)
#define U1STA		PIC32_R (0x6010) /* Status and control */
#define U1STACLR	PIC32_R (0x6014)
#define U1STASET	PIC32_R (0x6018)
#define U1STAINV	PIC32_R (0x601C)
#define U1TXREG		PIC32_R (0x6020) /* Transmit */
#define U1RXREG		PIC32_R (0x6030) /* Receive */
#define U1BRG		PIC32_R (0x6040) /* Baud rate */
#define U1BRGCLR	PIC32_R (0x6044)
#define U1BRGSET	PIC32_R (0x6048)
#define U1BRGINV	PIC32_R (0x604C)

#define U2MODE		PIC32_R (0x6200) /* Mode */
#define U2MODECLR	PIC32_R (0x6204)
#define U2MODESET	PIC32_R (0x6208)
#define U2MODEINV	PIC32_R (0x620C)
#define U2STA		PIC32_R (0x6210) /* Status and control */
#define U2STACLR	PIC32_R (0x6214)
#define U2STASET	PIC32_R (0x6218)
#define U2STAINV	PIC32_R (0x621C)
#define U2TXREG		PIC32_R (0x6220) /* Transmit */
#define U2RXREG		PIC32_R (0x6230) /* Receive */
#define U2BRG		PIC32_R (0x6240) /* Baud rate */
#define U2BRGCLR	PIC32_R (0x6244)
#define U2BRGSET	PIC32_R (0x6248)
#define U2BRGINV	PIC32_R (0x624C)

/*
 * UART Mode register.
 */
#define PIC32_UMODE_STSEL	0x0001	/* 2 Stop bits */
#define PIC32_UMODE_PDSEL	0x0006	/* Bitmask: */
#define PIC32_UMODE_PDSEL_8NPAR	0x0000	/* 8-bit data, no parity */
#define PIC32_UMODE_PDSEL_8EVEN	0x0002	/* 8-bit data, even parity */
#define PIC32_UMODE_PDSEL_8ODD	0x0004	/* 8-bit data, odd parity */
#define PIC32_UMODE_PDSEL_9NPAR	0x0006	/* 9-bit data, no parity */
#define PIC32_UMODE_BRGH	0x0008	/* High Baud Rate Enable */
#define PIC32_UMODE_RXINV	0x0010	/* Receive Polarity Inversion */
#define PIC32_UMODE_ABAUD	0x0020	/* Auto-Baud Enable */
#define PIC32_UMODE_LPBACK	0x0040	/* UARTx Loopback Mode */
#define PIC32_UMODE_WAKE	0x0080	/* Wake-up on start bit during Sleep Mode */
#define PIC32_UMODE_UEN		0x0300	/* Bitmask: */
#define PIC32_UMODE_UEN_RTS	0x0100	/* Using UxRTS pin */
#define PIC32_UMODE_UEN_RTSCTS	0x0200	/* Using UxCTS and UxRTS pins */
#define PIC32_UMODE_UEN_BCLK	0x0300	/* Using UxBCLK pin */
#define PIC32_UMODE_RTSMD	0x0800	/* UxRTS Pin Simplex mode */
#define PIC32_UMODE_IREN	0x1000	/* IrDA Encoder and Decoder Enable bit */
#define PIC32_UMODE_SIDL	0x2000	/* Stop in Idle Mode */
#define PIC32_UMODE_FRZ		0x4000	/* Freeze in Debug Exception Mode */
#define PIC32_UMODE_ON		0x8000	/* UART Enable */

/*
 * UART Control and status register.
 */
#define PIC32_USTA_URXDA	0x00000001 /* Receive Data Available (read-only) */
#define PIC32_USTA_OERR		0x00000002 /* Receive Buffer Overrun */
#define PIC32_USTA_FERR		0x00000004 /* Framing error detected (read-only) */
#define PIC32_USTA_PERR		0x00000008 /* Parity error detected (read-only) */
#define PIC32_USTA_RIDLE	0x00000010 /* Receiver is idle (read-only) */
#define PIC32_USTA_ADDEN	0x00000020 /* Address Detect mode */
#define PIC32_USTA_URXISEL	0x000000C0 /* Bitmask: receive interrupt is set when... */
#define PIC32_USTA_URXISEL_NEMP	0x00000000 /* ...receive buffer is not empty */
#define PIC32_USTA_URXISEL_HALF	0x00000040 /* ...receive buffer becomes 1/2 full */
#define PIC32_USTA_URXISEL_3_4	0x00000080 /* ...receive buffer becomes 3/4 full */
#define PIC32_USTA_TRMT		0x00000100 /* Transmit shift register is empty (read-only) */
#define PIC32_USTA_UTXBF	0x00000200 /* Transmit buffer is full (read-only) */
#define PIC32_USTA_UTXEN	0x00000400 /* Transmit Enable */
#define PIC32_USTA_UTXBRK	0x00000800 /* Transmit Break */
#define PIC32_USTA_URXEN	0x00001000 /* Receiver Enable */
#define PIC32_USTA_UTXINV	0x00002000 /* Transmit Polarity Inversion */
#define PIC32_USTA_UTXISEL	0x0000C000 /* Bitmask: TX interrupt is generated when... */
#define PIC32_USTA_UTXISEL_1	0x00000000 /* ...the transmit buffer contains at least one empty space */
#define PIC32_USTA_UTXISEL_ALL	0x00004000 /* ...all characters have been transmitted */
#define PIC32_USTA_UTXISEL_EMP	0x00008000 /* ...the transmit buffer becomes empty */
#define PIC32_USTA_ADDR		0x00FF0000 /* Automatic Address Mask */
#define PIC32_USTA_ADM_EN	0x01000000 /* Automatic Address Detect */

/*
 * Compute the 16-bit baud rate divisor, given
 * the oscillator frequency and baud rate.
 * Round to the nearest integer.
 */
#define PIC32_BRG_BAUD(fr,bd)	((((fr)/8 + (bd)) / (bd) / 2) - 1)

/*--------------------------------------
 * Port A-G registers.
 */
#define TRISA		PIC32_R (0x86000) /* Port A: mask of inputs */
#define TRISACLR	PIC32_R (0x86004)
#define TRISASET	PIC32_R (0x86008)
#define TRISAINV	PIC32_R (0x8600C)
#define PORTA		PIC32_R (0x86010) /* Port A: read inputs, write outputs */
#define PORTACLR	PIC32_R (0x86014)
#define PORTASET	PIC32_R (0x86018)
#define PORTAINV	PIC32_R (0x8601C)
#define LATA		PIC32_R (0x86020) /* Port A: read/write outputs */
#define LATACLR		PIC32_R (0x86024)
#define LATASET		PIC32_R (0x86028)
#define LATAINV		PIC32_R (0x8602C)
#define ODCA		PIC32_R (0x86030) /* Port A: open drain configuration */
#define ODCACLR		PIC32_R (0x86034)
#define ODCASET		PIC32_R (0x86038)
#define ODCAINV		PIC32_R (0x8603C)

#define TRISB		PIC32_R (0x86040) /* Port B: mask of inputs */
#define TRISBCLR	PIC32_R (0x86044)
#define TRISBSET	PIC32_R (0x86048)
#define TRISBINV	PIC32_R (0x8604C)
#define PORTB		PIC32_R (0x86050) /* Port B: read inputs, write outputs */
#define PORTBCLR	PIC32_R (0x86054)
#define PORTBSET	PIC32_R (0x86058)
#define PORTBINV	PIC32_R (0x8605C)
#define LATB		PIC32_R (0x86060) /* Port B: read/write outputs */
#define LATBCLR		PIC32_R (0x86064)
#define LATBSET		PIC32_R (0x86068)
#define LATBINV		PIC32_R (0x8606C)
#define ODCB		PIC32_R (0x86070) /* Port B: open drain configuration */
#define ODCBCLR		PIC32_R (0x86074)
#define ODCBSET		PIC32_R (0x86078)
#define ODCBINV		PIC32_R (0x8607C)

#define TRISC		PIC32_R (0x86080) /* Port C: mask of inputs */
#define TRISCCLR	PIC32_R (0x86084)
#define TRISCSET	PIC32_R (0x86088)
#define TRISCINV	PIC32_R (0x8608C)
#define PORTC		PIC32_R (0x86090) /* Port C: read inputs, write outputs */
#define PORTCCLR	PIC32_R (0x86094)
#define PORTCSET	PIC32_R (0x86098)
#define PORTCINV	PIC32_R (0x8609C)
#define LATC		PIC32_R (0x860A0) /* Port C: read/write outputs */
#define LATCCLR		PIC32_R (0x860A4)
#define LATCSET		PIC32_R (0x860A8)
#define LATCINV		PIC32_R (0x860AC)
#define ODCC		PIC32_R (0x860B0) /* Port C: open drain configuration */
#define ODCCCLR		PIC32_R (0x860B4)
#define ODCCSET		PIC32_R (0x860B8)
#define ODCCINV		PIC32_R (0x860BC)

#define TRISD		PIC32_R (0x860C0) /* Port D: mask of inputs */
#define TRISDCLR	PIC32_R (0x860C4)
#define TRISDSET	PIC32_R (0x860C8)
#define TRISDINV	PIC32_R (0x860CC)
#define PORTD		PIC32_R (0x860D0) /* Port D: read inputs, write outputs */
#define PORTDCLR	PIC32_R (0x860D4)
#define PORTDSET	PIC32_R (0x860D8)
#define PORTDINV	PIC32_R (0x860DC)
#define LATD		PIC32_R (0x860E0) /* Port D: read/write outputs */
#define LATDCLR		PIC32_R (0x860E4)
#define LATDSET		PIC32_R (0x860E8)
#define LATDINV		PIC32_R (0x860EC)
#define ODCD		PIC32_R (0x860F0) /* Port D: open drain configuration */
#define ODCDCLR		PIC32_R (0x860F4)
#define ODCDSET		PIC32_R (0x860F8)
#define ODCDINV		PIC32_R (0x860FC)

#define TRISE		PIC32_R (0x86100) /* Port E: mask of inputs */
#define TRISECLR	PIC32_R (0x86104)
#define TRISESET	PIC32_R (0x86108)
#define TRISEINV	PIC32_R (0x8610C)
#define PORTE		PIC32_R (0x86110) /* Port E: read inputs, write outputs */
#define PORTECLR	PIC32_R (0x86114)
#define PORTESET	PIC32_R (0x86118)
#define PORTEINV	PIC32_R (0x8611C)
#define LATE		PIC32_R (0x86120) /* Port E: read/write outputs */
#define LATECLR		PIC32_R (0x86124)
#define LATESET		PIC32_R (0x86128)
#define LATEINV		PIC32_R (0x8612C)
#define ODCE		PIC32_R (0x86130) /* Port E: open drain configuration */
#define ODCECLR		PIC32_R (0x86134)
#define ODCESET		PIC32_R (0x86138)
#define ODCEINV		PIC32_R (0x8613C)

#define TRISF		PIC32_R (0x86140) /* Port F: mask of inputs */
#define TRISFCLR	PIC32_R (0x86144)
#define TRISFSET	PIC32_R (0x86148)
#define TRISFINV	PIC32_R (0x8614C)
#define PORTF		PIC32_R (0x86150) /* Port F: read inputs, write outputs */
#define PORTFCLR	PIC32_R (0x86154)
#define PORTFSET	PIC32_R (0x86158)
#define PORTFINV	PIC32_R (0x8615C)
#define LATF		PIC32_R (0x86160) /* Port F: read/write outputs */
#define LATFCLR		PIC32_R (0x86164)
#define LATFSET		PIC32_R (0x86168)
#define LATFINV		PIC32_R (0x8616C)
#define ODCF		PIC32_R (0x86170) /* Port F: open drain configuration */
#define ODCFCLR		PIC32_R (0x86174)
#define ODCFSET		PIC32_R (0x86178)
#define ODCFINV		PIC32_R (0x8617C)

#define TRISG		PIC32_R (0x86180) /* Port G: mask of inputs */
#define TRISGCLR	PIC32_R (0x86184)
#define TRISGSET	PIC32_R (0x86188)
#define TRISGINV	PIC32_R (0x8618C)
#define PORTG		PIC32_R (0x86190) /* Port G: read inputs, write outputs */
#define PORTGCLR	PIC32_R (0x86194)
#define PORTGSET	PIC32_R (0x86198)
#define PORTGINV	PIC32_R (0x8619C)
#define LATG		PIC32_R (0x861A0) /* Port G: read/write outputs */
#define LATGCLR		PIC32_R (0x861A4)
#define LATGSET		PIC32_R (0x861A8)
#define LATGINV		PIC32_R (0x861AC)
#define ODCG		PIC32_R (0x861B0) /* Port G: open drain configuration */
#define ODCGCLR		PIC32_R (0x861B4)
#define ODCGSET		PIC32_R (0x861B8)
#define ODCGINV		PIC32_R (0x861BC)

#define CNCON		PIC32_R (0x861C0) /* Interrupt-on-change control */
#define CNCONCLR	PIC32_R (0x861C4)
#define CNCONSET	PIC32_R (0x861C8)
#define CNCONINV	PIC32_R (0x861CC)
#define CNEN		PIC32_R (0x861D0) /* Input change interrupt enable */
#define CNENCLR		PIC32_R (0x861D4)
#define CNENSET		PIC32_R (0x861D8)
#define CNENINV		PIC32_R (0x861DC)
#define CNPUE		PIC32_R (0x861E0) /* Input pin pull-up enable */
#define CNPUECLR	PIC32_R (0x861E4)
#define CNPUESET	PIC32_R (0x861E8)
#define CNPUEINV	PIC32_R (0x861EC)

#endif /* _IO_PIC32MX_H */
