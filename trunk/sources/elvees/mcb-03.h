/*
 * Карта памяти микросхемы MCB-xx.
 */

/*
 * Константа MCB_BASE, означающая базовый адрес микросхемы в адресном
 * пространстве процессора, должна быть объявлена перед включением этого файла,
 * т.е., например,
 *
 * #define MCB_BASE     0xae000000
 * #include <elvees/mcb.h>
 */

#define MCB_PERIF(base, reg)    ((base) | (reg))
#define MCB_DPRAM_BASE(n)       (0x01000000 + ((n) << 17))
#define MCB_HMDPRAM_REG(r)      MCB_PERIF (0x01100000, r)
#define MCB_PMSC_REG(r)         MCB_PERIF (0x01200000, r)
#define MCB_EMAC_REG(r)         MCB_PERIF (0x01300000, r)
#define MCB_EMAC_DMA_REG(r)     MCB_PERIF (0x01380000, r)
#define MCB_SWIC_REG(n,r)       MCB_PERIF (0x01400000 + ((n)<<21), r)
#define MCB_SWIC_DMA_REG(n,r)   MCB_PERIF (0x01500000 + ((n)<<21), r)

#define MCB_MBA_BASE            (MCB_BASE | 0x01c00000)
#define MCB_MBA_REG(r)          (*(volatile unsigned*) (MCB_MBA_BASE | (r)))

/*
 * Регистры MBA
 */
#define MCB_MBA_QSTR            MCB_MBA_REG (0x00)      /* Регистр заявок 0 */
#define MCB_MBA_MASK            MCB_MBA_REG (0x04)      /* Регистр маски 0 */
#define MCB_MBA_BDR             MCB_MBA_REG (0x08)      /* Регистр буферных данных */
#define MCB_MBA_BUSY            MCB_MBA_REG (0x0c)      /* Регистр признака занятости */
#define MCB_MBA_QSTR1           MCB_MBA_REG (0x10)      /* Регистр заявок 1 */
#define MCB_MBA_MASK1           MCB_MBA_REG (0x14)      /* Регистр маски 1 */

/*
 * Чтение/запись памяти и регистров MBA осуществляется с помощью обычного обращения
 * по адресу, чтение/запись регистров SWIC, PMSC и адресного окна PCI должна
 * осущестляться только с помощью функций MCB_EMAC_REGead_reg/mcb_write_reg.
 * Для разъяснений см. Руководство пользователя на микросхему 1892ХД1Я, п. 5.3.
 */
unsigned mcb_read_reg (unsigned addr);
void mcb_write_reg (unsigned addr, unsigned value);

/*
 * Регистры контроля по коду Хэмминга памяти DPRAM
 */
#define MCB_HMDPRAM_CSR(n)      MCB_HMDPRAM_REG(0x0000 | ((n) << 12))
#define MCB_HMDPRAM_AERROR(n)   MCB_HMDPRAM_REG(0x0004 | ((n) << 12))

/*
 * Регистр отключения и включения частоты CLK_EN
 */
#define MCB_CLK_EN              MCB_PERIF (0x01180000, 0x4)
#define  MCB_CLKEN_SWITCH       0x00000001
#define  MCB_CLKEN_MBA          0x00000002
#define  MCB_CLKEN_PMSC         0x00010000
#define  MCB_CLKEN_EMAC         0x00100000
#define  MCB_CLKEN_SWIC(n)      ((1 << 24) << (n))

/*
 * Регистры контроллера PCI
 */
#define MCB_PCI_DEVICE_VENDOR_ID        MCB_PMSC_REG (0x00)     /* Идентификация устройства */
#define MCB_PCI_STATUS_COMMAND          MCB_PMSC_REG (0x04)     /* Состояние и управление */
#define MCB_PCI_CLASS_REVISION          MCB_PMSC_REG (0x08)     /* Регистр кода */
#define MCB_PCI_LATENCY_TIMER           MCB_PMSC_REG (0x0c)     /* Таймер времени передачи (MLT) */
#define MCB_PCI_BAR                     MCB_PMSC_REG (0x10)     /* Базовый адрес 0 */
#define MCB_PCI_SUBSYSTEM_VENDOR_ID     MCB_PMSC_REG (0x2c)     /* Идентификация подсистемы */
#define MCB_PCI_INTERRUPT_LINE          MCB_PMSC_REG (0x3c)     /* Код прерывания */
#define MCB_PCI_SEM                     MCB_PMSC_REG (0x44)     /* Семафор */
#define MCB_PCI_MBR                     MCB_PMSC_REG (0x48)     /* Почтовый ящик */
#define MCB_PCI_CSR_PCI                 MCB_PMSC_REG (0x4C)     /* Управление шиной PCI */
#define MCB_PCI_CSR_MASTER              MCB_PMSC_REG (0x50)     /* Состояние и управление обменом в режиме Master */
#define MCB_PCI_IR_MASTER               MCB_PMSC_REG (0x54)     /* Индексный регистр адреса памяти при обмене в режиме Master */
#define MCB_PCI_AR_PCI                  MCB_PMSC_REG (0x58)     /* Адресный регистр PCI */
#define MCB_PCI_QSTR_PCI                MCB_PMSC_REG (0x5C)     /* Системные прерывания */
#define MCB_PCI_MASKR_PCI               MCB_PMSC_REG (0x60)     /* Маскирование прерываний */
#define MCB_PCI_QSTR1_PCI               MCB_PMSC_REG (0x7C)     /* Системные прерывания 1 */
#define MCB_PCI_MASKR1_PCI              MCB_PMSC_REG (0x80)     /* Маскирование прерываний 1 */

/*
 * Макрос, возвращающий номер регистра PCI по его адресу (для конфигурационных транзакций PCI)
 */
#define MCB_PCI_REG_NUM(reg_addr)       (reg_addr >> 2)

/*
 * Регистры Ethernet MAC
 */
#define MCB_MAC_CONTROL                 MCB_EMAC_REG (0x00)     /* Управление MAC */
#define MCB_MAC_ADDR_L                  MCB_EMAC_REG (0x04)     /* Младшая часть исходного адреса MAC */
#define MCB_MAC_ADDR_H                  MCB_EMAC_REG (0x08)     /* Старшая часть исходного адреса MAC */
#define MCB_MAC_DADDR_L                 MCB_EMAC_REG (0x0C)     /* Младшая часть адреса назначения */
#define MCB_MAC_DADDR_H                 MCB_EMAC_REG (0x10)     /* Старшая часть адреса назначения */
#define MCB_MAC_FCS_CLIENT              MCB_EMAC_REG (0x14)     /* Контрольная сумма кадра */
#define MCB_MAC_TYPE                    MCB_EMAC_REG (0x18)     /* Тип кадра */
#define MCB_MAC_IFS_COLL_MODE           MCB_EMAC_REG (0x1C)     /* IFS и режим обработки коллизии */
#define MCB_MAC_TX_FRAME_CONTROL        MCB_EMAC_REG (0x20)     /* Управление передачей кадра */
#define MCB_MAC_STATUS_TX               MCB_EMAC_REG (0x24)     /* Статус передачи кадра */
#define MCB_MAC_UCADDR_L                MCB_EMAC_REG (0x28)     /* Младшая часть уникального адреса MAC */
#define MCB_MAC_UCADDR_H                MCB_EMAC_REG (0x2C)     /* Старшая часть уникального адреса MAC */
#define MCB_MAC_MCADDR_L                MCB_EMAC_REG (0x30)     /* Младшая часть группового адреса */
#define MCB_MAC_MCADDR_H                MCB_EMAC_REG (0x34)     /* Старшая часть группового адреса */
#define MCB_MAC_MCADDR_MASK_L           MCB_EMAC_REG (0x38)     /* Младшая часть маски группового адреса */
#define MCB_MAC_MCADDR_MASK_H           MCB_EMAC_REG (0x3C)     /* Старшая часть маски группового адреса */
#define MCB_MAC_HASHT_L                 MCB_EMAC_REG (0x40)     /* Младшая часть хэш-таблицы */
#define MCB_MAC_HASHT_H                 MCB_EMAC_REG (0x44)     /* Старшая часть хэш-таблицы */
#define MCB_MAC_RX_FRAME_CONTROL        MCB_EMAC_REG (0x48)     /* Управление приемом кадра */
#define MCB_MAC_RX_FR_MAXSIZE           MCB_EMAC_REG (0x4C)     /* Максимальный размер принимаемого кадра */
#define MCB_MAC_STATUS_RX               MCB_EMAC_REG (0x50)     /* Статус приема кадра */
#define MCB_MAC_RX_FRAME_STATUS_FIFO    MCB_EMAC_REG (0x54)     /* FIFO статусов принятых кадров */
#define MCB_MAC_MD_CONTROL              MCB_EMAC_REG (0x58)     /* Управление порта MD */
#define MCB_MAC_MD_STATUS               MCB_EMAC_REG (0x5C)     /* Статус порта MD */
#define MCB_MAC_MD_MODE                 MCB_EMAC_REG (0x60)     /* Режим работы порта MD */
#define MCB_MAC_TX_TEST_CSR             MCB_EMAC_REG (0x64)     /* Управление и состояние режима тестирования TX_FIFO */
#define MCB_MAC_TX_FIFO                 MCB_EMAC_REG (0x68)     /* Передающее TX_FIFO */
#define MCB_MAC_RX_TEST_CSR             MCB_EMAC_REG (0x6C)     /* Управление и состояние режима тестирования RX_FIFO */
#define MCB_MAC_RX_FIFO                 MCB_EMAC_REG (0x70)     /* Принимающее RX_FIFO */

/*
 * Регистры DMA EMAC(0-1)
 */
#define MCB_CSR_EMAC(n)         MCB_EMAC_DMA_REG (0x00+(n<<6))  /* Управление и состояние */
#define MCB_CP_EMAC(n)          MCB_EMAC_DMA_REG (0x04+(n<<6))  /* Указатель цепочки */
#define MCB_IR_EMAC(n)          MCB_EMAC_DMA_REG (0x08+(n<<6))  /* Индекс */
#define MCB_RUN_EMAC(n)         MCB_EMAC_DMA_REG (0x0C+(n<<6))  /* Управление состоянием бита RUN */

/*
 * Регистры Spacewire
 */
#define MCB_SWIC_HW_VER(n)                MCB_SWIC_REG (n,0x00)   /* Регистр аппаратной версии */
#define MCB_SWIC_STATUS(n)                MCB_SWIC_REG (n,0x04)   /* Статус */
#define MCB_SWIC_RX_CODE(n)               MCB_SWIC_REG (n,0x08)   /* Принимаемый маркер времени */
#define MCB_SWIC_MODE_CR(n)               MCB_SWIC_REG (n,0x0c)   /* Управление */
#define MCB_SWIC_TX_SPEED(n)              MCB_SWIC_REG (n,0x10)   /* Скорость передачи */
#define MCB_SWIC_TX_CODE(n)               MCB_SWIC_REG (n,0x14)   /* Передаваемый маркер времени */
#define MCB_SWIC_RX_SPEED(n)              MCB_SWIC_REG (n,0x18)   /* Скорость приема */
#define MCB_SWIC_CNT_RX_PACK(n)           MCB_SWIC_REG (n,0x1c)   /* Счетчик принятых пакетов */
#define MCB_SWIC_CNT_RX0_PACK(n)          MCB_SWIC_REG (n,0x20)   /* Счетчик принятых пустых пакетов */
#define MCB_SWIC_ISR_L(n)                 MCB_SWIC_REG (n,0x24)   /* Статус прерываний (младшая часть) */
#define MCB_SWIC_ISR_H(n)                 MCB_SWIC_REG (n,0x28)   /* Статус прерываний (старшая часть) */
#define MCB_SWIC_TRUE_TIME(n)             MCB_SWIC_REG (n,0x2c)   /* Регистр достоверного маркера времени */
#define MCB_SWIC_TOUT_CODE(n)             MCB_SWIC_REG (n,0x30)   /* Регистр размера таймаутов */
#define MCB_SWIC_ISR_TOUT_L(n)            MCB_SWIC_REG (n,0x34)   /* Младшие разряды регистра флагов таймаутов ISR */
#define MCB_SWIC_ISR_TOUT_H(n)            MCB_SWIC_REG (n,0x38)   /* Старшие разряды регистра флагов таймаутов ISR */
#define MCB_SWIC_LOG_ADDR(n)              MCB_SWIC_REG (n,0x3c)   /* Регистр логического адреса */
#define MCB_SWIC_ACK_NONACK_MODE(n)       MCB_SWIC_REG (n,0x40)   /* Регистр управления режимом распределенных прерываний (с подтверждениями или без подтверждений) */
#define MCB_SWIC_ISR2_TOUT(n)             MCB_SWIC_REG (n,0x44)   /* Регистр таймаутов кодов распределенных прерываний 2 */
#define MCB_SWIC_ISR_HANDLER_FUNC(n)      MCB_SWIC_REG (n,0x48)   /* Регистр флагов функций терминального узла обработчика  */
#define MCB_SWIC_ISR_SPEC(n)              MCB_SWIC_REG (n,0x4c)   /* Регистр рассылки управляющих кодов в специальный набор портов */
#define MCB_SWIC_ISR_1101(n)              MCB_SWIC_REG (n,0x50)   /* Регистр флагов приема управляющих кодов, назначение которых не определено в стандарте ECSS-E-50-12С */
#define MCB_SWIC_ISR_MASK_1101(n)         MCB_SWIC_REG (n,0x54)   /* Регистр маски портов, из которых не должны приниматься управляющие коды, назначение которых не определено в текущей версии стандарта */
#define MCB_SWIC_INT_RESET(n)             MCB_SWIC_REG (n,0x58)   /* Регистр параметров команды внешнего сброса */
#define MCB_SWIC_MODE_CR2(n)              MCB_SWIC_REG (n,0x60)   /* Регистр режима работы 2 */
#define MCB_SWIC_INT_H_MASK               MCB_SWIC_REG (n,0x64)   /* Старшая половина регистра маски распределенных прерываний */
#define MCB_SWIC_INT_L_MASK               MCB_SWIC_REG (n,0x68)   /* Младшая половина регистра маски распределенных прерываний */
#define MCB_SWIC_ACK_H_MASK               MCB_SWIC_REG (n,0x6c)   /* Старшая половина регистра маски кодов подтверждения */
#define MCB_SWIC_ACK_L_MASK               MCB_SWIC_REG (n,0x70)   /* Младшая половина регистра маски кодов подтверждения */
#define MCB_SWIC_AUTO_SPEED_MANAGE(n)     MCB_SWIC_REG (n,0x74)   /* Регистр параметров автоматической установки скорости */
#define MCB_SWIC_ISR_SOURCE_TERM_FUNC(n)  MCB_SWIC_REG (n,0x78)   /* Регистр флагов функций терминального узла источника */
#define MCB_SWIC_ISR_SPEC_TERM_FUNC(n)    MCB_SWIC_REG (n,0x7c)   /* Регистр признака специальной функции для терминального узла обработчика */
#define MCB_SWIC_ISR_L_RESET(n)           MCB_SWIC_REG (n,0x80)   /* Младшая половина регистра глобального сброса ISR */
#define MCB_SWIC_ISR_H_RESET(n)           MCB_SWIC_REG (n,0x80)   /* Старшая половина регистра глобального сброса ISR */

/*
 * Регистры Spacewire DMA
 */
#define MCB_SWIC_RX_DESC_CSR(n) MCB_SWIC_DMA_REG(n,0x00) /* Управление и состояние канала RX_DESC */
#define MCB_SWIC_RX_DESC_CP(n)  MCB_SWIC_DMA_REG(n,0x04) /* Указатель цепочки канала RX_DESC */
#define MCB_SWIC_RX_DESC_IR(n)  MCB_SWIC_DMA_REG(n,0x08) /* Индексный регистр канала RX_DESC */
#define MCB_SWIC_RX_DESC_RUN(n) MCB_SWIC_DMA_REG(n,0x0c) /* Псевдорегистр управления битом RUN RX_DESC */

#define MCB_SWIC_RX_DATA_CSR(n) MCB_SWIC_DMA_REG(n,0x40) /* Управление и состояние канала RX_DATA */
#define MCB_SWIC_RX_DATA_CP(n)  MCB_SWIC_DMA_REG(n,0x44) /* Указатель цепочки канала RX_DATA */
#define MCB_SWIC_RX_DATA_IR(n)  MCB_SWIC_DMA_REG(n,0x48) /* Индексный регистр канала RX_DATA */
#define MCB_SWIC_RX_DATA_RUN(n) MCB_SWIC_DMA_REG(n,0x4c) /* Псевдорегистр управления битом RUN RX_DATA */

#define MCB_SWIC_TX_DESC_CSR(n) MCB_SWIC_DMA_REG(n,0x80) /* Управление и состояние канала TX_DESC */
#define MCB_SWIC_TX_DESC_CP(n)  MCB_SWIC_DMA_REG(n,0x84) /* Указатель цепочки канала TX_DESC */
#define MCB_SWIC_TX_DESC_IR(n)  MCB_SWIC_DMA_REG(n,0x88) /* Индексный регистр канала TX_DESC */
#define MCB_SWIC_TX_DESC_RUN(n) MCB_SWIC_DMA_REG(n,0x8c) /* Псевдорегистр управления битом RUN TX_DESC */

#define MCB_SWIC_TX_DATA_CSR(n) MCB_SWIC_DMA_REG(n,0xc0) /* Управление и состояние канала TX_DATA */
#define MCB_SWIC_TX_DATA_CP(n)  MCB_SWIC_DMA_REG(n,0xc4) /* Указатель цепочки канала TX_DATA */
#define MCB_SWIC_TX_DATA_IR(n)  MCB_SWIC_DMA_REG(n,0xc8) /* Индексный регистр канала TX_DATA */
#define MCB_SWIC_TX_DATA_RUN(n) MCB_SWIC_DMA_REG(n,0xcc) /* Псевдорегистр управления битом RUN TX_DATA */

