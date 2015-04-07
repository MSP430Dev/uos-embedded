/*
 * Описание регистров контроллера ППР.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 *        Дмитрий Подхватилин, НПП "Дозор" 2011-2015
 *
 * Версия спецификации ППР: С1 (микросхема версии 1)
 */

#ifndef _SVP_REG_C1_H_
#define _SVP_REG_C1_H_

/*
 * Адреса внутренней памяти.
 */
#define SVP_DATA_BASE       0x0000  /* Принимаемые и передаваемые пакеты */
#define SVP_MODE_BASE       0xe000  /* Таблица дескрипторов режимов */
#define SVP_MEDL_BASE       0xe100  /* Таблица расписания MEDL  */

#define SVP_REG_BASE        0xfe00  /* Управляющие регистры */

#ifdef SVP_MILANDR
#define SVP_DATA_SIZE       0xe000
#else
#define SVP_DATA_SIZE       0x4000
#endif

#define SVP_MEDL_SIZE       0x1c00

/* Адрес регистра во внутренней памяти контроллера */
#define SVP_REG_ADDR(a)     (SVP_REG_BASE + (a))

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
#define SVP_FRR     SVP_REG_ADDR (0x00)

/*
 * FDR – firmware date register, 16 R
 *
 * Формат в шестнадцатеричном виде ММДД, где:
 * ММ - месяц
 * ДД - день месяца
 */
#define SVP_FDR     SVP_REG_ADDR (0x04)

/*
 * SID – schedule identification register, 32 R/WO
 */
#define SVP_SID     SVP_REG_ADDR (0x08)

/*
 * NID – node identifier, 16 R/WO
 */
#define SVP_NID     SVP_REG_ADDR (0x0c)

/*
 * GCR – global configuration register, 16 R/W, default 0x0000
 */
#define SVP_GCR     SVP_REG_ADDR (0x10)

#define SVP_GRST        (1 << 0)    /* общий сброс */
#define SVP_GRUN        (1 << 1)    /* общий пуск */
#define SVP_IEN         (1 << 4)    /* разрешение прерывания */
#define SVP_N32         (1 << 5)    /* 32 узла в кластере */
#define SVP_N64         (1 << 6)    /* 64 узла в кластере */
#define SVP_IRQP        (1 << 8)    /* полярность IRQL: 1 - положительная */
#define SVP_HW_START    (1 << 9)    /* аппаратный старт */
#define SVP_MST         (1 << 10)   /* признак ведущего узла (важно только при
                                   старте) */
#define SVP_HW_CM       (1 << 11)   /* включение аппаратного переключения режима */
#define SVP_LC(n)       ((n) << 13) /* количество циклов подряд - 1, в которых
                                   AVEC=0, после которых в GSR выставляется
                                   признак SVP_GSR_CONL */

/*
 * GSR – global status register, 16 R
 * IER – interrupt enable register, 16 R/W
 */
#define SVP_GSR     SVP_REG_ADDR (0x14)
#define SVP_IER     SVP_REG_ADDR (0x18)

#define SVP_TDN     (1 << 0)    /* передан пакет с данными */
#define SVP_TSN     (1 << 1)    /* передан стартовый пакет */
#define SVP_TABT    (1 << 2)    /* передача оборвана на границе слота */
#define SVP_TXP     (1 << 3)    /* идет передача из текущего слота
                                   (этот признак не устанавливается в GSR,
                                   он виден только в слове статуса
                                   выдаваемого слота) */
#define SVP_WORK    (1 << 4)    /* режим аппаратного старта закончился */
#define SVP_CONL    (1 << 5)    /* в течение 4-х циклов рабочего режима
                                   не получено ни одного пакета от других
                                   устройств */
#define SVP_SE      (1 << 6)    /* признак того, что данный узел получал
                                   в предыдущем цикле хотя бы один
                                   корректный пакет*/
#define SVP_NM      (1 << 11)   /* признак произошедшей смены режима */
#define SVP_ISN     (1 << 12)   /* прошел слот с номером, указанном в
                                   регистре ISNR */
#define SVP_SF      (1 << 13)   /* закончился слот */
#define SVP_SINT    (1 << 14)   /* закончился слот с флагом INTR */
#define SVP_CCL     (1 << 15)   /* закончился цикл кластера */

/*
 * LEDR – LED control register, 16 W
 */
#define SVP_LEDR    SVP_REG_ADDR (0x1c)

/*
 * RSRi – receive status registers, 16 R/W
 * RIER – receive interrupt enable register, 16 R/W
 */
#define SVP_RSR(n)  SVP_REG_ADDR (0xA0 + ((n)<<5))
#define SVP_RIER    SVP_REG_ADDR (0x20)

#define SVP_RDN     (1 << 0)    /* успешно принят пакет с данными */
#define SVP_RSN     (1 << 1)    /* успешно принят стартовый пакет */
#define SVP_REV     (1 << 2)    /* ошибка кодирования приёмника */
#define SVP_RABT    (1 << 3)    /* приём оборван на границе слота */
#define SVP_RLE     (1 << 4)    /* неверная длина пакета */
#define SVP_RCSE    (1 << 5)    /* ошибка CRC приёмника */
#define SVP_RCME    (1 << 6)    /* неверный режим кластера */
#define SVP_RSPE    (1 << 7)    /* стартовый пакет вместо данных */
#define SVP_RXP     (1 << 8)    /* идет прием в слот (этот признак не
                                   устанавливается в RSR, он виден только в
                                   слове статуса принимаемого слота) */
#define SVP_C0      (1 << 14)   /* Признак С0 */
#define SVP_C1      (1 << 15)   /* Признак С1 */

/*
 * CMR – current mode register, 16 R
 * NMR – next mode register, 16 R/W
 */
#define SVP_CMR     SVP_REG_ADDR (0x24)
#define SVP_NMR     SVP_REG_ADDR (0x28)

#define SVP_MR_CMODE_MASK   0x00ff      /* режим кластера */
#define SVP_MR_RXEN0        (1 << 8)    /* пуск приёмника шины 0 */
#define SVP_MR_RXEN1        (1 << 9)    /* пуск приёмника шины 1 */
#define SVP_MR_RXEN2        (1 << 10)   /* пуск приёмника шины 2 */
#define SVP_MR_TXEN0        (1 << 11)   /* пуск передатчика шины 0 */
#define SVP_MR_TXEN1        (1 << 12)   /* пуск передатчика шины 1 */
#define SVP_MR_TXEN2        (1 << 13)   /* пуск передатчика шины 2 */
#define SVP_MR_STRT         (1 << 14)   /* режим старта */

/*
 * SSR – schedule start register, 16 R/W
 * SCP – schedule current pointer, 16 R
 */
#define SVP_SSR     SVP_REG_ADDR (0x2c)
#define SVP_NSSR    SVP_REG_ADDR (0x90)
#define SVP_SCP     SVP_REG_ADDR (0x30)

/*
 * ISNR – interrupt by slot number register, 16 R/W
 */
#define SVP_ISNR    SVP_REG_ADDR (0x34)

/*
 * TPLR – transmit preamble length register, 16 R/W
 */
#define SVP_TPLR    SVP_REG_ADDR (0x38)
#define SVP_NTPLR   SVP_REG_ADDR (0x58)

/*
 * BDR – baudrate divisor register, 16 R/W
 */
#define SVP_BDR     SVP_REG_ADDR (0x3c)
#define SVP_NBDR    SVP_REG_ADDR (0x5c)

/*
 * AKR - access key register, 32 R/W
 */
#define SVP_AKR     SVP_REG_ARDR (0x94)

#define SVP_ACCESS_KEY  0x2012A2B3

/*----------------------------------------------
 * Вектора
 */
/*
 * RVEC – receive vector, 64 R
 */
#define SVP_RVEC    SVP_REG_ADDR (0x60)

/*
 * AVEC – acknowledge vector, 64 R
 */
#define SVP_AVEC    SVP_REG_ADDR (0x68)


/*----------------------------------------------
 * Время
 */
/*
 * CTR – cycle time register, 32 R
 */
#define SVP_CTR     SVP_REG_ADDR (0x40)

/*
 * CNR – cycle number register, 32 R
 */
#define SVP_CNR     SVP_REG_ADDR (0x44)

/*
 * TDR – time delta register, 32 R
 */
#define SVP_TDR     SVP_REG_ADDR (0x48)

/*
 * MTR – max time register, 32 R/W
 */
#define SVP_MTR     SVP_REG_ADDR (0x4c)
#define SVP_NMTR    SVP_REG_ADDR (0x50)

/*
 * MINLi – slowest node time, 32 R
 * MINUi – next to slowest node time, 32 R
 * MAXLi – next to fastest node time, 32 R
 * MAXUi – fastest node time, 32 R
 */
#define SVP_MINL(n) SVP_REG_ADDR (0x70 + ((n)<<4))
#define SVP_MINU(n) SVP_REG_ADDR (0x74 + ((n)<<4))
#define SVP_MAXL(n) SVP_REG_ADDR (0x78 + ((n)<<4))
#define SVP_MAXU(n) SVP_REG_ADDR (0x7c + ((n)<<4))

/*
 * RPCi – receive packets counter, 32 R
 * TPC – transmit packets counter, 32 R
 * RSPCi – receive start packets counter, 32 R
 * EVCi – encoding violations counter, 32 R
 * CECi – checksum errors counter, 32 R
 * CMECi – cluster mode errors counter, 32 R
 * RADSi – bus i receive address for start packet, 16 R/W
 */
#define SVP_RPC(n)  SVP_REG_ADDR (0xa4 + ((n)<<5))
#define SVP_TPC     SVP_REG_ADDR (0x54)
#define SVP_RSPC(n) SVP_REG_ADDR (0xa8 + ((n)<<5))
#define SVP_EVC(n)  SVP_REG_ADDR (0xac + ((n)<<5))
#define SVP_CEC(n)  SVP_REG_ADDR (0xb0 + ((n)<<5))
#define SVP_CMEC(n) SVP_REG_ADDR (0xb4 + ((n)<<5))
#define SVP_RADS(n) SVP_REG_ADDR (0xb8 + ((n)<<5))

/*
 * CVECi – регистр маски для формирования сигнала Сi, 64 R/W
 */
#define SVP_CVEC(n) SVP_REG_ADDR (0x100 + ((n)<<3))

/*
 * CATR – control analogue transceiver, 16 R/W
 */
#define SVP_CATR    SVP_REG_ADDR (0x140)

#define SVP_STBY1   (1 << 14)
#define SVP_STBY2   (1 << 15)

/*
 * STATR - status analogue transceiver, 16 R/W
 */
#define SVP_STATR   SVP_REG_ADDR (0x144)
 
#define SVP_TEMP1   (1 << 0)
#define SVP_T1TRIG  (1 << 1)
#define SVP_TEMP2   (1 << 2)
#define SVP_T2TRIG  (1 << 3)
#define SVP_CUR1    (1 << 4)
#define SVP_C1TRIG  (1 << 5)
#define SVP_CUR2    (1 << 6)
#define SVP_C2TRIG  (1 << 7)

/*
 * TSTATR – test analogue transceiver, 16 R/W
 */
#define SVP_TSTATR  SVP_REG_ADDR (0x148)

#define SVP_ATREXT  (1 << 0)
#define SVP_TRD1    (1 << 8)
#define SVP_TEN     (1 << 9)
#define SVP_TTD1    (1 << 10)
#define SVP_TST1    (1 << 11)
#define SVP_TRD2    (1 << 12)
#define SVP_TTD2    (1 << 14)
#define SVP_TST2    (1 << 15)



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
#define SVP_MEDL_START      0x0001  /* стартовый пакет */
#define SVP_MEDL_TSYN       0x0002  /* запомнить время приёма */
#define SVP_MEDL_CLKSYN     0x0004  /* синхронизация в конце слота */
#define SVP_MEDL_ETS        0x0008  /* установка внешнего сигнала индикации */
#define SVP_MEDL_INTR       0x0010  /* прерывание в начале слота */
#define SVP_MEDL_LAST_SLOT  0x0020  /* признак последнего слота  */
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

#endif /* _SVP_REG_C1_H_ */
