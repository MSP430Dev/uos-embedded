/*
 * Hardware register defines for Elvees MC-24 microcontroller.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
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
#ifndef _IO_MC24_H
#define _IO_MC24_H

#define MC_HAVE_SPORT
#define MC_HAVE_LPORT
#define MC_HAVE_SWIC

#define MC_R(a)		*(volatile unsigned*)(0xB82F0000 + (a))

/*
 * Системные регистры
 */
#define MC_MASKR	MC_R (0x4000)	/* Регистр маски */
#define MC_QSTR		MC_R (0x4004)	/* Регистр заявок */
#define MC_CSR		MC_R (0x4008)	/* Регистр управления */
#define MC_MASKR_DSP	MC_R (0x4010)	/* Регистр маски DSP */
#define MC_QSTR_DSP	MC_R (0x4014)	/* Регистр заявок DSP */
#define MC_MASKR_HM	MC_R (0x4018)	/* Регистр маски кода Хемминга */
#define MC_QSTR_HM	MC_R (0x401C)	/* Регистр заявок кода Хемминга */
#define MC_CSR_CRAM	MC_R (0x4410)	/* Регистр управления CRAM */
#define MC_AERROR_CRAM	MC_R (0x4414)	/* FIFO ошибочных адресов CRAM */
#define MC_CSR_ICACHE	MC_R (0x4800)	/* Регистр управления CRAM */
#define MC_AERROR_ICACHE MC_R (0x4814)	/* FIFO ошибочных адресов CRAM */

/*
 * Регистры порта внешней памяти
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100C)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_CKE_CTR	MC_R (0x1018)	/* Регистр управления состоянием вывода CKE микросхемы */
#define MC_CSR_EXT	MC_R (0x101C)	/* Регистр управления и состояния внешней памяти */
#define MC_AER_ROR_EXT	MC_R (0x1020)	/* FIFO ошибочных адресов внешней памяти */

/*
 * Регистры UART
 */
#define MC_RBR		MC_R (0x3000)	/* Приемный буферный регистр */
#define MC_THR		MC_R (0x3000)	/* Передающий буферный регистр */
#define MC_IER		MC_R (0x3004)	/* Регистр разрешения прерываний */
#define MC_IIR		MC_R (0x3008)	/* Регистр идентификации прерывания */
#define MC_FCR		MC_R (0x3008)	/* Регистр управления FIFO */
#define MC_LCR		MC_R (0x300C)	/* Регистр управления линией */
#define MC_MCR		MC_R (0x3010)	/* Регистр управления модемом */
#define MC_LSR		MC_R (0x3014)	/* Регистр состояния линии */
#define MC_MSR		MC_R (0x3018)	/* Регистр состояния модемом */
#define MC_SPR		MC_R (0x301C)	/* Регистр Scratch Pad */
#define MC_DLL		MC_R (0x3000)	/* Регистр делителя младший */
#define MC_DLM		MC_R (0x3004)	/* Регистр делителя старший */
#define MC_SCLR		MC_R (0x3014)	/* Регистр предделителя (scaler) */

/*
 * Регистры интервального таймера (IT)
 */
#define MC_ITCSR	MC_R (0xD000)	/* Регистр управления */
#define MC_ITPERIOD	MC_R (0xD004)	/* Регистр периода работы таймера */
#define MC_ITCOUNT	MC_R (0xD008)	/* Регистр счетчика */
#define MC_ITSCALE	MC_R (0xD00C)	/* Регистр предделителя */

/*
 * Регистры WDT
 */
#define MC_WTCSR	MC_R (0xD010)	/* Регистр управления */
#define MC_WTPERIOD	MC_R (0xD014)	/* Регистр периода работы таймера */
#define MC_WTCOUNT	MC_R (0xD018)	/* Регистр счетчика */
#define MC_WTSCALE	MC_R (0xD01C)	/* Регистр предделителя */

/*
 * Регистры RTT
 */
#define MC_RTCSR	MC_R (0xD020)	/* Регистр управления */
#define MC_RTPERIOD	MC_R (0xD024)	/* Регистр периода работы таймера */
#define MC_RTCOUNT	MC_R (0xD028)	/* Регистр счетчика */

/*
 * Регистры линковых портов LPORT0...LPORT3
 */
#ifdef MC_HAVE_LPORT
#define MC_LTX(n)	MC_R (0x7000+(n<<12))	/* Буфер передачи */
#define MC_LRX(n)	MC_R (0x7000+(n<<12))	/* Буфер приема */
#define MC_LCSR(n)	MC_R (0x7004+(n<<12))	/* Регистр управления и состояния */
#define MC_LDIR(n)	MC_R (0x7008+(n<<12))	/* Регистр управления */
#define MC_LDR(n)	MC_R (0x700C+(n<<12))	/* Регистр данных */
#endif

/*
 * Регистры портов обмена последовательным кодом SPORT0, SPORT1
 */
#ifdef MC_HAVE_SPORT
#define MC_STX(n)	MC_R (0x5000+(n<<12))	/* Буфер передачи данных */
#define MC_RX(n)	MC_R (0x5000+(n<<12))	/* Буфер приема данных */
#define MC_STCTL(n)	MC_R (0x5004+(n<<12))	/* Регистр управления передачей данных */
#define MC_SRCTL(n)	MC_R (0x5008+(n<<12))	/* Регистр управления приемом данных */
#define MC_TDIV(n)	MC_R (0x500C+(n<<12))	/* Регистр коэффициентов деления при передаче */
#define MC_RDIV(n)	MC_R (0x5010+(n<<12))	/* Регистр коэффициентов деления при приеме */
#define MC_MTCS(n)	MC_R (0x5014+(n<<12))	/* Выбор канала передачи в многоканальном режиме */
#define MC_MRCS(n)	MC_R (0x5018+(n<<12))	/* Выбор канала приема в многоканальном режиме */
#define MC_KEYWD(n)	MC_R (0x501C+(n<<12))	/* Регистр кода сравнения */
#define MC_KEYMASK(n)	MC_R (0x5020+(n<<12))	/* Регистр маски сравнения */
#define MC_MRCE(n)	MC_R (0x5024+(n<<12))	/* Выбор канала для сравнения принимаемых данных */
#endif /* MC_HAVE_SPORT */

/*
 * Регистры интерфейсов Space Wire, размещенных в контроллере (SWIC0, SWIC1)
 */
#ifdef MC_HAVE_SWIC

#define MC_SWIC_HW_VER(n)	MC_R (0x5000 + (n<<12))	/* Регистр аппаратной версии контроллера */
#define MC_SWIC_STATUS(n)	MC_R (0x5004 + (n<<12))	/* Регистр состояния */
#define MC_SWIC_RX_CODE(n)	MC_R (0x5008 + (n<<12))	/* Регистр принятого управляющего символа */
#define MC_SWIC_MODE_CR(n)	MC_R (0x500C + (n<<12))	/* Регистр управления режимом работы */
#define MC_SWIC_TX_SPEED(n)	MC_R (0x5010 + (n<<12))	/* Регистр управления скоростью передачи */
#define MC_SWIC_TX_CODE(n)	MC_R (0x5014 + (n<<12))	/* Регистр передаваемого управляющего символа */
#define MC_SWIC_RX_SPEED(n)	MC_R (0x5018 + (n<<12))	/* Регистр измерителя скорости приема */
#define MC_SWIC_CNT_RX_PACK(n)	MC_R (0x501C + (n<<12))	/* Регистр счетчика принятых пакетов ненулевой длины */
#define MC_SWIC_CNT_RX0_PACK(n)	MC_R (0x5020 + (n<<12))	/* Регистр счетчика принятых пакетов нулевой длины */
#define MC_SWIC_ISR_L(n)	MC_R (0x5024 + (n<<12))	/* Регистр кодов распределенных прерываний (младшая часть) */
#define MC_SWIC_ISR_H(n)	MC_R (0x5028 + (n<<12))	/* Регистр кодов распределенных прерываний (старшая часть) */

/* Каналы записи в память дескрипторов принимаемых пакетов SpaceWire */
#define MC_SWIC_RX_DESC_CSR(n)	MC_R (0x0200 + (n<<8))	/* Регистр управления и состояния */
#define MC_SWIC_RX_DESC_CP(n)	MC_R (0x0208 + (n<<8))	/* Регистр указателя цепочки */
#define MC_SWIC_RX_DESC_IR(n)	MC_R (0x020C + (n<<8))	/* Индексный регистр памяти */
#define MC_SWIC_RX_DESC_OR(n)	MC_R (0x0210 + (n<<8))	/* Регистр смещения памяти */
#define MC_SWIC_RX_DESC_Y(n)	MC_R (0x0214 + (n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
#define MC_SWIC_RX_DESC_RUN(n)	MC_R (0x0218 + (n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/* Каналы записи в память принимаемых слов данных SpaceWire */
#define MC_SWIC_RX_DATA_CSR(n)	MC_R (0x0240 + (n<<8))	/* Регистр управления и состояния */
#define MC_SWIC_RX_DATA_CP(n)	MC_R (0x0248 + (n<<8))	/* Регистр указателя цепочки */
#define MC_SWIC_RX_DATA_IR(n)	MC_R (0x024C + (n<<8))	/* Индексный регистр памяти */
#define MC_SWIC_RX_DATA_OR(n)	MC_R (0x0250 + (n<<8))	/* Регистр смещения памяти */
#define MC_SWIC_RX_DATA_Y(n)	MC_R (0x0254 + (n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
#define MC_SWIC_RX_DATA_RUN(n)	MC_R (0x0258 + (n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/* Каналы чтения из памяти дескрипторов передаваемых пакетов SpaceWire */
#define MC_SWIC_TX_DESC_CSR(n)	MC_R (0x0280 + (n<<8))	/* Регистр управления и состояния */
#define MC_SWIC_TX_DESC_CP(n)	MC_R (0x0288 + (n<<8))	/* Регистр указателя цепочки */
#define MC_SWIC_TX_DESC_IR(n)	MC_R (0x028C + (n<<8))	/* Индексный регистр памяти */
#define MC_SWIC_TX_DESC_OR(n)	MC_R (0x0290 + (n<<8))	/* Регистр смещения памяти */
#define MC_SWIC_TX_DESC_Y(n)	MC_R (0x0294 + (n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
#define MC_SWIC_TX_DESC_RUN(n)	MC_R (0x0298 + (n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/* Каналы чтения из памяти передаваемых слов данных SpaceWire */
#define MC_SWIC_TX_DATA_CSR(n)	MC_R (0x02C0 + (n<<8))	/* Регистр управления и состояния */
#define MC_SWIC_TX_DATA_CP(n)	MC_R (0x02C8 + (n<<8))	/* Регистр указателя цепочки */
#define MC_SWIC_TX_DATA_IR(n)	MC_R (0x02CC + (n<<8))	/* Индексный регистр памяти */
#define MC_SWIC_TX_DATA_OR(n)	MC_R (0x02D0 + (n<<8))	/* Регистр смещения памяти */
#define MC_SWIC_TX_DATA_Y(n)	MC_R (0x02D4 + (n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
#define MC_SWIC_TX_DATA_RUN(n)	MC_R (0x02D8 + (n<<8))	/* Псевдорегистр управления состоянием бита RUN */

#endif /* MC_HAVE_SWIC */

/*
 * Регистры DMA
 */

/* Каналы LpCh0...LpCh3 */
#define MC_CSR_LPCH(n)	MC_R (0x0400+(n<<8))	/* Регистр управления и состояния */
#define MC_CP_LPCH(n)	MC_R (0x0408+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_LPCH(n)	MC_R (0x040C+(n<<8))	/* Индексный регистр памяти */
#define MC_OR_LPCH(n)	MC_R (0x0410+(n<<8))	/* Регистр смещения памяти */
#define MC_Y_LPCH(n)	MC_R (0x0414+(n<<8))	/* Регистр параметров направления Y при
						 * двухмерной адресации памяти */
/* Каналы MemCh0...MemCh3 */
#define MC_CSR_MEMCH(n)	MC_R (0x0800+(n<<8))	/* Регистр управления и состояния */
#define MC_IOR_MEMCH(n)	MC_R (0x0804+(n<<8))	/* Регистр индекса и смещения внутренней памяти */
#define MC_CP_MEMCH(n)	MC_R (0x0808+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_MEMCH(n)	MC_R (0x080C+(n<<8))	/* Индексный регистр внешней памяти */
#define MC_OR_MEMCH(n)	MC_R (0x0810+(n<<8))	/* Регистр смещения внешней памяти */
#define MC_Y_MEMCH(n)	MC_R (0x0814+(n<<8))	/* Регистр параметров направления Y при
						 * двухмерной адресации внешней памяти */
#define MC_RUN(n)	MC_R (0x0818+(n<<8))	/* Псевдорегистр управления состоянием бита RUN */

#endif /* _IO_MC24_H */
