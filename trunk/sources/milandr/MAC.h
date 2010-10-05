//
// Объявления внешних функций и переменных, а также
// макроопределения констант и регистровой модели контроллера ЛВС.
//
// Project:	Ethernet MAC
// Author:	Stanislav V. Afanas'ev
// Company:	Milandr
// File:	MAC.h
// Version:	1.2
// Date:	22.11.07
//

//----------------------------------------------
// MAC address space
//
#ifndef __MAC__
#define __MAC__
#include "MAC_types.h"

#define BASE_MAC		0x60000000
#define MAC_RG			((t_MAC*) BASE_MAC)

#define BASE_TX_DESC		0x60001000
#define TX_Descriptors		((t_TX_Buffer_Descriptor*) BASE_TX_DESC)

#define BASE_RX_DESC		0x60002000
#define RX_Descriptors		((t_RX_Buffer_Descriptor*) BASE_RX_DESC)

#define BASE_TX_DATA		0x60003000
#define TX_buff			((unsigned*) BASE_TX_DATA)

#define BASE_RX_DATA		0x60004000
#define RX_buff			((unsigned*) BASE_RX_DATA)

#define DSCR_CNT		32		// 32 descriptors
#define DSCR_SIZE		8		// 32 descriptors
#define BUFF_SIZE		(4096/2)	// 4096 kb (WORD accessed)
#define BUFF_DSCR_SIZE		(DSCR_CNT*8/2)	// 8-byte length descriptors (WORD accessed)
#define RG_SET_SIZE		32		// 32 2byte registers

#define MAC_SPC_BASE		0xE000		// MC/MP Memory space size
#define MAC_SPC_SIZE		0x2000		// MC/MP Memory space size

#define MAC_RX_BUFF_BASE_ADDR	(MAC_SPC_BASE + (0*BUFF_SIZE))		// RX buffer is placed on top of MC/MP memory space
#define MAC_TX_BUFF_BASE_ADDR	(MAC_SPC_BASE + (2*BUFF_SIZE))		// TX buffer is placed on top of MC/MP memory space before RX buffer
#define MAC_RX_BD_ADDR		(MAC_RX_BUFF_BASE_ADDR + BUFF_SIZE)	// RX buffer decriptors is placed just before TX buffer
#define MAC_TX_BD_ADDR		(MAC_TX_BUFF_BASE_ADDR + BUFF_SIZE)	// TX buffer decriptors is placed just before RX buffer decriptors

#define MAC_RG_BASE_ADDR	(MAC_SPC_BASE + MAC_SPC_SIZE - 2*RG_SET_SIZE) // MAC register set is plased on top of MC/MP IO space

//----------------------------------------------
// INT_MASK/INT_SOURCE
//
#define bit_RXF			15	// Индикатор успешного приема пакета
#define bit_RXE			14	// Индикатор наличия ошибок при приеме пакета
#define bit_RXC			13	// Индикатор приема пакета управления
#define bit_RXBF_FULL		12	// Индикатор переполнения буфера ПРМ при приема пакета
#define bit_DSCR_nREADY		11	// (зарезервировано)
#define bit_RXL			10	// Индикатор приема пакета длиной более MaxFrame
#define bit_RXS			9	// Индикатор приема пакета длиной менее MinFrame
#define bit_MISSED_PKG		8	// (зарезервировано)

#define bit_TXF			7	// Индикатор успешной передачи пакета
#define bit_TXE			6	// Индикатор наличия ошибок при передаче пакета
#define bit_TXC			5	// Индикатор передачи пакета управления

#define bit_TXBF_EMPTY		4	// (зарезервировано)
#define bit_TXBF_EXHAUST 	3	// (зарезервировано)
#define bit_TX_BUSY		0	// Индикатор принятия и обслуживания пакета Pause

#define BIT_RXF			(1 << bit_RXF)
#define BIT_RXE			(1 << bit_RXE)
#define BIT_RXL			(1 << bit_RXL)
#define BIT_RXS			(1 << bit_RXS)
#define BIT_RXC			(1 << bit_RXC)
#define BIT_RXBF_FULL		(1 << bit_RXBF_FULL)
#define BIT_DSCR_nREADY		(1 << bit_DSCR_nREADY)

#define BIT_TXF			(1 << bit_TXF)
#define BIT_TXE			(1 << bit_TXE)
#define BIT_TXC			(1 << bit_TXC)
#define BIT_TXBF_EMPTY		(1 << bit_TXBF_EMPTY)
#define BIT_TXBF_EXHAUST	(1 << bit_TXBF_EXHAUST)
#define BIT_TX_BUSY		(1 << bit_TX_BUSY)

//----------------------------------------------
// MAC_CNTRL
//
#define bit_TX_RST		15	// Сброс ПРД МАС-уровня (активный уровень "1")
#define bit_RX_RST		14	// Сброс ПРM МАС-уровня (активный уровень "1")
#define bit_TX_DSCR_SCAN_EN	12	// Переключение ПРД в режим сканирования дескрипторов, когда переход к следующему осуществляется вне зависимости от готовности (активный уровень "1")
#define bit_PAUSE_EN		11	// Разрешения обработки Pause Frame (активный уровень "1")
#define bit_PRO_EN		10	// Включение приема всех пакетов независимо от их МАС-адреса (активный уровень "1")
#define bit_BCA_EN		9	// Включение приема всех широковещательных пакетов (активный уровень "1")
#define bit_MCA_EN		8	// Включение приема всех пакетов соотвествующих HASH-у (активный уровень "1")
#define bit_CTRL_FRAME_EN	7	// Разрешение приема управляющих пакетов (активный уровень "1")
#define bit_LONG_FRAME_EN	6	// Разрешение приема пакетов длиной более MaxFrame (активный уровень "1")
#define bit_SHORT_FRAME_EN	5	// Разрешение приема пакетов длиной менее MinFrame (активный уровень "1")
#define bit_ERR_FRAME_EN	4	// Разрешени приема пакетов с ошибками (активный уровень "1")
#define bit_BCKOF_DIS		3	// Отключение интервала ожидания перед повторением отправки пакета в случае коллизии (активный уровень "1")
#define bit_HALFD_EN		2	// Переключение в режим полудуплексных приема-передачи (активный уровень "1")
#define bit_BIG_ENDIAN		1	// Переключение в режим BIG ENDIAN формата данных (активный уровень "1")
#define bit_LB_EN		0	// Включение тестового замыкания ПРД на ПРМ м/у уровнями MAC и PHY (активный уровень "1")

#define BIT_TX_RST		(1 << bit_TX_RST)
#define BIT_RX_RST		(1 << bit_RX_RST)
#define BIT_TX_DSCR_SCAN_EN	(1 << bit_TX_DSCR_SCAN_EN)
#define BIT_PAUSE_EN		(1 << bit_PAUSE_EN)
#define BIT_PRO_EN		(1 << bit_PRO_EN)
#define BIT_BCA_EN		(1 << bit_BCA_EN)
#define BIT_MCA_EN		(1 << bit_MCA_EN)
#define BIT_CTRL_FRAME_EN	(1 << bit_CTRL_FRAME_EN)
#define BIT_LONG_FRAME_EN	(1 << bit_LONG_FRAME_EN)
#define BIT_SHORT_FRAME_EN	(1 << bit_SHORT_FRAME_EN)
#define BIT_ERR_FRAME_EN	(1 << bit_ERR_FRAME_EN)
#define BIT_HALFD_EN		(1 << bit_HALFD_EN)
#define BIT_BCKOF_DIS		(1 << bit_BCKOF_DIS)
#define BIT_BIG_ENDIAN		(1 << bit_BIG_ENDIAN)
#define BIT_LB_EN		(1 << bit_LB_EN)

//----------------------------------------------
// TX_DESCRIPTOR_CTRL
//
#define bit_TX_DSCR_RDY		15	// Индикатор состояния исходящего пакета (1-готов, 0-отправлен/не заполнен)
#define bit_TX_DSCR_WRAP	14	// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
#define bit_TX_DSCR_IRQ_EN	13	// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
#define bit_TX_DSCR_PRE_DIS	10	// Отключение передачи преамбулы (1-передача преамбулы отключена, 0-стандартный пакет)
#define bit_TX_DSCR_PAD_DIS	9	// Отключение дополнения пакетов длиной менее MinFrame до минимальной длины PAD-ами (1-дополнение не производится, дополнения производится)
#define bit_TX_DSCR_CRC_DIS	8	// Отключение дополнения пакетов полем CRC32
#define bit_TX_DSCR_RL		3	// Индикатор исчерпания разрешенного кол-ва повторения в случае неуспешной отправки исходящего пакета
#define bit_TX_DSCR_LC		2	// Индикатор наличия Late Collision
#define bit_TX_DSCR_UR		1	// Underrun
#define bit_TX_DSCR_CS		0	// Индикатор потери несущей во время передачи пакета

#define BIT_TX_DSCR_RDY		(1 << bit_TX_DSCR_RDY)
#define BIT_TX_DSCR_WRAP	(1 << bit_TX_DSCR_WRAP)
#define BIT_TX_DSCR_IRQ_EN	(1 << bit_TX_DSCR_IRQ_EN)
#define BIT_TX_DSCR_PRE_DIS	(1 << bit_TX_DSCR_PRE_DIS)
#define BIT_TX_DSCR_PAD_DIS	(1 << bit_TX_DSCR_PAD_DIS)
#define BIT_TX_DSCR_CRC_DIS	(1 << bit_TX_DSCR_CRC_DIS)
#define BIT_TX_DSCR_RL		(1 << bit_TX_DSCR_RL)
#define BIT_TX_DSCR_UR		(1 << bit_TX_DSCR_UR)
#define BIT_TX_DSCR_LC		(1 << bit_TX_DSCR_LC)
#define BIT_TX_DSCR_CS		(1 << bit_TX_DSCR_CS)

#define MSK_TX_DSCR_RTRY	0x00F0	// Retry count

//----------------------------------------------
// RX_DESCRIPTOR_CTRL
//
#define bit_RX_DSCR_RDY		15	// Индикатор состояния исходящего пакета (1-готов к приему пакета, 0-пакет принят/не готов)
#define bit_RX_DSCR_WRAP	14	// Индикатор последнего дескриптора в таблице (1-переход к дескриптору #0)
#define bit_RX_DSCR_IRQ_EN	13	// Разрешение формирования прерываний по передаче (1-вкл., 0-выкл.)
#define bit_RX_DSCR_nRDY	11	// (зарезервировано)
#define bit_RX_DSCR_MCA		10	// Индикатор приема группового пакета с MAC-адресом соответствующего HASH-таблице
#define bit_RX_DSCR_BCA		9	// Индикатор приема широковещательного пакета
#define bit_RX_DSCR_UCA		8	// Индикатор приема индивидуального пакета с полным совпадением MAC-адреса
#define bit_RX_DSCR_CF		7	// Индикатор приема пакета управления
#define bit_RX_DSCR_LF		6	// Индикатор приема пакета длиной более MaxFrame
#define bit_RX_DSCR_SF		5	// Индикатор приема пакета длиной менее MinFrame
#define bit_RX_DSCR_EF		4	// Индикатор наличия ошибок при приеме пакета (сводный бит по ошибкам см. ниже)
#define bit_RX_DSCR_CRC_ERR	3	// Индикатор наличия ошибки CRC при приеме пакета
#define bit_RX_DSCR_SMB_ERR	2	// Индикатор наличия ошибки в данных при приеме пакета
#define bit_RX_DSCR_LC		1	// Индикатор наличия Late Collision
#define bit_RX_DSCR_OR		0	// Индикатор переполнения буфера ПРМ при приеме пакета

#define BIT_RX_DSCR_RDY		(1 << bit_RX_DSCR_RDY)
#define BIT_RX_DSCR_WRAP	(1 << bit_RX_DSCR_WRAP)
#define BIT_RX_DSCR_IRQ_EN	(1 << bit_RX_DSCR_IRQ_EN)
#define BIT_RX_DSCR_nRDY	(1 << bit_RX_DSCR_nRDY)
#define BIT_RX_DSCR_SMB_ERR	(1 << bit_RX_DSCR_SMB_ERR)
#define BIT_RX_DSCR_CRC_ERR	(1 << bit_RX_DSCR_CRC_ERR)
#define BIT_RX_DSCR_LC		(1 << bit_RX_DSCR_LC)
#define BIT_RX_DSCR_OR		(1 << bit_RX_DSCR_OR)
#define BIT_RX_DSCR_MCA		(1 << bit_RX_DSCR_MCA)
#define BIT_RX_DSCR_BCA		(1 << bit_RX_DSCR_BCA)
#define BIT_RX_DSCR_UCA		(1 << bit_RX_DSCR_UCA)
#define BIT_RX_DSCR_CF		(1 << bit_RX_DSCR_CF)
#define BIT_RX_DSCR_LF		(1 << bit_RX_DSCR_LF)
#define BIT_RX_DSCR_SF		(1 << bit_RX_DSCR_SF)
#define BIT_RX_DSCR_EF		(1 << bit_RX_DSCR_EF)

#define bit_DSCR_RDY		bit_TX_DSCR_RDY
#define BIT_DSCR_RDY		(1 << bit_TX_DSCR_RDY)

//----------------------------------------------
// COLCONFIG
//
#define MSK_RetriesLimit	0x0F00
#define MSK_CollisionWindow	0x00FF

//----------------------------------------------
// PHY_CNTRL
//
#define bit_PHY_RST		15	// Сброс встроенного контроллера контроллера PHY-уровня (активный уровень "1")
#define bit_PHY_EXT_EN		14	// Переключение на работу с внешним кнтроллером PHY-уровня (активный уровень "1")
#define bit_PHY_TXEN		13	// Разрешение работы ПРД встроенного контроллера PHY-уровня (активный уровень "1")
#define bit_PHY_RXEN		12	// Разрешение работы ПРМ встроенного контроллера PHY-уровня (активный уровень "1")
#define bit_PHY_BASE_2		5	// Переключение на работу с коаксиальным кабелем в режиме полудуплесных приема-передачи (1-подлючение по коаксиальному кабелю,0-подключение по витой паре)
#define bit_PHY_DIR		4	// Порядок передачи битов в полубайте (1-прямой,0-инверсный)
#define bit_PHY_EARLY_DV	3	// Включение формирвания сигнала RxDV одновременно с сигналом CRS (активный уровень "1")
#define bit_PHY_HALFD		2	// Включение режима полудуплесных приема-передачи (активный уровень "1")
#define bit_PHY_DLB		1	// Включение тестового замыкания ПРД на ПРМ на входе контроллера PHY-уровня (активный уровень "1")
#define bit_PHY_LB		0	// Включение тестового замыкания ПРД на ПРМ на выходе контроллера PHY-уровня до аналоговой части ПРМ/ПРД (активный уровень "1")

#define BIT_PHY_RST		(1 << bit_PHY_RST)
#define BIT_PHY_EXT_EN		(1 << bit_PHY_EXT_EN)
#define BIT_PHY_TXEN		(1 << bit_PHY_TXEN)
#define BIT_PHY_RXEN		(1 << bit_PHY_RXEN)
#define BIT_PHY_DIR		(1 << bit_PHY_DIR)
#define BIT_PHY_BASE_2		(1 << bit_PHY_BASE_2)
#define BIT_PHY_ERLY_DV		(1 << bit_PHY_EARLY_DV)
#define BIT_PHY_HALFD		(1 << bit_PHY_HALFD)
#define BIT_PHY_DLB		(1 << bit_PHY_DLB)
#define BIT_PHY_LB		(1 << bit_PHY_LB)

#define MSK_PHY_LINK_PERIOD	0x0FC0

//----------------------------------------------
// PHY_STAT
//
#define bit_PHY_INT_JAM		13	// Индикатор встроенного контроллера PHY-уровня о передаче JAM последовательности в случае коллизии
#define bit_PHY_INT_JAB		12	// Индикатор встроенного контроллера PHY-уровня о превышении времени передачи максимально разрешенной
#define bit_PHY_INT_POL		11	// Индикатор встроенного контроллера PHY-уровня о смене полярности сигналов в линии ПРМ
#define bit_PHY_INT_LINK	10	// Индикатор встроенного контроллера PHY-уровня о наличии подключения в линии
#define bit_PHY_INT_COL		9	// Индикатор встроенного контроллера PHY-уровня о наличии коллизии в линии
#define bit_PHY_INT_CRS		8	// Индикатор встроенного контроллера PHY-уровня о наличии несущей в линии
#define bit_PHY_EXT_LINK	5	// Индикатор от внешнего контроллера PHY-уровня о наличии подключения в линии
#define bit_PHY_EXT_COL		1	// Индикатор от внешнего контроллера PHY-уровня о наличии коллизии в линии
#define bit_PHY_EXT_CRS		0	// Индикатор от внешнего контроллера PHY-уровня о наличии несущей в линии

#define BIT_PHY_INT_JAM		(1 << bit_PHY_INT_JAM)
#define BIT_PHY_INT_JAB		(1 << bit_PHY_INT_JAB)
#define BIT_PHY_INT_POL		(1 << bit_PHY_INT_POL)
#define BIT_PHY_INT_LINK	(1 << bit_PHY_INT_LINK)
#define BIT_PHY_INT_COL		(1 << bit_PHY_INT_COL)
#define BIT_PHY_INT_CRS		(1 << bit_PHY_INT_CRS)
#define BIT_PHY_EXT_LINK	(1 << bit_PHY_EXT_LINK)
#define BIT_PHY_EXT_COL		(1 << bit_PHY_EXT_COL)
#define BIT_PHY_EXT_CRS		(1 << bit_PHY_EXT_CRS)

//----------------------------------------------
// GCTRL
//
// PPI_CTRL
//
#define bit_GLBL_RST		15	// Общий сброс всего контроллера (активный уровень "1")
#define bit_READ_CLR_STAT	14	// Очистка статистики по чтению (активный уровень "1")
#define bit_SPI_RST		13	// Сброс встроенного контроллера последовательного порта (активный уровень "1")
#define bit_RG_inMEM		12	// (зарезервировано)

#define BIT_GLBL_RST		(1 << bit_GLBL_RST)
#define BIT_READ_CLR_STAT	(1 << bit_READ_CLR_STAT)
#define BIT_SPI_RST		(1 << bit_SPI_RST)
#define BIT_RG_inMEM		(1 << bit_RG_inMEM)

//
// SPI CTRL
//
#define bit_SPI_RX_EDGE		10	// Активный фронт ПРМ контроллера последовательного порта (1-положительный,0-отрицательный)
#define bit_SPI_TX_EDGE		9	// Активный фронт ПРД контроллера последовательного порта (1-положительный,0-отрицательный)
#define bit_SPI_DIR		8	// Порядок передачи бит (1-MSB,0-LSB)
#define bit_SPI_FRAME_POL	7	// Активный уровень сигнала кадровой синхронизации последовательного порта  (1-положительный,0-отрицательный)
#define bit_SPI_CLK_POL		6	// Полярность тактового сигнала последовательного порта  (1-инверсная,0-прямая)
#define bit_SPI_CLK_PHASE	5	// Фаза тактового сигнала последовательного порта  (1-инверсная,0-прямая)
#define bit_SPI_MASTER		4	// (зарезервировано)

#define BIT_SPI_RX_EDGE		(1 << bit_SPI_RX_EDGE)
#define BIT_SPI_TX_EDGE		(1 << bit_SPI_TX_EDGE)
#define BIT_SPI_DIR		(1 << bit_SPI_DIR)
#define BIT_SPI_FPOL		(1 << bit_SPI_FRAME_POL)
#define BIT_SPI_CPOL		(1 << bit_SPI_CLK_POL)
#define BIT_SPI_CPHASE		(1 << bit_SPI_CLK_PHASE)
#define BIT_SPI_MASTER		(1 << bit_SPI_MASTER)
#define MSK_SPI_DIV		0x0007

//----------------------------------------------
// DEFAULT VALUES
//
#define sizeof_RxBF		2048
#define sizeof_TxBF		2048
#define sizeof_RxBD		(32*4)
#define sizeof_TxBD		(32*4)
#define sizeof_RG		32	// 32 regs 2 bytes each

#define depth_RxBF		11	// = log2(size_BD)
#define depth_TxBF		11	// = log2(size_BD)
#define depth_RxBD		7	// = log2(size_BD)
#define depth_TxBD		7	// = log2(size_BD)
#define depth_RG		5	// = log2(size_BD)

#define mask_RxBF		(sizeof_RxBF - 1)	// 0x07FF
#define mask_TxBF		(sizeof_TxBF - 1)	// 0x07FF
#define mask_RxBD		(sizeof_RxBD - 1)	// 0x007F
#define mask_TxBD		(sizeof_TxBD - 1)	// 0x007F
#define mask_RG			(sizeof_RG - 1)		// 0x001F

#define dflt_SPI_DIV		(0x2 & MSK_SPI_DIV)
#define dflt_SPI_CTRL		(BIT_SPI_FPOL | BIT_SPI_DIR | dflt_SPI_DIV | BIT_SPI_TX_EDGE)
#define dflt_PPI_CTRL		(BIT_READ_CLR_STAT | BIT_RG_inMEM)

#define dflt_GCTRL		(dflt_PPI_CTRL | dflt_SPI_CTRL)
#define dflt_addr_RG		(MAC_SPC_BASE + MAC_SPC_SIZE -2*sizeof_RG)
#define dflt_addr_Rx_BF		(MAC_SPC_BASE + 0*sizeof_RxBF)
#define dflt_addr_Tx_BF		(MAC_SPC_BASE + 2*sizeof_TxBF)
#define dflt_addr_Rx_BD		(dflt_addr_Rx_BF + sizeof_RxBF)
#define dflt_addr_Tx_BD		(dflt_addr_Tx_BF + sizeof_TxBF)

#define dflt_MAC_CTRL		(BIT_TX_RST | BIT_RX_RST | BIT_CTRL_FRAME_EN | BIT_SHORT_FRAME_EN)
//BIT_TX_RST | BIT_RX_RST | BIT_READ_CLR_STAT | BIT_BIG_ENDIAN
//BIT_PRO_EN | BIT_BCA_EN | BIT_MCA_EN | BIT_CTRL_FRAME_EN | BIT_LONG_FRAME_EN | BIT_SHORT_FRAME_EN | BIT_ERR_FRAME_EN
//BIT_HALFD_EN | BIT_BCKOF_DIS | BIT_LB_EN

#define dflt_MinFrame		0x0040	// 64 bytes
#define dflt_MaxFrame		0x0600	// 1500 bytes

#define dflt_MAC_ADDR_H		0x0123	// MAC: 01_23_45_67_89_AB
#define dflt_MAC_ADDR_M		0x4567
#define dflt_MAC_ADDR_T		0x89ab

#define dflt_CollConfig		0x0F40
#define dflt_IPGTx		0x0060
#define dflt_INT_MASK		0x0000	// All interrupts disabled

#define dflt_HASH0		0x0000	// HAH is empty
#define dflt_HASH1		0x0000
#define dflt_HASH2		0x0000
#define dflt_HASH3		0x8000

#define dflt_PHY_LINK_PERIOD	(0x0B << 6)
#define dflt_PHY_CTRL		(BIT_PHY_RST | BIT_PHY_TXEN | BIT_PHY_RXEN | \
				 BIT_PHY_DIR | dflt_PHY_LINK_PERIOD)
//BIT_PHY_RST | BIT_PHY_EXT_EN | BIT_PHY_ERLY_DV | BIT_PHY_HALFD | BIT_PHY_DLB | BIT_PHY_LB

//----------------------------------------------
// SPI PROTOCOL defines
//
// SPI SubCMD bits
//
#define bit_BigEndian		0
#define bit_inMEM		1
#define bit_size0		2
#define bit_size1		3

#define BIG_ENDN		(1 << bit_BigEndian)
#define MEM_WRITE		(1 << bit_inMEM)

#define SIZE8			0x00
#define SIZE16			0x04
#define SIZE24			0x08
#define SIZE32			0x0C

#define SPI_SOP			0xDB
#define SPI_dSOP		0xDD
#define SPI_DSOP		0xDD
#define SPI_FILL		0x00
//#define SPI_FILL		SPI_SOP

#define cmd_RESET		0x0F
#define cmd_SelectSPI		0x01
#define cmd_SelectPPI		0x02
#define cmd_Write		0x03
#define cmd_Read		0x04

#define SPI_CMD_RESET		(cmd_RESET     << 4)
#define SPI_CMD_SelectSPI	(cmd_SelectSPI << 4)
#define SPI_CMD_SelectPPI	(cmd_SelectPPI << 4)
#define SPI_CMD_WRITE		(cmd_Write     << 4)
#define SPI_CMD_READ		(cmd_Read      << 4)

//
// SPI Errors
//
#define err_NoErr		0x00
#define err_ErrCMD		0x01
#define err_DblSOPinData	0x02
#define err_SnglSOPinData	0x03
#define err_TransferAbort	0x04

//----------------------------------------------
// FUNCTION DECLARATIONS
//
void	MAC_reset (void);
void	MAC_MEM_clear (void);
void	MAC_MEM_test (void);
void	MAC_MEM_CELL_test (unsigned int *addr, unsigned int value);
void	MAC_init (void);
int	Send_ETH_Pack (unsigned int *Dscr_Num, t_ETH_Pack *PKG, unsigned int PARAM);
int	Receive_ETH_Pack (unsigned int *Dscr_Num, t_ETH_Pack *PKG, unsigned int PARAM);
int	chk_Send_OK (unsigned int *Dscr_Num);
int	chk_Receive_Ready (unsigned int *Dscr_Num);

#endif // __MAC__
