/*
 * Описание регистров контроллера 5600ВГ1 фирмы Миландр.
 */
typedef volatile unsigned int eth_reg_t;

typedef	struct {
	eth_reg_t MAC_CTRL;	// Управление MAC-уровнем контроллера
	eth_reg_t MIN_FRAME;	// Минимальная допустимая длина пакета
	eth_reg_t MAX_FRAME;	// Максимальная допустимая длина пакета
	eth_reg_t COLLCONF;	// Управление обработкой коллизий
	eth_reg_t IPGT;		// Задание межпакетного интервала
	eth_reg_t MAC_ADDR [3];	// Задание MAC-адреса контроллера
	eth_reg_t HASH [4];	// HASH-таблица для расширенной фильтрации
	eth_reg_t INT_MASK;	// Маскирование прерываний
	eth_reg_t INT_SRC;	// Флаги прерываний
	eth_reg_t PHY_CTRL;	// Управление PHY-уровнем
	eth_reg_t PHY_STAT;	// Состояние PHY-уровня

	eth_reg_t RXBF_HEAD;	// Голова буфера приёма
	eth_reg_t RXBF_TAIL;	// Хвост буфера приёма
	eth_reg_t unused [2];	// (зарезервировано)

	eth_reg_t STAT_RX_ALL;	// Счетчик входящих пакетов
	eth_reg_t STAT_RX_OK;	// Счетчик успешно принятых
	eth_reg_t STAT_RX_OVF;	// Счетчик переполнениq буфера приёма
	eth_reg_t STAT_RX_LOST;	// Счетчик потеряных из-за неготовности
	eth_reg_t STAT_TX_ALL;	// Счетчик исходящих пакетов
	eth_reg_t STAT_TX_OK;	// Счетчик успешно отосланных пакетов

	eth_reg_t BASE_RXBF;	// Начало буфера приемника
	eth_reg_t BASE_TXBF;	// Начало буфера передатчика
	eth_reg_t BASE_RXBD;	// Начало дескриторов приемника
	eth_reg_t BASE_TXBD;	// Начало дескриторов передатчика
	eth_reg_t BASE_REG;	// Начало области регистров
	eth_reg_t GCTRL;	// Управление внешними интерфейсами
} eth_regs_t;

typedef	struct {
	eth_reg_t ctrl;		// Управление и состояние
	eth_reg_t len;		// Полная длина пакета в байтах
	eth_reg_t ptrh;		// Старшая часть указателя данных
	eth_reg_t ptrl;		// Указатель на данные пакета
} eth_desc_t;

/*
 * Адреса регистров и буферов приёма/передачи.
 */
#define ETH_BASE	0x60000000
#define ETH_RXBUF	((eth_reg_t*)  (ETH_BASE + 0x0000))
#define ETH_RXDESC	((eth_desc_t*) (ETH_BASE + 0x2000))
#define ETH_TXBUF	((eth_reg_t*)  (ETH_BASE + 0x4000))
#define ETH_TXDESC	((eth_desc_t*) (ETH_BASE + 0x6000))
#define ETH_REG		((eth_regs_t*) (ETH_BASE + 0x7F00))

/*
 * Регистр GCTRL - управление интерфейсом к процессору
 */
#define GCTRL_GLBL_RST		(1 << 15)	// Общий сброс всего контроллера (активный уровень "1")
#define GCTRL_READ_CLR_STAT	(1 << 14)	// Очистка статистики по чтению (активный уровень "1")
#define GCTRL_SPI_RST		(1 << 13)	// Сброс встроенного контроллера последовательного порта (активный уровень "1")
#define GCTRL_RG_INMEM		(1 << 12)	// (зарезервировано)
#define GCTRL_SPI_RX_EDGE	(1 << 10)	// Активный фронт ПРМ контроллера последовательного порта (1-положительный,0-отрицательный)
#define GCTRL_SPI_TX_EDGE	(1 << 9)	// Активный фронт ПРД контроллера последовательного порта (1-положительный,0-отрицательный)
#define GCTRL_SPI_DIR		(1 << 8)	// Порядок передачи бит (1-MSB,0-LSB)
#define GCTRL_SPI_FRAME_POL	(1 << 7)	// Активный уровень сигнала кадровой синхронизации последовательного порта  (1-положительный,0-отрицательный)
#define GCTRL_SPI_CLK_POL	(1 << 6)	// Полярность тактового сигнала последовательного порта  (1-инверсная,0-прямая)
#define GCTRL_SPI_CLK_PHASE	(1 << 5)	// Фаза тактового сигнала последовательного порта  (1-инверсная,0-прямая)

/*
 * Регистр MAC_CTRL - управление MAC-уровнем
 */

/*
 * Регистр COLLCONF - управление обработкой коллизий
 */

/*
 * Регистры INT_MASK, INT_SRC - флаги и маскирование прерываний
 */

/*
 * Регистр PHY_CTRL - управление PHY-уровнем
 */

/*
 * Регистр PHY_STAT - состояние PHY-уровня
 */

/*
 * Дескриптор передатчика - управление и состояние
 */
#define DESC_TX_RDY		(1 << 15)	// Индикатор состояния исходящего пакета (1-готов, 0-отправлен/не заполнен)
#define DESC_TX_WRAP		(1 << 14)	// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
#define DESC_TX_IRQ_EN		(1 << 13)	// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
#define DESC_TX_PRE_DIS		(1 << 10)	// Отключение передачи преамбулы (1-передача преамбулы отключена, 0-стандартный пакет)
#define DESC_TX_PAD_DIS		(1 << 9)	// Отключение дополнения пакетов длиной менее MinFrame до минимальной длины PAD-ами (1-дополнение не производится, дополнения производится)
#define DESC_TX_CRC_DIS		(1 << 8)	// Отключение дополнения пакетов полем CRC32
#define DESC_TX_RTRY(s)		(((s)>>4) & 15)	// Счётчик попыток передачи
#define DESC_TX_RL		(1 << 3)	// Индикатор исчерпания разрешенного кол-ва повторения в случае неуспешной отправки исходящего пакета
#define DESC_TX_LC		(1 << 2)	// Индикатор наличия Late Collision
#define DESC_TX_UR		(1 << 1)	// Underrun
#define DESC_TX_CS		(1 << 0)	// Индикатор потери несущей во время передачи пакета

/*
 * Дескриптор приёмника - управление и состояние
 */
#define DESC_RX_RDY		(1 << 15)	// Индикатор состояния исходящего пакета (1-готов к приему пакета, 0-пакет принят/не готов)
#define DESC_RX_WRAP		(1 << 14)	// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
#define DESC_RX_IRQ_EN		(1 << 13)	// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
#define DESC_RX_MCA		(1 << 10)	// Индикатор приема группового пакета с MAC-адресом соответствующего HASH-таблице
#define DESC_RX_BCA		(1 << 9)	// Индикатор приема широковещательного пакета
#define DESC_RX_UCA		(1 << 8)	// Индикатор приема индивидуального пакета с полным совпадением MAC-адреса
#define DESC_RX_CF		(1 << 7)	// Индикатор приема пакета управления
#define DESC_RX_LF		(1 << 6)	// Индикатор приема пакета длиной более MaxFrame
#define DESC_RX_SF		(1 << 5)	// Индикатор приема пакета длиной менее MinFrame
#define DESC_RX_EF		(1 << 4)	// Индикатор наличия ошибок при приеме пакета (сводный бит по ошибкам см. ниже)
#define DESC_RX_CRC_ERR		(1 << 3)	// Индикатор наличия ошибки CRC при приеме пакета
#define DESC_RX_SMB_ERR		(1 << 2)	// Индикатор наличия ошибки в данных при приеме пакета
#define DESC_RX_LC		(1 << 1)	// Индикатор наличия Late Collision
#define DESC_RX_OR		(1 << 0)	// Индикатор переполнения буфера ПРМ при приеме пакета

#if 0
/*
 * PHY Status Register (aka MII_BMSR)
 */
#define PHY_STAT_CAP_T4		0x8000	/* 1=100Base-T4 capable */
#define PHY_STAT_CAP_TXF	0x4000	/* 1=100Base-X full duplex capable */
#define PHY_STAT_CAP_TXH	0x2000	/* 1=100Base-X half duplex capable */
#define PHY_STAT_CAP_TF		0x1000	/* 1=10Mbps full duplex capable */
#define PHY_STAT_CAP_TH		0x0800	/* 1=10Mbps half duplex capable */
#define PHY_STAT_CAP_SUPR	0x0040	/* 1=recv mgmt frames with not preamble */
#define PHY_STAT_ANEG_ACK	0x0020	/* 1=ANEG has completed */
#define PHY_STAT_REM_FLT	0x0010	/* 1=Remote Fault detected */
#define PHY_STAT_CAP_ANEG	0x0008	/* 1=Auto negotiate capable */
#define PHY_STAT_LINK		0x0004	/* 1=valid link */
#define PHY_STAT_JAB		0x0002	/* 1=10Mbps jabber condition */
#define PHY_STAT_EXREG		0x0001	/* 1=extended registers implemented */
#define PHY_STAT_BITS	\
"\20\01exreg\02jab\03link\04cap_aneg\05rem_flt\06aneg_ack\07cap_supr" \
"\14cap_th\15cap_tf\16cap_txh\17cap_txf\20cap_t4"

#endif
