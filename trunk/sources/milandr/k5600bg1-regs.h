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
	eth_reg_t CTRL;		// Управление и состояние
	eth_reg_t LEN;		// Полная длина пакета в байтах
	eth_reg_t PTRH;		// Старшая часть указателя данных
	eth_reg_t PTRL;		// Указатель на данные пакета
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
#define GCTRL_GLBL_RST		(1 << 15)	// Общий сброс всего контроллера
#define GCTRL_READ_CLR_STAT	(1 << 14)	// Очистка статистики по чтению
#define GCTRL_SPI_RST		(1 << 13)	// Сброс последовательного порта
#define GCTRL_ASYNC_MODE	(1 << 12)	// Асинхронный режим сигнала RDY
#define GCTRL_SPI_RX_EDGE	(1 << 10)	// Положительный фронт приёмника SPI
#define GCTRL_SPI_TX_EDGE	(1 << 9)	// Положительный фронт передатчика SPI
#define GCTRL_SPI_DIR		(1 << 8)	// Левый бит вперёд (MSB)
#define GCTRL_SPI_FRAME_POL	(1 << 7)	// Положительный активный уровень
						// сигнала кадровой синхронизации SPI
#define GCTRL_SPI_CLK_POL	(1 << 6)	// Инверсная полярность тактового сигнала
#define GCTRL_SPI_CLK_PHASE	(1 << 5)	// Инверсная фаза тактового сигнала
#define GCTRL_SPI_DIV(x)	(x)		// (зарезервировано)

/*
 * Регистр MAC_CTRL - управление MAC-уровнем
 */
#define MAC_CTRL_TX_RST		(1 << 15)	// Сброс передатчика
#define MAC_CTRL_RX_RST		(1 << 14)	// Сброс приёмника
#define MAC_CTRL_DSCR_SCAN_EN	(1 << 12)	// Режим сканирования дескрипторов
#define MAC_CTRL_PAUSE_EN	(1 << 11)	// Разрешения обработки Pause Frame
#define MAC_CTRL_PRO_EN		(1 << 10)	// Прием всех пакетов
#define MAC_CTRL_BCA_EN		(1 << 9)	// Прием всех широковещательных пакетов
#define MAC_CTRL_MCA_EN		(1 << 8)	// Прием всех пакетов согласно Hash
#define MAC_CTRL_CTRL_FRAME_EN	(1 << 7)	// Прием управляющих пакетов
#define MAC_CTRL_LONG_FRAME_EN	(1 << 6)	// Прием пакетов длиной более MaxFrame
#define MAC_CTRL_SHORT_FRAME_EN	(1 << 5)	// Прием пакетов длиной менее MinFrame
#define MAC_CTRL_ERR_FRAME_EN	(1 << 4)	// Прием пакетов с ошибками
#define MAC_CTRL_BCKOF_DIS	(1 << 3)	// Без ожидания при повторе в случае коллизии
#define MAC_CTRL_HALFD_EN	(1 << 2)	// Полудуплексный режим
#define MAC_CTRL_BIG_ENDIAN	(1 << 1)	// Левый бит вперёд (big endian)
#define MAC_CTRL_LB_EN		(1 << 0)	// Тестовый шлейф на уровне MAC

/*
 * Регистр COLLCONF - управление обработкой коллизий
 */
#define COLLCONF_RETRIES_LIMIT(x)	((x) << 8)	// Лимит повторов передачи
#define COLLCONF_COLLISION_WINDOW(x)	(x)		// Окно коллизий

/*
 * Регистры INT_MASK, INT_SRC - флаги и маскирование прерываний
 */
#define INT_RXF			(1 << 15)	// Успешный прием пакета
#define INT_RXE			(1 << 14)	// Наличие ошибок приема
#define INT_RXC			(1 << 13)	// Прием пакета управления
#define INT_RXBF_FULL		(1 << 12)	// Переполнение буфера приема
#define INT_RXL			(1 << 10)	// Прием пакета длиной более MaxFrame
#define INT_RXS			(1 << 9)	// Прием пакета длиной менее MinFrame
#define INT_TXF			(1 << 7)	// Успешная передача пакета
#define INT_TXE			(1 << 6)	// Наличие ошибок при передаче пакета
#define INT_TXC			(1 << 5)	// Передача пакета управления
#define INT_TX_BUSY		(1 << 0)	// Приём и обслуживание пакета Pause

/*
 * Регистр PHY_CTRL - управление PHY-уровнем
 */
#define PHY_CTRL_RST		(1 << 15)	// Сброс встроенного контроллера PHY
#define PHY_CTRL_EXT_EN		(1 << 14)	// Работа с внешним кнтроллером PHY
#define PHY_CTRL_TXEN		(1 << 13)	// Разрешение передатчика встроенного PHY
#define PHY_CTRL_RXEN		(1 << 12)	// Разрешение приёмника встроенного PHY
#define PHY_CTRL_LINK_PERIOD(x)	((x) << 6)	// Период LINK-импульсов
#define PHY_CTRL_BASE_2		(1 << 5)	// Коаксиальный кабель в полудуплексе
#define PHY_CTRL_DIR		(1 << 4)	// Прямой порядок битов в полубайте
#define PHY_CTRL_EARLY_DV	(1 << 3)	// Формирвание RxDV одновременно с CRS
#define PHY_CTRL_HALFD		(1 << 2)	// Полудуплесный режим
#define PHY_CTRL_DLB		(1 << 1)	// Тестовый шлейф на входе PHY
#define PHY_CTRL_LB		(1 << 0)	// Тестовый шлейф на выходе PHY

/*
 * Регистр PHY_STAT - состояние PHY-уровня
 */
#define PHY_STAT_JAM		(1 << 13)	// Коллизия: передача JAM-последовательности
#define PHY_STAT_JAB		(1 << 12)	// Превышение максимального времени передачи
#define PHY_STAT_POL		(1 << 11)	// Смена полярности сигналов в приёмнике
#define PHY_STAT_LINK		(1 << 10)	// Наличие подключения
#define PHY_STAT_COL		(1 << 9)	// Наличие коллизии
#define PHY_STAT_CRS		(1 << 8)	// Наличие несущей в линии
#define PHY_STAT_EXT_LINK	(1 << 5)	// Внешний PHY: наличие подключения
#define PHY_STAT_EXT_COL	(1 << 1)	// Внешний PHY: наличие коллизии
#define PHY_STAT_EXT_CRS	(1 << 0)	// Внешний PHY: наличие несущей в линии
#define PHY_STAT_BITS		"\20\01ext_crs\02ext_col\06ext_link" \
				"\11crs\12col\13link\14pol\15jab\16jam"
/*
 * Дескриптор передатчика - управление и состояние
 */
#define DESC_TX_RDY		(1 << 15)	// Исходящий пакет готов
#define DESC_TX_WRAP		(1 << 14)	// Последний дескриптор: переход к началу
#define DESC_TX_IRQ_EN		(1 << 13)	// Разрешение прерываний по передаче
#define DESC_TX_PRE_DIS		(1 << 10)	// Отключение передачи преамбулы
#define DESC_TX_PAD_DIS		(1 << 9)	// Отключение дополнения до минимальной длины
#define DESC_TX_CRC_DIS		(1 << 8)	// Отключение добавления контрольной суммы
#define DESC_TX_RTRY(x)		(((x)>>4) & 15)	// Счётчик попыток передачи
#define DESC_TX_RL		(1 << 3)	// Исчерпание кол-ва попыток отправки
#define DESC_TX_LC		(1 << 2)	// Наличия Late Collision
#define DESC_TX_UR		(1 << 1)	// Underrun
#define DESC_TX_CS		(1 << 0)	// Потеря несущей во время передачи

/*
 * Дескриптор приёмника - управление и состояние
 */
#define DESC_RX_RDY		(1 << 15)	// Готов к приему пакета
#define DESC_RX_WRAP		(1 << 14)	// Последний дескриптор: переход к началу
#define DESC_RX_IRQ_EN		(1 << 13)	// Разрешение прерываний по приёму
#define DESC_RX_MCA		(1 << 10)	// Прием группового пакета по Hash-таблице
#define DESC_RX_BCA		(1 << 9)	// Прием широковещательного пакета
#define DESC_RX_UCA		(1 << 8)	// Прием индивидуального пакета
#define DESC_RX_CF		(1 << 7)	// Прием пакета управления
#define DESC_RX_LF		(1 << 6)	// Прием пакета длиной более MaxFrame
#define DESC_RX_SF		(1 << 5)	// Прием пакета длиной менее MinFrame
#define DESC_RX_EF		(1 << 4)	// Есть ошибки при приеме пакета
#define DESC_RX_CRC_ERR		(1 << 3)	// Ошибка CRC
#define DESC_RX_SMB_ERR		(1 << 2)	// Ошибка в данных
#define DESC_RX_LC		(1 << 1)	// Наличие Late Collision
#define DESC_RX_OR		(1 << 0)	// Переполнение буфера приема
