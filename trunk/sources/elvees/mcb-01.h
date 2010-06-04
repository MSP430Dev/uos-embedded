/*
 * Карта памяти микросхемы MCB-01.
 */
#define MCB_RAM_BASE			0xaf000000
#define MCB_MBA_BASE			0xafc00000
#define MCB_MBA_REG(r)			(*(volatile unsigned*) (MCB_MBA_BASE | (r)))

#define MCB_PERIF(base, reg)		((base) | (reg))
#define MCB_REG_RAM_BASE		0x01000000
#define MCB_PMSC_REG(r)			MCB_PERIF (0x01200000, r)
#define MCB_SWIC_REG(n,r)		MCB_PERIF (0x01400000 + ((n)<<21), r)
#define MCB_SWIC_DMA_REG(n,r)		MCB_PERIF (0x01500000 + ((n)<<21), r)

/*
 * Регистры MBA
 */
#define MCB_MBA_QSTR		MCB_MBA_REG (0x0000)	/* Регистр заявок */
#define MCB_MBA_MASK		MCB_MBA_REG (0x0004)	/* Регистр маски */
#define MCB_MBA_BDR		MCB_MBA_REG (0x0008)	/* Регистр буферных данных */
#define MCB_MBA_BUSY		MCB_MBA_REG (0x000c)	/* Регистр признака занятости */

/*
 * Чтение/запись памяти и регистров MBA осуществляется с помощью обычного обращения
 * по адресу, чтение/запись регистров SWIC, PMSC и адресного окна PCI должна
 * осущестляться только с помощью функций mcb_read_reg/mcb_write_reg.
 * Для разъяснений см. Руководство пользователя на микросхему 1892ХД1Я, п. 5.3.
 */
unsigned mcb_read_reg (unsigned addr);
void mcb_write_reg (unsigned addr, unsigned value);

/*
 * Регистры контроллера PCI
 */
#define MCB_PCI_DEVICE_VENDOR_ID MCB_PMSC_REG (0x00)	/* Идентификация устройства */

#define MCB_PCI_STATUS_COMMAND	MCB_PMSC_REG (0x04)	/* Состояние и управление */
#define  MCB_PCI_COMMAND_MEMORY		0x00000002	/* Разрешение выполнения команд обмена данными с памятью */
#define  MCB_PCI_COMMAND_MASTER		0x00000004	/* Разрешения работы на шине PCI в режиме задатчика */
#define  MCB_PCI_COMMAND_PARITY		0x00000040	/* Разрешение формирование сигнала PERR */
#define  MCB_PCI_STATUS_PARITY		0x01000000	/* Признак выдачи или приема сигнала PERR в режиме Master */
#define  MCB_PCI_STATUS_DEVSEL_MASK	0x06000000	/* DEVSEL timing = 01 (meduim) */
#define  MCB_PCI_STATUS_TARGET_ABORT	0x10000000	/* Признак завершения обмена по условию Target-abort */
#define  MCB_PCI_STATUS_MASTER_ABORT	0x20000000	/* Признак завершения обмена по условию Master-abort */
#define  MCB_PCI_STATUS_DETECTED_PARITY 0x80000000	/* Ошибка четности при приме данных из PCI */

#define MCB_PCI_CLASS_REVISION	MCB_PMSC_REG (0x08)	/* Регистр кода */
#define MCB_PCI_LATENCY_TIMER	MCB_PMSC_REG (0x0c)	/* Таймер времени передачи (MLT) */
#define MCB_PCI_BAR		MCB_PMSC_REG (0x10)	/* Базовый адрес 0 */
#define MCB_PCI_SUBSYSTEM_VENDOR_ID MCB_PMSC_REG (0x2c)	/* Идентификация подсистемы */
#define MCB_PCI_INTERRUPT_LINE	MCB_PMSC_REG (0x3c)	/* Код прерывания */
#define MCB_PCI_SEM		MCB_PMSC_REG (0x44)	/* Семафор */
#define MCB_PCI_MBR		MCB_PMSC_REG (0x48)	/* Почтовый ящик */

#define MCB_PCI_CSR_PCI		MCB_PMSC_REG (0x4C)	/* Управление шиной PCI */
#define  MCB_PCI_CSR_PCI_INTA		0x00000001	/* Формирование прерывания INTA */
#define  MCB_PCI_CSR_PCI_WN(n)		((n) << 1)	/* Уровень FIFO записи в память, исходно 8 */
#define  MCB_PCI_CSR_PCI_TESTPERR	0x00000040	/* Принудительное формирование сигнала nPERR */
#define  MCB_PCI_CSR_PCI_TESTPAR	0x00000080	/* Инвертирование сигнала PAR */
#define  MCB_PCI_CSR_PCI_MLTOVER	0x00010000	/* Признак срабатывания Latency Timer */
#define  MCB_PCI_CSR_PCI_NOTRDY		0x00020000	/* Признак отсутствия сигнала TRDY */
#define  MCB_PCI_CSR_PCI_DISCONNECT	0x00040000	/* Признак требования разъединения */
#define  MCB_PCI_CSR_PCI_RETRY		0x00080000	/* Признак требования повтора */
#define  MCB_PCI_CSR_PCI_NOGNT		0x00100000	/* Признак отсутствия сигнала nGNT */
#define  MCB_PCI_CSR_PCI_TARGET_ABORT	0x10000000	/* Признак окончания обмена с Target-Abort */
#define  MCB_PCI_CSR_PCI_MASTER_ABORT	0x10000000	/* Признак окончания обмена с Master-Abort */

#define MCB_PCI_CSR_MASTER	MCB_PMSC_REG (0x50)	/* Состояние и управление обменом в режиме Master */
#define  MCB_PCI_CSR_MASTER_RUN		0x00000001	/* Состояние работы канала DMA */
#define  MCB_PCI_CSR_MASTER_CMD		0x0000001e	/* Тип команды обмена в режиме Master */
#define  MCB_PCI_CSR_MASTER_IOREAD	0x00000004	/* I/O Read */
#define  MCB_PCI_CSR_MASTER_IOWRITE	0x00000006	/* I/O Write */
#define  MCB_PCI_CSR_MASTER_MEMREAD	0x0000000c	/* Memory Read */
#define  MCB_PCI_CSR_MASTER_MEMWRITE	0x0000000d	/* Memory Write */
#define  MCB_PCI_CSR_MASTER_CFGREAD	0x00000014	/* Configuration Read */
#define  MCB_PCI_CSR_MASTER_CFGWRITE	0x00000016	/* Configuration Write */
#define  MCB_PCI_CSR_MASTER_DONE	0x00008000	/* Признак завершения передачи */
#define  MCB_PCI_CSR_MASTER_WC(n)	((n) << 16)	/* Счетчик слов DMA обмена */

#define MCB_PCI_IR_MASTER	MCB_PMSC_REG (0x54)	/* Индексный регистр адреса памяти при обмене в режиме Master */
#define MCB_PCI_AR_PCI		MCB_PMSC_REG (0x58)	/* Адресный регистр PCI */

#define MCB_PCI_QSTR_PCI	MCB_PMSC_REG (0x5C)	/* Системные прерывания */
#define  MCB_PCI_QSTR_SWIC0_LINK	0x00000001	/* SWIC0: Установлено соединение/получен пакет */
#define  MCB_PCI_QSTR_SWIC0_ERR		0x00000002	/* SWIC0: Ошибка в канале приема */
#define  MCB_PCI_QSTR_SWIC0_TIM		0x00000004	/* SWIC0: Получен управляющий код */
#define  MCB_PCI_QSTR_SWIC1_LINK	0x00000008	/* SWIC1: Установлено соединение/получен пакет */
#define  MCB_PCI_QSTR_SWIC1_ERR		0x00000010	/* SWIC1: Ошибка в канале приема */
#define  MCB_PCI_QSTR_SWIC1_TIM		0x00000020	/* SWIC1: Получен управляющий код */
#define  MCB_PCI_QSTR_SWIC2_LINK	0x00000040	/* SWIC2: Установлено соединение/получен пакет */
#define  MCB_PCI_QSTR_SWIC2_ERR		0x00000080	/* SWIC2: Ошибка в канале приема */
#define  MCB_PCI_QSTR_SWIC2_TIM		0x00000100	/* SWIC2: Получен управляющий код */
#define  MCB_PCI_QSTR_SWIC3_LINK	0x00000200	/* SWIC3: Установлено соединение/получен пакет */
#define  MCB_PCI_QSTR_SWIC3_ERR		0x00000400	/* SWIC3: Ошибка в канале приема */
#define  MCB_PCI_QSTR_SWIC3_TIM		0x00000800	/* SWIC3: Получен управляющий код */
#define  MCB_PCI_QSTR_SWIC0_RX_DESC	0x00001000	/* SWIC0: Окончание DMA RX_DESC */
#define  MCB_PCI_QSTR_SWIC0_RX_DATA	0x00002000	/* SWIC0: Окончание DMA RX_DATA */
#define  MCB_PCI_QSTR_SWIC0_TX_DESC	0x00004000	/* SWIC0: Окончание DMA TX_DESC */
#define  MCB_PCI_QSTR_SWIC0_TX_DATA	0x00008000	/* SWIC0: Окончание DMA TX_DATA */
#define  MCB_PCI_QSTR_SWIC1_RX_DESC	0x00010000	/* SWIC1: Окончание DMA RX_DESC */
#define  MCB_PCI_QSTR_SWIC1_RX_DATA	0x00020000	/* SWIC1: Окончание DMA RX_DATA */
#define  MCB_PCI_QSTR_SWIC1_TX_DESC	0x00040000	/* SWIC1: Окончание DMA TX_DESC */
#define  MCB_PCI_QSTR_SWIC1_TX_DATA	0x00080000	/* SWIC1: Окончание DMA TX_DATA */
#define  MCB_PCI_QSTR_SWIC2_RX_DESC	0x00100000	/* SWIC2: Окончание DMA RX_DESC */
#define  MCB_PCI_QSTR_SWIC2_RX_DATA	0x00200000	/* SWIC2: Окончание DMA RX_DATA */
#define  MCB_PCI_QSTR_SWIC2_TX_DESC	0x00400000	/* SWIC2: Окончание DMA TX_DESC */
#define  MCB_PCI_QSTR_SWIC2_TX_DATA	0x00800000	/* SWIC2: Окончание DMA TX_DATA */
#define  MCB_PCI_QSTR_SWIC3_RX_DESC	0x01000000	/* SWIC3: Окончание DMA RX_DESC */
#define  MCB_PCI_QSTR_SWIC3_RX_DATA	0x02000000	/* SWIC3: Окончание DMA RX_DATA */
#define  MCB_PCI_QSTR_SWIC3_TX_DESC	0x04000000	/* SWIC3: Окончание DMA TX_DESC */
#define  MCB_PCI_QSTR_SWIC3_TX_DATA	0x08000000	/* SWIC3: Окончание DMA TX_DATA */

#define MCB_PCI_MASKR_PCI	MCB_PMSC_REG (0x60)	/* Маскирование прерываний */

/*
 * Регистры Spacewire
 */
#define MCB_SWIC_HW_VER(n)	MCB_SWIC_REG (n,0x00)	/* Регистр аппаратной версии */
#define MCB_SWIC_STATUS(n)	MCB_SWIC_REG (n,0x04)	/* Статус */
#define MCB_SWIC_RX_CODE(n)	MCB_SWIC_REG (n,0x08)	/* Принимаемый маркер времени */
#define MCB_SWIC_MODE_CR(n)	MCB_SWIC_REG (n,0x0c)	/* Управление */
#define MCB_SWIC_TX_SPEED(n)	MCB_SWIC_REG (n,0x10)	/* Скорость передачи */
#define MCB_SWIC_TX_CODE(n)	MCB_SWIC_REG (n,0x14)	/* Передаваемый маркер времени */
#define MCB_SWIC_RX_SPEED(n)	MCB_SWIC_REG (n,0x18)	/* Скорость приема */
#define MCB_SWIC_CNT_RX_PACK(n)	MCB_SWIC_REG (n,0x1c)	/* Счетчик принятых пакетов */
#define MCB_SWIC_CNT_RX0_PACK(n) MCB_SWIC_REG (n,0x20)	/* Счетчик принятых пустых пакетов */
#define MCB_SWIC_ISR_L(n)	MCB_SWIC_REG (n,0x24)	/* Статус прерываний (младшая часть) */
#define MCB_SWIC_ISR_H(n)	MCB_SWIC_REG (n,0x28)	/* Статус прерываний (старшая часть) */

/*
 * Регистры Spacewire DMA
 */
#define MCB_SWIC_RX_DESC_CSR(n)	MCB_SWIC_DMA_REG(n,0x00) /* Управление и состояние канала RX_DESC */
#define MCB_SWIC_RX_DESC_CP(n)	MCB_SWIC_DMA_REG(n,0x08) /* Указатель цепочки канала RX_DESC */
#define MCB_SWIC_RX_DESC_IR(n)	MCB_SWIC_DMA_REG(n,0x0c) /* Индексный регистр канала RX_DESC */
#define MCB_SWIC_RX_DESC_OR(n)	MCB_SWIC_DMA_REG(n,0x10) /* Смещение канала RX_DESC */
#define MCB_SWIC_RX_DESC_RUN(n)	MCB_SWIC_DMA_REG(n,0x18) /* Псевдорегистр управления битом RUN RX_DESC */

#define MCB_SWIC_RX_DATA_CSR(n)	MCB_SWIC_DMA_REG(n,0x40) /* Управление и состояние канала RX_DATA */
#define MCB_SWIC_RX_DATA_CP(n)	MCB_SWIC_DMA_REG(n,0x48) /* Указатель цепочки канала RX_DATA */
#define MCB_SWIC_RX_DATA_IR(n)	MCB_SWIC_DMA_REG(n,0x4c) /* Индексный регистр канала RX_DATA */
#define MCB_SWIC_RX_DATA_OR(n)	MCB_SWIC_DMA_REG(n,0x50) /* Смещение канала RX_DATA */
#define MCB_SWIC_RX_DATA_RUN(n)	MCB_SWIC_DMA_REG(n,0x58) /* Псевдорегистр управления битом RUN RX_DATA */

#define MCB_SWIC_TX_DESC_CSR(n)	MCB_SWIC_DMA_REG(n,0x80) /* Управление и состояние канала TX_DESC */
#define MCB_SWIC_TX_DESC_CP(n)	MCB_SWIC_DMA_REG(n,0x88) /* Указатель цепочки канала TX_DESC */
#define MCB_SWIC_TX_DESC_IR(n)	MCB_SWIC_DMA_REG(n,0x8c) /* Индексный регистр канала TX_DESC */
#define MCB_SWIC_TX_DESC_OR(n)	MCB_SWIC_DMA_REG(n,0x90) /* Смещение канала TX_DESC */
#define MCB_SWIC_TX_DESC_RUN(n)	MCB_SWIC_DMA_REG(n,0x98) /* Псевдорегистр управления битом RUN TX_DESC */

#define MCB_SWIC_TX_DATA_CSR(n)	MCB_SWIC_DMA_REG(n,0xc0) /* Управление и состояние канала TX_DATA */
#define MCB_SWIC_TX_DATA_CP(n)	MCB_SWIC_DMA_REG(n,0xc8) /* Указатель цепочки канала TX_DATA */
#define MCB_SWIC_TX_DATA_IR(n)	MCB_SWIC_DMA_REG(n,0xcc) /* Индексный регистр канала TX_DATA */
#define MCB_SWIC_TX_DATA_OR(n)	MCB_SWIC_DMA_REG(n,0xd0) /* Смещение канала TX_DATA */
#define MCB_SWIC_TX_DATA_RUN(n)	MCB_SWIC_DMA_REG(n,0xd8) /* Псевдорегистр управления битом RUN TX_DATA */
