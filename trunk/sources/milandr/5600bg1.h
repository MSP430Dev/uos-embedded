/*
 * Описание регистров контроллера 5600ВГ1 фирмы Миландр.
 */
typedef volatile unsigned int eth_reg_t;

typedef	struct {
	eth_reg_t MAC_CTRL;		// Управление MAC-уровнем контроллера
	eth_reg_t MIN_FRAME;		// Минимальная допустимая длина пакета
	eth_reg_t MAX_FRAME;		// Максимальная допустимая длина пакета
	eth_reg_t COLL_CONFIG;		// Управление обработкой коллизий
	eth_reg_t IPGT;			// Задание межпакетного интервала
	eth_reg_t MAC_ADDR [3];		// Задание MAC-адреса контроллера
	eth_reg_t HASH [4];		// HASH-таблица для расширенной фильтрации
	eth_reg_t INT_MASK;		// Маскирование прерываний
	eth_reg_t INT_SRC;		// Флаги прерываний
	eth_reg_t PHY_CTRL;		// Управление PHY-уровнем
	eth_reg_t PHY_STAT;		// Состояние PHY-уровня

	eth_reg_t RXBF_HEAD;		// Голова буфера приёма
	eth_reg_t RXBF_TAIL;		// Хвост буфера приёма
	eth_reg_t unused [2];		// (зарезервировано)

	eth_reg_t STAT_RX_ALL;		// Счетчик входящих пакетов
	eth_reg_t STAT_RX_OK;		// Счетчик успешно принятых
	eth_reg_t STAT_RX_OVF;		// Счетчик переполнениq буфера приёма
	eth_reg_t STAT_RX_LOST;		// Счетчик потеряных из-за неготовности
	eth_reg_t STAT_TX_ALL;		// Счетчик исходящих пакетов
	eth_reg_t STAT_TX_OK;		// Счетчик успешно отосланных пакетов

	eth_reg_t BASE_RXBF;		// Начало буфера приемника
	eth_reg_t BASE_TXBF;		// Начало буфера передатчика
	eth_reg_t BASE_RXBD;		// Начало дескриторов приемника
	eth_reg_t BASE_TXBD;		// Начало дескриторов передатчика
	eth_reg_t BASE_REG;		// Начало области регистров
	eth_reg_t GCTRL;		// Управления внешними интерфейсами
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
