/*
 * Hardware register defines for Elvees MC-0226 microcontroller.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
 *               2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
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
#ifndef _IO_MC0226_H
#define _IO_MC0226_H

#define MC_R(a)		*(volatile unsigned*)(0xB82F0000 + (a))

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
#define MC_RUN_LPCH(n)	MC_R (0x0818+(n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/* Каналы MemCh0...MemCh3 */
#define MC_CSR_MEMCH(n)	MC_R (0x0800+(n<<8))	/* Регистр управления и состояния */
#define MC_IOR_MEMCH(n)	MC_R (0x0804+(n<<8))	/* Регистр индекса и смещения внутренней памяти */
#define MC_CP_MEMCH(n)	MC_R (0x0808+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_MEMCH(n)	MC_R (0x080C+(n<<8))	/* Индексный регистр внешней памяти */
#define MC_OR_MEMCH(n)	MC_R (0x0810+(n<<8))	/* Регистр смещения внешней памяти */
#define MC_Y_MEMCH(n)	MC_R (0x0814+(n<<8))	/* Регистр параметров направления Y при
						 * двухмерной адресации внешней памяти */
#define MC_RUN_MEMCH(n)	MC_R (0x0818+(n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры линковых портов LPORT0...LPORT3
 */
#define MC_LTX(n)	MC_R (0x7000+(n<<12))	/* Буфер передачи */
#define MC_LRX(n)	MC_R (0x7000+(n<<12))	/* Буфер приема */
#define MC_LCSR(n)	MC_R (0x7004+(n<<12))	/* Регистр управления и состояния */
#define MC_LDIR(n)	MC_R (0x7008+(n<<12))	/* Регистр управления */
#define MC_LDR(n)	MC_R (0x700C+(n<<12))	/* Регистр данных */

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
 * Регистры порта внешней памяти
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100C)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_CKE_CTR	MC_R (0x1018)	/* Регистр управления состоянием вывода CKE микросхемы */

/*
 * Регистры контроллера PCI
 */
#define MC_PCI_DEVICE_VENDOR_ID MC_R (0x2000)	/* Идентификация устройства */
#define MC_PCI_STATUS_COMMAND	MC_R (0x2004)	/* Состояние и управление */
#define MC_PCI_CLASS_REVISION	MC_R (0x2008)	/* Регистр кода */
#define MC_PCI_LATENCY_TIMER	MC_R (0x200c)	/* Таймер времени передачи (MLT) */
#define MC_PCI_BAR0		MC_R (0x2010)	/* Базовый адрес 0 */
#define MC_PCI_BAR1		MC_R (0x2014)	/* Базовый адрес 0 */
#define MC_PCI_INTERRUPT_LINE	MC_R (0x203c)	/* Код прерывания */
#define MC_PIC_IR_SLAVE		MC_R (0x2040)	/* Индексный регистр адреса памяти 
						   при обмене данными с 
                                             	   PCI в режиме Slave */
#define MC_PCI_SEM		MC_R (0x44)	/* Семафор */
#define MC_PCI_MBR		MC_R (0x48)	/* Почтовый ящик */
#define MC_PCI_CSR_PCI		MC_R (0x4C)	/* Управление шиной PCI */
#define MC_PCI_CSR_MASTER	MC_R (0x50)	/* Состояние и управление обменом 
						   в режиме Master */
#define MC_PCI_IR_MASTER	MC_R (0x54)	/* Индексный регистр адреса памяти 
						   при обмене в режиме Master */
#define MC_PCI_AR_PCI		MC_R (0x58)	/* Адресный регистр PCI */
#define MC_PCI_QSTR_PCI		MC_R (0x5C)	/* Системные прерывания */

/*
 * Системные регистры
 */
#define MC_MASKR	MC_R (0x4000)	/* Регистр маски */
#define MC_QSTR		MC_R (0x4004)	/* Регистр заявок */
#define MC_CSR		MC_R (0x4008)	/* Регистр управления */

#endif /* _IO_MC0266_H */
