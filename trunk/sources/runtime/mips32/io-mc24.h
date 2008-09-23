/*
 * Hardware register defines for Elvees MC-24 microcontroller.
 */
#ifndef _IO_MC24_H
#define _IO_MC24_H

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
#define ST_IM		0x0000ff00	/* разрешение прерываний */

#define ST_NMI		0x00080000	/* причина сброса - NMI */
#define ST_TS		0x00200000	/* TLB-закрытие системы */
#define ST_BEV		0x00400000	/* размещение векторов: начальная загрузка */

/*
 * Сause register.
 */
#define CA_EXC_CODE	0x0000007c
#define CA_Int		0
#define CA_Mod		(1 << 2)
#define CA_TLBL		(2 << 2)
#define CA_TLBS		(3 << 2)
#define CA_AdEL		(4 << 2)
#define CA_AdES		(5 << 2)
#define CA_Sys		(8 << 2)
#define CA_Bp		(9 << 2)
#define CA_RI		(10 << 2)
#define CA_CpU		(11 << 2)
#define CA_Ov		(12 << 2)
#define CA_Tr		(13 << 2)
#define CA_MCheck	(24 << 2)

#define CA_IP		0x0000ff00
#define CA_IV		0x00800000
#define CA_BD		0x80000000


/*--------------------------------------
 * Регистры управления периферией Elvees Multicore.
 */
#define MC_R(a)		*(volatile unsigned*)(0xB82F0000 + (a))

/*
 * Системные регистры
 */
#define MC_MASKR	MC_R (0x4000)	/* Регистр маски */
#define MC_QSTR		MC_R (0x4004)	/* Регистр заявок */
#define MC_CSR		MC_R (0x4008)	/* Регистр управления */

/*
 * Регистры порта внешней памяти
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100С)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_CKE_CTR	MC_R (0x1018)	/* Регистр управления состоянием вывода CKE микросхемы */

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
#define MC_LTX(n)	MC_R (0x7000+(n<<12))	/* Буфер передачи */
#define MC_LRX(n)	MC_R (0x7000+(n<<12))	/* Буфер приема */
#define MC_LCSR(n)	MC_R (0x7004+(n<<12))	/* Регистр управления и состояния */
#define MC_LDIR(n)	MC_R (0x7008+(n<<12))	/* Регистр управления */
#define MC_LDR(n)	MC_R (0x700C+(n<<12))	/* Регистр данных */

/*
 * Регистры портов обмена последовательным кодом SPORT0, SPORT1
 */
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

/*
 * Регистры DMA
 */
/* Каналы SpTx0, SpTx1 */
#define MC_CSR_SPTX(n)	MC_R (0x0000+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPTX(n)	MC_R (0x0008+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPTX(n)	MC_R (0x000C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPTX(n)	MC_R (0x0010+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPTX(n)	MC_R (0x0014+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы SpRx0, SpRx1 */
#define MC_CSR_SPRX(n)	MC_R (0x0100+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPRX(n)	MC_R (0x0108+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPRX(n)	MC_R (0x010C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPRX(n)	MC_R (0x0110+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPRX(n)	MC_R (0x0114+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
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

/*
 * Системный регистр CSR
 */
#define MC_CSR_FM	0x00000001	/* Fixed mapping */
#define MC_CSR_CLK(n)	(n << 4)	/* PLL clock multiply, 1..31, 0=1/16 */
#define MC_CSR_FLUSH	0x00001000	/* instriction cache invalidate */
#define MC_CSR_CLKEN	0x00010000	/* PLL clock enable */

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
#define MC_DL_BAUD(fr,bd)	((fr/8 + (bd)) / (bd) / 2)

#endif /* _IO_MC24_H */
