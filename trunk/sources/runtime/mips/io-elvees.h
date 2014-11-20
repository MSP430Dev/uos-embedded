/*
 * Hardware register defines for all Elvees MIPS microcontrollers.
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
#ifndef _IO_ELVEES_H
#define _IO_ELVEES_H

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

#ifdef ELVEES_MC24
#define ST_IM_IRQ0	0x00000400	/* внешнее прерывание 0 */
#define ST_IM_IRQ1	0x00000800	/* внешнее прерывание 1 */
#define ST_IM_IRQ2	0x00001000	/* внешнее прерывание 2 */
#define ST_IM_IRQ3	0x00002000	/* внешнее прерывание 3 */
#define ST_IM_MCU	0x00008000	/* от внутренних устройств микроконтроллера */
#endif

#ifdef ELVEES_MC0226
#define ST_IM_IRQ0	0x00000400	/* внешнее прерывание 0 */
#define ST_IM_IRQ1	0x00000800	/* внешнее прерывание 1 */
#define ST_IM_IRQ2	0x00001000	/* внешнее прерывание 2 */
#define ST_IM_IRQ3	0x00002000	/* внешнее прерывание 3 */
#define ST_IM_MCU	0x00008000	/* от внутренних устройств микроконтроллера и DMA*/
#define ST_CU2		0x40000000	/* разрешение сопроцессора 2 */
#define ST_CU3		0x80000000	/* разрешение сопроцессора 3 */
#endif

#ifdef ELVEES_MC24R2
#define ST_IM_QSTR0	0x00000400
#define ST_IM_QSTR1	0x00000800
#define ST_IM_QSTR2	0x00001000
#define ST_IM_QSTR3	0x00002000
#define ST_IM_DSP	0x00004000
#define ST_IM_COMPARE	0x00008000
#endif

#ifdef ELVEES_NVCOM01
#define ST_IM_QSTR0	0x00000400	/* от внутренних устройств микроконтроллера */
#define ST_IM_QSTR1	0x00000800	/* от DMA MEM */
#define ST_IM_QSTR2	0x00001000	/* от MFBSP */
#define ST_IM_DSP	0x00004000	/* от DSP */
#define ST_IM_COMPARE	0x00008000	/* от таймера 0 */
#endif

#ifdef ELVEES_NVCOM02
/* TODO */
#define ST_IM_QSTR0	0x00000400	/* от внутренних устройств микроконтроллера */
#define ST_IM_QSTR1	0x00000800	/* от DMA MEM */
#define ST_IM_QSTR2	0x00001000	/* от MFBSP */
#define ST_IM_DSP	0x00004000	/* от DSP */
#define ST_IM_COMPARE	0x00008000	/* от таймера 0 */
#endif

#ifdef ELVEES_MCT02
#define ST_IM_QSTR0	0x00000400	/* от IT, RTT, WDT, UART[3:0], nIRQ[3:0] */
#define ST_IM_QSTR1	0x00000800	/* от DMA MEM */
#define ST_IM_QSTR2	0x00001000	/* от SWIC */
#define ST_IM_QSTR3	0x00002000	/* от контроллера Хэмминга */
#define ST_IM_QSTR4	0x00004000	/* от MFBSP */
#define ST_IM_COMPARE	0x00008000	/* от таймера 0 */
#endif

#ifdef ELVEES_MCT03P
#define ST_IM_QSTR0	0x00000400	/* от IT, RTT, WDT, UART[3:0], nIRQ[3:0] */
#define ST_IM_QSTR1	0x00000800	/* от DMA MEM */
#define ST_IM_QSTR2	0x00001000	/* от SWIC */
#define ST_IM_QSTR3	0x00002000	/* от контроллера Хэмминга */
#define ST_IM_QSTR4	0x00004000	/* от MFBSP */
#define ST_IM_COMPARE	0x00008000	/* от таймера 0 */
#endif

#ifdef ELVEES_MC0428
#define ST_IM_QSTR0	0x00000400	/* от IT0, IT1, WDT, VPOUT, VPIN, ENET, MBR, PMCh, I2C, UART, nIRQ[3:0] */
#define ST_IM_QSTR1	0x00000800	/* от DMA MEM */
#define ST_IM_QSTR2	0x00001000	/* от SRIO */
#define ST_IM_QSTR3	0x00002000	/* от SWIC, MFBSP */
#define ST_IM_DSP	0x00004000	/* от DSP */
#define ST_IM_COMPARE	0x00008000	/* от таймера 0 */
#endif

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


/*-----------------------------------------------------------------------------
 *
 * Регистры интерфейсов Space Wire, размещенных в контроллере (SWIC0, SWIC1)
 */
#ifdef MC_HAVE_SWIC

/*
 * Маски для установки отдельных полей регистров
 */

/* STATUS */
#define MC_SWIC_DC_ERR		0x00000001	/* Признак ошибки рассоединения */
#define MC_SWIC_P_ERR		0x00000002	/* Признак ошибки четности */
#define MC_SWIC_ESC_ERR		0x00000004	/* Признак ошибки в ESC-последовательности */
#define MC_SWIC_CREDIT_ERR	0x00000008	/* Признак ошибки кредитования */
#define MC_SWIC_DS_STATE	0x000000E0	/* Состояние DS-макроячейки */
#define MC_SWIC_RX_BUF_FULL	0x00000100	/* Буфер приема полон */
#define MC_SWIC_RX_BUF_EMPTY	0x00000200	/* Буфер приема пуст */
#define MC_SWIC_TX_BUF_FULL	0x00000400	/* Буфер передачи полон */
#define MC_SWIC_TX_BUF_EMPTY	0x00000800	/* Буфер передачи пуст */
#define MC_SWIC_GOT_FIRST_BIT	0x00001000	/* Признак получения первого бита */
#define MC_SWIC_CONNECTED	0x00002000	/* Признак установленного соединения */
#define MC_SWIC_GOT_TIME	0x00004000	/* Принят маркер времени из сети */
#define MC_SWIC_GOT_INT		0x00008000	/* Принят код распределенного прерывания из сети */
#define	MC_SWIC_GOT_POLL	0x00010000	/* Принят poll-код из сети */
#define MC_SWIC_FL_CONTROL	0x00020000	/* Признак занятости передачей управляющего кода */
#define MC_SWIC_IRQ_LINK	0x00040000	/* Состояние запроса прерырывания LINK */
#define MC_SWIC_IRQ_TIM		0x00080000	/* Состояние запроса прерырывания TIM */
#define MC_SWIC_IRQ_ERR		0x00100000	/* Состояние запроса прерырывания ERR */

/* Значения поля DS_STATE регистра STATUS */
#define MC_SWIC_DS_ERROR_RESET	0
#define MC_SWIC_DS_ERROR_WAIT	1
#define MC_SWIC_DS_READY	2
#define MC_SWIC_DS_STARTED	3
#define MC_SWIC_DS_CONNECTING	4
#define MC_SWIC_DS_RUN		5

/* RX_CODE */
#define MC_SWIC_TIME_CODE	0x000000FF	/* Значение маркера времени, принятого из сети последним */
#define MC_SWIC_INT_CODE	0x0000FF00	/* Значение кода распределенного прерывания, принятого из сети последним */
#define MC_SWIC_POLE_CODE	0x00FF0000	/* Значение poll-кода, принятого из сети последним */

/* MODE_CR */
#define MC_SWIC_LinkDisabled	0x00000001	/* Установка LinkDisabled для блока DS-кодирования */
#define MC_SWIC_AutoStart	0x00000002	/* Установка AutoStart для блока DS-кодирования */
#define MC_SWIC_LinkStart	0x00000004	/* Установка LinkStart для блока DS-кодирования */
#define MC_SWIC_RX_RST		0x00000008	/* Установка блока приема в начальное состояние */
#define MC_SWIC_TX_RST		0x00000010	/* Установка блока передачи в начальное состояние */
#define MC_SWIC_DS_RST		0x00000020	/* Установка DS-макроячейки в начальное состояние */
#define MC_SWIC_SWCORE_RST	0x00000040	/* Установка контроллера в начальное состояние */
#define MC_SWIC_WORK_TYPE	0x00000100	/* Тип режима работы */
#define MC_SWIC_TIMING_WR_EN	0x00004000	/* Разрешение записи в поле TIMING регистра TX_SPEED */
#define MC_SWIC_AUTO_TX_SPEED	0x00008000	/* Признак автоматического установления скорости передачи 
						   после соединения (см. Спецификацию!!!) */
#define MC_SWIC_LINK_MASK	0x00040000	/* Маска прерывания LINK */
#define MC_SWIC_TIM_MASK	0x00080000	/* Маска прерывания TIM */
#define MC_SWIC_ERR_MASK	0x00100000	/* Маска прерывания ERR */

/* TX_SPEED */
#define MC_SWIC_TX_SPEED_PRM_MASK	0xFF		/* Маска коэффициента умножения TX_PLL */
#define MC_SWIC_TX_SPEED_PRM(x)	((x) & 0xFF)		/* Установка коэффициента умножения TX_PLL */
#define MC_SWIC_PLL_TX_EN	0x00000100		/* Разрешение работы TX_PLL */
#define MC_SWIC_LVDS_EN		0x00000200		/* Разрешение работы приемопередатчиков LVDS */
#define MC_SWIC_TX_SPEED_CON(x)	(((x) & 0xFF) << 10)	/* Скорость передачи данных при установлении соединения */
#define MC_SWIC_TIMING(x)	(((x) & 0xF) << 20)	/* В это поле необходимо записать код, равный тактовой 
							   частоте работы CPU, деленной на 10*/

/* TX_CODE */
#define MC_SWIC_TXCODE		0x0000001F	/* Управляющий код (содержимое) */
#define MC_SWIC_CODETYPE	0x000000E0	/* Признак кода управления */

/* Значение поля CODETYPE регистра TX_CODE */
#define MC_SWIC_CODETYPE_TIME	2
#define MC_SWIC_CODETYPE_INT	3
#define MC_SWIC_CODETYPE_POLL	5

/* Прерывания для SWIC */
#define MC_SWIC0_IRQ		5
#define MC_SWIC0_TX_DESC_IRQ	0
#define MC_SWIC0_TX_DATA_IRQ	1
#define MC_SWIC0_RX_DESC_IRQ	2
#define MC_SWIC0_RX_DATA_IRQ	3
#define MC_SWIC1_IRQ		6
#define MC_SWIC1_TX_DESC_IRQ	15
#define MC_SWIC1_TX_DATA_IRQ	16
#define MC_SWIC1_RX_DESC_IRQ	17
#define MC_SWIC1_RX_DATA_IRQ	18

#define MC_SWIC_STATUS_BITS "\20"\
"\1DC_ERR\2P_ERR\3ESC_ERR\4CREDIT_ERR\6DS_STATE0\7DS_STATE1\10DS_STATE2"\
"\11RX_BUF_FULL\12RX_BUF_EMPTY\13TX_BUF_FULL\14TX_BUF_EMPTY\15CONNECTED"\
"\17GOT_TIME\20GOT_INT\21GOT_POLL\22FL_CONTROL\23IRQ_LINK\24IRQ_TIM\25IRQ_ERR"

#define MC_SWIC_MODE_CR_BITS "\20"\
"\1LinkDisabled\2AutoStart\3LinkStart\4RX_RST\5TX_RST\6DS_RST\7SWCORE_RST"\
"\11WORK_TYPE\12TX_single\13RX_single\14LVDS_Loopback\15CODEC_Loopback"\
"\16DS_Loopback\21LINK_MASK\24TIM_MASK\25ERR_MASK"

#endif /* MC_HAVE_SWIC */

/*
 * Регистры DMA
 */

/* Регистр CSR для каналов DMA */
#define MC_DMA_CSR_RUN		0x00000001	/* Состояние работы канала DMA */
#define MC_DMA_CSR_DIR		0x00000002	/* Направление передачи для каналов MEM_CH */
#define MC_DMA_CSR_WN(n)	((n) << 2)	/* Установка длины пачки */
#define MC_DMA_CSR_EN64		0x00000040	/* Передача 64-разрядных данных (для MEM_CH) */
#define MC_DMA_CSR_IPD		0x00000040	/* Запрет прерывания по запросу от порта при выключенном канале DMA(RUN=0) */
#define MC_DMA_CSR_START_DSP	0x00000080	/* Разрешение запуска DSP (для MEM_CH) */
#define MC_DMA_CSR_MODE		0x00000100	/* Режим модификация адреса регистра IR0 */
#define MC_DMA_CSR_2D		0x00000200	/* Режим модификации адреса регистра IR1 */
#define MC_DMA_CSR_CHEN		0x00001000	/* Признак разрешения самоинициализации */
#define MC_DMA_CSR_IM		0x00002000	/* Маска прерывания при окончании передачи блока */
#define MC_DMA_CSR_END		0x00004000	/* Признак завершения передачи блока данных */
#define MC_DMA_CSR_DONE		0x00008000	/* Признак завершения передачи цепочки блоков данных */
#define MC_DMA_CSR_WCX_MASK	0xffff0000	/* Маска счетчика слов */
#define MC_DMA_CSR_WCX(n)	((n) << 16)	/* Установка счетчика слов */

/* Регистр OR для каналов DMA типа MEM_CH */
#define MC_DMA_OR0(n)		(n)		/* Приращение регистра IR0 (в словах) */
#define MC_DMA_OR1(n)		((n) << 16)	/* Приращение регистра IR1 (в словах) */


/* Псевдорегистр управления RUN */
#define MC_DMA_RUN		0x00000001	/* Управление битом RUN */

/*--------------------------------------
 * Системный регистр CSR
 */
#define MC_CSR_FM		0x00000001	/* Fixed mapping */
#define MC_CSR_TR		0x00000002	/* vectors in CRAM */
#define MC_CSR_TST_CACHE	0x00000800	/* random access to cache */
#define MC_CSR_FLUSH_I		0x00001000	/* instruction cache invalidate */
#define MC_CSR_FLUSH_D		0x00004000	/* data cache invalidate */

#if defined (ELVEES_MC24) || defined (ELVEES_MC0226)
#define MC_CSR_CLK(n)		((n) << 4)	/* PLL clock multiply, 1..31, 0=1/16 */
#define MC_CSR_CLKEN		0x00010000	/* PLL clock enable */
#endif

#if defined (ELVEES_MC0226)
#define MC_CSR_SNS_DSP		0x01000000	/* DSP0 and DSP1 synchronous start */
#define MC_CSR_SNW_DSP		0x02000000	/* DSP0 and DSP1 synchronous run status */
#endif

/*
 * Системный регистр MASKR
 */
#ifdef ELVEES_MC24
#define MC_MASKR_SRX0		0x00000001	/* SPORT0 receive */
#define MC_MASKR_STX0		0x00000002	/* SPORT0 transmit */
#define MC_MASKR_SRX1		0x00000004	/* SPORT0 receive */
#define MC_MASKR_STX1		0x00000008	/* SPORT0 transmit */
#define MC_MASKR_UART		0x00000010	/* UART */
#define MC_MASKR_LTRX0		0x00000080	/* LPORT0 data */
#define MC_MASKR_LSRQ0		0x00000100	/* LPORT0 service */
#define MC_MASKR_LTRX1		0x00000200	/* LPORT1 data */
#define MC_MASKR_LSRQ1		0x00000400	/* LPORT1 service */
#define MC_MASKR_LTRX2		0x00000800	/* LPORT2 data */
#define MC_MASKR_LSRQ2		0x00001000	/* LPORT2 service */
#define MC_MASKR_LTRX3		0x00002000	/* LPORT3 data */
#define MC_MASKR_LSRQ3		0x00004000	/* LPORT3 service */
#define MC_MASKR_COMPARE	0x00080000	/* CPU timer */
#define MC_MASKR_MEMCH0		0x00200000	/* DMA MemCh0 */
#define MC_MASKR_MEMCH1		0x00400000	/* DMA MemCh1 */
#define MC_MASKR_MEMCH2		0x00800000	/* DMA MemCh2 */
#define MC_MASKR_MEMCH3		0x01000000	/* DMA MemCh3 */
#define MC_MASKR_TIMER		0x20000000	/* timers IT, WDT, RTT */
#define MC_MASKR_PI		0x40000000	/* DSP software interrupt */
#define MC_MASKR_SBS		0x80000000	/* DSP status */

/* For MC24RT3 */
#define MC_MASKR_TxDesCh0	0x00000001	/* TX DESC DMA for SWIC0 */
#define MC_MASKR_TxDatCh0	0x00000002	/* TX DATA DMA for SWIC0 */
#define MC_MASKR_RxDesCh0	0x00000004	/* RX DESC DMA for SWIC0 */
#define MC_MASKR_RxDatCh0	0x00000008	/* RX DATA DMA for SWIC0 */
#define MC_MASKR_SWIC0		0x00000020	/* SWIC0 */
#define MC_MASKR_SWIC1		0x00000040	/* SWIC1 */
#define MC_MASKR_TxDesCh1	0x00008000	/* TX DESC DMA for SWIC1 */
#define MC_MASKR_TxDatCh1	0x00010000	/* TX DATA DMA for SWIC1 */
#define MC_MASKR_RxDesCh1	0x00020000	/* RX DESC DMA for SWIC1 */
#define MC_MASKR_RxDatCh1	0x00040000	/* RX DATA DMA for SWIC1 */
#endif

#ifdef ELVEES_MC0226
#define MC_MASKR_PMCH		0x00000001	/* DMA PMCh */
#define MC_MASKR_MBR		0x00000002	/* write into PCI MBR */
#define MC_MASKR_UART		0x00000010	/* UART */
#define MC_MASKR_LTRX0		0x00000080	/* LPORT0 data */
#define MC_MASKR_LSRQ0		0x00000100	/* LPORT0 service */
#define MC_MASKR_LTRX1		0x00000200	/* LPORT1 data */
#define MC_MASKR_LSRQ1		0x00000400	/* LPORT1 service */
#define MC_MASKR_LTRX2		0x00000800	/* LPORT2 data */
#define MC_MASKR_LSRQ2		0x00001000	/* LPORT2 service */
#define MC_MASKR_LTRX3		0x00002000	/* LPORT3 data */
#define MC_MASKR_LSRQ3		0x00004000	/* LPORT3 service */
#define MC_MASKR_WSE		0x00008000	/* DSP event */
#define MC_MASKR_STP0		0x00010000	/* DSP0 stop */
#define MC_MASKR_PI1		0x00020000	/* DSP1 software interrupt */
#define MC_MASKR_STP1		0x00040000	/* DSP1 stop */
#define MC_MASKR_COMPARE	0x00080000	/* CPU timer */
#define MC_MASKR_MEMCH0		0x00200000	/* DMA MemCh0 */
#define MC_MASKR_MEMCH1		0x00400000	/* DMA MemCh1 */
#define MC_MASKR_MEMCH2		0x00800000	/* DMA MemCh2 */
#define MC_MASKR_MEMCH3		0x01000000	/* DMA MemCh3 */
#define MC_MASKR_MEMCH4		0x02000000	/* DMA MemCh4 */
#define MC_MASKR_MEMCH5		0x04000000	/* DMA MemCh5 */
#define MC_MASKR_MEMCH6		0x08000000	/* DMA MemCh6 */
#define MC_MASKR_MEMCH7		0x10000000	/* DMA MemCh7 */
#define MC_MASKR_TIMER		0x20000000	/* timers IT, WDT, RTT */
#define MC_MASKR_PI0		0x40000000	/* DSP0 software interrupt */
#define MC_MASKR_SBS		0x80000000	/* DSPs status */
#endif

#ifdef ELVEES_NVCOM01
#define MC_MASKR0_MCC		(1 << 31)	/* Прерывание от МСС */
#define MC_MASKR0_I2C		(1 << 23)	/* Прерывание от I2C */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_VPOUT_TX	(1 << 19)	/* от канала DMA VPOUT */
#define MC_MASKR0_VPOUT		(1 << 18)	/* от контроллера VPOUT */
#define MC_MASKR0_VPIN_RX	(1 << 17)	/* от канала DMA VPIN */
#define MC_MASKR0_VPIN		(1 << 16)	/* от контроллера VPIN */
#define MC_MASKR0_ETH_DMA_TX	(1 << 15)	/* от DMA передачи Ethernet */
#define MC_MASKR0_ETH_DMA_RX	(1 << 14)	/* от DMA приёма Ethernet */
#define MC_MASKR0_ETH_TX_FRAME	(1 << 13)	/* от передатчика Ethernet */
#define MC_MASKR0_ETH_RX_FRAME	(1 << 12)	/* от приёмника Ethernet */
#define MC_MASKR0_USB_EP4	(1 << 11)	/* от передатчика USB end point 4 */
#define MC_MASKR0_USB_EP3	(1 << 10)	/* от приёмника USB end point 3 */
#define MC_MASKR0_USB_EP2	(1 << 9)	/* от передатчика USB end point 2 */
#define MC_MASKR0_USB_EP1	(1 << 8)	/* от приёмника USB end point 1 */
#define MC_MASKR0_USB		(1 << 7)	/* Прерывание от USB */
#define MC_MASKR0_UART1		(1 << 5)	/* Прерывание от UART1 */
#define MC_MASKR0_UART0		(1 << 4)	/* Прерывание от UART0 */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */

#define MC_MASKR1_DMAMEM_CH3	(1 << 3)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH2	(1 << 2)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH1	(1 << 1)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH0	(1 << 0)	/* от канала DMA MEM_CH0 */

#define MC_MASKR2_DMA_MFBSP3	(1 << 15)	/* от канала DMA порта MFBSP3 */
#define MC_MASKR2_MFBSP_TX3	(1 << 14)	/* готовность MFBSP3 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX3	(1 << 13)	/* готовность MFBSP3 к выдаче по DMA */
#define MC_MASKR2_SRQ3		(1 << 12)	/* Запрос обслуживания от порта MFBSP3 */
#define MC_MASKR2_DMA_MFBSP2	(1 << 11)	/* от канала DMA порта MFBSP2 */
#define MC_MASKR2_MFBSP_TX2	(1 << 10)	/* готовность MFBSP2 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX2	(1 << 9)	/* готовность MFBSP2 к выдаче по DMA */
#define MC_MASKR2_SRQ2		(1 << 8)	/* Запрос обслуживания от порта MFBSP2 */
#define MC_MASKR2_DMA_MFBSP1	(1 << 7)	/* от канала DMA порта MFBSP1 */
#define MC_MASKR2_MFBSP_TX1	(1 << 6)	/* готовность MFBSP1 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX1	(1 << 5)	/* готовность MFBSP1 к выдаче по DMA */
#define MC_MASKR2_SRQ1		(1 << 4)	/* Запрос обслуживания от порта MFBSP1 */
#define MC_MASKR2_DMA_MFBSP0	(1 << 3)	/* от канала DMA порта MFBSP0 */
#define MC_MASKR2_MFBSP_TX0	(1 << 2)	/* готовность MFBSP0 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX0	(1 << 1)	/* готовность MFBSP0 к выдаче по DMA */
#define MC_MASKR2_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0 */
#endif

#ifdef ELVEES_NVCOM02
/* TODO */
#define MC_MASKR0_MCC		(1 << 31)	/* Прерывание от МСС */
#define MC_MASKR0_I2C		(1 << 23)	/* Прерывание от I2C */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_VPOUT_TX	(1 << 19)	/* от канала DMA VPOUT */
#define MC_MASKR0_VPOUT		(1 << 18)	/* от контроллера VPOUT */
#define MC_MASKR0_VPIN_RX	(1 << 17)	/* от канала DMA VPIN */
#define MC_MASKR0_VPIN		(1 << 16)	/* от контроллера VPIN */
#define MC_MASKR0_ETH_DMA_TX	(1 << 15)	/* от DMA передачи Ethernet */
#define MC_MASKR0_ETH_DMA_RX	(1 << 14)	/* от DMA приёма Ethernet */
#define MC_MASKR0_ETH_TX_FRAME	(1 << 13)	/* от передатчика Ethernet */
#define MC_MASKR0_ETH_RX_FRAME	(1 << 12)	/* от приёмника Ethernet */
#define MC_MASKR0_USB_EP4	(1 << 11)	/* от передатчика USB end point 4 */
#define MC_MASKR0_USB_EP3	(1 << 10)	/* от приёмника USB end point 3 */
#define MC_MASKR0_USB_EP2	(1 << 9)	/* от передатчика USB end point 2 */
#define MC_MASKR0_USB_EP1	(1 << 8)	/* от приёмника USB end point 1 */
#define MC_MASKR0_USB		(1 << 7)	/* Прерывание от USB */
#define MC_MASKR0_UART1		(1 << 5)	/* Прерывание от UART1 */
#define MC_MASKR0_UART0		(1 << 4)	/* Прерывание от UART0 */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */

#define MC_MASKR1_DMAMEM_CH3	(1 << 3)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH2	(1 << 2)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH1	(1 << 1)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH0	(1 << 0)	/* от канала DMA MEM_CH0 */

#define MC_MASKR2_DMA_MFBSP3	(1 << 15)	/* от канала DMA порта MFBSP3 */
#define MC_MASKR2_MFBSP_TX3	(1 << 14)	/* готовность MFBSP3 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX3	(1 << 13)	/* готовность MFBSP3 к выдаче по DMA */
#define MC_MASKR2_SRQ3		(1 << 12)	/* Запрос обслуживания от порта MFBSP3 */
#define MC_MASKR2_DMA_MFBSP2	(1 << 11)	/* от канала DMA порта MFBSP2 */
#define MC_MASKR2_MFBSP_TX2	(1 << 10)	/* готовность MFBSP2 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX2	(1 << 9)	/* готовность MFBSP2 к выдаче по DMA */
#define MC_MASKR2_SRQ2		(1 << 8)	/* Запрос обслуживания от порта MFBSP2 */
#define MC_MASKR2_DMA_MFBSP1	(1 << 7)	/* от канала DMA порта MFBSP1 */
#define MC_MASKR2_MFBSP_TX1	(1 << 6)	/* готовность MFBSP1 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX1	(1 << 5)	/* готовность MFBSP1 к выдаче по DMA */
#define MC_MASKR2_SRQ1		(1 << 4)	/* Запрос обслуживания от порта MFBSP1 */
#define MC_MASKR2_DMA_MFBSP0	(1 << 3)	/* от канала DMA порта MFBSP0 */
#define MC_MASKR2_MFBSP_TX0	(1 << 2)	/* готовность MFBSP0 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX0	(1 << 1)	/* готовность MFBSP0 к выдаче по DMA */
#define MC_MASKR2_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0 */
#endif

#ifdef ELVEES_MC24R2
/* TODO */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_UART0		(1 << 4)	/* Прерывание от UART0 */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */

#define MC_MASKR1_DMAMEM_CH3	(1 << 3)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH2	(1 << 2)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH1	(1 << 1)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH0	(1 << 0)	/* от канала DMA MEM_CH0 */

#define MC_MASKR2_DMA_MFBSP3	(1 << 15)	/* от канала DMA порта MFBSP3 */
#define MC_MASKR2_MFBSP_TX3	(1 << 14)	/* готовность MFBSP3 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX3	(1 << 13)	/* готовность MFBSP3 к выдаче по DMA */
#define MC_MASKR2_SRQ3		(1 << 12)	/* Запрос обслуживания от порта MFBSP3 */
#define MC_MASKR2_DMA_MFBSP2	(1 << 11)	/* от канала DMA порта MFBSP2 */
#define MC_MASKR2_MFBSP_TX2	(1 << 10)	/* готовность MFBSP2 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX2	(1 << 9)	/* готовность MFBSP2 к выдаче по DMA */
#define MC_MASKR2_SRQ2		(1 << 8)	/* Запрос обслуживания от порта MFBSP2 */
#define MC_MASKR2_DMA_MFBSP1	(1 << 7)	/* от канала DMA порта MFBSP1 */
#define MC_MASKR2_MFBSP_TX1	(1 << 6)	/* готовность MFBSP1 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX1	(1 << 5)	/* готовность MFBSP1 к выдаче по DMA */
#define MC_MASKR2_SRQ1		(1 << 4)	/* Запрос обслуживания от порта MFBSP1 */
#define MC_MASKR2_DMA_MFBSP0	(1 << 3)	/* от канала DMA порта MFBSP0 */
#define MC_MASKR2_MFBSP_TX0	(1 << 2)	/* готовность MFBSP0 к приёму по DMA */
#define MC_MASKR2_MFBSP_RX0	(1 << 1)	/* готовность MFBSP0 к выдаче по DMA */
#define MC_MASKR2_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0 */
#endif


#ifdef ELVEES_MCT02
#define MC_MASKR0_MCC		(1 << 31)	/* Прерывание от МСС */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_UART3		(1 << 7)	/* Прерывание от UART3 */
#define MC_MASKR0_UART2		(1 << 6)	/* Прерывание от UART2 */
#define MC_MASKR0_UART1		(1 << 5)	/* Прерывание от UART1 */
#define MC_MASKR0_UART0		(1 << 4)	/* Прерывание от UART0 */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */
/* QSTR1 */
#define MC_MASKR1_DMAMEM_CH7	(1 << 7)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH6	(1 << 6)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH5	(1 << 5)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH4	(1 << 4)	/* от канала DMA MEM_CH0 */
#define MC_MASKR1_DMAMEM_CH3	(1 << 3)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH2	(1 << 2)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH1	(1 << 1)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH0	(1 << 0)	/* от канала DMA MEM_CH0 */
/* QSTR2 */
#define MC_MASKR2_SW3_TX_DT_CH0	(1 << 31)
#define MC_MASKR2_SW3_TX_DS_CH0	(1 << 30)
#define MC_MASKR2_SW3_RX_DT_CH0	(1 << 29)
#define MC_MASKR2_SW3_RX_DS_CH0	(1 << 28)
// 27
#define MC_MASKR2_SW3_LINK	(1 << 26)
#define MC_MASKR2_SW3_TIME	(1 << 25)
#define MC_MASKR2_SW3_ERR	(1 << 24)
#define MC_MASKR2_SW2_TX_DT_CH0	(1 << 23)
#define MC_MASKR2_SW2_TX_DS_CH0	(1 << 22)
#define MC_MASKR2_SW2_RX_DT_CH0	(1 << 21)
#define MC_MASKR2_SW2_RX_DS_CH0	(1 << 20)
// 19
#define MC_MASKR2_SW2_LINK	(1 << 18)
#define MC_MASKR2_SW2_TIME	(1 << 17)
#define MC_MASKR2_SW2_ERR	(1 << 16)
#define MC_MASKR2_SW1_TX_DT_CH0	(1 << 15)
#define MC_MASKR2_SW1_TX_DS_CH0	(1 << 14)
#define MC_MASKR2_SW1_RX_DT_CH0	(1 << 13)
#define MC_MASKR2_SW1_RX_DS_CH0	(1 << 12)
// 11
#define MC_MASKR2_SW1_LINK	(1 << 10)
#define MC_MASKR2_SW1_TIME	(1 << 9)
#define MC_MASKR2_SW1_ERR	(1 << 8)
#define MC_MASKR2_SW0_TX_DT_CH0	(1 << 7)
#define MC_MASKR2_SW0_TX_DS_CH0	(1 << 6)
#define MC_MASKR2_SW0_RX_DT_CH0	(1 << 5)
#define MC_MASKR2_SW0_RX_DS_CH0	(1 << 4)
// 3
#define MC_MASKR2_SW0_LINK	(1 << 2)
#define MC_MASKR2_SW0_TIME	(1 << 1)
#define MC_MASKR2_SW0_ERR	(1 << 0)

/* QSTR4 */

#define MC_MASKR4_DMA_MFBSP3	(1 << 15)	/* от канала DMA порта MFBSP3 */
#define MC_MASKR4_MFBSP_TX3	(1 << 14)	/* готовность MFBSP3 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX3	(1 << 13)	/* готовность MFBSP3 к выдаче по DMA */
#define MC_MASKR4_SRQ3		(1 << 12)	/* Запрос обслуживания от порта MFBSP3 */
#define MC_MASKR4_DMA_MFBSP2	(1 << 11)	/* от канала DMA порта MFBSP2 */
#define MC_MASKR4_MFBSP_TX2	(1 << 10)	/* готовность MFBSP2 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX2	(1 << 9)	/* готовность MFBSP2 к выдаче по DMA */
#define MC_MASKR4_SRQ2		(1 << 8)	/* Запрос обслуживания от порта MFBSP2 */
#define MC_MASKR4_DMA_MFBSP1	(1 << 7)	/* от канала DMA порта MFBSP1 */
#define MC_MASKR4_MFBSP_TX1	(1 << 6)	/* готовность MFBSP1 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX1	(1 << 5)	/* готовность MFBSP1 к выдаче по DMA */
#define MC_MASKR4_SRQ1		(1 << 4)	/* Запрос обслуживания от порта MFBSP1 */
#define MC_MASKR4_DMA_MFBSP0	(1 << 3)	/* от канала DMA порта MFBSP0 */
#define MC_MASKR4_MFBSP_TX0	(1 << 2)	/* готовность MFBSP0 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX0	(1 << 1)	/* готовность MFBSP0 к выдаче по DMA */
#define MC_MASKR4_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0 */
#endif


#ifdef ELVEES_MCT03P
#define MC_MASKR0_MCC		(1 << 31)	/* Прерывание от МСС */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_UART3		(1 << 7)	/* Прерывание от UART3 */
#define MC_MASKR0_UART2		(1 << 6)	/* Прерывание от UART2 */
#define MC_MASKR0_UART1		(1 << 5)	/* Прерывание от UART1 */
#define MC_MASKR0_UART0		(1 << 4)	/* Прерывание от UART0 */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */
/* QSTR1 */
#define MC_MASKR1_DMAMEM_CH13	(1 << 11)	/* от канала DMA MEM_CH13 */
#define MC_MASKR1_DMAMEM_CH12	(1 << 10)	/* от канала DMA MEM_CH12 */
#define MC_MASKR1_DMAMEM_CH11	(1 << 9)	/* от канала DMA MEM_CH11 */
#define MC_MASKR1_DMAMEM_CH10	(1 << 8)	/* от канала DMA MEM_CH10 */
#define MC_MASKR1_DMAMEM_CH3	(1 << 3)	/* от канала DMA MEM_CH3 */
#define MC_MASKR1_DMAMEM_CH2	(1 << 2)	/* от канала DMA MEM_CH2 */
#define MC_MASKR1_DMAMEM_CH1	(1 << 1)	/* от канала DMA MEM_CH1 */
#define MC_MASKR1_DMAMEM_CH0	(1 << 0)	/* от канала DMA MEM_CH0 */
/* QSTR2 */
#define MC_MASKR2_SW3_TX_DT_CH0	(1 << 31)
#define MC_MASKR2_SW3_TX_DS_CH0	(1 << 30)
#define MC_MASKR2_SW3_RX_DT_CH0	(1 << 29)
#define MC_MASKR2_SW3_RX_DS_CH0	(1 << 28)
// 27
#define MC_MASKR2_SW3_LINK	(1 << 26)
#define MC_MASKR2_SW3_TIME	(1 << 25)
#define MC_MASKR2_SW3_ERR	(1 << 24)
#define MC_MASKR2_SW2_TX_DT_CH0	(1 << 23)
#define MC_MASKR2_SW2_TX_DS_CH0	(1 << 22)
#define MC_MASKR2_SW2_RX_DT_CH0	(1 << 21)
#define MC_MASKR2_SW2_RX_DS_CH0	(1 << 20)
// 19
#define MC_MASKR2_SW2_LINK	(1 << 18)
#define MC_MASKR2_SW2_TIME	(1 << 17)
#define MC_MASKR2_SW2_ERR	(1 << 16)
#define MC_MASKR2_SW1_TX_DT_CH0	(1 << 15)
#define MC_MASKR2_SW1_TX_DS_CH0	(1 << 14)
#define MC_MASKR2_SW1_RX_DT_CH0	(1 << 13)
#define MC_MASKR2_SW1_RX_DS_CH0	(1 << 12)
// 11
#define MC_MASKR2_SW1_LINK	(1 << 10)
#define MC_MASKR2_SW1_TIME	(1 << 9)
#define MC_MASKR2_SW1_ERR	(1 << 8)
#define MC_MASKR2_SW0_TX_DT_CH0	(1 << 7)
#define MC_MASKR2_SW0_TX_DS_CH0	(1 << 6)
#define MC_MASKR2_SW0_RX_DT_CH0	(1 << 5)
#define MC_MASKR2_SW0_RX_DS_CH0	(1 << 4)
// 3
#define MC_MASKR2_SW0_LINK	(1 << 2)
#define MC_MASKR2_SW0_TIME	(1 << 1)
#define MC_MASKR2_SW0_ERR	(1 << 0)

/* QSTR4 */

#define MC_MASKR4_DMA_MFBSP3	(1 << 15)	/* от канала DMA порта MFBSP3 */
#define MC_MASKR4_MFBSP_TX3	(1 << 14)	/* готовность MFBSP3 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX3	(1 << 13)	/* готовность MFBSP3 к выдаче по DMA */
#define MC_MASKR4_SRQ3		(1 << 12)	/* Запрос обслуживания от порта MFBSP3 */
#define MC_MASKR4_DMA_MFBSP2	(1 << 11)	/* от канала DMA порта MFBSP2 */
#define MC_MASKR4_MFBSP_TX2	(1 << 10)	/* готовность MFBSP2 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX2	(1 << 9)	/* готовность MFBSP2 к выдаче по DMA */
#define MC_MASKR4_SRQ2		(1 << 8)	/* Запрос обслуживания от порта MFBSP2 */
#define MC_MASKR4_DMA_MFBSP1	(1 << 7)	/* от канала DMA порта MFBSP1 */
#define MC_MASKR4_MFBSP_TX1	(1 << 6)	/* готовность MFBSP1 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX1	(1 << 5)	/* готовность MFBSP1 к выдаче по DMA */
#define MC_MASKR4_SRQ1		(1 << 4)	/* Запрос обслуживания от порта MFBSP1 */
#define MC_MASKR4_DMA_MFBSP0	(1 << 3)	/* от канала DMA порта MFBSP0 */
#define MC_MASKR4_MFBSP_TX0	(1 << 2)	/* готовность MFBSP0 к приёму по DMA */
#define MC_MASKR4_MFBSP_RX0	(1 << 1)	/* готовность MFBSP0 к выдаче по DMA */
#define MC_MASKR4_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0 */
#endif


#ifdef ELVEES_MC0428
/* QSTR0 / MASKR0 */
#define MC_MASKR0_I2C		(1 << 23)	/* Прерывание от I2C */
#define MC_MASKR0_IT		(1 << 22)	/* от таймера IT */
#define MC_MASKR0_RTT		(1 << 21)	/* от таймера RTT */
#define MC_MASKR0_WDT		(1 << 20)	/* от таймера WDT */
#define MC_MASKR0_VPOUT_TX	(1 << 19)	/* от канала DMA VPOUT */
#define MC_MASKR0_VPOUT		(1 << 18)	/* от контроллера VPOUT */
#define MC_MASKR0_VPIN_RX	(1 << 17)	/* от канала DMA VPIN */
#define MC_MASKR0_VPIN		(1 << 16)	/* от контроллера VPIN */
#define MC_MASKR0_ETH_DMA_TX	(1 << 15)	/* от DMA передачи Ethernet */
#define MC_MASKR0_ETH_DMA_RX	(1 << 14)	/* от DMA приёма Ethernet */
#define MC_MASKR0_ETH_TX_FRAME	(1 << 13)	/* от передатчика Ethernet */
#define MC_MASKR0_ETH_RX_FRAME	(1 << 12)	/* от приёмника Ethernet */
#define MC_MASKR0_USB_EP4	(1 << 11)	/* от передатчика USB end point 4 */
#define MC_MASKR0_USB_EP3	(1 << 10)	/* от приёмника USB end point 3 */
#define MC_MASKR0_USB_EP2	(1 << 9)	/* от передатчика USB end point 2 */
#define MC_MASKR0_USB_EP1	(1 << 8)	/* от приёмника USB end point 1 */
#define MC_MASKR0_USB		(1 << 7)	/* Прерывание от USB */
#define MC_MASKR0_PMSC_MASTER (1 << 6)	/* Прерывание от PMSC_MASTER (PCI) */
#define MC_MASKR0_PMSC_MBR	(1 << 5)	/* Прерывание от PMSC_MBR (PCI) */
#define MC_MASKR0_UART		(1 << 4)	/* Прерывание от UART */
#define MC_MASKR0_IRQ3		(1 << 3)	/* Внешнее прерывание nIRQ3 */
#define MC_MASKR0_IRQ2		(1 << 2)	/* Внешнее прерывание nIRQ2 */
#define MC_MASKR0_IRQ1		(1 << 1)	/* Внешнее прерывание nIRQ1 */
#define MC_MASKR0_IRQ0		(1 << 0)	/* Внешнее прерывание nIRQ0 */
/* QSTR1 / MASKR1 */
#define MC_MASKR1_DMAMEM_CH17	(1 << 15)	/* от канала DMA MEM_CH17 */
#define MC_MASKR1_DMAMEM_CH16	(1 << 14)	/* от канала DMA MEM_CH16 */
#define MC_MASKR1_DMAMEM_CH15	(1 << 13)	/* от канала DMA MEM_CH15 */
#define MC_MASKR1_DMAMEM_CH14	(1 << 12)	/* от канала DMA MEM_CH14 */
#define MC_MASKR1_DMAMEM_CH13	(1 << 11)	/* от канала DMA MEM_CH13 */
#define MC_MASKR1_DMAMEM_CH12	(1 << 10)	/* от канала DMA MEM_CH12 */
#define MC_MASKR1_DMAMEM_CH11	(1 << 9)	/* от канала DMA MEM_CH11 */
#define MC_MASKR1_DMAMEM_CH10	(1 << 8)	/* от канала DMA MEM_CH10 */
#define MC_MASKR1_DMAMEM_CH07	(1 << 7)	/* от канала DMA MEM_CH07 */
#define MC_MASKR1_DMAMEM_CH06	(1 << 6)	/* от канала DMA MEM_CH06 */
#define MC_MASKR1_DMAMEM_CH05	(1 << 5)	/* от канала DMA MEM_CH05 */
#define MC_MASKR1_DMAMEM_CH04	(1 << 4)	/* от канала DMA MEM_CH04 */
#define MC_MASKR1_DMAMEM_CH03	(1 << 3)	/* от канала DMA MEM_CH03 */
#define MC_MASKR1_DMAMEM_CH02	(1 << 2)	/* от канала DMA MEM_CH02 */
#define MC_MASKR1_DMAMEM_CH01	(1 << 1)	/* от канала DMA MEM_CH01 */
#define MC_MASKR1_DMAMEM_CH00	(1 << 0)	/* от канала DMA MEM_CH00 */
/* QSTR2 / MASKR2 */
#define MC_MASKR2_SRIO1_MCE_DEC	(1 << 15)	/* В SRIO1 принял символ Multicast-Event. 
											   Повторяет состояние бита MCE_DEC регистра LPU_CSR */
#define MC_MASKR2_SRIO1_RESET_DEVICE_CMD (1 << 14)	/* В SRIO1 поступили 4 команды Reset-Device Command. 
													Повторяет состояние бита RESET_DEVICE_CMD регистра LPU_CSR */
#define MC_MASKR2_SRIO1_PORT_ERROR (1 << 13) /* LPU SRIO1 находится в нерабочем состоянии из-за 
											   обнаружения невосстанавливаемой ошибки. 
											   Повторяет состояние бита PORT_ERROR регистра ERROR_STATUS_CSR */
#define MC_MASKR2_SRIO1_MPU_TX	(1 << 12)	/* Прерывание от MPU_TX SRIO1 */
#define MC_MASKR2_SRIO1_MPU_RX	(1 << 11)	/* Прерывание от MPU_RX SRIO1 */
#define MC_MASKR2_SRIO1_LSU		(1 << 10)	/* Прерывание от LSU SRIO1 */
#define MC_MASKR2_SRIO1_DOORBELL (1 << 9)	/* В SRIO1 поступил пакет типа DOORBELL */
#define MC_MASKR2_SRIO1_PWRITE	(1 << 8)	/* В SRIO1 поступил пакет типа PORT_WRITE */

#define MC_MASKR2_SRIO0_MCE_DEC	(1 << 7)	/* В SRIO0 принял символ Multicast-Event. 
											   Повторяет состояние бита MCE_DEC регистра LPU_CSR */
#define MC_MASKR2_SRIO0_RESET_DEVICE_CMD (1 << 6)	/* В SRIO0 поступили 4 команды Reset-Device Command. 
													Повторяет состояние бита RESET_DEVICE_CMD регистра LPU_CSR */
#define MC_MASKR2_SRIO0_PORT_ERROR (1 << 5)	/* LPU SRIO0 находится в нерабочем состоянии из-за 
											   обнаружения невосстанавливаемой ошибки. 
											   Повторяет состояние бита PORT_ERROR регистра ERROR_STATUS_CSR */
#define MC_MASKR2_SRIO0_MPU_TX	(1 << 4)	/* Прерывание от MPU_TX SRIO0 */
#define MC_MASKR2_SRIO0_MPU_RX	(1 << 3)	/* Прерывание от MPU_RX SRIO0 */
#define MC_MASKR2_SRIO0_LSU		(1 << 2)	/* Прерывание от LSU SRIO0 */
#define MC_MASKR2_SRIO0_DOORBELL (1 << 1)	/* В SRIO0 поступил пакет типа DOORBELL */
#define MC_MASKR2_SRIO0_PWRITE	(1 << 0)	/* В SRIO0 поступил пакет типа PORT_WRITE */
/* QSTR3 / MASKR3 */
#define MC_MASKR3_SW_TX_DAT_CH1 (1 << 31) /* Прерывание от канала DMA SW_TX_DAT_CH1 */
#define MC_MASKR3_SW_TX_DES_CH1 (1 << 30) /* Прерывание от канала DMA SW_TX_DES_CH1 */
#define MC_MASKR3_SW_RX_DAT_CH1 (1 << 29) /* Прерывание от канала DMA SW_RX_DAT_CH1 */
#define MC_MASKR3_SW_RX_DES_CH1 (1 << 28) /* Прерывание от канала DMA SW_RX_DES_CH1 */
/* Unused (1 << 27) */
#define MC_MASKR3_SW_TIME1		(1 << 26) /* Прерывание от SWIC1 при получении маркер времени 
										     или распределенного прерывания */
#define MC_MASKR3_SW_ERR1		(1 << 25) /* Прерывание от SWIC1 при ошибке в канале */
#define MC_MASKR3_SW_LINK1		(1 << 24) /* Прерывание от SWIC1 при установлении соединения */

#define MC_MASKR3_SW_TX_DAT_CH0 (1 << 23) /* Прерывание от канала DMA SW_TX_DAT_CH0 */
#define MC_MASKR3_SW_TX_DES_CH0 (1 << 22) /* Прерывание от канала DMA SW_TX_DES_CH0 */
#define MC_MASKR3_SW_RX_DAT_CH0 (1 << 21) /* Прерывание от канала DMA SW_RX_DAT_CH0 */
#define MC_MASKR3_SW_RX_DES_CH0 (1 << 20) /* Прерывание от канала DMA SW_RX_DES_CH0 */
/* Unused (1 << 19) */
#define MC_MASKR3_SW_TIME0		(1 << 18) /* Прерывание от SWIC0 при получении маркер времени 
										     или распределенного прерывания */
#define MC_MASKR3_SW_ERR0		(1 << 17) /* Прерывание от SWIC0 при ошибке в канале */
#define MC_MASKR3_SW_LINK0		(1 << 16) /* Прерывание от SWIC0 при установлении соединения */
/* Unused (1 << 14) and (1 << 15) */
#define MC_MASKR3_MFBSP_RX_CH1	(1 << 13) /* Прерывание от канала DMA порта MFBSP1 при приеме данных */
#define MC_MASKR3_MFBSP_TX_CH1	(1 << 12) /* Прерывание от канала DMA порта MFBSP1 при передаче данных */
/* Unused (1 << 11) */
#define MC_MASKR3_MFBSP_RXBUF1 (1 << 10) /* Формируется, если порт MFBSP1 включен на прием данных (в одном из режимов), 
										   а число 64-х разрядных слов в буфере приёма больше чем RLEV 
										   (RLEV устанавливается в регистре состояния приёмника RSR) */
#define MC_MASKR3_MFBSP_TXBUF1 (1 << 9)	 /* Формируется, если порт MFBSP1 включен на передачу данных (в одном из режимов), 
										   а число 64-х разрядных слов, находящихся в буфере  передачи меньше, 
										   либо равно TLEV (TLEV устанавливается в регистре состояния передатчика TSR) */
#define MC_MASKR3_SRQ1		(1 << 8)	 /* Запрос обслуживания от порта MFBSP1.
										   Формируется, если порт выключен (LEN=0, SPI_I2S_EN=0), 
										   а на выводах LACK или LCLK присутствует сигнал высокого уровня */
/* Unused (1 << 6) and (1 << 7) */
#define MC_MASKR3_MFBSP_RX_CH0 (1 << 5)	/* Прерывание от канала DMA порта MFBSP0 при приеме данных */
#define MC_MASKR3_MFBSP_TX_CH0 (1 << 4)	/* Прерывание от канала DMA порта MFBSP0 при передаче данных */
/* Unused (1 << 3) */
#define MC_MASKR3_MFBSP_RXBUF0 (1 << 2)	/* Формируется, если порт MFBSP0 включен на прием данных (в одном из режимов), 
										   а число 64-х разрядных слов в буфере приёма больше чем RLEV 
										   (RLEV устанавливается в регистре состояния приёмника RSR) */
#define MC_MASKR3_MFBSP_TXBUF0 (1 << 1)	/* Формируется, если порт MFBSP0 включен на передачу данных (в одном из режимов), 
										   а число 64-х разрядных слов, находящихся в буфере  передачи меньше, 
										   либо равно TLEV (TLEV устанавливается в регистре состояния передатчика TSR) */
#define MC_MASKR3_SRQ0		(1 << 0)	/* Запрос обслуживания от порта MFBSP0.
										   Формируется, если порт выключен (LEN=0, SPI_I2S_EN=0), 
										   а на выводах LACK или LCLK присутствует сигнал высокого уровня */
#endif

/*
 * Системный регистр IRQM
 */
#define MC_IRQM_MODE(n)     ((1 << (n)) << 8)
#define MC_IRQM_NULL(n)     (1 << (n))

/*
 * Системный регистр CLKEN
 */
#define MC_CLKEN_MCC		(1 << 31)	/* Включение тактовой частоты MCC */
#define MC_CLKEN_SWIC(n)	(1 << (24+(n)))	/* Включение тактовой частоты SWIC */
#define MC_CLKEN_USB		(1 << 22)	/* Включение тактовой частоты USB */
#define MC_CLKEN_EMAC		(1 << 20)	/* Включение тактовой частоты EMAC */
#define MC_CLKEN_VPOUT		(1 << 19)	/* Включение тактовой частоты VPOUT */
#define MC_CLKEN_VPIN		(1 << 18)	/* Включение тактовой частоты VPIN */
#define MC_CLKEN_PMSC       (1 << 16)   /* Включение тактовой частоты PCI */
#define MC_CLKEN_DMA_MEM	(1 << 12)	/* Включение тактовой частоты DMA_MEM */
#define MC_CLKEN_MFBSP		(1 << 8)	/* Включение тактовой частоты MFBSP 1-3 */
#define MC_CLKEN_DSP1		(1 << 5)	/* Включение тактовой частоты DSP1 */
#define MC_CLKEN_DSP0		(1 << 4)	/* Включение тактовой частоты DSP0 */
#define MC_CLKEN_CORE2		(1 << 2)	/* Отключение (нулем) тактовой частоты CPU, CRAM, коммутатора AXI,
						 MPORT, UART, I2C, MFBSP0 (от TIMER частота не отключается) */
#define MC_CLKEN_CPU		(1 << 1)	/* Отключение (нулем) тактовой частоты CPU */
#define MC_CLKEN_CORE		(1 << 0)	/* Отключение (нулем) тактовой частоты CPU, CRAM, коммутатора AXI,
						 MPORT, UART, I2C, MFBSP0, TIMER */

/*
 * Системный регистр CR_PLL
 */
#define MC_CRPLL_DSP		(1 << 23)	/* Выбор тактовой частоты PLL_DSP */
#define MC_CRPLL_CLKSEL_DSP(n)	((n) << 16)	/* Коэффициент частоты PLL_DSP */
#define MC_CRPLL_CLKSEL_MPORT(n) ((n) << 8)	/* Коэффициент частоты PLL_MPORT */
#define MC_CRPLL_CLKSEL_CORE(n) (n)		/* Коэффициент частоты PLL_CORE */

/*
 * Регистры порта внешней памяти CSCONi
 */
#define MC_CSCON_CSMASK(addr)	((addr) >> 24 & 0xff)
						/* Address mask bits 31:24 */
#define MC_CSCON_CSBA(addr)	((addr) >> 16 & 0xff00)
						/* Base address bits 31:24 */
#define MC_CSCON_WS(n)		((n) << 16)	/* Wait states for async memory */
#define MC_CSCON_E		(1 << 20)	/* Enable for nCS0, nCS1, nCS2 */
#define MC_CSCON_T		(1 << 21)	/* Sync memory flag (only nCS0, nCS1) */
#define MC_CSCON_AE		(1 << 22)	/* Wait for nACK */
#define MC_CSCON_W64		(1 << 23)	/* 64-bit data width */
#define MC_CSCON3_BYTE		(1 << 23)	/* 8-bit data width for nCS3 */
#define MC_CSCON3_OVER		(1 << 24)	/* Status: no nACK for 256 CLK periods */
#define MC_CSCON3_ADDR(addr)	(((addr) & 3) << 20)
						/* Address bits 1:0 for 8-bit memory access */

/*
 * Регистр конфигурации синхронной динамической памяти SDRCON
 */
#define MC_SDRCON_PS_512	(0 << 0)	/* Page size 512 */
#define MC_SDRCON_PS_1024	(1 << 0)	/* Page size 1024 */
#define MC_SDRCON_PS_2048	(2 << 0)	/* Page size 2048 */
#define MC_SDRCON_PS_4096	(3 << 0)	/* Page size 4096 */

#if defined(ELVEES_MC24) || defined(ELVEES_MC0226)
#define MC_SDRCON_RFR(nsec,khz)	((((nsec)*(khz)+999999)/1000000) << 4)
						/* Refresh period */
#define MC_SDRCON_BL_1		(0 << 16)	/* Bursh length 1 */
#define MC_SDRCON_BL_2		(1 << 16)	/* Bursh length 2 */
#define MC_SDRCON_BL_4		(2 << 16)	/* Bursh length 4 */
#define MC_SDRCON_BL_8		(3 << 16)	/* Bursh length 8 */
#define MC_SDRCON_BL_PAGE	(7 << 16)	/* Bursh full page */

#define MC_SDRCON_WBM		(1 << 19)	/* Write burst mode - single write */
#define MC_SDRCON_CL		(1 << 20)	/* CAS latency: 0 - 2, 1 - 3 cycles */
#define MC_SDRCON_INIT		(1 << 31)	/* Initialize SDRAM, 2 usec */
#endif

#if defined(ELVEES_NVCOM01) || defined (ELVEES_MC24R2) || defined (ELVEES_MCT02) || defined(ELVEES_MC0428)
#define MC_SDRCON_RFR(nsec,khz)	(((nsec*khz+999999)/1000000) << 16)
						/* Refresh period */
#define MC_SDRCON_CL_2		(2 << 4)	/* CAS latency 2 cycles */
#define MC_SDRCON_CL_3		(3 << 4)	/* CAS latency 3 cycles */
#endif

#ifdef ELVEES_NVCOM02
/* TODO */
#define MC_SDRCON_RFR(nsec,khz)	(((nsec*khz+999999)/1000000) << 16)
						/* Refresh period */
#define MC_SDRCON_CL_2		(2 << 4)	/* CAS latency 2 cycles */
#define MC_SDRCON_CL_3		(3 << 4)	/* CAS latency 3 cycles */
#endif

/*
 * Регистр параметров синхронной динамической памяти SDRTMR
 */
#define MC_SDRTMR_TWR(n)	((n) << 0)	/* Write recovery delay */
#define MC_SDRTMR_TRP(n)	((n) << 4)	/* Минимальный период Precharge */
#define MC_SDRTMR_TRCD(n)	((n) << 8)	/* Минимальная задержка между
						 * Active и Read/Write */
#define MC_SDRTMR_TRAS(n)	((n) << 16)	/* Минимальная задержка между
						 * Active и Precharge */
#define MC_SDRTMR_TRFC(n)	((n) << 20)	/* Минимальный интервал между Refresh */

/*--------------------------------------
 * Интервальный таймер, регистр управления ITCSR
 */
#define MC_ITCSR_EN		0x00000001	/* разрешение работы таймера */
#define MC_ITCSR_INT		0x00000002	/* признак срабатывания таймера */

/*--------------------------------------
 * Сторожевой таймер, регистр управления WTCSR
 */
#define MC_WTCSR_KEY1		0x000000A0	/* первый ключ */
#define MC_WTCSR_KEY2		0x000000F5	/* второй ключ */
#define MC_WTCSR_EN		0x00000100	/* разрешение работы таймера */
#define MC_WTCSR_INT		0x00000200	/* признак срабатывания таймера */
#define MC_WTCSR_MODE_ITM	0x00000400	/* режим обычного таймера */
#define MC_WTCSR_RLD		0x00000800	/* периодическая перезагрузка */

#define MC_WTCSR_INT_CTR	0x00003000	/* управление типом прерывания */
#define MC_WTCSR_INT_DISABLE	0x00000000	/* прерывание не формируется */
#define MC_WTCSR_INT_TIMER	0x00001000	/* обычное прерывание QSTR[29] */
#define MC_WTCSR_INT_NMI	0x00002000	/* немаскируемое прерывание */
#define MC_WTCSR_INT_EXTSIG	0x00003000	/* внешний сигнал WDT */

/*--------------------------------------
 * UART.
 */
/*
 * Line control register
 */
#define MC_LCR_5BITS            0x00    /* character length: 5 bits */
#define MC_LCR_6BITS            0x01    /* character length: 6 bits */
#define MC_LCR_7BITS            0x02    /* character length: 7 bits */
#define MC_LCR_8BITS            0x03    /* character length: 8 bits */

#define MC_LCR_STOPB            0x04    /* use 2 stop bits */
#define MC_LCR_PENAB            0x08    /* parity enable */
#define MC_LCR_PEVEN            0x10    /* even parity */
#define MC_LCR_PFORCE           0x20    /* force parity */
#define MC_LCR_SBREAK           0x40    /* break control */
#define MC_LCR_DLAB             0x80    /* divisor latch access bit */

/*
 * FIFO control register
 */
#define MC_FCR_ENABLE		0x01	/* enable FIFO */
#define MC_FCR_RCV_RST		0x02	/* clear receive FIFO */
#define MC_FCR_XMT_RST		0x04	/* clear transmit FIFO */

#define MC_FCR_TRIGGER_1	0x00	/* receive FIFO level: 1/4 byte */
#define MC_FCR_TRIGGER_4	0x40	/* receive FIFO level: 4/16 bytes */
#define MC_FCR_TRIGGER_8	0x80	/* receive FIFO level: 8/56 bytes */
#define MC_FCR_TRIGGER_14	0xc0	/* receive FIFO level: 14/60 bytes */

/*
 * Line status register
 */
#define MC_LSR_RXRDY		0x01	/* receiver ready */
#define MC_LSR_OE		0x02	/* overrun error */
#define MC_LSR_PE		0x04	/* parity error */
#define MC_LSR_FE		0x08	/* framing error */
#define MC_LSR_BI		0x10	/* break interrupt */
#define MC_LSR_TXRDY		0x20	/* transmitter holding register empty */
#define MC_LSR_TEMT		0x40	/* transmitter empty */
#define MC_LSR_FIFOE		0x80	/* error in receive FIFO */

/*
 * Interrupt enable register
 */
#define MC_IER_ERXRDY		0x01	/* enable receive data/timeout intr */
#define MC_IER_ETXRDY		0x02	/* enable transmitter interrupts */
#define MC_IER_ERLS		0x04	/* enable receive line status intr */
#define MC_IER_EMSC		0x08	/* enable modem status interrupts */

/*
 * Interrupt identification register
 */
#define MC_IIR_NOPEND		0x01	/* no interrupt pending */
#define MC_IIR_IMASK		0x0e	/* interrupt type mask */
#define MC_IIR_FENAB		0xc0	/* set if FIFOs are enabled */

#define MC_IIR_RLS		0x06	/* receiver line status */
#define MC_IIR_RXRDY		0x04	/* receiver data available */
#define MC_IIR_RXTOUT		0x0c	/* character timeout indication */
#define MC_IIR_TXRDY		0x02	/* transmitter holding reg empty */
#define MC_IIR_MLSC		0x00	/* modem status */

/*
 * Modem control register
 */
#define MC_MCR_DTR		0x01	/* control DTR output */
#define MC_MCR_RTS		0x02	/* control RTS output */
#define MC_MCR_OUT1		0x04	/* control OUT1 output */
#define MC_MCR_OUT2		0x08	/* control OUT2 output, used as
					 * global interrupt enable in PCs */
#define MC_MCR_LOOPBACK		0x10	/* set local loopback mode */

/*
 * Modem status register
 */
#define MC_MSR_DCTS		0x01	/* CTS changed */
#define MC_MSR_DDSR		0x02	/* DSR changed */
#define MC_MSR_TERI		0x04	/* RI changed from 0 to 1 */
#define MC_MSR_DDCD		0x08	/* DCD changed */
#define MC_MSR_CTS		0x10	/* CTS input */
#define MC_MSR_DSR		0x20	/* DSR input */
#define MC_MSR_RI		0x40	/* RI input */
#define MC_MSR_DCD		0x80	/* DCD input */

/*
 * Compute the 16-bit baud rate divisor, given
 * the oscillator frequency and baud rate.
 * Round to the nearest integer.
 */
#define MC_DL_BAUD(fr,bd)	(((fr)/8 + (bd)) / (bd) / 2)

/*--------------------------------------
 * Coprocessor 1 (FPU) registers.
 */
#define C1_FIR		0	/* implementation and revision */
#define C1_FCCR		25	/* condition codes */
#define C1_FEXR		26	/* exceptions */
#define C1_FENR		28	/* enables */
#define C1_FCSR		31	/* control/status */

/*
 * FPU control/status register
 */
#define FCSR_ROUND	0x00000003	/* round mode */
#define	FCSR_ROUND_N	0x00000000	/* round to nearest */
#define	FCSR_ROUND_Z	0x00000001	/* round toward zero */
#define	FCSR_ROUND_P	0x00000002	/* round toward positive infinity */
#define	FCSR_ROUND_M	0x00000003	/* round toward negative infinity */
#define	FCSR_FLAG_I	0x00000004	/* flag: inexact result */
#define	FCSR_FLAG_U	0x00000008	/* flag: underflow */
#define	FCSR_FLAG_O	0x00000010	/* flag: overflow */
#define	FCSR_FLAG_Z	0x00000020	/* flag: divide by zero */
#define	FCSR_FLAG_V	0x00000040	/* flag: invalid operation */
#define FCSR_ENABLES	0x00000f80	/* enables */
#define	FCSR_ENABLE_I	0x00000080	/* enables: inexact result */
#define	FCSR_ENABLE_U	0x00000100	/* enables: underflow */
#define	FCSR_ENABLE_O	0x00000200	/* enables: overflow */
#define	FCSR_ENABLE_Z	0x00000400	/* enables: divide by zero */
#define	FCSR_ENABLE_V	0x00000800	/* enables: invalid operation */
#define	FCSR_CAUSE_I	0x00001000	/* cause: inexact result */
#define	FCSR_CAUSE_U	0x00002000	/* cause: underflow */
#define	FCSR_CAUSE_O	0x00004000	/* cause: overflow */
#define	FCSR_CAUSE_Z	0x00008000	/* cause: divide by zero */
#define	FCSR_CAUSE_V	0x00010000	/* cause: invalid operation */
#define	FCSR_CAUSE_E	0x00020000	/* cause: unimplemented */
#define FCSR_FS		0x01000000	/* flush to zero */
#define	FCSR_COND_0	0x00800000	/* condition code 0 */
#define	FCSR_COND_1	0x02000000	/* condition code 1 */
#define	FCSR_COND_2	0x04000000	/* condition code 2 */
#define	FCSR_COND_3	0x08000000	/* condition code 3 */
#define	FCSR_COND_4	0x10000000	/* condition code 4 */
#define	FCSR_COND_5	0x20000000	/* condition code 5 */
#define	FCSR_COND_6	0x40000000	/* condition code 6 */
#define	FCSR_COND_7	0x80000000	/* condition code 7 */

/*
 * MFBSP_CSR - регистр управления MFBSP
 */
#define MC_MFBSP_CSR_LEN		(1 << 0)
#define MC_MFBSP_CSR_LTRAN		(1 << 1)
#define MC_MFBSP_CSR_LCLK_RATE(n)	((((n) & 1) << 2) | (((n) & 0x1e) << 10))
#define MC_MFBSP_CSR_LSTAT(n)		((n) << 3)
#define MC_MFBSP_CSR_LRERR		(1 << 5)
#define MC_MFBSP_CSR_LDW		(1 << 6)
#define MC_MFBSP_CSR_SRQ_TX		(1 << 7)
#define MC_MFBSP_CSR_SRQ_RX		(1 << 8)
#define MC_MFBSP_CSR_SPI_I2S_EN		(1 << 9)

#define MC_MFBSP_CSR_GET_LCLK_RATE(x)	(((x) >> 2) & 1) | (((x) >> 10) & 0x1e)

/* 
 * MFBSP_GPIO_DR и MFBSP_DIR
 */
#define MC_MFBSP_GPIO_LACK		(1 << 0)
#define MC_MFBSP_GPIO_LCLK		(1 << 1)
#define MC_MFBSP_GPIO_LDAT(n)		(1 << ((n) + 2))

/*
 * MFBSP_RSR
 */
#define MC_MFBSP_RSR_RBE		(1 << 0)
#define MC_MFBSP_RSR_RBF		(1 << 1)
#define MC_MFBSP_RSR_RBHF		(1 << 2)
#define MC_MFBSP_RSR_RBHL		(1 << 3)
#define MC_MFBSP_RSR_RSBE		(1 << 4)
#define MC_MFBSP_RSR_RSBF		(1 << 5)
#define MC_MFBSP_RSR_RXBUF_R		(1 << 8)
#define MC_MFBSP_RSR_RXBUF_D		(1 << 9)
#define MC_MFBSP_RSR_RXBUF		(1 << 10)
#define MC_MFBSP_RSR_RLEV(n)		((n) << 16)
#define MC_MFBSP_RSR_RB_DIFF(x)		(((x) >> 24) & 0xF)

/*
 * MFBSP_TSR
 */
#define MC_MFBSP_TSR_TBE		(1 << 0)
#define MC_MFBSP_TSR_TBF		(1 << 1)
#define MC_MFBSP_TSR_TBHF		(1 << 2)
#define MC_MFBSP_TSR_TBLL		(1 << 3)
#define MC_MFBSP_TSR_TSBE		(1 << 4)
#define MC_MFBSP_TSR_TSBF		(1 << 5)
#define MC_MFBSP_TSR_TXBUF_R		(1 << 8)
#define MC_MFBSP_TSR_TXBUF_D		(1 << 9)
#define MC_MFBSP_TSR_TXBUF		(1 << 10)
#define MC_MFBSP_TSR_TLEV(n)		((n) << 16)
#define MC_MFBSP_TSR_TBES(n)		((n) << 20)
#define MC_MFBSP_TSR_TB_DIFF(x)		(((x) >> 24) & 0xF)


/*--------------------------------------
 * Регистры контроллера Ethernet
 */
/*
 * MAC_CONTROL - управление MAC
 */
#define MAC_CONTROL_FULLD		(1 << 0) 	/* дуплексный режим */
#define MAC_CONTROL_EN_TX_DMA		(1 << 1) 	/* разрешение передающего TX DMА */
#define MAC_CONTROL_EN_TX		(1 << 2) 	/* разрешение передачи */
#define MAC_CONTROL_IRQ_TX_DONE		(1 << 3) 	/* прерывание от передачи */
#define MAC_CONTROL_EN_RX		(1 << 4) 	/* разрешение приема */
#define MAC_CONTROL_LOOPBACK		(1 << 5) 	/* режим зацикливания */
#define MAC_CONTROL_FULLD_RX		(1 << 6) 	/* тестовый режим приема */
#define MAC_CONTROL_IRQ_RX_DONE		(1 << 7) 	/* прерывание по приёму */
#define MAC_CONTROL_IRQ_RX_OVF		(1 << 8) 	/* прерывание по переполнению */
#define MAC_CONTROL_CP_TX		(1 << 9) 	/* сброс передающего TX_FIFO */
#define MAC_CONTROL_RST_TX		(1 << 10) 	/* сброс блока передачи */
#define MAC_CONTROL_CP_RX		(1 << 11) 	/* сброс принимающего RX_FIFO */
#define MAC_CONTROL_RST_RX		(1 << 12) 	/* сброс блока приема */

/*
 * IFS_COLL_MODE - режим обработки коллизии
 */
#define IFS_COLL_MODE_ATTEMPT_NUM(n)	((n) & 15)	/* Количество попыток повторных передач */
#define IFS_COLL_MODE_EN_CW		(1 << 4)	/* Разрешение отслеживания окна коллизии */
#define IFS_COLL_MODE_TM_BACKOFF	(1 << 7)	/* Тестовый режим */
#define IFS_COLL_MODE_COLL_WIN(n)	((n) << 8)	/* Размер окна коллизии */
#define IFS_COLL_MODE_JAMB(n)		((n) << 16)	/* Байт jam-сообщения */
#define IFS_COLL_MODE_IFS(n)		((n) << 24)	/* Межкадровый интервал */

/*
 * TX_FRAME_CONTROL - управление передачей кадра
 */
#define TX_FRAME_CONTROL_LENGTH(n)	((n) & 0xfff)	/* Число байт данных */
#define TX_FRAME_CONTROL_TYPE_EN	(1 << 12) 	/* Поле длины задаёт тип */
#define TX_FRAME_CONTROL_FCS_CLT_EN	(1 << 13) 	/* Контрольная сумма из регистра */
#define TX_FRAME_CONTROL_DISENCAPFR	(1 << 14) 	/* Запрет формирования кадра в блоке передачи */
#define TX_FRAME_CONTROL_DISPAD		(1 << 15) 	/* Запрет заполнителей */
#define TX_FRAME_CONTROL_TX_REQ		(1 << 16) 	/* Передача кадра */

/*
 * STATUS_TX - статус передачи кадра
 */
#define STATUS_TX_ONTX_REQ		(1 << 0)	/* Выполняется запрос передачи */
#define STATUS_TX_ONTRANSMIT		(1 << 1)	/* Выполняется передача кадра */
#define STATUS_TX_BUSY			(1 << 2)	/* Среда передачи занята */
#define STATUS_TX_DONE			(1 << 3)	/* Передача завершена */
#define STATUS_TX_TRANSMIT_DISABLED	(1 << 4)	/* Передача не разрешена */
#define STATUS_TX_NOT_ENOUGH_DATA	(1 << 5)	/* Мало данных в TX FIFO */
#define STATUS_TX_TRANSMIT_OK		(1 << 6)	/* Передача кадра успешно выполнена */
#define STATUS_TX_EXCESSIVE_COLL	(1 << 7)	/* Повторная передача не удалась */
#define STATUS_TX_LATE_COLL		(1 << 8)	/* Поздняя коллизия */
#define STATUS_TX_ONCOL			(1 << 11)	/* Коллизия в среде передачи */
#define STATUS_TX_COLL_NUM(s)		((s) >> 12 & 15) /* Счетчик попыток повторных передач */
#define STATUS_TX_TXW(s)		((s) >> 16 & 0x3ff) /* Число слов в TX FIFO */
#define STATUS_TX_BITS			"\20"\
"\1ONTX_REQ\2ONTRANSMIT\3BUSY\4DONE\5DISABLED\6NDATA\7TXOK\10EXCCOLL"\
"\11LATECOLL\14ONCOL"

/*
 * RX_FRAME_CONTROL - управление приемом кадра
 */
#define RX_FRAME_CONTROL_DIS_RCV_FCS	(1 << 0) 	/* Отключение сохранения контрольной суммы */
#define RX_FRAME_CONTROL_DIS_PAD_DEL	(1 << 1) 	/* Отключение удаления заполнителей */
#define RX_FRAME_CONTROL_ACC_TOOSHORT	(1 << 2) 	/* Прием коротких кадров */
#define RX_FRAME_CONTROL_DIS_TOOLONG	(1 << 3) 	/* Отбрасывание слишком длинных кадров */
#define RX_FRAME_CONTROL_DIS_FCSCHERR	(1 << 4) 	/* Отбрасывание кадров с ошибкой контрольной суммы */
#define RX_FRAME_CONTROL_DIS_LENGTHERR	(1 << 5) 	/* Отбрасывание кадров с ошибкой длины */
#define RX_FRAME_CONTROL_DIS_BC		(1 << 6) 	/* Запрещение приема кадров с широковещательным адресом */
#define RX_FRAME_CONTROL_EN_MCM		(1 << 7) 	/* Разрешение приема кадров с групповым адресом по маске */
#define RX_FRAME_CONTROL_EN_MCHT	(1 << 8) 	/* Разрешение приема кадров с групповым адресом по хеш-таблице */
#define RX_FRAME_CONTROL_EN_ALL		(1 << 9) 	/* Разрешение приема кадров с любым адресом */

/*
 * STATUS_RX - статус приема кадра
 */
#define STATUS_RX_RCV_DISABLED		(1 << 0)		/* Приём не разрешён */
#define STATUS_RX_ONRECEIVE		(1 << 1)		/* Выполняется приём кадра */
#define STATUS_RX_DONE			(1 << 3)		/* Есть кадры в RX FIFO */
#define STATUS_RX_NUM_FR(s)		((s) >> 4 & 0x7f)	/* Число принятых кадров */
#define STATUS_RX_STATUS_OVF		(1 << 11)		/* Переполнение FIFO статусов */
#define STATUS_RX_RXW(s)		((s) >> 12 & 0x3ff)	/* Число слов в RX FIFO */
#define STATUS_RX_FIFO_OVF		(1 << 23)		/* Переполнение FIFO данных */
#define STATUS_RX_NUM_MISSED(s)		((s) >> 24 & 0x3f)	/* Число пропущенных кадров */
#define STATUS_RX_BITS			"\30"\
"\1DISABLED\2ONRECEIVE\4DONE\14STATUS_OVF\30FIFO_OVF"

/*
 * RX_FRAME_STATUS_FIFO - FIFO статусов принятых кадров
 */
#define RX_FRAME_STATUS_LEN(s)		((s) & 0xfff)	/* Число байт в принятом кадре */
#define RX_FRAME_STATUS_OK		(1 << 12)	/* Кадр принят без ошибок */
#define RX_FRAME_STATUS_LENGTH_ERROR	(1 << 13)	/* Ошибка длины данных */
#define RX_FRAME_STATUS_ALIGN_ERROR	(1 << 14)	/* Ошибка выравнивания */
#define RX_FRAME_STATUS_FRAME_ERROR	(1 << 15)	/* Ошибка формата кадра */
#define RX_FRAME_STATUS_TOO_LONG	(1 << 16)	/* Слишком длинный кадр */
#define RX_FRAME_STATUS_TOO_SHORT	(1 << 17)	/* Слишком короткий кадр */
#define RX_FRAME_STATUS_DRIBBLE_NIBBLE	(1 << 18)	/* Нечётное число полубайт */
#define RX_FRAME_STATUS_LEN_FIELD	(1 << 19)	/* Распознавание поля LENGTH */
#define RX_FRAME_STATUS_FCS_DEL		(1 << 20)	/* Удаление поля FCS */
#define RX_FRAME_STATUS_PAD_DEL		(1 << 21)	/* Удаление поля PAD */
#define RX_FRAME_STATUS_UC		(1 << 22)	/* Распознавание адреса MAC */
#define RX_FRAME_STATUS_MCM		(1 << 23)	/* Групповой адрес по маске */
#define RX_FRAME_STATUS_MCHT		(1 << 24)	/* Групповой адрес по хэш-таблице */
#define RX_FRAME_STATUS_BC		(1 << 25)	/* Широковещательный адрес */
#define RX_FRAME_STATUS_ALL		(1 << 26)	/* Приём кадров с любым адресом */

/*
 * MD_MODE - режим работы порта MD
 */
#define MD_MODE_DIVIDER(n)		((n) & 0xff)	/* делитель для частоты MDC */
#define MD_MODE_RST			(1 << 8)	/* сброс порта PHY */

/*
 * MD_CONTROL - управление порта MD
 */
#define MD_CONTROL_DATA(n)		((n) & 0xffff)		/* данные для записи */
#define MD_CONTROL_REG(n)		(((n) & 0x1f) << 16)	/* адрес регистра PHY */
#define MD_CONTROL_PHY(n)		(((n) & 0x1f) << 24)	/* адрес PHY */
#define MD_CONTROL_IRQ(n)		(1 << 29)		/* нужно прерывание */
#define MD_CONTROL_OP_READ		(1 << 30)		/* операция чтения */
#define MD_CONTROL_OP_WRITE		(2 << 30)		/* операция записи */

/*
 * MD_STATUS - статус порта MD
 */
#define MD_STATUS_DATA			0xffff		/* прочитанные данные */
#define MD_STATUS_BUSY			(1 << 29)	/* порт занят */
#define MD_STATUS_END_READ		(1 << 30)	/* завершилась операция чтения */
#define MD_STATUS_END_WRITE		(1 << 31)	/* завершилась операция записи */

/*-------------------------------------------
 * Регистры порта вывода видеоданных VPOUT
 */
 
/*
 * CSR_VPOUT - регистр управления и состояния VPOUT
 */
#define VPOUT_CSR_INT			(1 << 0)	/* Флаг прерывания */
#define VPOUT_CSR_FIFO_EMPTY		(1 << 1)	/* Признак пустой очереди FIFO */
#define VPOUT_CSR_FIFO_ERR		(1 << 2)	/* Признак ошибки FIFO */
#define VPOUT_CSR_SNAPSHOT		(1 << 19)	/* Режим Snapshot */
#define VPOUT_CSR_EN_VSYNC		(1 << 20)	/* 0 - внешняя, 1 - внутренняя синхронизация VSYNC */
#define VPOUT_CSR_EN_HSYNC		(1 << 21)	/* 0 - внешняя, 1 - внутренняя синхронизация HSYNC */
#define VPOUT_CSR_EN_VCLKO		(1 << 22)	/* 0 - внешняя, 1 - внутренняя синхронизация VCLKO */
#define VPOUT_CSR_MASK_RD_EMPTY_FIFO	(1 << 23)	/* Разрешение прерывания по чтению из пустого FIFO */
#define VPOUT_CSR_MASK_HEND		(1 << 24)	/* Разрешение прерывания по концу строки */
#define VPOUT_CSR_MASK_HSTART		(1 << 25)	/* Разрешение прерывания по началу строки */
#define VPOUT_CSR_MASK_VEND		(1 << 26)	/* Разрешение прерывания по концу кадра */
#define VPOUT_CSR_MASK_VSTART		(1 << 27)	/* Разрешение прерывания по началу кадра */
#define VPOUT_CSR_MASK_FIFO_EMPTY	(1 << 28)	/* Разрешение прерывания по пустому FIFO */
#define VPOUT_CSR_FEN			(1 << 29)	/* Разрешение переключения сигнала поля (F) */
#define VPOUT_CSR_RUN			(1 << 30)	/* Запуск порта */
#define VPOUT_CSR_CLR			(1 << 31)	/* Очистка порта */

/*
 * FRAME_CNT_VPOUT - счетчик кадров VPOUT
 */
#define VPOUT_FC_FRAME_CNT(n)		(n & 0xFFFFFF)	/* Счетчик кадров */
#define VPOUT_FC_HSYNC			(1 << 29)	/* Текущее состояние сигнала строчной синхронизации */
#define VPOUT_FC_VSYNC			(1 << 30)	/* Текущее состояние сигнала кадровой синхронизации */
#define VPOUT_FC_F			(1 << 31)	/* Текущее поле */


/*-------------------------------------------
 * Регистры порта MFBSP
 */
 
/*
 * MFBSP_CSR - регистр управления MFBSP
 */
#define MC_MFBSP_LEN			(1 << 0)
#define MC_MFBSP_LTRAN			(1 << 1)
#define MC_MFBSP_LCLK_RATE(n)		((((n) & 1) << 2) | (((n) & 0x1e) << 10))
#define MC_MFBSP_LSTAT(n)		((n) << 3)
#define MC_MFBSP_LRERR			(1 << 5)
#define MC_MFBSP_LDW			(1 << 6)
#define MC_MFBSP_SRQ_TX			(1 << 7)
#define MC_MFBSP_SRQ_RX			(1 << 8)
#define MC_MFBSP_SPI_I2S_EN		(1 << 9)

#define MC_MFBSP_GET_LCLK_RATE(x)	(((x) >> 2) & 1) | (((x) >> 10) & 0x1e)

/*
 * MFBSP_DIR - регистр управления направлением выводов MFBSP
 */
#define MC_MFBSP_RCLK_DIR		(1 << 0)
#define MC_MFBSP_TCLK_DIR		(1 << 1)
#define MC_MFBSP_RCS_DIR		(1 << 2)
#define MC_MFBSP_TCS_DIR		(1 << 3)
#define MC_MFBSP_RD_DIR			(1 << 4)
#define MC_MFBSP_TD_DIR			(1 << 5)

/*
 * MC_MFBSP_RCTR - регистр управления приёмником MFBSP
 */
#define MC_MFBSP_REN			(1 << 0)
#define MC_MFBSP_RMODE			(1 << 1)
#define MC_MFBSP_RCLK_CP		(1 << 2)
#define MC_MFBSP_RCS_CP			(1 << 3)
#define MC_MFBSP_RNEG			(1 << 10)
#define MC_MFBSP_RDEL			(1 << 11)
#define MC_MFBSP_RWORDCNT(n)		((n) << 12)
#define MC_MFBSP_RWORDCNT_MASK		MC_MFBSP_RWORDCNT(0x1F)
#define MC_MFBSP_RMBF			(1 << 19)
#define MC_MFBSP_RWORDLEN(n)		((n) << 20)
#define MC_MFBSP_RWORDLEN_MASK		MC_MFBSP_RWORDCNT(0x3F)
#define MC_MFBSP_RPACK			(1 << 25)
#define MC_MFBSP_RSIGN			(1 << 26)

/*
 * MC_MFBSP_TCTR - регистр управления передатчиком MFBSP
 */
#define MC_MFBSP_TEN			(1 << 0)
#define MC_MFBSP_TMODE			(1 << 1)
#define MC_MFBSP_TD_ZER_EN		(1 << 2)
#define MC_MFBSP_SS_DO			(1 << 3)
#define MC_MFBSP_TDSPMODE		(1 << 9)
#define MC_MFBSP_TNEG			(1 << 10)
#define MC_MFBSP_TDEL			(1 << 11)
#define MC_MFBSP_TWORDCNT(n)		((n) << 12)
#define MC_MFBSP_TWORDCNT_MASK		MC_MFBSP_TWORDCNT(0x1F)
#define MC_MFBSP_TCSNEG			(1 << 18)
#define MC_MFBSP_TMBF			(1 << 19)
#define MC_MFBSP_TWORDLEN(n)		((n) << 20)
#define MC_MFBSP_TWORDLEN_MASK		MC_MFBSP_TWORDCNT(0x3F)
#define MC_MFBSP_TPACK			(1 << 25)
#define MC_MFBSP_TSWAP			(1 << 27)
#define MC_MFBSP_TCLK_CONT		(1 << 28)
#define MC_MFBSP_TCS_CONT		(1 << 29)
#define MC_MFBSP_SS(n)			(1 << (30 + (n)))

/* 
 * MFBSP_GPIO_DR и MFBSP_DIR
 */
#define MC_MFBSP_GPIO_LACK		(1 << 0)
#define MC_MFBSP_GPIO_LCLK		(1 << 1)
#define MC_MFBSP_GPIO_LDAT(n)		(1 << ((n) + 2))

/*
 * MFBSP_RSR
 */
#define MC_MFBSP_RBE			(1 << 0)
#define MC_MFBSP_RBF			(1 << 1)
#define MC_MFBSP_RBHF			(1 << 2)
#define MC_MFBSP_RBHL			(1 << 3)
#define MC_MFBSP_RSBE			(1 << 4)
#define MC_MFBSP_RSBF			(1 << 5)
#define MC_MFBSP_RXBUF_R		(1 << 8)
#define MC_MFBSP_RXBUF_D		(1 << 9)
#define MC_MFBSP_RXBUF			(1 << 10)
#define MC_MFBSP_RLEV(n)		((n) << 16)
#define MC_MFBSP_RB_DIFF(x)		(((x) >> 24) & 0xF)

/*
 * MFBSP_TSR
 */
#define MC_MFBSP_TBE			(1 << 0)
#define MC_MFBSP_TBF			(1 << 1)
#define MC_MFBSP_TBHF			(1 << 2)
#define MC_MFBSP_TBLL			(1 << 3)
#define MC_MFBSP_TSBE			(1 << 4)
#define MC_MFBSP_TSBF			(1 << 5)
#define MC_MFBSP_TXBUF_R		(1 << 8)
#define MC_MFBSP_TXBUF_D		(1 << 9)
#define MC_MFBSP_TXBUF			(1 << 10)
#define MC_MFBSP_TLEV(n)		((n) << 16)
#define MC_MFBSP_TBES(n)		((n) << 20)
#define MC_MFBSP_TB_DIFF(x)		(((x) >> 24) & 0xF)

/*
 * MFBSP_RCTR_RATE
 */
#define MC_MFBSP_RCLK_RATE(n)		(n)
#define MC_MFBSP_RSS_RATE(n)		((n) << 12)
#define MC_MFBSP_RCS_RATE(n)		((n) << 16)

/*
 * MFBSP_TCTR_RATE
 */
#define MC_MFBSP_TCLK_RATE(n)		(n)
#define MC_MFBSP_TSS_RATE(n)		((n) << 12)
#define MC_MFBSP_TCS_RATE(n)		((n) << 16)

/*
 * MFBSP_EMERG
 */
#define MC_MFBSP_RST_LPTBUF		(1 << 0)
#define MC_MFBSP_RST_TXBUF		(1 << 1)
#define MC_MFBSP_RST_RXBUF		(1 << 2)
#define MC_MFBSP_RST_TX_DBG		(1 << 4)
#define MC_MFBSP_RST_RX_DBG		(1 << 5)

/*
 * MFBSP_IMASK
 */
#define MC_MFBSP_LPT_IRQ_EN		(1 << 0)
#define MC_MFBSP_TX_ERR_IRQ_EN		(1 << 4)
#define MC_MFBSP_TX_LEV_IRQ_EN		(1 << 5)
#define MC_MFBSP_TXBUF_R_EN		(1 << 6)
#define MC_MFBSP_RX_ERR_IRQ_EN		(1 << 12)
#define MC_MFBSP_RX_LEV_IRQ_EN		(1 << 13)
#define MC_MFBSP_RXBUF_R_EN		(1 << 14)

/*-------------------------------------------
 * Регистры порта I2C
 */

/*
 * I2C_CTR - регистр управления
 */
#define MC_I2C_IEN           (1 << 6)
#define MC_I2C_EN            (1 << 7)
#define MC_I2C_PRST          (1 << 8)
#define MC_I2C_TM_CNT        (1 << 9)
#define MC_I2C_TICK          (1 << 10)

/*
 * I2C_CR - регистр команд
 */
#define MC_I2C_IACK          (1 << 0)
#define MC_I2C_NACK          (1 << 3)
#define MC_I2C_SND           (1 << 4)
#define MC_I2C_RCV           (1 << 5)
#define MC_I2C_STO           (1 << 6)
#define MC_I2C_STA           (1 << 7)

/*
 * I2C_SR - регистр состояния
 */
#define MC_I2C_IF            (1 << 0)
#define MC_I2C_TIP           (1 << 1)
#define MC_I2C_AL            (1 << 5)
#define MC_I2C_BUSY          (1 << 6)
#define MC_I2C_RXACK         (1 << 7)

/*
 * I2C_SYNC - регистр синхронизации
 */
#define MC_I2C_WAIT_PER(x)   ((x) & 0xFFFF)
#define MC_I2C_SYNC_EN       (1 << 16)

/*-------------------------------------------
 * Регистры PCI
 */

/*
 * Состояние и управление (STATUS/COMMAND)
 */
#define MC_PCI_COMMAND_MEMORY		    0x00000002	/* Разрешение выполнение в режиме Target команд Memory Read, Memory Write */
#define MC_PCI_COMMAND_MASTER		    0x00000004	/* Разрешение работы на шине PCI в режиме Master  */
#define MC_PCI_COMMAND_PARITY		    0x00000040	/* Разрешение формирование сигнала PERR */
#define MC_PCI_COMMAND_IDISABLE	        0x00000400	/* Запрещение формирования сигнала прерывания */
#define MC_PCI_STATUS_ISTATUS  	        0x00080000	/* Наличие незамаскированных прерываний */
#define MC_PCI_STATUS_FASTB2B  	        0x00800000	/* Отражает способность PMSC выполнять транзакции типа  “Fast Back-to-Back” */
#define MC_PCI_STATUS_PARITY		    0x01000000	/* Признак выдачи или приема сигнала PERR в режиме Master */
#define MC_PCI_STATUS_DEVSEL_MASK	    0x06000000	/* DEVSEL timing = 01 (medium) */
#define MC_PCI_STATUS_SIG_TARGET_ABORT  0x08000000	/* Signaled Target Abort */
#define MC_PCI_STATUS_TARGET_ABORT	    0x10000000	/* Признак завершения обмена по условию Target-abort */
#define MC_PCI_STATUS_MASTER_ABORT	    0x20000000	/* Признак завершения обмена по условию Master-abort */
#define MC_PCI_STATUS_DETECTED_PARITY	0x80000000	/* Ошибка четности при приме данных из PCI */

/*
 * Latency Timer
 */
#define MC_PCI_MLT(x)               ((x) << 8)  /* Время в тактах PCLK, отведенное для выполнения транзакции в режиме Master. */

/*
 * Interrupt Line
 */
#define MC_PCI_INTR_LINE(x)         (x)
#define MC_PCI_GET_INTR_PIN(r)      (((r) >> 8) & 0xFF)
#define MC_PCI_GET_MIN_GNT(r)       (((r) >> 16) & 0xFF)
#define MC_PCI_GET_MIN_LAT(r)       (((r) >> 24) & 0xFF)

/* 
 * Управление шиной PCI (CSR_PCI)
 */
#ifdef ELVEES_MC0428

#define MC_PCI_CSR_PCI_WNT(n)       (n)         /* Количество слов, которое должно накопиться в буфере WFIFO для передачи
                                                   очередной порции данных в коммутатор SWITCH в режиме Target */
#define MC_PCI_CSR_PCI_SEL_TS_LAT   0x00000010  /* Разрешение изменения параметра ”Target Subsequent Latency” */
#define MC_PCI_CSR_PCI_SEL_TI_LAT   0x00000020  /* Разрешение изменения параметра ”Target Initial Latency” */
#define MC_PCI_CSR_PCI_TEST_PERR    0x00000040  /* Режим формирования выходного сигнала nPERR */
#define MC_PCI_CSR_PCI_TEST_PAR     0x00000080  /* Режим формирования выходного сигнала PAR */
#define MC_PCI_CSR_PCI_TS_LAT(n)    ((n) << 8)  /* Target Subsequent Latency в тактах PCLK */
#define MC_PCI_CSR_PCI_TI_LAT(n)    ((n) << 12) /* Target Initial Latency в тактах PCLK */
#define MC_PCI_CSR_PCI_TGT_PAR_STOP 0x00010000  /* Разрешение завершения транзакции в режиме Target установкой признака Signaled Target Abort */
#define MC_PCI_CSR_PCI_TGT_DPEA     0x00040000  /* Устанавливается в 1 в режиме Target, если в фазе адреса обнаружена ошибка чётности */
#define MC_PCI_CSR_PCI_TGT_DPED     0x00080000  /* Устанавливается в 1, если обнаружена ошибка чётности в фазе данных 
                                                   при выполнении записи на шине PCI в режиме Target */
#define MC_PCI_CSR_PCI_MST_PAR_STOP 0x00100000  /* Разрешение прекращения передачи блока данных и формирования прерывания MASTER _ERROR */
#define MC_PCI_CSR_PCI_MST_DPEWR    0x00400000  /* Обнаружен низкий уровень сигнала nPERR при выполнении записи на шине PCI в режиме Master */
#define MC_PCI_CSR_PCI_MST_DPERD    0x00800000  /* Обнаружена ошибка чётности при выполнении чтения на шине PCI в режиме Master */
#define MC_PCI_CSR_PCI_MST_BREAK    0x01000000  /* Программный останов передачи блока данных */
#define MC_PCI_CSR_PCI_BREAK_DONE   0x02000000  /* Состояние признака Break Done в регистре CSR_MASTER */
#define MC_PCI_CSR_PCI_NO_GNT       0x04000000  /* Состояние признака No Gnt в регистре STATUS_MASTER */
#define MC_PCI_CSR_PCI_NO_TRDY      0x08000000  /* Состояние признака No Trdy в регистре STATUS_MASTER */
#define MC_PCI_CSR_PCI_TGT_ABORT    0x10000000  /* Состояние признака Received Target Abort в регистре Status/Command */
#define MC_PCI_CSR_PCI_MST_ABORT    0x20000000  /* Состояние признака Reсeived Master Abort в регистре Status/Command */
#define MC_PCI_CSR_PCI_WR_PAR_ERR   0x40000000  /* Ошибка при выполнении записи на шине PCI. */
#define MC_PCI_CSR_PCI_RD_PAR_ERR   0x80000000  /* Ошибка при выполнении чтения на шине PCI. */

#else

#define MC_PCI_CSR_PCI_INTA		    0x00000001	/* Формирование прерывания INTA */
#define MC_PCI_CSR_PCI_WN(n)		((n) << 1)	/* Уровень FIFO записи в память, исходно 8 */
#define MC_PCI_CSR_PCI_TESTPERR	    0x00000040	/* Принудительное формирование сигнала nPERR */
#define MC_PCI_CSR_PCI_TESTPAR		0x00000080	/* Инвертирование сигнала PAR */
#define MC_PCI_CSR_PCI_MLTOVER		0x00010000	/* Признак срабатывания Latency Timer */
#define MC_PCI_CSR_PCI_NOTRDY		0x00020000	/* Признак отсутствия сигнала TRDY */
#define MC_PCI_CSR_PCI_DISCONNECT	0x00040000	/* Признак требования разъединения */
#define MC_PCI_CSR_PCI_RETRY		0x00080000	/* Признак требования повтора */
#define MC_PCI_CSR_PCI_NOGNT		0x00100000	/* Признак отсутствия сигнала nGNT */
#define MC_PCI_CSR_PCI_TARGET_ABORT	0x10000000	/* Признак окончания обмена с Target-Abort */
#define MC_PCI_CSR_PCI_MASTER_ABORT	0x20000000	/* Признак окончания обмена с Master-Abort */

#endif

/*
 * Состояние и управление обменом в режиме Master (CSR_MASTER)
 */
#define MC_PCI_CSR_MASTER_RUN		    0x00000001	/* Состояние работы канала DMA */
#define MC_PCI_CSR_MASTER_CMD		    0x0000001e	/* Тип команды обмена в режиме Master */
#define MC_PCI_CSR_MASTER_IOREAD	    0x00000004	/* I/O Read */
#define MC_PCI_CSR_MASTER_IOWRITE	    0x00000006	/* I/O Write */
#define MC_PCI_CSR_MASTER_MEMREAD	    0x0000000c	/* Memory Read */
#define MC_PCI_CSR_MASTER_MEMWRITE	    0x0000000e	/* Memory Write */
#define MC_PCI_CSR_MASTER_CFGREAD	    0x00000014	/* Configuration Read */
#define MC_PCI_CSR_MASTER_CFGWRITE	    0x00000016	/* Configuration Write */
#define MC_PCI_CSR_MASTER_SEL_IRD_LAT   0x00000020  /* Разрешение изменения параметра  ”nIRDY Latency” */
#define MC_PCI_CSR_MASTER_SEL_MS_LAT    0x00000040  /* Разрешение изменения параметра  ”Master Subsequent Latency” */
#define MC_PCI_CSR_MASTER_SEL_MI_LAT    0x00000080  /* Разрешение изменения параметра  ”Master Initial Latency” */
#define MC_PCI_CSR_MASTER_WNM(n)        ((n) << 8)  /* Количество слов, которое должно накопиться в буфере WFIFO 
                                                       для передачи очередной порции данных в коммутатор SWITCH 
                                                       в режиме Master. */
#define MC_PCI_CSR_MASTER_WINDOW        0x00001000  /* Индикатор выполнения обмена через адресное окно. */
#define MC_PCI_CSR_MASTER_BREAK_DONE    0x00002000  /* Индикатор выполнения программного останова передачи блока данных. */
#define MC_PCI_CSR_MASTER_FATAL_ERR     0x00004000  /* Индикатор останова передачи  блока данных по фатальной ошибке. */
#define MC_PCI_CSR_MASTER_DONE		    0x00008000	/* Признак завершения передачи */
#define MC_PCI_CSR_MASTER_WC(n)	        ((n) << 16)	/* Счетчик слов DMA обмена */

/*
 * STATUS_MASTER
 */
#define MC_PCI_STATUS_MASTER_WCC(n)     (n)             /* Текущий размер блока данных */
#define MC_PCI_STATUS_MASTER_RUN        0x00010000      /* Состояние признака RUN в регистре CSR_MASTER */
#define MC_PCI_STATUS_MASTER_TRDY_OUT   0x00100000      /* Транзакция завершена с признаком TRDY OUT */
#define MC_PCI_STATUS_MASTER_IRDY_OUT   0x00200000      /* Транзакция завершена с признаком IRDY OUT */
#define MC_PCI_STATUS_MASTER_TIMEOUT    0x00400000      /* Транзакция завершена с признаком Timeout */
#define MC_PCI_STATUS_MASTER_RETRY      0x00800000      /* Транзакция завершена с признаком Retry */
#define MC_PCI_STATUS_MASTER_DISCONN    0x01000000      /* Транзакция завершена с признаком Disconnect */
#define MC_PCI_STATUS_MASTER_BREAK_DONE 0x02000000      /* Состояние признака Break Done в регистре CSR_MASTER */
#define MC_PCI_STATUS_MASTER_NO_GNT     0x04000000      /* Признак отсутствия сигнала nGNT в течение 4095 тактов 
                                                           шины PCI после установки сигнала nREQ */
#define MC_PCI_STATUS_MASTER_NO_TRDY    0x08000000      /* Транзакция завершена с признаком NO TRDY */
#define MC_PCI_STATUS_MASTER_TARGET_ABORT   0x10000000  /* Состояние признака Received Target Abort в регистре Status/Command */
#define MC_PCI_STATUS_MASTER_MASTER_ABORT   0x20000000  /* Состояние признака Reсeived Master Abort в регистре Status/Command */
#define MC_PCI_STATUS_MASTER_WR_PAR_ERR     0x40000000  /* Ошибка при выполнении записи на шине PCI. */
#define MC_PCI_STATUS_MASTER_RD_PAR_ERR     0x80000000  /* Ошибка при выполнении чтения на шине PCI. */

/*
 * TMR_PCI
 */
#define MC_PCI_TMR_PCI_IRD_LAT(n)       (n)             /* nIRDY Latency  в тактах PCLK */
#define MC_PCI_TMR_PCI_MS_LAT(n)        ((n) << 4)      /* Master Subsequent Latency в тактах PCLK */
#define MC_PCI_TMR_PCI_MI_LAT(n)        ((n) << 8)      /* Master Initial Latency в тактах PCLK */
#define MC_PCI_TMR_PCI_WATERMARK(n)     ((n) << 16)     /* Пороговое значение для формирования прерывания WMARK */

/*
 * CSR_WIN
 */
#define MC_PCI_CSR_WIN_CMD		        0x0000001e	/* Тип команды при обмене через адресное окно */
#define MC_PCI_CSR_WIN_IOREAD	        0x00000004	/* I/O Read */
#define MC_PCI_CSR_WIN_IOWRITE	        0x00000006	/* I/O Write */
#define MC_PCI_CSR_WIN_MEMREAD	        0x0000000c	/* Memory Read */
#define MC_PCI_CSR_WIN_MEMWRITE	        0x0000000e	/* Memory Write */
#define MC_PCI_CSR_WIN_CFGREAD	        0x00000014	/* Configuration Read */
#define MC_PCI_CSR_WIN_CFGWRITE	        0x00000016	/* Configuration Write */
#define MC_PCI_CSR_WIN_SEL_NCBE         0x00000020  /* Выбор nCBE */
#define MC_PCI_CSR_WIN_SEL_ADR          0x00000040  /* Выбор адреса */
#define MC_PCI_CSR_WIN_SEL_MIL          0x00000080  /* Выбор параметра ”Master Initial Latency” */
#define MC_PCI_CSR_WIN_MIL_WIN(n)       ((n) << 12) /* Значение ”Master Initial Latency”в тактах PCLK */
#define MC_PCI_CSR_WIN_MASK_DPE         0x00010000  /* Разрешение формирования  прерывания  MASTER _ERROR */
#define MC_PCI_CSR_WIN_NCBE_WIN(n)      ((n) << 20) /* если SEL_nCBE=1, то состояние этого поля передается на выводы nCBE[3:0] в фазе данных */
#define MC_PCI_CSR_WIN_AR_WIN(n)        ((n) << 24) /* если SEL_ ADR=1, то состояние этого поля передается на выводы AD[31:24] в фазе адреса */

#endif /* _IO_ELVEES_H */
