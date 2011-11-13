/*
 * Hardware register defines for Elvees MC24R2 microcontroller.
 *
 * Copyright (C) 2011 Elvees
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
#ifndef _IO_MC24R2_H
#define _IO_MC24R2_H

#define MC_BASE_ADDR	0xB82F0000
#define MC_R(a)		*(volatile unsigned*)(MC_BASE_ADDR + (a))
#define MC_A(a)		(MC_BASE_ADDR + (a))

/*
 * Регистры DMA MEM(0-3)
 */
#define MC_CSR_MEM(n)	MC_R (0x0000+(n<<7))	/* Управление и состояние */
#define MC_CP_MEM(n)	MC_R (0x0004+(n<<7))	/* Указатель цепочки */
#define MC_IR0_MEM(n)	MC_R (0x0008+(n<<7))	/* Индекс "0" внешней памяти */
#define MC_IR1_MEM(n)	MC_R (0x000C+(n<<7))	/* Индекс "1" внешней памяти */
#define MC_OR_MEM(n)	MC_R (0x0010+(n<<7))	/* Смещение внешней памяти */
#define MC_Y_MEM(n)	MC_R (0x0014+(n<<7))	/* Параметры направления Y при
						 * двухмерной адресации */
#define MC_RUN_MEM(n)	MC_R (0x0018+(n<<7))	/* Управление состоянием бита RUN */

/*
 * Регистры DMA VPIN
 */
#define MC_DMACSR_VPIN	MC_R (0x8800)   /* Управление и состояние */
#define MC_CP_VPIN	MC_R (0x8804)   /* Указатель цепочки */
#define MC_IR_VPIN	MC_R (0x8808)   /* Индекс */
#define MC_OR_VPIN	MC_R (0x880C)   /* Смещение */
#define MC_Y_VPIN	MC_R (0x8810)   /* Параметры направления Y при двухмерной адресации */
#define MC_RUN_VPIN	MC_R (0x8814)   /* Управления состоянием бита RUN */

/*
 * Регистры DMA VPOUT
 */
#define MC_DMACSR_VPOUT	MC_R (0x9800)   /* Управление и состояние */
#define MC_CP_VPOUT	MC_R (0x9804)   /* Указатель цепочки */
#define MC_IR_VPOUT	MC_R (0x9808)   /* Индекс */
#define MC_OR_VPOUT	MC_R (0x980C)   /* Смещение */
#define MC_Y_VPOUT	MC_R (0x9810)   /* Параметры направления Y при двухмерной адресации */
#define MC_RUN_VPOUT	MC_R (0x9814)   /* Управление состоянием бита RUN */

/*
 * Регистры DMA MFBSP(0-3)
 */
#define MC_CSR_MFBSP_RX(n)	MC_R (0x7800+(n<<6))	/* Управление и состояние */
#define MC_CP_MFBSP_RX(n)	MC_R (0x7804+(n<<6))	/* Указатель цепочки */
#define MC_IR_MFBSP_RX(n)	MC_R (0x7808+(n<<6))	/* Индекс */
#define MC_RUN_MFBSP_RX(n)	MC_R (0x780C+(n<<6))	/* Управления состоянием бита RUN */

#define MC_CSR_MFBSP_TX(n)	MC_R (0x7C00+(n<<6))	/* Управление и состояние */
#define MC_CP_MFBSP_TX(n)	MC_R (0x7C04+(n<<6))	/* Указатель цепочки */
#define MC_IR_MFBSP_TX(n)	MC_R (0x7C08+(n<<6))	/* Индекс */
#define MC_RUN_MFBSP_TX(n)	MC_R (0x7C0C+(n<<6))	/* Управления состоянием бита RUN */


/*
 * Регистры UART0
 */
#define MC_UART0_BASE	MC_A (0x3000)
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
 * Регистры интервального таймера IT
 */
#define MC_ITCSR	MC_R (0xD000)	/* Управление */
#define MC_ITPERIOD	MC_R (0xD004)	/* Период работы таймера */
#define MC_ITCOUNT	MC_R (0xD008)	/* Счетчик */
#define MC_ITSCALE	MC_R (0xD00C)	/* Предделитель */

/*
 * Регистры WDT
 */
#define MC_WTCSR	MC_R (0xD010)	/* Управление */
#define MC_WTPERIOD	MC_R (0xD014)	/* Период работы таймера */
#define MC_WTCOUNT	MC_R (0xD018)	/* Счетчик */
#define MC_WTSCALE	MC_R (0xD01C)	/* Предделитель */

/*
 * Регистры RTT
 */
#define MC_RTCSR	MC_R (0xD020)	/* Управление */
#define MC_RTPERIOD	MC_R (0xD024)	/* Период работы таймера */
#define MC_RTCOUNT	MC_R (0xD028)	/* Счетчик */

/*
 * Регистры MFBSP(0-3)
 */
#define MC_MFBSP_TX(n)		MC_R (0x7000+(n<<8))   /* Буфер передачи данных */
#define MC_MFBSP_RX(n)		MC_R (0x7000+(n<<8))   /* Буфер приема данных */
#define MC_MFBSP_CSR(n)		MC_R (0x7004+(n<<8))   /* Управление и состояние */
#define MC_MFBSP_DIR(n)		MC_R (0x7008+(n<<8))   /* Управление направлением
							* порта ввода-вывода */
#define MC_MFBSP_GPIO_DR(n)	MC_R (0x700C+(n<<8))   /* Данные порта ввода-вывода */
#define MC_MFBSP_TCTR(n)	MC_R (0x7010+(n<<8))   /* Управление передатчиком */
#define MC_MFBSP_RCTR(n)	MC_R (0x7014+(n<<8))   /* Управление приёмником */
#define MC_MFBSP_TSR(n)		MC_R (0x7018+(n<<8))   /* Состояние передатчика */
#define MC_MFBSP_RSR(n)		MC_R (0x701C+(n<<8))   /* Состояние приёмника */
#define MC_MFBSP_TCTR_RATE(n)	MC_R (0x7020+(n<<8))   /* Управление темпом передачи данных */
#define MC_MFBSP_RCTR_RATE(n)	MC_R (0x7024+(n<<8))   /* Управление темпом приёма данных */

/*
 * Регистры VPIN
 */
#define MC_CSR_VPIN		MC_R (0x8000)	/* Управление и состояние */
#define MC_CNT_VPIN		MC_R (0x8004)	/* Счетчик строк / счетчик пикселов */
#define MC_FRAME_CNT_VPIN	MC_R (0x8008)	/* Счетчик кадров */
#define MC_FIFO_VPIN		MC_R (0x800C)	/* Выход FIFO */

/*
 * Регистры VPOUT
 */
#define MC_CSR_VPOUT		MC_R (0x9000)	/* Управление и состояние */
#define MC_DIV			MC_R (0x9004)	/* Период сигнала VCLKO_out */
#define MC_HSTART_HEND		MC_R (0x9008)	/* Начало/конец активной части строки */
#define MC_VSTART_VEND		MC_R (0x900C)	/* Начало/конец активной части кадра */
#define MC_CNT_VPOUT		MC_R (0x9010)	/* Счетчик строк / счетчик пикселов */
#define MC_FRAME_CNT_VPOUT	MC_R (0x9014)	/* Счетчик кадров */
#define MC_FIFO_VPOUT		MC_R (0x901C)	/* Вход FIFO */

/*
 * Регистры USB
 */
#define MC_USB_CSR		MC_R (0xC000)	/* Управление и статус контроллера */
#define MC_USB_INT_CSR		MC_R (0xC004)	/* Управление и статус прерываний */
#define MC_USB_VENDOR_DATA	MC_R (0xC008)   /* Данные для передачи по Vendor-каналу */
#define MC_USB_VENDOR_INDEX	MC_R (0xC00C)   /* Указатель на данные по Vendor-каналу */
#define MC_USB_VENDOR_VALUE	MC_R (0xC010)   /* Принятые данные по Vendor-каналу */
#define MC_USB_CFG_ADDR		MC_R (0xC014)   /* Адрес массива конфигурации */
#define MC_USB_CFG_DATA		MC_R (0xC018)   /* Данные массива конфигурации */
#define MC_USB_REVISION		MC_R (0xC01C)   /* Номер ревизии */
#define MC_USB_CSR_EP1		MC_R (0xC020)   /* Управление и статус ЕР1 */
#define MC_USB_CSR_EP2		MC_R (0xC024)   /* Управление и статус ЕР2 */
#define MC_USB_CSR_EP3		MC_R (0xC028)   /* Управление и статус ЕР3 */
#define MC_USB_CSR_EP4		MC_R (0xC02C)   /* Управление и статус ЕР4 */

/*
 * Регистры SpaceWire
 */
#define MC_SWIC_HW_VER(n)	MC_R (0x5000 + ((n) << 12))	/* Регистр аппаратной версии контроллера */
#define MC_SWIC_STATUS(n)	MC_R (0x5004 + ((n) << 12))	/* Регистр состояния */
#define MC_SWIC_RX_CODE(n)	MC_R (0x5008 + ((n) << 12))	/* Регистр принятого управляющего символа */
#define MC_SWIC_MODE_CR(n)	MC_R (0x500C + ((n) << 12))	/* Регистр управления режимом работы */
#define MC_SWIC_TX_SPEED(n)	MC_R (0x5010 + ((n) << 12))	/* Регистр управления скоростью передачи */
#define MC_SWIC_TX_CODE(n)	MC_R (0x5014 + ((n) << 12))	/* Регистр передаваемого управляющего символа */
#define MC_SWIC_RX_SPEED(n)	MC_R (0x5018 + ((n) << 12))	/* Регистр измерителя скорости приема */
#define MC_SWIC_CNT_RX_PACK(n)	MC_R (0x501C + ((n) << 12))	/* Регистр счетчика принятых пакетов ненулевой длины */
#define MC_SWIC_CNT_RX0_PACK(n)	MC_R (0x5020 + ((n) << 12))	/* Регистр счетчика принятых пакетов нулевой длины */
#define MC_SWIC_ISR_L(n)	MC_R (0x5024 + ((n) << 12))	/* Регистр кодов распределенных прерываний (младшая часть) */
#define MC_SWIC_ISR_H(n)	MC_R (0x5028 + ((n) << 12))	/* Регистр кодов распределенных прерываний (старшая часть) */

/*
 * Регистры канала DMA записи в память дескрипторов принимаемых пакетов SWIC
 */
#define MC_SWIC_RX_DESC_CSR(n)	MC_R (0x5800 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_RX_DESC_CP(n)	MC_R (0x5804 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_RX_DESC_IR(n)	MC_R (0x5808 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_RX_DESC_RUN(n)	MC_R (0x580C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA записи в память принимаемых слов данных SWIC
 */
#define MC_SWIC_RX_DATA_CSR(n)	MC_R (0x5840 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_RX_DATA_CP(n)	MC_R (0x5844 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_RX_DATA_IR(n)	MC_R (0x5848 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_RX_DATA_RUN(n)	MC_R (0x584C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA чтения из памяти дескрипторов передаваемых пакетов SWIC
 */
#define MC_SWIC_TX_DESC_CSR(n)	MC_R (0x5880 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_TX_DESC_CP(n)	MC_R (0x5884 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_TX_DESC_IR(n)	MC_R (0x5888 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_TX_DESC_RUN(n)	MC_R (0x588C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA чтения из памяти передаваемых слов данных SWIC
 */
#define MC_SWIC_TX_DATA_CSR(n)	MC_R (0x58C0 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_TX_DATA_CP(n)	MC_R (0x58C4 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_TX_DATA_IR(n)	MC_R (0x58C8 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_TX_DATA_RUN(n)	MC_R (0x58CC + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры порта внешней памяти MPORT
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100C)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_SDRTMR	MC_R (0x1018)	/* Временные параметры памяти SDRAM */
#define MC_SDRCSR	MC_R (0x101C)	/* Управление режимами памяти SDRAM */

/*
 * Системные регистры
 */
#define MC_CRPLL	MC_R (0x4000)	/* Управление PLL */
#define MC_CLKEN	MC_R (0x4004)	/* Управление отключением частоты от устройств */
#define MC_CSR		MC_R (0x4008)	/* Управление и состояние */
#define MC_MASKR0	MC_R (0x4010)	/* Маска прерываний */
#define MC_QSTR0	MC_R (0x4014)	/* Запросы прерываний */
#define MC_MASKR1	MC_R (0x4018)	/* Маска прерываний */
#define MC_QSTR1	MC_R (0x401C)	/* Запросы прерываний*/
#define MC_MASKR2	MC_R (0x4020)	/* Маска прерываний */
#define MC_QSTR2	MC_R (0x4024)	/* Запросы прерываний */
#define MC_MASKR3	MC_R (0x4028)	/* Маска прерываний*/
#define MC_QSTR3	MC_R (0x402c)	/* Запросы прерываний*/
#define MC_IRQM		MC_R (0x4030)	/* Режимы внешних прерываний nIRQ[3:0] */

#endif /* _IO_MC24R2_H */
