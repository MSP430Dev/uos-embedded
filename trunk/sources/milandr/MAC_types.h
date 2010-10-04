//
// Определения структур управления и данных, используемых в контроллере ЛВС.
//
// Project: Ethernet MAC
// Author: Stanislav V. Afanas'ev
// Company: Milandr
// File: MAC_types.h
// Version: 1.2
// Date: 22.11.07
//
#ifndef __MAC_TYPES__
#define __MAC_TYPES__

#ifdef __TMS320C50__
#   define __BIT_ORDER_REVERSE__
#endif

typedef struct PACK_state {
	unsigned num;			// Номер дескритора
	unsigned stat;			// Словосостояние дескриптора
} t_PACK_state;

typedef struct TX_BD_ctrl {
#ifdef __BIT_ORDER_REVERSE__
	volatile unsigned CS : 1;	// Индикатор потери несущей во время передачи пакета
	volatile unsigned UR : 1;	// Индикатор наличия Late Collision
	volatile unsigned LC : 1;	// Underrun
	volatile unsigned RL : 1;	// Индикатор исчерпания разрешенного кол-ва повторения в случае
					// неуспешной отправки исходящего пакета
	volatile unsigned RTRY : 4;	// Счетчик кол-ва попыток передачи исходящего пакета
	unsigned CRC_DIS : 1;		// Отключение дополнения пакетов полем CRC32
	unsigned PAD_DIS : 1;		// Отключение дополнения пакетов длиной менее MinFrame до
					// минимальной длины PAD-ами (1-дополнение не производится,
					// 0-дополнения производится)
	unsigned PRE_DIS : 1;		// Отключение передачи преамбулы (1-передача преамбулы отключена,
					// 0-стандартный пакет)
	unsigned reserved : 2;		// (зарезервировано)
	unsigned IRQ_EN : 1;		// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
	unsigned WRAP : 1;		// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
	volatile unsigned RDY : 1;	// Индикатор состояния исходящего пакета (1-готов, 0-отправлен/не заполнен)
#else
	volatile unsigned RDY : 1;	// Индикатор состояния исходящего пакета (1-готов, 0-отправлен/не заполнен)
	unsigned WRAP : 1;		// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
	unsigned IRQ_EN : 1;		// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
	unsigned reserved : 2;		// (зарезервировано)
	unsigned PRE_DIS : 1;		// Отключение передачи преамбулы (1-передача преамбулы отключена, 0-стандартный пакет)
	unsigned PAD_DIS : 1;		// Отключение дополнения пакетов длиной менее MinFrame до минимальной длины PAD-ами (1-дополнение не производится, 0-дополнение производится)
	unsigned CRC_DIS : 1;		// Отключение дополнения пакетов полем CRC32
	volatile unsigned RTRY : 4;	// Счетчик кол-ва попыток передачи исходящего пакета
	volatile unsigned RL : 1;	// Индикатор исчерпания разрешенного кол-ва повторения в случае неуспешной отправки исходящего пакета
	volatile unsigned LC : 1;	// (зарезервировано)
	volatile unsigned UR : 1;	// Индикатор наличия Late Collision
	volatile unsigned CS : 1;	// Индикатор потери несущей во время передачи пакета
#endif
} t_TX_BD_ctrl;

typedef struct RX_BD_ctrl {
#ifdef __BIT_ORDER_REVERSE__
	volatile unsigned OR : 1;	// Индикатор переполнения буфера ПРМ при приеме пакета
	volatile unsigned LC : 1;	// Индикатор наличия Late Collision
	volatile unsigned SMB_ERR : 1;	// Индикатор наличия ошибки в данных при приеме пакета
	volatile unsigned CRC_ERR : 1;	// Индикатор наличия ошибки CRC при приеме пакета
	volatile unsigned EF : 1;	// Индикатор наличия ошибок при приеме пакета (сводный бит по ошибкам см. ниже)
	volatile unsigned SF : 1;	// Индикатор приема пакета длиной менее MinFrame
	volatile unsigned LF : 1;	// Индикатор приема пакета длиной более MaxFrame
	volatile unsigned CF : 1;	// Индикатор приема пакета управления
	volatile unsigned UCA : 1;	// Индикатор приема индивидуального пакета с полным совпадением MAC-адреса
	volatile unsigned MCA : 1;	// Индикатор приема широковещательного пакета
	volatile unsigned BCA : 1;	// Индикатор приема группового пакета с MAC-адресом соответствующего HASH-таблице
	unsigned reserved : 2;		// (зарезервировано)
	unsigned IRQ : 1;		// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
	unsigned WRAP : 1;		// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
	volatile unsigned EMPTY : 1;	// Индикатор состояния исходящего пакета (1-готов к приему пакета, 0-пакет принят/не готов)
#else
	volatile unsigned EMPTY : 1;	// Индикатор состояния исходящего пакета (1-готов к приему пакета, 0-пакет принят/не готов)
	unsigned WRAP : 1;		// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
	unsigned IRQ : 1;		// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
	unsigned reserved : 2;		// (зарезервировано)
	volatile unsigned BCA : 1;	// Индикатор приема группового пакета с MAC-адресом соответствующего HASH-таблице
	volatile unsigned MCA : 1;	// Индикатор приема широковещательного пакета
	volatile unsigned UCA : 1;	// Индикатор приема индивидуального пакета с полным совпадением MAC-адреса
	volatile unsigned CF : 1;	// Индикатор приема пакета управления
	volatile unsigned LF : 1;	// Индикатор приема пакета длиной более MaxFrame
	volatile unsigned SF : 1;	// Индикатор приема пакета длиной менее MinFrame
	volatile unsigned EF : 1;	// Индикатор наличия ошибок при приеме пакета (сводный бит по ошибкам см. ниже)
	volatile unsigned CRC_ERR : 1;	// Индикатор наличия ошибки CRC при приеме пакета
	volatile unsigned SMB_ERR : 1;	// Индикатор наличия ошибки в данных при приеме пакета
	volatile unsigned LC : 1;	// Индикатор наличия Late Collision
	volatile unsigned OR : 1;	// Индикатор переполнения буфера ПРМ при приеме пакета
#endif
} t_RX_BD_ctrl;

typedef union TX_ctrl {
	unsigned all;
	t_TX_BD_ctrl bit;
} t_TX_ctrl;

typedef union RX_ctrl {
	unsigned all;
	t_RX_BD_ctrl bit;
} t_RX_ctrl;

typedef struct TX_Buffer_Descriptor {
	t_TX_ctrl TX_ctrl;		// Словосостояние отправки пакета
	unsigned len;			// Полная длина пакета (включая поля SA, DA и Length) !!! в байтах !!!
	unsigned NULL;			// Старшая часть указателя данных пакета (0x0000)
	unsigned *data;			// Указатель на данные пакета (включая поля SA, DA и Length)
} t_TX_Buffer_Descriptor;

typedef struct RX_Buffer_Descriptor {
	t_RX_ctrl RX_ctrl;		// Словосостояние приема пакета
	unsigned len;			// Полная длина пакета (включая поля SA, DA и Length) !!! в байтах !!!
	unsigned NULL;			// Старшая часть указателя данных пакета (0x0000)
	unsigned *data;			// Указатель на данные пакета (включая поля SA, DA и Length)
} t_RX_Buffer_Descriptor;

typedef struct ETH_Packet {
	unsigned *DA;			// Указатель на адрес получателя пакета (DA)
	unsigned *SA;			// Указатель на адрес отправителя пакета (SA)
	unsigned len;			// Длина пакета
	unsigned *data;			// Указатель на данные пакета
} t_ETH_Pack;

typedef struct bits_MAC_CTRL {
#ifdef __BIT_ORDER_REVERSE__
	unsigned LOOPBACK_EN : 1;	// Включение тестового замыкания ПРД на ПРМ м/у уровнями MAC и PHY (активный уровень "1")
	unsigned BIG_ENDIAN : 1;	// Переключение в режим BIG ENDIAN формата данных (активный уровень "1")
	unsigned HALFD_EN : 1;		// Переключение в режим полудуплексных приема-передачи (активный уровень "1")
	unsigned BCKOF_DIS : 1;		// Отключение интервала ожидания перед повторением отправки пакета в случае коллизии (активный уровень "1")
	unsigned ERR_FRAME_EN : 1;	// Разрешение приема пакетов с ошибками (активный уровень "1")
	unsigned SHORT_FRAME_EN : 1;	// Разрешение приема пакетов длиной менее MinFrame (активный уровень "1")
	unsigned LONG_FRAME_EN : 1;	// Разрешение приема пакетов длиной более MaxFrame (активный уровень "1")
	unsigned CTRL_FRAME_EN : 1;	// Разрешение приема управляющих пакетов (активный уровень "1")
	unsigned MCA_EN : 1;		// Включение приема всех пакетов соотвествующих HASH-у (активный уровень "1")
	unsigned BCA_EN : 1;		// Включение приема всех широковещательных пакетов (активный уровень "1")
	unsigned PRO_EN : 1;		// Включение приема всех пакетов независимо от их МАСадреса (активный уровень "1")
	unsigned PAUSE_EN : 1;		// Разрешения обработки Pause Frame (активный уровень "1")
	unsigned reserved : 2;		// Переключение ПРД в режим сканирования дескрипторов, когда переход к следующему осуществляется вне зависимости от готовности (активный уровень "1")
	unsigned RX_RST : 1;		// Сброс ПРM МАС-уровня (активный уровень "1")
	unsigned TX_RST : 1;		// Сброс ПРД МАС-уровня (активный уровень "1")
#else
	unsigned TX_RST : 1;		// Сброс ПРД МАС-уровня (активный уровень "1")
	unsigned RX_RST : 1;		// Сброс ПРM МАС-уровня (активный уровень "1")
	unsigned reserved : 2;		// Переключение ПРД в режим сканирования дескрипторов, когда переход к следующему осуществляется вне зависимости от готовности (активный уровень "1")
	unsigned PAUSE_EN : 1;		// Разрешения обработки Pause Frame (активный уровень "1")
	unsigned PRO_EN : 1;		// Включение приема всех пакетов независимо от их МАСадреса (активный уровень "1")
	unsigned BCA_EN : 1;		// Включение приема всех широковещательных пакетов (активный уровень "1")
	unsigned MCA_EN : 1;		// Включение приема всех пакетов соотвествующих HASH-у (активный уровень "1")
	unsigned CTRL_FRAME_EN : 1;	// Разрешение приема управляющих пакетов (активный уровень "1")
	unsigned LONG_FRAME_EN : 1;	// Разрешение приема пакетов длиной более MaxFrame (активный уровень "1")
	unsigned SHORT_FRAME_EN : 1;	// Разрешение приема пакетов длиной менее MinFrame (активный уровень "1")
	unsigned ERR_FRAME_EN : 1;	// Разрешение приема пакетов с ошибками (активный уровень "1")
	unsigned BCKOF_DIS : 1;		// Отключение интервала ожидания перед повторением отправки пакета в случае коллизии (активный уровень "1")
	unsigned HALFD_EN : 1;		// Переключение в режим полудуплексных приема-передачи (активный уровень "1")
	unsigned BIG_ENDIAN : 1;	// Переключение в режим BIG ENDIAN формата данных (активный уровень "1")
	unsigned LOOPBACK_EN : 1;	// Включение тестового замыкания ПРД на ПРМ м/у уровнями MAC и PHY (активный уровень "1")
#endif
} t_bits_MAC_CTRL;

typedef struct bits_MAC_INT {
#ifdef __BIT_ORDER_REVERSE__
	volatile unsigned TX_BUSY : 1;	// Индикатор принятия и обслуживания пакета Pause
	volatile unsigned reserved : 4;	// (зарезервировано)
	volatile unsigned TXC : 1;	// Индикатор передачи пакета управления
	volatile unsigned TXE : 1;	// Индикатор наличия ошибок при передаче пакета
	volatile unsigned TXF : 1;	// Индикатор успешной передачи пакета
	volatile unsigned Missed_PKG : 1; // (зарезервировано)
	volatile unsigned RXS : 1;	// Индикатор приема пакета длиной менее MinFrame
	volatile unsigned RXL : 1;	// Индикатор приема пакета длиной более MaxFrame
	volatile unsigned RXBD_nREADY : 1; // (зарезервировано)
	volatile unsigned RXBF_FULL : 1; // Индикатор переполнения буфера ПРМ при приема пакета
	volatile unsigned RXC : 1;	// Индикатор приема пакета управления
	volatile unsigned RXE : 1;	// Индикатор наличия ошибок при приеме пакета
	volatile unsigned RXF : 1;	// Индикатор успешного приема пакета
#else
	volatile unsigned RXF : 1;	// Индикатор успешного приема пакета
	volatile unsigned RXE : 1;	// Индикатор наличия ошибок при приеме пакета
	volatile unsigned RXC : 1;	// Индикатор приема пакета управления
	volatile unsigned RXBF_FULL : 1; // Индикатор переполнения буфера ПРМ при приема пакета
	volatile unsigned RXBD_nREADY : 1; // (зарезервировано)
	volatile unsigned RXL : 1;	// Индикатор приема пакета длиной более MaxFrame
	volatile unsigned RXS : 1;	// Индикатор приема пакета длиной менее MinFrame
	volatile unsigned Missed_PKG : 1; // (зарезервировано)
	volatile unsigned TXF : 1;	// Индикатор успешной передачи пакета
	volatile unsigned TXE : 1;	// Индикатор наличия ошибок при передаче пакета
	volatile unsigned TXC : 1;	// Индикатор передачи пакета управления
	volatile unsigned reserved : 4; // (зарезервировано)
	volatile unsigned TX_BUSY : 1;	// Индикатор принятия и обслуживания пакета Pause
#endif
} t_bits_MAC_INT;

typedef struct MAC_PACKETLEN {
	unsigned MIN_FRAME;		// Минимальная допустимая длина пакета
	unsigned MAX_FRAME;		// Максимальная допустимая длина пакета
} t_MAC_PACKETLEN;

typedef struct bits_MAC_COLLCONF {
#ifdef __BIT_ORDER_REVERSE__
	unsigned COLLVALID : 8;		// Допустимое время появления коллизии в линии (в 4хBT = 400нс)
	unsigned MAXRET : 4;		// Максимальное кол-во поторений отправки пакета
	unsigned reserved : 4;		// (зарезервировано)
#else
	unsigned reserved : 4;		// (зарезервировано)
	unsigned MAXRET : 4;		// Максимальное кол-во поторений отправки пакета
	unsigned COLLVALID : 8;		// Допустимое время появления коллизии в линии (в 4хBT = 400нс)
#endif
} t_bits_MAC_COLLCONF;

typedef struct bits_GCTRL {
#ifdef __BIT_ORDER_REVERSE__
	unsigned GLBL_RST :1;		// Общий сброс всего контроллера (активный уровень "1")
	unsigned READ_CLR_STAT :1;	// Очистка статистики по чтению (активный уровень "1")
	unsigned SPI_RST :1;		// Сброс встроенного контроллера последовательного порта (активный уровень "1")
	unsigned reserved :2;		// (зарезервировано)
	unsigned SPI_RX_EDGE :1;	// Активный фронт ПРМ контроллера последовательного порта (1-положительный, 0-отрицательный)
	unsigned SPI_TX_EDGE :1;	// Активный фронт ПРД контроллера последовательного порта (1-положительный, 0-отрицательный)
	unsigned SPI_DIR :1;		// Порядок передачи бит (1-MSB, 0-LSB)
	unsigned SPI_FRAME_POL :1;	// Активный уровень сигнала кадровой синхронизации последовательного порта (1-положительный, 0-отрицательный)
	unsigned SPI_CLK_POL :1;	// Полярность тактового сигнала последовательного порта (1-инверсная,0-прямая)
	unsigned SPI_CLK_PHASE :1;	// Фаза тактового сигнала последовательного порта (1-инверсная, 0-прямая)
	unsigned SPI_MASTER :1;		// (зарезервировано)
	unsigned SPI_DIV :4;		// (зарезервировано)
#else
	unsigned SPI_DIV :4;		// (зарезервировано)
	unsigned SPI_MASTER :1;		// (зарезервировано)
	unsigned SPI_CLK_PHASE :1;	// Фаза тактового сигнала последовательного порта (1-инверсная, 0-прямая)
	unsigned SPI_CLK_POL :1;	// Полярность тактового сигнала последовательного порта (1-инверсная, 0-прямая)
	unsigned SPI_FRAME_POL :1;	// Активный уровень сигнала кадровой синхронизации последовательного порта (1-положительный, 0-отрицательный)
	unsigned SPI_DIR :1;		// Порядок передачи бит (1-MSB, 0-LSB)
	unsigned SPI_TX_EDGE :1;	// Активный фронт ПРД контроллера последовательного порта (1-положительный, 0-отрицательный)
	unsigned SPI_RX_EDGE :1;	// Активный фронт ПРМ контроллера последовательного порта (1-положительный, 0-отрицательный)
	unsigned reserved :2;		// (зарезервировано)
	unsigned SPI_RST :1;		// Сброс встроенного контроллера последовательного порта (активный уровень "1")
	unsigned READ_CLR_STAT :1;	// Очистка статистики по чтению (активный уровень "1")
	unsigned GLBL_RST :1;		// Общий сброс всего контроллера (активный уровень "1")
#endif
} t_bits_GCTRL;

typedef struct bits_PHY_CTRL {
#ifdef __BIT_ORDER_REVERSE__
	unsigned PHY_LB :1;		// Включение тестового замыкания ПРД на ПРМ на выходе контроллера PHY-уровня до аналоговой части ПРМ/ПРД (активный уровень "1")
	unsigned PHY_DLB :1;		// Включение тестового замыкания ПРД на ПРМ на входе контроллера PHY-уровня (активный уровень "1")
	unsigned PHY_HALFD :1;		// Включение режима полудуплесных приема-передачи (активный уровень "1")
	unsigned PHY_EARLY_DV :1;	// Включение формирвания сигнала RxDV одновременно с сигналом CRS (активный уровень "1")
	unsigned PHY_DIR :1;		// Порядок передачи битов в полубайте (1-прямой,0инверсный)
	unsigned PHY_BASE_2 :1;		// Переключение на работу с коаксиальным кабелем в режиме полудуплесных приема-передачи (1-подлючение по коаксиальному кабелю,0-подключение по витой паре)
	unsigned PHY_LINK_PERIOD :6;	// Период следования LINK импульсов -1 (диапазон 1..64 мc)
					// Период ожидания LINK импульсов вдвое больше заданного периода следования LINK импульсов
	unsigned PHY_RXEN :1;		// Разрешение работы ПРМ встроенного контроллера PHYуровня (активный уровень "1")
	unsigned PHY_TXEN :1;		// Разрешение работы ПРД встроенного контроллера PHYуровня (активный уровень "1")
	unsigned PHY_EXT_EN :1;		// Переключение на работу с внешним кнтроллером PHYуровня (активный уровень "1")
	unsigned PHY_RST :1;		// Сброс встроенного контроллера контроллера PHYуровня (активный уровень "1")
#else
	unsigned PHY_RST :1;		// Сброс встроенного контроллера контроллера PHYуровня (активный уровень "1")
	unsigned PHY_EXT_EN :1;		// Переключение на работу с внешним кнтроллером PHYуровня (активный уровень "1")
	unsigned PHY_TXEN :1;		// Разрешение работы ПРД встроенного контроллера PHYуровня (активный уровень "1")
	unsigned PHY_RXEN :1;		// Разрешение работы ПРМ встроенного контроллера PHYуровня (активный уровень "1")
	unsigned PHY_LINK_PERIOD :6;	// Период следования LINK импульсов -1 (диапазон 1..64 мc)
					// Период ожидания LINK импульсов вдвое больше заданного периода следования LINK импульсов
	unsigned PHY_BASE_2 :1;		// Переключение на работу с коаксиальным кабелем в режиме полудуплесных приема-передачи (1-подлючение по коаксиальному кабелю,0-подключение по витой паре)
	unsigned PHY_DIR :1;		// Порядок передачи битов в полубайте (1-прямой,0инверсный)
	unsigned PHY_EARLY_DV :1;	// Включение формирвания сигнала RxDV одновременно с сигналом CRS (активный уровень "1")
	unsigned PHY_HALFD :1;		// Включение режима полудуплесных приема-передачи (активный уровень "1")
	unsigned PHY_DLB :1;		// Включение тестового замыкания ПРД на ПРМ на входе контроллера PHY-уровня (активный уровень "1")
	unsigned PHY_LB :1;		// Включение тестового замыкания ПРД на ПРМ на выходе контроллера PHY-уровня до аналоговой части ПРМ/ПРД (активный уровень "1")
#endif
} t_bits_PHY_CTRL;

typedef struct bits_PHY_STAT {
#ifdef __BIT_ORDER_REVERSE__
	volatile unsigned PHY_EXT_CRS :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии несущей в линии
	volatile unsigned PHY_EXT_COL :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии коллизии в линии
	volatile unsigned reserved3 :3;		// (зарезервировано)
	volatile unsigned PHY_EXT_LINK :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии подключения в линии
	volatile unsigned reserved2 :2;		// (зарезервировано)
	volatile unsigned PHY_INT_CRS :1;	// Индикатор встроенного контроллера PHY-уровня о наличии несущей в линии
	volatile unsigned PHY_INT_COL :1;	// Индикатор встроенного контроллера PHY-уровня о наличии коллизии в линии
	volatile unsigned PHY_INT_LINK :1;	// Индикатор встроенного контроллера PHY-уровня о наличии подключения в линии
	volatile unsigned PHY_INT_POL :1;	// Индикатор встроенного контроллера PHY-уровня о смене полярности сигналов в линии ПРМ
	volatile unsigned PHY_INT_JAB :1;	// Индикатор встроенного контроллера PHY-уровня о превышении времени передачи максимально разрешенной
	volatile unsigned PHY_INT_JAM :1;	// Индикатор встроенного контроллера PHY-уровня о передаче JAM последовательности в случае коллизии
	volatile unsigned reserved1 :2;		// (зарезервировано)
#else
	volatile unsigned reserved1 :2;		// (зарезервировано)
	volatile unsigned PHY_INT_JAM :1;	// Индикатор встроенного контроллера PHY-уровня о передаче JAM последовательности в случае коллизии
	volatile unsigned PHY_INT_JAB :1;	// Индикатор встроенного контроллера PHY-уровня о превышении времени передачи максимально разрешенной
	volatile unsigned PHY_INT_POL :1;	// Индикатор встроенного контроллера PHY-уровня о смене полярности сигналов в линии ПРМ
	volatile unsigned PHY_INT_LINK :1;	// Индикатор встроенного контроллера PHY-уровня о наличии подключения в линии
	volatile unsigned PHY_INT_COL :1;	// Индикатор встроенного контроллера PHY-уровня о наличии коллизии в линии
	volatile unsigned PHY_INT_CRS :1;	// Индикатор встроенного контроллера PHY-уровня о наличии несущей в линии
	volatile unsigned reserved2 :2;		// (зарезервировано)
	volatile unsigned PHY_EXT_LINK :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии подключения в линии
	volatile unsigned reserved3 :3;		// (зарезервировано)
	volatile unsigned PHY_EXT_COL :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии коллизии в линии
	volatile unsigned PHY_EXT_CRS :1;	// Индикатор от внешнего контроллера PHY-уровня о наличии несущей в линии
#endif
} t_bits_PHY_STAT;

typedef union MAC_CTRL {
	unsigned all;
	t_bits_MAC_CTRL bit;
} t_MAC_CTRL;

typedef union MAC_COLLCONF {
	unsigned all;
	t_bits_MAC_COLLCONF field;
} t_MAC_COLLCONF;

typedef union INT_SOURCE {
	volatile unsigned all;
	t_bits_MAC_INT bit;
} t_INT_SOURCE;

typedef union INT_MASK {
	unsigned all;
	t_bits_MAC_INT bit;
} t_INT_MASK;

typedef union GCTRL {
	unsigned all;
	t_bits_GCTRL bit;
} t_GCTRL;

typedef union PHY_CTRL {
	unsigned all;
	t_bits_PHY_CTRL bit;
} t_PHY_CTRL;

typedef union PHY_STAT {
	volatile unsigned all;
	t_bits_PHY_STAT bit;
} t_PHY_STAT;

typedef struct MAC {
	t_MAC_CTRL MAC_CTRL;		// Регистр управления MAC-уровнем контроллера
	t_MAC_PACKETLEN PACKETLEN;	// Регистр управления границами допустимых длин пакетов (MinFrame и MaxFrame)
	t_MAC_COLLCONF COLLCONF;	// Регистр управления обработки коллизий
	unsigned IPGT;			// Регистр задания межпакетного интервала
	unsigned MAC_ADDR[3];		// Регистр задания MAC-адреса контроллера
	unsigned HASH[4];		// Регистр задания HASH-таблицы для расширенной фильтрации MAC-адресов
	t_INT_MASK INT_MASK;		// Регистр маскирования прерываний
	t_INT_SOURCE INT_SOURCE;	// Регистр флагов прерываний
	t_PHY_CTRL PHY_CTRL;		// Регистр управления PHY-уровнем контроллера
	t_PHY_STAT PHY_STAT;		// Регистр состояния PHY-уровня контроллера
	unsigned RXBF_HEAD;		// Регистр "головы" буфера ПРМ
	volatile unsigned RXBF_TAIL;	// Регистр "хвоста" буфера ПРМ
	unsigned TXBF_HEAD;		// Регистр "головы" буфера ПРД (зарезервировано)
	volatile unsigned TXBF_TAIL;	// Регистр "хорста" буфера ПРД (зарезервировано)
	volatile unsigned STAT_RX_ALL;	// счетчик кол-ва входящих пакетов дошедших до МАС-уровня
	volatile unsigned STAT_RX_OK;	// счетчик кол-ва успешно принятых входящих пакетов (!!! при выставленном бите ERROR_FRAME_EN -считает все пакеты)
	volatile unsigned STAT_RX_OVF;	// счетчик кол-ва входящих пакетов, вызвавших переполнение буфера ПРМ
	volatile unsigned STAT_RX_LOST;	// счетчик кол-ва входящих пакетов, потеряных из-за неготовности МАС-уровня к примему (неготовность дескриптора)
	volatile unsigned STAT_TX_ALL;	// счетчик кол-ва исходящих пакетов
	volatile unsigned STAT_TX_OK;	// счетчик кол-ва успешно отосланных исходящих пакетов
	unsigned base_MAC_RxBF;		// Указатель начала буфера ПРМ в адресном пространстве контроллера (default = 0x0000)
	unsigned base_MAC_TxBF;		// Указатель начала буфера ПРД в адресном пространстве контроллера (default = 0x1000)
	unsigned base_MAC_RxBD;		// Указатель начала таблицы дескриторов ПРМ в адресном пространстве контроллера (default = 0x0800)
	unsigned base_MAC_TxBD;		// Указатель начала таблицы дескриторов ПРМ в адресном пространстве контроллера (default = 0x1800)
	unsigned base_MAC_RG;		// Указатель расположения области регистров управления контроллера (default = 0x1FC0)
	t_GCTRL GCTRL;			// Регистр управления внешними интерфейсами контроллера
} t_MAC;

#endif // __MAC_TYPES__
