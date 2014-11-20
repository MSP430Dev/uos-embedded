/*
 * Hardware register defines for Elvees NVCom-01 microcontroller.
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
#ifndef _IO_MCT02_H
#define _IO_MCT02_H

#define MC_R(a)		*(volatile unsigned*)(0xB82F0000 + (a))

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
 * Регистры DMA MFBSP(0-3)
 */
#define MC_CSR_MFBSP(n)	MC_R (0x7800+(n<<6))	/* Управление и состояние */
#define MC_CP_MFBSP(n)	MC_R (0x7804+(n<<6))	/* Указатель цепочки */
#define MC_IR_MFBSP(n)	MC_R (0x7808+(n<<6))	/* Индекс */
#define MC_RUN_MFBSP(n)	MC_R (0x780C+(n<<6))	/* Управления состоянием бита RUN */

/*
 * Регистры UART0, UART1, UART2, UART3
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

#define MC_RBR1		MC_R (0x3400)	/* Приемный буферный регистр */
#define MC_THR1		MC_R (0x3400)	/* Передающий буферный регистр */
#define MC_IER1		MC_R (0x3404)	/* Регистр разрешения прерываний */
#define MC_IIR1		MC_R (0x3408)	/* Регистр идентификации прерывания */
#define MC_FCR1		MC_R (0x3408)	/* Регистр управления FIFO */
#define MC_LCR1		MC_R (0x340C)	/* Регистр управления линией */
#define MC_MCR1		MC_R (0x3410)	/* Регистр управления модемом */
#define MC_LSR1		MC_R (0x3414)	/* Регистр состояния линии */
#define MC_MSR1		MC_R (0x3418)	/* Регистр состояния модемом */
#define MC_SPR1		MC_R (0x341C)	/* Регистр Scratch Pad */
#define MC_DLL1		MC_R (0x3400)	/* Регистр делителя младший */
#define MC_DLM1		MC_R (0x3404)	/* Регистр делителя старший */
#define MC_SCLR1	MC_R (0x3414)	/* Регистр предделителя (scaler) */

#define MC_RBR2		MC_R (0x3800)	/* Приемный буферный регистр */
#define MC_THR2		MC_R (0x3800)	/* Передающий буферный регистр */
#define MC_IER2		MC_R (0x3804)	/* Регистр разрешения прерываний */
#define MC_IIR2		MC_R (0x3808)	/* Регистр идентификации прерывания */
#define MC_FCR2		MC_R (0x3808)	/* Регистр управления FIFO */
#define MC_LCR2		MC_R (0x380C)	/* Регистр управления линией */
#define MC_MCR2		MC_R (0x3810)	/* Регистр управления модемом */
#define MC_LSR2		MC_R (0x3814)	/* Регистр состояния линии */
#define MC_MSR2		MC_R (0x3818)	/* Регистр состояния модемом */
#define MC_SPR2		MC_R (0x381C)	/* Регистр Scratch Pad */
#define MC_DLL2		MC_R (0x3800)	/* Регистр делителя младший */
#define MC_DLM2		MC_R (0x3804)	/* Регистр делителя старший */
#define MC_SCLR2	MC_R (0x3814)	/* Регистр предделителя (scaler) */

#define MC_RBR3		MC_R (0x3c00)	/* Приемный буферный регистр */
#define MC_THR3		MC_R (0x3c00)	/* Передающий буферный регистр */
#define MC_IER3		MC_R (0x3c04)	/* Регистр разрешения прерываний */
#define MC_IIR3		MC_R (0x3c08)	/* Регистр идентификации прерывания */
#define MC_FCR3		MC_R (0x3c08)	/* Регистр управления FIFO */
#define MC_LCR3		MC_R (0x3c0C)	/* Регистр управления линией */
#define MC_MCR3		MC_R (0x3c10)	/* Регистр управления модемом */
#define MC_LSR3		MC_R (0x3c14)	/* Регистр состояния линии */
#define MC_MSR3		MC_R (0x3c18)	/* Регистр состояния модемом */
#define MC_SPR3		MC_R (0x3c1C)	/* Регистр Scratch Pad */
#define MC_DLL3		MC_R (0x3c00)	/* Регистр делителя младший */
#define MC_DLM3		MC_R (0x3c04)	/* Регистр делителя старший */
#define MC_SCLR3	MC_R (0x3c14)	/* Регистр предделителя (scaler) */

/*
 * Регистры интервального таймера IT0, IT1
 */
#define MC_ITCSR	MC_R (0x5000)	/* Управление */
#define MC_ITPERIOD	MC_R (0x5004)	/* Период работы таймера */
#define MC_ITCOUNT	MC_R (0x5008)	/* Счетчик */
#define MC_ITSCALE	MC_R (0x500C)	/* Предделитель */

#define MC_ITCSR1	MC_R (0x5020)	/* Управление */
#define MC_ITPERIOD1 	MC_R (0x5024)	/* Период работы таймера */
#define MC_ITCOUNT1	MC_R (0x5028)	/* Счетчик */
#define MC_ITSCALE1	MC_R (0x502C)	/* Предделитель */

/*
 * Регистры WDT
 */
#define MC_WTCSR	MC_R (0x5010)	/* Управление */
#define MC_WTPERIOD	MC_R (0x5014)	/* Период работы таймера */
#define MC_WTCOUNT	MC_R (0x5018)	/* Счетчик */
#define MC_WTSCALE	MC_R (0x501C)	/* Предделитель */

/*
 * Регистры MFBSP(0-1) MCT03P
 */
#define MC_MFBSP_TX(n)		MC_R (0x8000+(n<<8))   /* Буфер передачи данных */
#define MC_MFBSP_RX(n)		MC_R (0x8000+(n<<8))   /* Буфер приема данных */
#define MC_MFBSP_CSR(n)		MC_R (0x8004+(n<<8))   /* Управление и состояние */
#define MC_MFBSP_DIR(n)		MC_R (0x8008+(n<<8))   /* Управление направлением
							* порта ввода-вывода */
#define MC_MFBSP_GPIO_DR(n)	MC_R (0x800C+(n<<8))   /* Данные порта ввода-вывода */
#define MC_MFBSP_TCTR(n)	MC_R (0x8010+(n<<8))   /* Управление передатчиком */
#define MC_MFBSP_RCTR(n)	MC_R (0x8014+(n<<8))   /* Управление приёмником */
#define MC_MFBSP_TSR(n)		MC_R (0x8018+(n<<8))   /* Состояние передатчика */
#define MC_MFBSP_RSR(n)		MC_R (0x801C+(n<<8))   /* Состояние приёмника */
#define MC_MFBSP_TCTR_RATE(n)	MC_R (0x8020+(n<<8))   /* Управление темпом передачи данных */
#define MC_MFBSP_RCTR_RATE(n)	MC_R (0x8024+(n<<8))   /* Управление темпом приёма данных */
#define MC_MFBSP_TSTART(n)	MC_R (0x8028+(n<<8))   /* Запуск/останов передатчика без изменения настроек передатчика */
#define MC_MFBSP_RSTART(n)	MC_R (0x802C+(n<<8))   /* Запуск/останов передатчика без изменения настроек приёмника */
#define MC_MFBSP_EMERG(n)	MC_R (0x8030+(n<<8))   /* Регистр аварийного управления портом */
#define MC_MFBSP_IMASK(n) 	MC_R (0x8034+(n<<8))   /* Регистр маски прерываний от порта */

/*
 * Регистры DMA MFBSP(0-1) MCT03P
 */
#define MC_CSR_MFBSP_RX(n)	MC_R (0x8840+(n<<12))	/* Управление и состояние */
#define MC_CP_MFBSP_RX(n)	MC_R (0x8844+(n<<12))	/* Указатель цепочки */
#define MC_IR_MFBSP_RX(n)	MC_R (0x8848+(n<<12))	/* Индекс */
#define MC_RUN_MFBSP_RX(n)	MC_R (0x884C+(n<<12))	/* Управления состоянием бита RUN */

#define MC_CSR_MFBSP_TX(n)	MC_R (0x8800+(n<<12))	/* Управление и состояние */
#define MC_CP_MFBSP_TX(n)	MC_R (0x8804+(n<<12))	/* Указатель цепочки */
#define MC_IR_MFBSP_TX(n)	MC_R (0x8808+(n<<12))	/* Индекс */
#define MC_RUN_MFBSP_TX(n)	MC_R (0x880C+(n<<12))	/* Управления состоянием бита RUN */

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
#define MC_CSREXT	MC_R (0x1024)	/* Регистр управления режимами контроля внешней памяти */
#define MC_SRTMR	MC_R (0x2000)	/* Временные параметры памяти SRAM */

/*
 * Системные регистры
 */
#define MC_CRPLL	MC_R (0x4000)	/* Управление PLL */
#define MC_CLKEN	MC_R (0x4004)	/* Управление отключением частоты от устройств */
#define MC_CSR		MC_R (0x4008)	/* Управление и состояние */
#define MC_MASKR0	MC_R (0x4010)	/* Маска прерываний */
#define MC_QSTR0	MC_R (0x4014)	/* Запросы прерываний от IT, RTT, WDT, UART[3:0], nIRQ[3:0] */
#define MC_MASKR1	MC_R (0x4018)	/* Маска прерываний от DMA MEM */
#define MC_QSTR1	MC_R (0x401C)	/* Запросы прерываний от DMA MEM */
#define MC_MASKR2	MC_R (0x4020)	/* Маска прерываний от SWIC */
#define MC_QSTR2	MC_R (0x4024)	/* Запросы прерываний от SWIC */
#define MC_MASKR3	MC_R (0x4028)	/* Маска прерываний от контроллера Хэмминга */
#define MC_QSTR3	MC_R (0x402c)	/* Запросы прерываний от контроллера Хэмминга */
#define MC_MASKR4	MC_R (0x4038)	/* Маска прерываний от MFBSP */
#define MC_QSTR4	MC_R (0x403c)	/* Запросы прерываний от MFBSP */
#define MC_IRQM		MC_R (0x4030)	/* Режимы внешних прерываний nIRQ[3:0] */

/*
 * Регистры SpaceWire
 */
#define MC_SWIC_HW_VER(n)	MC_R (0xC000 + ((n) << 12))	/* Регистр аппаратной версии контроллера */
#define MC_SWIC_STATUS(n)	MC_R (0xC004 + ((n) << 12))	/* Регистр состояния */
#define MC_SWIC_RX_CODE(n)	MC_R (0xC008 + ((n) << 12))	/* Регистр принятого управляющего символа */
#define MC_SWIC_MODE_CR(n)	MC_R (0xC00C + ((n) << 12))	/* Регистр управления режимом работы */
#define MC_SWIC_TX_SPEED(n)	MC_R (0xC010 + ((n) << 12))	/* Регистр управления скоростью передачи */
#define MC_SWIC_TX_CODE(n)	MC_R (0xC014 + ((n) << 12))	/* Регистр передаваемого управляющего символа */
#define MC_SWIC_RX_SPEED(n)	MC_R (0xC018 + ((n) << 12))	/* Регистр измерителя скорости приема */
#define MC_SWIC_CNT_RX_PACK(n)	MC_R (0xC01C + ((n) << 12))	/* Регистр счетчика принятых пакетов ненулевой длины */
#define MC_SWIC_CNT_RX0_PACK(n)	MC_R (0xC020 + ((n) << 12))	/* Регистр счетчика принятых пакетов нулевой длины */
#define MC_SWIC_ISR_L(n)	MC_R (0xC024 + ((n) << 12))	/* Регистр кодов распределенных прерываний (младшая часть) */
#define MC_SWIC_ISR_H(n)	MC_R (0xC028 + ((n) << 12))	/* Регистр кодов распределенных прерываний (старшая часть) */

/*
 * Регистры канала DMA записи в память дескрипторов принимаемых пакетов SWIC
 */
#define MC_SWIC_RX_DESC_CSR(n)	MC_R (0xC800 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_RX_DESC_CP(n)	MC_R (0xC804 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_RX_DESC_IR(n)	MC_R (0xC808 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_RX_DESC_RUN(n)	MC_R (0xC80C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA записи в память принимаемых слов данных SWIC
 */
#define MC_SWIC_RX_DATA_CSR(n)	MC_R (0xC840 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_RX_DATA_CP(n)	MC_R (0xC844 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_RX_DATA_IR(n)	MC_R (0xC848 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_RX_DATA_RUN(n)	MC_R (0xC84C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA чтения из памяти дескрипторов передаваемых пакетов SWIC
 */
#define MC_SWIC_TX_DESC_CSR(n)	MC_R (0xC880 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_TX_DESC_CP(n)	MC_R (0xC884 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_TX_DESC_IR(n)	MC_R (0xC888 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_TX_DESC_RUN(n)	MC_R (0xC88C + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Регистры канала DMA чтения из памяти передаваемых слов данных SWIC
 */
#define MC_SWIC_TX_DATA_CSR(n)	MC_R (0xC8C0 + ((n) << 12))	/* Регистр управления и состояния канала */
#define MC_SWIC_TX_DATA_CP(n)	MC_R (0xC8C4 + ((n) << 12))	/* Регистр указателя цепочки канала */
#define MC_SWIC_TX_DATA_IR(n)	MC_R (0xC8C8 + ((n) << 12))	/* Индексный регистр внешней памяти канала */
#define MC_SWIC_TX_DATA_RUN(n)	MC_R (0xC8CC + ((n) << 12))	/* Псевдорегистр управления состоянием бита RUN */


#endif /* _IO_MCT02_H */
