#ifndef _SPW_H_
#define _SPW_H_

#include <kernel/uos.h>
#include <elvees/mcb-03.h>

// Стартовая скорость
#ifndef SPW_START_SPEED
#define SPW_START_SPEED         10
#endif

// Размер буфера принимаемых дескрипторов RX DESC DMA
#ifndef SPW_RX_DESC_BUFSZ
#define SPW_RX_DESC_BUFSZ       384
#endif

#define SPW_ONE_BUFFER_DESC_CNT (SPW_RX_DESC_BUFSZ / 48)

// Размер буфера для принятых данных RX DATA DMA
#ifndef SPW_RX_DATA_BUFSZ
#define SPW_RX_DATA_BUFSZ       4096
#endif

// Размер буфера для принятых данных RX DATA DMA
#ifndef SPW_TX_DATA_BUFSZ
#define SPW_TX_DATA_BUFSZ       2048
#endif

#ifndef SPW1_BASE_OFFSET
#define SPW1_BASE_OFFSET        0
#endif

#ifndef SPW2_BASE_OFFSET
#define SPW2_BASE_OFFSET        0
#endif

#ifndef SPW3_BASE_OFFSET
#define SPW3_BASE_OFFSET        0
#endif

// Ошибки
#define SPW_RX_QUEUE_EMPTY      -1      // Приёмная очередь пуста (возращается только в неблокирующем режиме)
#define SPW_TX_BUSY             -2      // Передатчик занят (возвращается только в неблокирующем режиме)
#define SPW_NOT_CONNECTED       -3      // Нет соединения (возвращается только в неблокирующем режиме)
#define SPW_PACKET_TOO_BIG      -4      // Пользователь пытается передать слишком большой пакет
#define SPW_SMALL_BUF           -5      // Пользователь запросил приём в буфер меньшего размера, чем принятый пакет

// Доступ к регистрам любого канала SWIC DMA по его адресу
#define SWIC_DMA_CSR(addr)      ((addr) + 0x0)     // Регистр управления и состояния канала
#define SWIC_DMA_CP(addr)       ((addr) + 0x4)     // Регистр указателя цепочки канала
#define SWIC_DMA_IR(addr)       ((addr) + 0x8)     // Индексный регистр внешней памяти канала
#define SWIC_DMA_RUN(addr)      ((addr) + 0xC)     // Псевдорегистр управления состоянием бита RUN

// Формат дескриптора пакета
typedef struct __attribute__((__packed__)) _spw_descriptor_t
{
        unsigned        size   : 25;    // Размер данных
        unsigned        unused : 4;     // Не используется
        unsigned        type   : 2;     // Тип конца пакета: EOP или EEP
        unsigned        valid  : 1;     // Признак заполнения дескриптора
                                        // действительными данными
        unsigned        padding;        // Дополнительные неиспользуемые 4 байта,
                                        // т.к. DMA передаёт 8-байтовыми словами.
} spw_descriptor_t;

// Один буфер двойной буферизации
typedef struct _io_buffer {
        char *          buf;            // Непосредственно буфер (должен быть выравнен на границу 8 байт для
                                        // нормальной работы DMA)
        char *          reader_p;       // Текущий указатель, с которого производится чтение
        int             empty;          // Признак пустого буфера
        unsigned        chain_addr;     // Адрес цепочки инициализации (физ.), если она используется, иначе 0
} one_buffer;

// Двойной буфер
typedef struct _double_buffer {
        one_buffer      half [2];       // Две половины буфера
        int             size;           // Размер одной половины
        int             reader_idx;     // Индекс читателя
        int             dma_idx;        // Индекс DMA
        int             dma_chan;       // Канал DMA
} double_buffer;

// Элемент цепочки DMA
typedef struct __attribute__((__packed__)) _dma_params_t 
{
        uint32_t        zero;           // Неиспользуемое поле
        uint32_t        ir;             // Значение, загружаемое в IR
        uint32_t        cp;             // Значение, загружаемое в CP
        uint32_t        csr;            // Значение, загружаемое в CSR
} dma_params_t;

struct _spw_t {
        mutex_t                         st_lock;                // Мьютекс
        mutex_t                         rx_desc_lock;           // Мьютекс потока приёма дескрипторов
        mutex_t                         rx_data_lock;           // Мьютекс потока приёма данных
        mutex_t                         tx_data_lock;           // Мьютекс потока выдачи данных
        //mutex_t                         err_lock;               // Мьютекс передающего потока
        int                             port;                   // Номер канала spacewire, считая от 0
        unsigned                        speed;                  // Рабочая скорость канала
        spw_descriptor_t *              tx_desc;                // Указатель на буфер с дескриптором передаваемого пакета
        char *                          tx_data_buffer;         // Указатель на буфер с данными передаваемого пакета
        double_buffer                   rx_desc_buf;            // Двойной буфер приёма дескрипторов
        double_buffer                   rx_data_buf;            // Двойной буфер приёма данных
        dma_params_t *                  rx_desc_chain [2];      // Цепочки DMA для приёма дескрипторов
        
#ifdef SPW_USE_MEM_DMA
        int                             mem_dma_chan;           // Номер канала MEM DMA
#endif

// Буфер для дескриптора передаваемого пакета
#ifndef SPW_TX_DESC_ADDR
        spw_descriptor_t                txdescbuf __attribute__ ((aligned (8)));
#endif

// Буфер для передаваемого пакета
#ifndef SPW_TX_DATA_ADDR
        char                            txbuf [SPW_TX_DATA_BUFSZ] __attribute__ ((aligned (8)));
#endif

// Буфер для принятых дескрипторов RX DESC DMA
#ifndef SPW_RX_DESC_ADDR
        spw_descriptor_t                rxdescbuf [2][SPW_ONE_BUFFER_DESC_CNT] __attribute__ ((aligned (8)));
        dma_params_t                    rxdescchain [2][SPW_ONE_BUFFER_DESC_CNT] __attribute__ ((aligned (8)));
#endif

// Буфер для принятых данных RX DATA DMA
#ifndef SPW_RX_DATA_ADDR
        char                            rxdatabuf [2][SPW_RX_DATA_BUFSZ / 2] __attribute__ ((aligned (8)));
#endif
        
        // Статистика
        unsigned                        rx_eop;         // Количество принятых пакетов с EOP
        unsigned                        rx_eep;         // Количество принятых пакетов с EEP
        unsigned                        rx_bytes;       // Количество принятых байт
        unsigned                        tx_packets;     // Количество переданных пакетов
        unsigned                        tx_bytes;       // Количество переданных байт
        unsigned                        txdma_waits;    // Количество ожиданий освобождения DMA передатчика
        
        mcb_intr_handler_t              mcb_irq_rx_data;
        mcb_intr_handler_t              mcb_irq_rx_desc;
        mcb_intr_handler_t              mcb_irq_tx_data;
        mcb_intr_handler_t              mcb_irq_connected;
};
typedef struct _spw_t spw_t;

//
// Инициализация одного интерфейса spacewire
//      u               указатель на структуру, описывающую интерфейс
//      port            номер канала spacewire, с которым нужно связать структуру u
//      mbit_per_sec    требуемая рабочая скорость в Мбит/с
//
void spw_init (spw_t *u, int port, unsigned mbit_per_sec);

//
// Получение принятого пакета
//      u               указатель на структуру, описывающую интерфейс
//      buf             указатель на буфер, в который будет положен принятый пакет
//      size            размер этого буфера
//      nonblock        неблокирующий режим
// Возвращает:          количество принятых байт (длина пакета) или код ошибки.
//                      В неблокирующем режиме при отсутствии принятых пакетов
//                      возвращает 0.
//
int spw_input (spw_t *u, void *buf, int size, int nonblock);

//
// Выдача пакета
//      u               указатель на структуру, описывающую интерфейс
//      buf             указатель на буфер с передаваемым пакетом
//      size            размер этого буфера
//      nonblock        неблокирующий режим
// Возвращает:          количество переданных байт (длина пакета) или код ошибки.
//                      В неблокирующем режиме при невозможности передать пакет
//                      возвращает 0.
//
int spw_output (spw_t *u, const void *buf, int size, int nonblock);

//
// Установка скорости передачи
//      u               указатель на структуру, описывающую интерфейс
//      mbit_per_sec    устанавливаемая рабочая скорость в Мбит/с
//
void spw_set_tx_speed (spw_t *u, unsigned mbit_per_sec);

//
// Чтение скорости передачи
//      u               указатель на структуру, описывающую интерфейс
// Возвращает:          текущую скорость передачи
//
unsigned spw_get_tx_speed (spw_t *u);

//
// Чтение скорости передачи
//      u               указатель на структуру, описывающую интерфейс
// Возвращает:          текущую скорость приёма
//
unsigned spw_get_rx_speed (spw_t *u);


#endif /* _SPW_H_ */

