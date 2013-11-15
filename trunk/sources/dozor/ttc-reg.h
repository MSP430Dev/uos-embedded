/*
 * Описание регистров контроллера ППР.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 *        Дмитрий Подхватилин, НПП "Дозор" 2011, 2012
 *
 * Версия спецификации ППР: D2
 */

#ifndef _TTC_REG_H_
#define _TTC_REG_H_
/*
 * Адрес контроллера на внешней шине (при использовании параллельной шины)
 */
#define TTC_ADDR_BASE(n)    (0x10000000 + ((n) << 19))

/*
 * Адреса внутренней памяти.
 */
#define TTC_DATA_BASE       0x0000  /* Принимаемые и передаваемые пакеты */
#define TTC_MODE_BASE       0xe000  /* Таблица дескрипторов режимов */
#define TTC_DELAY_BASE      0xe100  /* Таблица задержек */
#define TTC_MEDL_BASE       0xe120  /* Таблица расписания MEDL */

#define TTC_REG_BASE        0xfe00  /* Управляющие регистры */

#ifdef TTC_MILANDR
#define TTC_DATA_SIZE       0xe000
#else
#define TTC_DATA_SIZE       0x4000
#endif

#define TTC_MEDL_SIZE       0x1c00

/* Адрес регистра во внутренней памяти контроллера */
#define TTC_REG_ADDR(a)     (TTC_REG_BASE + (a))

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
#define TTC_FRR     TTC_REG_ADDR (0x00)

/*
 * FDR – firmware date register, 16 R
 *
 * Формат в шестнадцатеричном виде ММДД, где:
 * ММ - месяц
 * ДД - день месяца
 */
#define TTC_FDR     TTC_REG_ADDR (0x04)

/*
 * SID – schedule identification register, 32 R/WO
 */
#define TTC_SID     TTC_REG_ADDR (0x08)

/*
 * NID – node identifier, 16 R/WO
 */
#define TTC_NID     TTC_REG_ADDR (0x0c)

/*
 * GCR – global configuration register, 16 R/W, default 0x0000
 */
#define TTC_GCR     TTC_REG_ADDR (0x10)

#define TTC_GRST    (1 << 0)    /* общий сброс */
#define TTC_GRUN    (1 << 1)    /* общий пуск */
#define TTC_IEN     (1 << 4)    /* разрешение прерывания */
#define TTC_N32     (1 << 5)    /* 32 узла в кластере */
#define TTC_N64     (1 << 6)    /* 64 узла в кластере */
#define TTC_IRQP    (1 << 8)    /* полярность IRQL: 1 - положительная */
#define TTC_MST     (1 << 10)   /* признак ведущего узла (важно только при
                                   старте) */
#define TTC_DBEN    (1 << 12)   /* включение режима двойной буферизации */
#define TTC_LC(n)   ((n) << 13) /* количество циклов подряд - 1, в которых
                                   AVEC=0, после которых в GSR выставляется
                                   признак TTC_GSR_CONL */

/*
 * GSR – global status register, 16 R
 * IER – interrupt enable register, 16 R/W
 */
#define TTC_GSR     TTC_REG_ADDR (0x14)
#define TTC_IER     TTC_REG_ADDR (0x18)

#define TTC_TDN     (1 << 0)    /* передан пакет с данными */
#define TTC_TSN     (1 << 1)    /* передан стартовый пакет */
#define TTC_TABT    (1 << 2)    /* передача оборвана на границе слота */
#define TTC_TXP     (1 << 3)    /* идет передача из текущего слота
                                   (этот признак не устанавливается в GSR,
                                   он виден только в слове статуса
                                   выдаваемого слота) */
#define TTC_WORK    (1 << 4)    /* режим аппаратного старта закончился */
#define TTC_CONL    (1 << 5)    /* в течение 4-х циклов рабочего режима
                                   не получено ни одного пакета от других
                                   устройств */
#define TTC_SE      (1 << 6)    /* признак того, что данный узел получал
                                   в предыдущем цикле хотя бы один
                                   корректный пакет*/
#define TTC_NM      (1 << 11)   /* признак произошедшей смены режима */
#define TTC_ISN     (1 << 12)   /* прошел слот с номером, указанном в
                                   регистре ISNR */
#define TTC_SF      (1 << 13)   /* закончился слот */
#define TTC_SINT    (1 << 14)   /* закончился слот с флагом INTR */
#define TTC_CCL     (1 << 15)   /* закончился цикл кластера */

/*
 * LEDR – LED control register, 16 W
 */
#define TTC_LEDR    TTC_REG_ADDR (0x1c)

/*
 * RSRi – receive status registers, 16 R/W
 * RIER – receive interrupt enable register, 16 R/W
 */
#define TTC_RSR(n)  TTC_REG_ADDR (0xA0 + ((n)<<5))
#define TTC_RIER    TTC_REG_ADDR (0x20)

#define TTC_RDN     (1 << 0)    /* успешно принят пакет с данными */
#define TTC_RSN     (1 << 1)    /* успешно принят стартовый пакет */
#define TTC_REV     (1 << 2)    /* ошибка кодирования приёмника */
#define TTC_RABT    (1 << 3)    /* приём оборван на границе слота */
#define TTC_RLE     (1 << 4)    /* неверная длина пакета */
#define TTC_RCSE    (1 << 5)    /* ошибка CRC приёмника */
#define TTC_RCME    (1 << 6)    /* неверный режим кластера */
#define TTC_RSPE    (1 << 7)    /* стартовый пакет вместо данных */
#define TTC_RXP     (1 << 8)    /* идет прием в слот (этот признак не
                                   устанавливается в RSR, он виден только в
                                   слове статуса принимаемого слота) */
#define TTC_RINV    (1 << 9)    /* принят пакет с признаком недостоверности
                                   данных, из-за того, что в передающем узле
                                   сработал механизм DARQ; пакет не записан
                                   в память */
#define TTC_RIGN    (1 << 10)   /* принят правильный пакет данных, но он был
                                   проигнорирован из-за того, что в данном
                                   узле для соответствующего слота
                                   процессором был установлен признак DARQ */
#define TTC_C0      (1 << 14)   /* Признак С0 */
#define TTC_C1      (1 << 15)   /* Признак С1 */

/*
 * CMR – current mode register, 16 R
 * NMR – next mode register, 16 R/W
 */
#define TTC_CMR     TTC_REG_ADDR (0x24)
#define TTC_NMR     TTC_REG_ADDR (0x28)

#define TTC_CMODE(x)    ((x) & 0xF)         /* текущий режим кластера */
#define TTC_NMODE(x)    (((x) & 0xF) << 4)  /* режим кластера на след. цикле */

/*
 * SSR – schedule start register, 16 R/W
 * SCP – schedule current pointer, 16 R
 */
#define TTC_SSR     TTC_REG_ADDR (0x2c)
#define TTC_SCP     TTC_REG_ADDR (0x30)

/*
 * ISNR – interrupt by slot number register, 16 R/W
 */
#define TTC_ISNR    TTC_REG_ADDR (0x34)

/*
 * TPLR – transmit preamble length register, 16 R/W
 */
#define TTC_TPLR    TTC_REG_ADDR (0x38)

/*
 * BDR – baudrate divisor register, 16 R/W
 */
#define TTC_BDR     TTC_REG_ADDR (0x3c)

#define TTC_BDR_NDIV(x) ((x) & 0xF)         /* Делитель */
#define TTC_BDR_TDC(x)  (((x) & 0x1F) << 4) /* Компенсация задержек */

/*
 * SPDR - start packet delay register, 32 R/W
 */
#define TTC_SPDR    TTC_REG_ADDR (0x9c)

/*
 * DAR - data access register, 32 R/W
 */
#define TTC_DAR     TTC_REG_ARDR (0x98)

#define TTC_DAR_ASLT(x) ((x) & 0xFFF)   /* Номер слота для синхронизации */
#define TTC_DAR_DARQ    (1 << 12)       /* Бит запроса/разрешения доступа к слоту */
#define TTC_DAR_SBRQ    (1 << 14)       /* Требование на переключение буфера */
#define TTC_DAR_CBN (1 << 15)           /* Номер текущего буфера */

/*
 * AKR - access key register, 32 R/W
 */
#define TTC_AKR     TTC_REG_ARDR (0x94)

#define TTC_ACCESS_KEY  0x2012A2B3

/*
 * DB1_BASE - double buffer 1 base address, 16 R/W
 */
#define TTC_DB1_BASE    TTC_REG_ARDR (0x90)


/*----------------------------------------------
 * Вектора
 */
/*
 * RVEC – receive vector, 64 R
 */
#define TTC_RVEC    TTC_REG_ADDR (0x60)

/*
 * AVEC – acknowledge vector, 64 R
 */
#define TTC_AVEC    TTC_REG_ADDR (0x68)


/*----------------------------------------------
 * Время
 */
/*
 * CTR – cycle time register, 32 R
 */
#define TTC_CTR     TTC_REG_ADDR (0x40)

/*
 * CNR – cycle number register, 32 R
 */
#define TTC_CNR     TTC_REG_ADDR (0x44)

/*
 * TDR – time delta register, 32 R
 */
#define TTC_TDR     TTC_REG_ADDR (0x48)

/*
 * MTR – max time register, 32 R/W
 */
#define TTC_MTR     TTC_REG_ADDR (0x4c)

/*
 * MINLi – slowest node time, 32 R
 * MINUi – next to slowest node time, 32 R
 * MAXLi – next to fastest node time, 32 R
 * MAXUi – fastest node time, 32 R
 */
#define TTC_MINL(n) TTC_REG_ADDR (0x70 + ((n)<<4))
#define TTC_MINU(n) TTC_REG_ADDR (0x74 + ((n)<<4))
#define TTC_MAXL(n) TTC_REG_ADDR (0x78 + ((n)<<4))
#define TTC_MAXU(n) TTC_REG_ADDR (0x7c + ((n)<<4))

/*
 * RPCi – receive packets counter, 32 R
 * TPC – transmit packets counter, 32 R
 * RSPCi – receive start packets counter, 32 R
 * EVCi – encoding violations counter, 32 R
 * CECi – checksum errors counter, 32 R
 * CMECi – cluster mode errors counter, 32 R
 * RADSi – bus i receive address for start packet, 16 R/W
 */
#define TTC_RPC(n)  TTC_REG_ADDR (0xa4 + ((n)<<5))
#define TTC_TPC     TTC_REG_ADDR (0x54)
#define TTC_RSPC(n) TTC_REG_ADDR (0xa8 + ((n)<<5))
#define TTC_EVC(n)  TTC_REG_ADDR (0xac + ((n)<<5))
#define TTC_CEC(n)  TTC_REG_ADDR (0xb0 + ((n)<<5))
#define TTC_CMEC(n) TTC_REG_ADDR (0xb4 + ((n)<<5))
#define TTC_RADS(n) TTC_REG_ADDR (0xb8 + ((n)<<5))

/*
 * CVECi – регистр маски для формирования сигнала Сi, 64 R/W
 */
#define TTC_CVEC(n) TTC_REG_ADDR (0x100 + ((n)<<3))


/*----------------------------------------------
 * Структура таблицы расписания узла (MEDL)
 */
struct _medl_t {
    uint32_t    ticks;      /* время окончания слота (в тактах шины) */
    uint16_t    node;       /* номер узла */
    uint16_t    bytes;      /* длина пакета в байтах */
    uint16_t    data_addr0; /* буфер данных передачи или приема шины 0 */
    uint16_t    data_addr1; /* буфер данных передачи или приема шины 1 */
    uint16_t    reserved;   /* зарезервировано */
    uint16_t    flags;
#define TTC_MEDL_START      0x0001  /* стартовый пакет */
#define TTC_MEDL_TSYN       0x0002  /* запомнить время приёма */
#define TTC_MEDL_CLKSYN     0x0004  /* синхронизация в конце слота */
#define TTC_MEDL_ETS        0x0008  /* установка внешнего сигнала индикации */
#define TTC_MEDL_INTR       0x0010  /* прерывание в начале слота */
} __attribute__((__packed__));
typedef struct _medl_t medl_t;

/*----------------------------------------------
 * Дескриптор режима
 */
struct _cluster_mode_t {
    uint32_t    nssr;       /* Указатель расписания */
    uint32_t    nmtr;       /* Длительность цикла */
    uint32_t    ntplr;      /* Длина преамбулы */
    uint32_t    nbdr;       /* Делитель частоты передачи */
};
typedef struct _cluster_mode_t cluster_mode_t;

struct _delay_table_item_t {
    unsigned node_low   :   4;
    unsigned node_hi    :   4;
} __attribute__((__packed__));
typedef struct _delay_table_item_t delay_table_item_t;

/* Конец описания регистров контроллера TTP.
 *----------------------------------------------*/

#endif /* _TTC_REG_H_ */
