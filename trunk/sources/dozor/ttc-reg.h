/*
 * Описание регистров контроллера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 *        Дмитрий Подхватилин, НПП "Дозор" 2011
 */

#ifndef _TTC_REG_H_
#define _TTC_REG_H_
/*
 * Адрес контроллера на внешней шине (при использовании параллельной шины)
 */
#define TTC_ADDR_BASE(n)	(0x10000000 + ((n) << 19))
//#define TTC_ADDR_BASE(n)	0x10000000

/*
 * Адреса внутренней памяти.
 */
#define TTC_DATA_BASE		0x0000	/* Принимаемые и передаваемые пакеты */
#define TTC_MEDL_BASE		0xe000	/* Таблица расписания MEDL */
#define TTC_REG_BASE		0xfe00  /* Управляющие регистры */

#ifdef TTC_MILANDR
#define TTC_DATA_SIZE		0xe000
#else
#define TTC_DATA_SIZE		0x4000 
#endif

#define TTC_MEDL_SIZE		0x1c00

/* Адрес регистра во внутренней памяти контроллера */
#define TTC_REG_ADDR(a)		(TTC_REG_BASE + (a))

/*----------------------------------------------
 * Общесистемные регистры
 */
/*
 * FRR – firmware revision register, 16 R
 *
 * Формат в шестнадцатеричном виде РНГГ, где:
 * Р - буква ревизии прошивки (0=A, 1=B и т.д.)
 * Н - цифра ревизии прошивки
 * ГГ - последние две цифры номера года
 */
#define TTC_FRR			TTC_REG_ADDR (0x00)

/*
 * FDR – firmware date register, 16 R
 *
 * Формат в шестнадцатеричном виде ММДД, где:
 * ММ - месяц
 * ДД - день месяца
 */
#define TTC_FDR			TTC_REG_ADDR (0x04)

/*
 * SID – schedule identification register, 32 R/WO
 */
#define TTC_SID			TTC_REG_ADDR (0x08)

/*
 * NID – node identifier, 16 R/WO
 */
#define TTC_NID			TTC_REG_ADDR (0x0c)

/*
 * GCR – global configuration register, 16 R/W, default 0x0000
 */
#define TTC_GCR			TTC_REG_ADDR (0x10)

#define TTC_GCR_GRST		0x0001		/* общий сброс */
#define TTC_GCR_GRUN		0x0002		/* общий пуск */

#define TTC_GCR_ISEL_MASK	0x000с		/* выбор интерфейса */
#define TTC_GCR_ISEL_MANCHESTER	0x0000		/* RS-485 кодирование Manchester */
#define TTC_GCR_ISEL_SNI	0x0004		/* 10Base2 */
#define TTC_GCR_ISEL_MII	0x0008		/* 100Base-TX или 100Base-SX*/
#define TTC_GCR_ISEL_CAN	0x000с		/* шина CAN */

#define TTC_GCR_IEN		0x0010		/* разрешение прерывания */
#define TTC_GCR_N32		0x0020		/* 32 узла в кластере */
#define TTC_GCR_N64		0x0040		/* 64 узла в кластере */
#define TTC_GCR_IRQ_POSITIVE	0x0100		/* полярность IRQL: 1 - положительная */
#define TTC_GCR_HW_START	0x0200		/* аппаратный старт */
#define TTC_GSR_HW_CM		0x0800		/* включение аппаратного переключения режима */
#define TTC_GCR_MASTER		0x0400		/* признак ведущего узла (важно только при старте) */
#define TTC_GCR_LC(n)		((n) << 13)	/* количество циклов подряд - 1, в которых AVEC=0, после которых
						   в GSR выставляется признак TTC_GSR_HW_CON_LOST */

/*
 * GSR – global status register, 16 R
 * IER – interrupt enable register, 16 R/W
 */
#define TTC_GSR			TTC_REG_ADDR (0x14)
#define TTC_IER			TTC_REG_ADDR (0x18)

#define TTC_GSR_TDN		0x0001		/* передан пакет с данными */
#define TTC_GSR_TSN		0x0002		/* передан стартовый пакет */
#define TTC_GSR_TABT		0x0004		/* передача оборвана на границе слота */
#define TTC_GSR_TXP		0x0008		/* идет передача из текущего слота (этот признак не 
						   устанавливается в GSR, он виден только в слове статуса
						   выдаваемого слота) */
#define TTC_GSR_HW_ST_FIN	0x0010		/* режим аппаратного старта закончился */
#define TTC_GSR_HW_CON_LOST	0x0020		/* в течение 4-х циклов рабочего режима не получено ни одного пакета от других устройств */
#define TTC_GSR_SMB_ELSE	0x0040		/* признак того, что данный узел получал в предыдущем цикле хотя бы один корректный пакет*/
#define TTC_GSR_NM		0x0800		/* признак произошедшей смены режима */
#define TTC_GSR_ISNR		0x1000		/* прошел слот с номером, указанном в регистре ISNR */
#define TTC_GSR_SF		0x2000		/* закончился слот */
#define TTC_GSR_SINT		0x4000		/* закончился слот с флагом INTR */
#define TTC_GSR_CCL		0x8000		/* закончился цикл кластера */

#define TTC_GSR_BITS		"\20"\
				"\01tdn\02tabt\14c0\15c1\16sf\17sint\20ccl"
/*
 * LEDR – LED control register, 16 W
 */
#define TTC_LEDR		TTC_REG_ADDR (0x1c)

/*
 * RSRi – receive status registers, 16 R/W
 * RIER – receive interrupt enable register, 16 R/W
 */
#define TTC_RSR(n)		TTC_REG_ADDR (0xA0 + ((n)<<5))
#define TTC_RIER		TTC_REG_ADDR (0x20)

#define TTC_RSR_RDN		0x0001		/* успешно принят пакет */
#define TTC_RSR_STRT		0x0002		/* принят стартовый пакет */
#define TTC_RSR_REV		0x0004		/* ошибка кодирования приёмника */
#define TTC_RSR_RABT		0x0008		/* приём оборван на границе слота */
#define TTC_RSR_RLE		0x0010		/* неверная длина пакета */
#define TTC_RSR_CSE		0x0020		/* ошибка CRC приёмника */
#define TTC_RSR_RCME		0x0040		/* неверный режим кластера */
#define TTC_RSR_RSPE		0x0080		/* стартовый пакет вместо данных */
#define TTC_RSR_RXP		0x0100		/* идет прием в слот (этот признак не 
						   устанавливается в RSR, он виден только в слове статуса
						   принимаемого слота)*/
#define TTC_RSR_C0		0x4000		/* Признак С0 */
#define TTC_RSR_C1		0x8000		/* Признак С1 */

#define TTC_RSR_BITS		"\20"\
				"\01rdn\02strt\03rev\04rabt\05rle\06cse\07rcme\10rspe"
/*
 * CMR – current mode register, 16 R
 * NMR – next mode register, 16 R/W
 */
#define TTC_CMR			TTC_REG_ADDR (0x24)
#define TTC_NMR			TTC_REG_ADDR (0x28)

#define TTC_MR_CMODE_MASK	0x00ff		/* режим кластера */
#define TTC_MR_RXEN0		0x0100		/* пуск приёмника шины 0 */
#define TTC_MR_RXEN1		0x0200		/* пуск приёмника шины 1 */
#define TTC_MR_RXEN2		0x0400		/* пуск приёмника шины 2 */
#define TTC_MR_TXEN0		0x0800		/* пуск передатчика шины 0 */
#define TTC_MR_TXEN1		0x1000		/* пуск передатчика шины 1 */
#define TTC_MR_TXEN2		0x2000		/* пуск передатчика шины 2 */
#define TTC_MR_STRT		0x4000		/* режим старта */

#define TTC_MR_BITS		"\20"\
				"\11rxen0\12rxen1\13rxen2\14txen0\15txen1\16txen2\17strt"
/*
 * SSR – schedule start register, 16 R/W
 * SCP – schedule current pointer, 16 R
 */
#define TTC_SSR			TTC_REG_ADDR (0x2c)
#define TTC_NSSR			TTC_REG_ADDR (0x90)
#define TTC_SCP			TTC_REG_ADDR (0x30)

/*
 * ISNR – interrupt by slot number register, 16 R/W
 */
#define TTC_ISNR		TTC_REG_ADDR (0x34)

/*
 * TPLR – transmit preamble length register, 16 R/W
 */
#define TTC_TPLR		TTC_REG_ADDR (0x38)
#define TTC_NTPLR		TTC_REG_ADDR (0x58)


/*
 * BDR – baudrate divisor register, 16 R/W
 */
#define TTC_BDR			TTC_REG_ADDR (0x3c)
#define TTC_NBDR		TTC_REG_ADDR (0x5c)


/*----------------------------------------------
 * Вектора
 */
/*
 * RVEC – receive vector, 64 R
 */
#define TTC_RVEC		TTC_REG_ADDR (0x60)

/*
 * AVEC – acknowledge vector, 64 R
 */
#define TTC_AVEC		TTC_REG_ADDR (0x68)


/*----------------------------------------------
 * Время
 */
/*
 * CTR – cycle time register, 32 R
 */
#define TTC_CTR			TTC_REG_ADDR (0x40)

/*
 * CNR – cycle number register, 32 R
 */
#define TTC_CNR			TTC_REG_ADDR (0x44)

/*
 * TDR – time delta register, 32 R
 */
#define TTC_TDR			TTC_REG_ADDR (0x48)

/*
 * MTR – max time register, 32 R/W
 * NMTR – next max time register, 32 R/W
 */
#define TTC_MTR			TTC_REG_ADDR (0x4c)
#define TTC_NMTR		TTC_REG_ADDR (0x50)


/*
 * AKR - access key register, 32 R/W
 */
#define TTC_AKR			TTC_REG_ADDR (0x94)
#define TTC_ACCESS_KEY		0x2012A2B3
 
/*
 * MINLi – slowest node time, 32 R
 * MINUi – next to slowest node time, 32 R
 * MAXLi – next to fastest node time, 32 R
 * MAXUi – fastest node time, 32 R
 */
#define TTC_MINL(n)		TTC_REG_ADDR (0x70 + ((n)<<4))
#define TTC_MINU(n)		TTC_REG_ADDR (0x74 + ((n)<<4))
#define TTC_MAXL(n)		TTC_REG_ADDR (0x78 + ((n)<<4))
#define TTC_MAXU(n)		TTC_REG_ADDR (0x7c + ((n)<<4))

/*
 * RPCi – receive packets counter, 32 R
 * TPC – transmit packets counter, 32 R
 * RSPCi – receive start packets counter, 32 R
 * EVCi – encoding violations counter, 32 R
 * CECi – checksum errors counter, 32 R
 * CMECi – cluster mode errors counter, 32 R
 * RADSi – bus i receive address for start packet, 16 R/W
 */
#define TTC_RPC(n)		TTC_REG_ADDR (0xa4 + ((n)<<5))
#define TTC_TPC			TTC_REG_ADDR (0x54)
#define TTC_RSPC(n)		TTC_REG_ADDR (0xa8 + ((n)<<5))
#define TTC_EVC(n)		TTC_REG_ADDR (0xac + ((n)<<5))
#define TTC_CEC(n)		TTC_REG_ADDR (0xb0 + ((n)<<5))
#define TTC_CMEC(n)		TTC_REG_ADDR (0xb4 + ((n)<<5))
#define TTC_RADS(n)		TTC_REG_ADDR (0xb8 + ((n)<<5))

/*
 * CVECi – регистр маски для формирования сигнала Сi, 64 R/W
 */
#define TTC_CVEC(n)		TTC_REG_ADDR (0x100 + ((n)<<3))

/*----------------------------------------------
 * Структура таблицы расписания узла (MEDL)
 */
struct _medl_t {
	uint32_t	ticks;			/* время окончания слота (в тактах шины) */
	uint16_t	node;			/* номер узла */
	uint16_t	bytes;			/* длина пакета в байтах */
	uint16_t	data_addr0;		/* буфер данных передачи или приема шины 0 */
	uint16_t	data_addr1;		/* буфер данных передачи или приема шины 1 */
	uint16_t	reserved;		/* зарезервировано */
	uint16_t 	flags;
#define TTC_MEDL_START		0x0001		/* стартовый пакет */
#define TTC_MEDL_TSYN		0x0002		/* запомнить время приёма */
#define TTC_MEDL_CLKSYN		0x0004		/* синхронизация в конце слота */
#define TTC_MEDL_ETS		0x0008		/* установка внешнего сигнала индикации */
#define TTC_MEDL_INTR		0x0010		/* прерывание в начале слота */
#define TTC_MEDL_LAST_SLOT	0x0020		/* признак последнего слота */
} __attribute__((__packed__));
typedef struct _medl_t medl_t;

/*----------------------------------------------
 * Дескриптор режима
 */
struct _cluster_mode_t {
	uint32_t	nssr;			/* Указатель расписания */
	uint32_t	nmtr;			/* Длительность цикла */
	uint32_t	ntplr;			/* Длина преамбулы */
	uint32_t	nbdr;			/* Делитель частоты передачи */
};
typedef struct _cluster_mode_t cluster_mode_t;

/* Конец описания регистров контроллера TTP.
 *----------------------------------------------*/

#endif /* _TTC_REG_H_ */
