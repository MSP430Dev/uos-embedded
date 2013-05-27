/*
 * Hardware register defines for Microchip PIC32MX microcontrollers.
 *
 * Copyright (C) 2010-2012 Serge Vakulenko, <serge@vak.ru>
 *                    2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#ifndef _IO_MALTA_H
#define _IO_MALTA_H

/*--------------------------------------
 * Coprocessor 0 registers.
 */
#define C0_HWRENA	7	/* Enable RDHWR in non-privileged mode */
#define C0_BADVADDR	8	/* Virtual address of last exception */
#define C0_COUNT	9	/* Processor cycle count */
#define C0_COMPARE	11	/* Timer interrupt control */
#define C0_STATUS	12	/* Processor status and control */
#define C0_INTCTL	12	/* Select 1: interrupt control */
#define C0_SRSCTL	12	/* Select 2: GPR shadow set control */
#define C0_SRSMAP	12	/* Select 3: vector to shadow set mapping */
#define C0_CAUSE	13	/* Cause of last exception */
#define C0_EPC		14	/* Program counter at last exception */
#define C0_PRID		15	/* Processor identification (read only) */
#define C0_EBASE	15	/* Select 1: exception base address */
#define C0_CONFIG	16	/* Configuration */
#define C0_CONFIG1	16	/* Select 1: configuration 1 */
#define C0_CONFIG2	16	/* Select 2: configuration 2 */
#define C0_CONFIG3	16	/* Select 3: configuration 3 */
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

#define DB_DBD          (1 << 31)       /* Debug exception in a branch delay slot */
#define DB_DM           (1 << 30)       /* Debug mode */
#define DB_NODCR        (1 << 29)       /* No dseg present */
#define DB_LSNM         (1 << 28)       /* Load/stores in dseg go to main memory */
#define DB_DOZE         (1 << 27)       /* Processor was in low-power mode */
#define DB_HALT         (1 << 26)       /* Internal system bus clock was running */
#define DB_COUNTDM      (1 << 25)       /* Count register is running in Debug mode */
#define DB_IBUSEP       (1 << 24)       /* Instruction fetch bus error exception */
#define DB_DBUSEP       (1 << 21)       /* Data access bus error exception */
#define DB_IEXI         (1 << 20)       /* Imprecise error exception */
#define DB_VER          (7 << 15)       /* EJTAG version number */
#define DB_DEXCCODE     (0x1f << 10)    /* Cause of exception in Debug mode */
#define DB_SST          (1 << 8)        /* Single step exception enabled */
#define DB_DIBImpr      (1 << 6)        /* Imprecise debug instruction break */
#define DB_DINT         (1 << 5)        /* Debug interrupt exception */
#define DB_DIB          (1 << 4)        /* Debug instruction break exception */
#define DB_DDBS         (1 << 3)        /* Debug data break exception on store */
#define DB_DDBL         (1 << 2)        /* Debug data break exception on load */
#define DB_DBP          (1 << 1)        /* Debug software breakpoint exception */
#define DB_DSS          (1 << 0)        /* Debug single-step exception */

/*--------------------------------------
 * Peripheral Registers
 */
#define MALTA_R(a)		*(volatile unsigned*)(0xBF000000 + (a))

/*--------------------------------------
 * Malta-R Internal Registers
 */
#define MALTA_REVISION          MALTA_R(0xC00000)
#define MALTA_NMISTATUS         MALTA_R(0x024)
#define MALTA_NMIACK            MALTA_R(0x104)
#define MALTA_SWITCH            MALTA_R(0x200)
#define MALTA_STATUS            MALTA_R(0x208)
#define MALTA_JMPRS             MALTA_R(0x210)
#define MALTA_LEDBAR            MALTA_R(0x408)
#define MALTA_ASCIIWORD         MALTA_R(0x410)
#define MALTA_ASCIIPOS_BEGIN    MALTA_R(0x410)
#define MALTA_ASCIIPOS_END      MALTA_R(0x450)
#define MALTA_SOFTRES           MALTA_R(0x500)
#define MALTA_BRKRES            MALTA_R(0x508)

/* NMISTATUS Register */
#define NMISTATUS_FPGRV     (1 << 1)
#define NMISTATUS_CORID     (1 << 0)

/* STATUS Register */
#define STATUS_MFWR         (1 << 4)
#define STATUS_SW5_4        (1 << 3)
#define STATUS_SW5_3        (1 << 2)
#define STATUS_SW5_2        (1 << 1)

/* JMPRS Register */
#define JMPRS_PCICLK(x)     ((x) << 2)
#define JMPRS_EELOCK        (1 << 1)


/*--------------------------------------
 * UART Registers.
 */
#define UART_DLL        MALTA_R(0x900)
#define UART_DLM        MALTA_R(0x908)

#define UART_RHR        MALTA_R(0x900)
#define UART_THR        MALTA_R(0x900)
#define UART_IER        MALTA_R(0x908)
#define UART_FCR        MALTA_R(0x910)
#define UART_ISR        MALTA_R(0x910)
#define UART_LCR        MALTA_R(0x918)
#define UART_MCR        MALTA_R(0x920)
#define UART_LSR        MALTA_R(0x928)
#define UART_MSR        MALTA_R(0x930)
#define UART_SPR        MALTA_R(0x938)

/* UART IER */
#define IER_MSR         (1 << 3)
#define IER_LSR         (1 << 2)
#define IER_THR         (1 << 1)
#define IER_RHR         (1 << 0)

/* UART FCR */
#define FCR_RCVR_MSB        (1 << 7)
#define FCR_RCVR_LSB        (1 << 6)
#define FCR_DMA_MODE        (1 << 3)
#define FCR_XMIT_FIFO_RST   (1 << 2)
#define FCR_RCVR_FIFO_RST   (1 << 1)
#define FCR_FIFO_EN         (1 << 0)

/* UART ISR */
#define ISR_FIFO_EN         (3 << 6)
#define ISR_PRIORITY(x)     ((x) << 1)
#define ISR_STATUS          (1 << 0)

/* UART LCR */
#define LCR_DIV_EN          (1 << 7)
#define LCR_SET_BREAK       (1 << 6)
#define LCR_SET_PARITY      (1 << 5)
#define LCR_EVEN_PARITY     (1 << 4)
#define LCR_PARITY_EN       (1 << 3)
#define LCR_STOP_BITS       (1 << 2)
#define LCR_LEN8            3
#define LCR_LEN7            2
#define LCR_LEN6            1
#define LCR_LEN5            0

/* UART MCR */
#define MCR_LOOP_EN         (1 << 4)
#define MCR_OP2             (1 << 3)
#define MCR_OP1             (1 << 2)
#define MCR_RTS             (1 << 1)
#define MCR_DTR             (1 << 0)

/* UART LSR */
#define LSR_FIFO_DATA_ERR   (1 << 7)
#define LSR_TX_EMPTY        (1 << 6)
#define LSR_TX_HOLD_EMPTY   (1 << 5)
#define LSR_BREAK_INT       (1 << 4)
#define LSR_FRAME_ERR       (1 << 3)
#define LSR_PARITY_ERR      (1 << 2)
#define LSR_OVERRUN_ERR     (1 << 1)
#define LSR_RX_DATA_READY   (1 << 0)

/* UART MSR */
#define MSR_CD              (1 << 7)
#define MSR_RI              (1 << 6)
#define MSR_DSR             (1 << 5)
#define MSR_CTS             (1 << 4)
#define MSR_DCD             (1 << 3)
#define MSR_DRI             (1 << 2)
#define MSR_DDSR            (1 << 1)
#define MSR_DCTS            (1 << 0)

#endif /* _IO_MALTA_H */
