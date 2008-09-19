/*
 * Hardware register defines for Elvees MC-24 microcontroller.
 */
#ifndef _IO_MC24_H
#define _IO_MC24_H

/*
 * Coprocessor 0 registers.
 */
#define C0_INDEX	$0	/* индекс доступа к TLB */
#define C0_RANDOM	$1	/* индекс TLB для команды Write Random */
#define C0_ENTRYLO0	$2	/* строки для чётных страниц TLB */
#define C0_ENTRYLO1	$3	/* строки для нечётных страниц TLB */
#define C0_CONTEXT	$4	/* указатель на таблицу PTE */
#define C0_PAGEMASK	$5	/* маска размера страниц TLB */
#define C0_WIRED	$6	/* граница привязанных строк TLB */
#define C0_BADVADDR	$8	/* виртуальный адрес последнего исключения */
#define C0_COUNT	$9	/* таймер */
#define C0_ENTRYHI	$10	/* информация соответствия виртуального адреса */
#define C0_COMPARE	$11	/* предельное значение для прерывания по таймеру */
#define C0_STATUS	$12	/* режимы функционирования процессора */
#define C0_CAUSE	$13	/* причина последнего исключения */
#define C0_EPC		$14	/* адрес возврата из исключения */
#define C0_PRID		$15	/* идентификатор процессора */
#define C0_CONFIG	$16	/* информация о возможностях процессора */
#define C0_LLADDR	$17	/* физический адрес последней команды LL */
#define C0_ERROREPC	$30	/* адрес возврата из исключения ошибки */

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

#endif /* _IO_MC24_H */
