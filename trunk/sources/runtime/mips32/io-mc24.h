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
/*
 * Системные регистры
 */
#define MC_MASKR	*(unsigned*)(0xB82F4000)	/* Регистр маски */
#define MC_QSTR		*(unsigned*)(0xB82F4004)	/* Регистр заявок */
#define MC_CSR		*(unsigned*)(0xB82F4008)	/* Регистр управления */

/*
 * Регистры порта внешней памяти
 */
#define MC_CSCON0	*(unsigned*)(0xB82F1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	*(unsigned*)(0xB82F1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	*(unsigned*)(0xB82F1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	*(unsigned*)(0xB82F100С)	/* Регистр конфигурации 3 */
#define MC_CSCON4	*(unsigned*)(0xB82F1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	*(unsigned*)(0xB82F1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_CKE_CTR	*(unsigned*)(0xB82F1018)	/* Регистр управления состоянием вывода CKE микросхемы */

/*
 * Регистры UART
 */
#define MC_RBR		*(unsigned*)(0xB82F3000)	/* Приемный буферный регистр */
#define MC_THR		*(unsigned*)(0xB82F3000)	/* Передающий буферный регистр */
#define MC_IER		*(unsigned*)(0xB82F3004)	/* Регистр разрешения прерываний */
#define MC_IIR		*(unsigned*)(0xB82F3008)	/* Регистр идентификации прерывания */
#define MC_FCR		*(unsigned*)(0xB82F3008)	/* Регистр управления FIFO */
#define MC_LCR		*(unsigned*)(0xB82F300C)	/* Регистр управления линией */
#define MC_MCR		*(unsigned*)(0xB82F3010)	/* Регистр управления модемом */
#define MC_LSR		*(unsigned*)(0xB82F3014)	/* Регистр состояния линии */
#define MC_MSR		*(unsigned*)(0xB82F3018)	/* Регистр состояния модемом */
#define MC_SPR		*(unsigned*)(0xB82F301C)	/* Регистр Scratch Pad */
#define MC_DLL		*(unsigned*)(0xB82F3000)	/* Регистр делителя младший */
#define MC_DLM		*(unsigned*)(0xB82F3004)	/* Регистр делителя старший */
#define MC_SCLR		*(unsigned*)(0xB82F3014)	/* Регистр предделителя (scaler) */

/*
 * Регистры интервального таймера (IT)
 */
#define MC_ITCSR	*(unsigned*)(0xB82FD000)	/* Регистр управления */
#define MC_ITPERIOD	*(unsigned*)(0xB82FD004)	/* Регистр периода работы таймера */
#define MC_ITCOUNT	*(unsigned*)(0xB82FD008)	/* Регистр счетчика */
#define MC_ITSCALE	*(unsigned*)(0xB82FD00C)	/* Регистр предделителя */

/*
 * Регистры WDT
 */
#define MC_WTCSR	*(unsigned*)(0xB82FD010)	/* Регистр управления */
#define MC_WTPERIOD	*(unsigned*)(0xB82FD014)	/* Регистр периода работы таймера */
#define MC_WTCOUNT	*(unsigned*)(0xB82FD018)	/* Регистр счетчика */
#define MC_WTSCALE	*(unsigned*)(0xB82FD01C)	/* Регистр предделителя */

/*
 * Регистры RTT
 */
#define MC_RTCSR	*(unsigned*)(0xB82FD020)	/* Регистр управления */
#define MC_RTPERIOD	*(unsigned*)(0xB82FD024)	/* Регистр периода работы таймера */
#define MC_RTCOUNT	*(unsigned*)(0xB82FD028)	/* Регистр счетчика */

/*
 * Регистры линковых портов LPORT0...LPORT3
 */
#define MC_LTX(n)	*(unsigned*)(0xB82F7000+(n<<12)) /* Буфер передачи */
#define MC_LRX(n)	*(unsigned*)(0xB82F7000+(n<<12)) /* Буфер приема */
#define MC_LCSR(n)	*(unsigned*)(0xB82F7004+(n<<12)) /* Регистр управления и состояния */
#define MC_LDIR(n)	*(unsigned*)(0xB82F7008+(n<<12)) /* Регистр управления */
#define MC_LDR(n)	*(unsigned*)(0xB82F700C+(n<<12)) /* Регистр данных */

/*
 * Регистры портов обмена последовательным кодом SPORT0, SPORT1
 */
#define MC_STX(n)	*(unsigned*)(0xB82F5000+(n<<12)) /* Буфер передачи данных */
#define MC_RX(n)	*(unsigned*)(0xB82F5000+(n<<12)) /* Буфер приема данных */
#define MC_STCTL(n)	*(unsigned*)(0xB82F5004+(n<<12)) /* Регистр управления передачей данных */
#define MC_SRCTL(n)	*(unsigned*)(0xB82F5008+(n<<12)) /* Регистр управления приемом данных */
#define MC_TDIV(n)	*(unsigned*)(0xB82F500C+(n<<12)) /* Регистр коэффициентов деления при передаче */
#define MC_RDIV(n)	*(unsigned*)(0xB82F5010+(n<<12)) /* Регистр коэффициентов деления при приеме */
#define MC_MTCS(n)	*(unsigned*)(0xB82F5014+(n<<12)) /* Выбор канала передачи в многоканальном режиме */
#define MC_MRCS(n)	*(unsigned*)(0xB82F5018+(n<<12)) /* Выбор канала приема в многоканальном режиме */
#define MC_KEYWD(n)	*(unsigned*)(0xB82F501C+(n<<12)) /* Регистр кода сравнения */
#define MC_KEYMASK(n)	*(unsigned*)(0xB82F5020+(n<<12)) /* Регистр маски сравнения */
#define MC_MRCE(n)	*(unsigned*)(0xB82F5024+(n<<12)) /* Выбор канала для сравнения принимаемых данных */

/*
 * Регистры DMA
 */
/* Каналы SpTx0, SpTx1 */
#define MC_CSR_SPTX(n)	*(unsigned*)(0xB82F0000+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPTX(n)	*(unsigned*)(0xB82F0008+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPTX(n)	*(unsigned*)(0xB82F000C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPTX(n)	*(unsigned*)(0xB82F0010+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPTX(n)	*(unsigned*)(0xB82F0014+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы SpRx0, SpRx1 */
#define MC_CSR_SPRX(n)	*(unsigned*)(0xB82F0100+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPRX(n)	*(unsigned*)(0xB82F0108+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPRX(n)	*(unsigned*)(0xB82F010C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPRX(n)	*(unsigned*)(0xB82F0110+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPRX(n)	*(unsigned*)(0xB82F0114+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы LpCh0...LpCh3 */
#define MC_CSR_LPCH(n)	*(unsigned*)(0xB82F0400+(n<<8))	/* Регистр управления и состояния */
#define MC_CP_LPCH(n)	*(unsigned*)(0xB82F0408+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_LPCH(n)	*(unsigned*)(0xB82F040C+(n<<8))	/* Индексный регистр памяти */
#define MC_OR_LPCH(n)	*(unsigned*)(0xB82F0410+(n<<8))	/* Регистр смещения памяти */
#define MC_Y_LPCH(n)	*(unsigned*)(0xB82F0414+(n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы MemCh0...MemCh3 */
#define MC_CSR_MEMCH(n)	*(unsigned*)(0xB82F0800+(n<<8))	/* Регистр управления и состояния */
#define MC_IOR_MEMCH(n)	*(unsigned*)(0xB82F0804+(n<<8))	/* Регистр индекса и смещения внутренней памяти */
#define MC_CP_MEMCH(n)	*(unsigned*)(0xB82F0808+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_MEMCH(n)	*(unsigned*)(0xB82F080C+(n<<8))	/* Индексный регистр внешней памяти */
#define MC_OR_MEMCH(n)	*(unsigned*)(0xB82F0810+(n<<8))	/* Регистр смещения внешней памяти */
#define MC_Y_MEMCH(n)	*(unsigned*)(0xB82F0814+(n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации внешней памяти */
#define MC_RUN(n)	*(unsigned*)(0xB82F0818+(n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/*
 * Системный регистр CSR
 */
#define MC_CSR_FM	0x00000001	/* Fixed mapping */
#define MC_CSR_CLK(n)	(n << 4)	/* PLL clock multiply, 1..31, 0=1/16 */
#define MC_CSR_FLUSH	0x00001000	/* instriction cache invalidate */
#define MC_CSR_CLKEN	0x00010000	/* PLL clock enable */

#endif /* _IO_MC24_H */
