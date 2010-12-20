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
#define C0_INDEX	0	/* индекс доступа к TLB */
#define C0_RANDOM	1	/* индекс TLB для команды Write Random */
#define C0_ENTRYLO0	2	/* строки для чётных страниц TLB */
#define C0_ENTRYLO1	3	/* строки для нечётных страниц TLB */
#define C0_CONTEXT	4	/* указатель на таблицу PTE */
#define C0_PAGEMASK	5	/* маска размера страниц TLB */
#define C0_WIRED	6	/* граница привязанных строк TLB */
#define C0_BADVADDR	8	/* виртуальный адрес последнего исключения */
#define C0_COUNT	9	/* таймер */
#define C0_ENTRYHI	10	/* информация соответствия виртуального адреса */
#define C0_COMPARE	11	/* предельное значение для прерывания по таймеру */
#define C0_STATUS	12	/* режимы функционирования процессора */
#define C0_CAUSE	13	/* причина последнего исключения */
#define C0_EPC		14	/* адрес возврата из исключения */
#define C0_PRID		15	/* идентификатор процессора */
#define C0_CONFIG	16	/* информация о возможностях процессора */
#define C0_LLADDR	17	/* физический адрес последней команды LL */
#define C0_ERROREPC	30	/* адрес возврата из исключения ошибки */

/*
 * Status register.
 */
#define ST_IE		0x00000001	/* разрешение прерываний */
#define ST_EXL		0x00000002	/* уровень исключения */
#define ST_ERL		0x00000004	/* уровень ошибки */
#define ST_UM		0x00000010	/* режим пользователя */
#define ST_IM_SW0	0x00000100	/* программное прерывание 0 */
#define ST_IM_SW1	0x00000200	/* программное прерывание 1 */

#define ST_NMI		0x00080000	/* причина сброса - NMI */
#define ST_TS		0x00200000	/* TLB-закрытие системы */
#define ST_BEV		0x00400000	/* размещение векторов: начальная загрузка */

#define ST_CU0		0x10000000	/* разрешение сопроцессора 0 */
#define ST_CU1		0x20000000	/* разрешение сопроцессора 1 (FPU) */

/*
 * Сause register.
 */
#define CA_EXC_CODE	0x0000007c	/* код исключения */
#define CA_Int		0		/* прерывание */
#define CA_Mod		(1 << 2)	/* TLB-исключение модификации */
#define CA_TLBL		(2 << 2)	/* TLB-исключение, загрузка или вызов команды */
#define CA_TLBS		(3 << 2)	/* TLB-исключение, сохранение */
#define CA_AdEL		(4 << 2)	/* ошибка адресации, загрузка или вызов команды */
#define CA_AdES		(5 << 2)	/* ошибка адресации, сохранение */
#define CA_Sys		(8 << 2)	/* системное исключение */
#define CA_Bp		(9 << 2)	/* breakpoint */
#define CA_RI		(10 << 2)	/* зарезервированная команда */
#define CA_CpU		(11 << 2)	/* недоступность сопроцессора */
#define CA_Ov		(12 << 2)	/* целочисленное переполнение */
#define CA_Tr		(13 << 2)	/* trap */
#define CA_MCheck	(24 << 2)	/* аппаратный контроль */

#define CA_ID		0x00000080	/* прерывание от блока отладки OnCD */
#define CA_IP_SW0	0x00000100	/* программное прерывание 0 */
#define CA_IP_SW1	0x00000200	/* программное прерывание 1 */
#define CA_IP_IRQ0	0x00000400	/* внешнее прерывание 0 */
#define CA_IP_IRQ1	0x00000800	/* внешнее прерывание 1 */
#define CA_IP_IRQ2	0x00001000	/* внешнее прерывание 2 */
#define CA_IP_IRQ3	0x00002000	/* внешнее прерывание 3 */
#define CA_IP_MCU	0x00008000	/* от внутренних устройств микроконтроллера */
#define CA_IV		0x00800000	/* 1 - используется спец.вектор 0x200 */
#define CA_BD		0x80000000	/* исключение в слоте задержки перехода */

/*--------------------------------------
 * Peripheral registers.
 */
#define PIC32_R(a)		*(volatile unsigned*)(0xBF800000 + (a))

/*
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

#endif /* _IO_PIC32MX_H */
