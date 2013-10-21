#include <runtime/lib.h>
#include <elvees/mcb-03.h>
#include <elvees/spw-mcb.h>

#define EOP     1
#define EEP     2

#define SWIC_RESET              0x00000079
#define SWIC_START              0x00000101
#define SWIC_RESET_ALL_IRQS     0x0001d00f

// Макросы для вычисления номеров прерываний
#define IRQ_SW0_LINK         (1 << 0)
#define IRQ_SW0_ERR          (1 << 1)
#define IRQ_SW0_TIME         (1 << 2)
#define IRQ_SW0_RX_DESC      (1 << 12)
#define IRQ_SW0_RX_DATA      (1 << 13)
#define IRQ_SW0_TX_DESC      (1 << 14)
#define IRQ_SW0_TX_DATA      (1 << 15)

#define IRQ_SHIFT           3
#define spw_err_irq(port_id)        (IRQ_SW0_ERR << ((port_id) * 3))
#define spw_time_irq(port_id)       (IRQ_SW0_TIME << ((port_id) * 3))
#define spw_link_irq(port_id)       (IRQ_SW0_LINK << ((port_id) * 3))
#define spw_rx_data_irq(port_id)    (IRQ_SW0_RX_DATA << ((port_id) * 4))
#define spw_rx_desc_irq(port_id)    (IRQ_SW0_RX_DESC << ((port_id) * 4))
#define spw_tx_data_irq(port_id)    (IRQ_SW0_TX_DATA << ((port_id) * 4))
#define spw_tx_desc_irq(port_id)    (IRQ_SW0_TX_DESC << ((port_id) * 4))

#define DEBUG 0

#if DEBUG
#   define PDEBUG(fmt, args...) debug_printf(fmt, ##args)
#else
#   define PDEBUG(fmt, args...)
#endif
#define PDEBUGG(fmt, args...)


static inline int spw_connected (spw_t *u)
{
    if ((mcb_read_reg (MCB_SWIC_STATUS (u->port)) & 0x30E0) != 0x30A0)
        PDEBUGG ("STATUS = %08X, channel %d\n", 
            mcb_read_reg (MCB_SWIC_STATUS (u->port)), u->port);
    return ((mcb_read_reg (MCB_SWIC_STATUS (u->port)) & 0x30E0) == 0x30A0);
}

static inline unsigned size_to_end_of_cur_buffer (double_buffer *pdbuf)
{
    one_buffer *pcur = &pdbuf->half[pdbuf->reader_idx];
    return (pdbuf->size - (pcur->reader_p - pcur->buf));
}

//
// Расчет доступной для чтения информации в указанном буфере @pbuf.
//
static inline unsigned avail_size (double_buffer *pdbuf)
{
    one_buffer *pcur = &pdbuf->half[pdbuf->reader_idx];

    int sz = mcb_read_reg (SWIC_DMA_IR (pdbuf->dma_chan)) - 
        mips_virtual_addr_to_physical ((unsigned) pcur->reader_p);
    if (sz == SPW_RX_DATA_BUFSZ && pdbuf->half[0].empty) return 0;
    else if (sz >= 0) return sz;
    else return (sz + SPW_RX_DATA_BUFSZ);
}

//
// Запуск канала DMA для указанного двойного буфера
//
static inline void start_dma (double_buffer *pdbuf)
{
    one_buffer *pcur;

    PDEBUG ("start_dma 0x%04X, dma_idx = %d\n", pdbuf->dma_chan, pdbuf->dma_idx);

    pcur = &pdbuf->half[pdbuf->dma_idx];
    pcur->empty = 0;
    PDEBUG ("pcur->chain_addr = %08X\n", pcur->chain_addr);
    if (pcur->chain_addr) {
        PDEBUG ("First CSR in chain: %08X\n", ((dma_params_t *) pcur->chain_addr)->csr);
        mcb_write_reg (SWIC_DMA_CP (pdbuf->dma_chan),
            mips_virtual_addr_to_physical (pcur->chain_addr) | 1);
    } else {
        PDEBUG ("pcur->buf = %08X\n", pcur->buf);
        mcb_write_reg (SWIC_DMA_IR (pdbuf->dma_chan),
            mips_virtual_addr_to_physical ((unsigned) pcur->buf));
        mcb_write_reg (SWIC_DMA_CSR (pdbuf->dma_chan), MC_DMA_CSR_WN(0) |
            MC_DMA_CSR_IM | MC_DMA_CSR_WCX((pdbuf->size >> 3) - 1) | MC_DMA_CSR_RUN);
    }
}

//
// Запуск DMA, если соблюдены необходимые условия
//
static void start_rx_dma_if_needed (double_buffer *pdbuf)
{
    PDEBUGG ("start_rx_dma_if_needed, SWIC_DMA_RUN(0x%04X) = %08X, second empty: %d, dma idx: %d, rd idx: %d\n",
        pdbuf->dma_chan, SWIC_DMA_RUN (pdbuf->dma_chan), pdbuf->half[!pdbuf->dma_idx].empty,
        pdbuf->dma_idx, pdbuf->reader_idx);
    if ( !(mcb_read_reg (SWIC_DMA_RUN (pdbuf->dma_chan)) & 1)  // Если канал DMA сейчас остановлен
         && pdbuf->half[!pdbuf->dma_idx].empty)                // и соседний буфер пуст, то
    {
        mcb_read_reg (SWIC_DMA_CSR (pdbuf->dma_chan));
        // Переключаем DMA на приём в соседний буфер
        pdbuf->dma_idx = !pdbuf->dma_idx;
        start_dma (pdbuf);
    }
}

//
// Продвижение указателя читателя в двойном буфере
//
static void move_reader_p (double_buffer *pdbuf, int distance)
{
    one_buffer *pcur = &pdbuf->half[pdbuf->reader_idx];
    int size_to_end = size_to_end_of_cur_buffer (pdbuf);
    int aligned_distance = (distance + 7) & ~7;

    PDEBUGG ("move_reader_p, distance = %d, size_to_end = %d\n",
        aligned_distance, size_to_end);

    if (aligned_distance < size_to_end)
        pcur->reader_p += aligned_distance;
    else {
        pcur->empty = 1;
        pdbuf->reader_idx = !pdbuf->reader_idx;
        pcur = &pdbuf->half[pdbuf->reader_idx];
        pcur->reader_p = pcur->buf + (aligned_distance - size_to_end);
        //if (pcur->chain_addr) {
            start_rx_dma_if_needed (pdbuf);
        //}
    }
}

//
// Копирование данных из буфера принятых данных в буфер пользователя
// с помощью MEM DMA или обычным способом, в зависимости от настроек драйвера
//
static inline void copy_data (spw_t *u, void *dest, void *src, int size)
{
#ifdef SPW_USE_MEM_DMA
    while (MC_RUN_MEM(u->mem_dma_chan) & MC_DMA_CSR_RUN);
    MC_IR0_MEM(u->mem_dma_chan) = mips_virtual_addr_to_physical ((unsigned) src);
    MC_IR1_MEM(u->mem_dma_chan) = mips_virtual_addr_to_physical ((unsigned) dest);
    MC_OR_MEM(u->mem_dma_chan) = MC_DMA_OR0(1) | MC_DMA_OR1(1);
    MC_CSR_MEM(u->mem_dma_chan) = MC_DMA_CSR_RUN | MC_DMA_CSR_EN64 | MC_DMA_CSR_WCX(((size + 7) >> 3) - 1);
    while (MC_CSR_MEM(u->mem_dma_chan) & MC_DMA_CSR_RUN);
#else
    memcpy (dest, src, size);
#endif
}

//
// Обработчик прерывания по завершению приёма данных
//
static void spw_dma_rx_data_ih (void *arg)
{
    spw_t *u = arg;
    
    PDEBUG ("spw_dma_rx_data_ih started, channel %d!\n", u->port);

    // Снимаем признак прерывания
    mcb_read_reg (MCB_SWIC_RX_DATA_CSR (u->port));

    start_rx_dma_if_needed (&u->rx_data_buf);
}

static void spw_dma_rx_desc_ih (void *arg)
{
    spw_t *u = arg;
    
    PDEBUG ("spw_dma_rx_desc_ih started, channel %d, MCB_MBA_QSTR0 = %08X!\n", 
        u->port, MCB_MBA_QSTR);

    // Снимаем признак прерывания
    mcb_read_reg (MCB_SWIC_RX_DESC_CSR (u->port));
    
    PDEBUG ("MCB_MBA_QSTR0 = %08X, MCB_SWIC_RX_DESC_CSR = %08X!\n", 
        MCB_MBA_QSTR, mcb_read_reg (MCB_SWIC_RX_DESC_CSR (u->port)));
}

static void spw_dma_tx_data_ih (void *arg)
{
    spw_t *u = arg;
    
    PDEBUG ("spw_dma_tx_data_ih started, channel %d!\n", u->port);

    // Снимаем признак прерывания
    mcb_read_reg (MCB_SWIC_TX_DATA_CSR (u->port));
}

static void spw_start (spw_t *u)
{
    // Сброс контроллера
    mcb_write_reg (MCB_SWIC_MODE_CR(u->port), SWIC_RESET);

    // Запись тайминга для того, чтобы SWIC отрабатывал временные интер-
    // валы в соответствии со стандартом SpaceWire
    //mcb_write_reg (MCB_SWIC_MODE_CR(u->port), MC_SWIC_TIMING_WR_EN);
    //mcb_write_reg (MCB_SWIC_TX_SPEED(u->port), MC_SWIC_TIMING(KHZ/10000));

    mcb_write_reg (MCB_SWIC_MODE_CR(u->port), SWIC_START);

    // Начальные установки и пуск
    mcb_write_reg (MCB_SWIC_TX_SPEED(u->port),
        MC_SWIC_TX_SPEED_PRM(SPW_START_SPEED / 5) | MC_SWIC_PLL_TX_EN | MC_SWIC_LVDS_EN);
    //mcb_write_reg (MCB_SWIC_TX_SPEED(u->port), MC_SWIC_TX_SPEED_CON(SPW_START_SPEED / 5) |
    //  MC_SWIC_TX_SPEED_PRM(u->speed / 5) | MC_SWIC_PLL_TX_EN | MC_SWIC_LVDS_EN);

    //mdelay (20);

    mcb_write_reg (MCB_SWIC_MODE_CR(u->port), MC_SWIC_LinkStart |
        MC_SWIC_AutoStart | MC_SWIC_WORK_TYPE | MC_SWIC_LINK_MASK);

    //mcb_write_reg (MCB_SWIC_MODE_CR(u->port) = MC_SWIC_LinkStart |
    // MC_SWIC_AutoStart | MC_SWIC_WORK_TYPE | MC_SWIC_LINK_MASK |
    // MC_SWIC_AUTO_TX_SPEED);

    // Сброс всех признаков прерываний
    mcb_write_reg (MCB_SWIC_STATUS(u->port), SWIC_RESET_ALL_IRQS);

    // Сброс счетчиков принятых пакетов
    mcb_write_reg (MCB_SWIC_CNT_RX_PACK(u->port), 0);
    mcb_write_reg (MCB_SWIC_CNT_RX0_PACK(u->port), 0);
}

static void spw_clean_dma (spw_t *u)
{
    uint32_t mode;

    PDEBUGG ("+++ spw_clean_dma: channel %d\n", u->port);

    mode = mcb_read_reg (MCB_SWIC_MODE_CR (u->port));
    mode &= ~ (MC_SWIC_AutoStart | MC_SWIC_LinkStart);
    mode |= MC_SWIC_LinkDisabled;
    mcb_write_reg (MCB_SWIC_MODE_CR (u->port), mode);

    mcb_write_reg (MCB_SWIC_RX_DESC_RUN (u->port), 0);
    mcb_write_reg (MCB_SWIC_RX_DATA_RUN (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DESC_RUN (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DATA_RUN (u->port), 0);

    mode |= MC_SWIC_SWCORE_RST;
    //mode |= MC_SWIC_DS_RST;
    mcb_write_reg (MCB_SWIC_MODE_CR (u->port), mode);

    while ((mcb_read_reg (MCB_SWIC_RX_DESC_RUN (u->port)) |
            mcb_read_reg (MCB_SWIC_RX_DATA_RUN (u->port)) |
            mcb_read_reg (MCB_SWIC_TX_DESC_RUN (u->port)) |
            mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port))) & 1) {
        PDEBUGG ("RX_DESC = %08X, RX_DATA = %08X, TX_DESC = %08X, TX_DATA = %08X, MODE_CR = %08X\n",
            mcb_read_reg (MCB_SWIC_RX_DESC_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_RX_DATA_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DESC_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_MODE_CR (u->port)));
    }

    mcb_read_reg (MCB_SWIC_RX_DESC_CSR (u->port));
    mcb_read_reg (MCB_SWIC_RX_DATA_CSR (u->port));
    mcb_read_reg (MCB_SWIC_TX_DESC_CSR (u->port));
    mcb_read_reg (MCB_SWIC_TX_DATA_CSR (u->port));

    mode &= ~MC_SWIC_SWCORE_RST;
    //mode &= ~MC_SWIC_DS_RST;
    mcb_write_reg (MCB_SWIC_MODE_CR (u->port), mode);

    mcb_write_reg (MCB_SWIC_RX_DESC_CP (u->port), 0);
    mcb_write_reg (MCB_SWIC_RX_DATA_CP (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DESC_CP (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DATA_CP (u->port), 0);

    mcb_write_reg (MCB_SWIC_RX_DESC_IR (u->port), 0);
    mcb_write_reg (MCB_SWIC_RX_DATA_IR (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DESC_IR (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DATA_IR (u->port), 0);

    mcb_write_reg (MCB_SWIC_RX_DESC_CSR (u->port), 0);
    mcb_write_reg (MCB_SWIC_RX_DATA_CSR (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DESC_CSR (u->port), 0);
    mcb_write_reg (MCB_SWIC_TX_DATA_CSR (u->port), 0);

    PDEBUGG ("    spw_clean_dma done\n");
}

//
// Функция чтения пользователем одного принятого пакета из приемного кольцевого буфера
//
int spw_input (spw_t *u, void *buf, int size, int nonblock)
{
    unsigned sz_to_end, nbytes, rest, completed;
    char *pdata;
    spw_descriptor_t *pdesc = (spw_descriptor_t *) u->rx_desc_buf.half[u->rx_desc_buf.reader_idx].reader_p;

    PDEBUG ("=== spw_input enter, channel = %d, u @ %p\n", u->port, u);

    mutex_lock (&u->st_lock);
    if (! spw_connected (u)) {
        PDEBUGG ("spw_input: channel %d not connected, wait for connection\n", u->port);
        spw_clean_dma (u);
        spw_start (u);
        if (nonblock) {
            mutex_unlock (&u->st_lock);
            return SPW_NOT_CONNECTED;
        }
        mutex_wait (&u->st_lock);
    }
    mutex_unlock (&u->st_lock);

    PDEBUG ("descriptor: %08X @ %p, reader_idx = %08X\n", *pdesc, pdesc, u->rx_desc_buf.reader_idx);

    while (1) {
        if (!pdesc->valid) {
            // Нет принятых дескрипторов
            PDEBUG ("=== spw_input, channel = %d, no data\n", u->port);

            // Запускаем RX DMA, если он ещё не запущен.
            start_rx_dma_if_needed (&u->rx_desc_buf);
            mutex_lock (&u->rx_data_lock);
            start_rx_dma_if_needed (&u->rx_data_buf);
            mutex_unlock (&u->rx_data_lock);

            // Если неблокирующий режим ввода-вывода и данных нет,
            // то сразу выходим с нулевым результатом.
            if (nonblock) return 0;

            // Если блокирующий режим, то дожидаемся получения
            // пакета без ошибок
            while (!pdesc->valid) {
                PDEBUG ("=== spw_input, channel = %d, waiting for a packet\n", u->port);
                PDEBUGG ("RX DESC DMA CSR = %08X, RX DATA DMA CSR = %08X\n",
                    mcb_read_reg (SWIC_DMA_RUN (u->rx_desc_buf.dma_chan)),
                    mcb_read_reg (SWIC_DMA_RUN (u->rx_data_buf.dma_chan)));
                mutex_wait (&u->rx_desc_lock);
                mcb_read_reg (SWIC_DMA_CSR (u->rx_desc_buf.dma_chan)); // Сброс признака прерывания
                PDEBUG ("=== spw_input, channel = %d, waiting done\n", u->port);
            }
        }

        pdesc->valid = 0;

        if (pdesc->type == EOP) {
            break;
        } else {
            // Получен дескриптор с ошибкой, ждём следующего дескриптора
            u->rx_eep++;
            move_reader_p (&u->rx_desc_buf, sizeof (spw_descriptor_t));
            pdesc = (spw_descriptor_t *) u->rx_desc_buf.half[u->rx_desc_buf.reader_idx].reader_p;

            PDEBUGG ("EEP, size = %d\n", pdesc->size);
            mutex_lock (&u->rx_data_lock);
            move_reader_p (&u->rx_data_buf, pdesc->size);
            mutex_unlock (&u->rx_data_lock);
        }
    }

    PDEBUG ("EOP, size = %d\n", pdesc->size);

    completed = 0;
    rest = pdesc->size;
    if (rest > size) {
        // Ошибка: пользователь предоставил маленький буфер
        mutex_lock (&u->rx_data_lock);
        move_reader_p (&u->rx_data_buf, rest);
        mutex_unlock (&u->rx_data_lock);

        move_reader_p (&u->rx_desc_buf, sizeof (spw_descriptor_t));
        return SPW_SMALL_BUF;
    }



    while (rest > 0) {
        pdata = u->rx_data_buf.half[u->rx_data_buf.reader_idx].reader_p;
        PDEBUGG ("pdata = %08X\n", pdata);
        do nbytes = avail_size (&u->rx_data_buf);
        while (nbytes == 0);
        sz_to_end = size_to_end_of_cur_buffer (&u->rx_data_buf);
        PDEBUGG ("avail_size = %d, sz_to_end = %d, rest = %d\n", nbytes, sz_to_end, rest);
        if (nbytes > sz_to_end) nbytes = sz_to_end;
        if (nbytes > rest) nbytes = rest;
        copy_data (u, (char *)buf + completed, pdata, nbytes);
        rest -= nbytes;
        completed += nbytes;

        mutex_lock (&u->rx_data_lock);
        move_reader_p (&u->rx_data_buf, nbytes);
        mutex_unlock (&u->rx_data_lock);
    }

    move_reader_p (&u->rx_desc_buf, sizeof (spw_descriptor_t));

    u->rx_eop++;
    u->rx_bytes += completed;

    PDEBUG ("=== spw_input exit, channel = %d, completed = %d\n", u->port, completed);
    return completed;
}

int spw_output (spw_t *u, const void *buf, int size, int nonblock)
{
    PDEBUG ("=== spw_output enter, channel = %d, size = %d\n", u->port, size);

    mutex_lock (&u->st_lock);
    if (! spw_connected (u)) {
        PDEBUGG ("spw_output: channel %d not connected, wait for connection\n", u->port);
        PDEBUGG ("    STATUS = %08X, TX DESC CSR = %08X, TX DATA CSR = %08X\n",
            mcb_read_reg (MCB_SWIC_STATUS (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DESC_RUN (u->port))
            mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port)));
        spw_clean_dma (u);
        spw_start (u);
        if (nonblock) {
            mutex_unlock (&u->st_lock);
            return SPW_NOT_CONNECTED;
        }
        mutex_wait (&u->st_lock);
    }
    mutex_unlock (&u->st_lock);

    // Если занят канал TX DMA, ...
    while (mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port)) & 1) {

        // При неблокирующем вводе-выводе, если канал TX DMA занят, сразу выходим
        if (nonblock) {
            return SPW_TX_BUSY;
        }

        // При блокирующем вводе-выводе ждем сигнала о завершении
        // текущей передачи TX DMA. Сигнал приходит от обработчика
        // прерывания по окончанию передачи TX DMA.
        PDEBUGG ("Waiting for TX DATA DMA free, channel %d\n", u->port);

        mutex_wait (&u->tx_data_lock);

        PDEBUGG ("Waiting for TX DMA done\n");
        u->txdma_waits++;
    }

    // Настраиваем DMA на выдачу пакета
    u->tx_desc->size = size;
    u->tx_desc->type = EOP;
    u->tx_desc->valid = 1;
    u->tx_desc->unused = 0;
    mcb_write_reg (MCB_SWIC_TX_DESC_IR(u->port),
        mips_virtual_addr_to_physical ((unsigned) u->tx_desc)); // Адрес начала буфера
    mcb_write_reg (MCB_SWIC_TX_DESC_CSR(u->port),
        MC_DMA_CSR_WCX(0) | MC_DMA_RUN); // 1 8-байтовое слово

    unsigned nbytes, completed = 0;
    while (size > 0) {
        nbytes = size;
        if (nbytes > SPW_TX_DATA_BUFSZ) nbytes = SPW_TX_DATA_BUFSZ;

        memcpy (u->tx_data_buffer, buf + completed, nbytes);    // Приходится копировать из-за проблем в DMA
        //u->tx_data_buffer = (char *)buf + completed;

        mcb_write_reg (MCB_SWIC_TX_DATA_IR(u->port),
            mips_virtual_addr_to_physical ((unsigned) u->tx_data_buffer));
        PDEBUGG ("tp1, TX_DATA_CSR = %08X, TX_DESC_CSR = %08X, channel %d\n",
            mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DESC_RUN (u->port)), u->port);
        PDEBUGG ("transmit: TX_DATA_IR = %08X, TX_DESC_IR = %08X, channel %d\n",
            mcb_read_reg (MCB_SWIC_TX_DATA_IR (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DESC_IR (u->port)), u->port);
        mcb_write_reg (MCB_SWIC_TX_DATA_CSR(u->port),
            MC_DMA_CSR_IM | MC_DMA_CSR_WCX(((nbytes + 7) >> 3) - 1)  | MC_DMA_RUN);

        //if (MC_SWIC_TX_DESC_CSR(u->port) == 0xFFFF8000)
        //  MC_SWIC_TX_DESC_CSR(u->port) = MC_DMA_CSR_WCX(0) | MC_DMA_RUN; // 1 8-байтовое слово
        PDEBUGG ("tp2, TX_DATA_CSR = %08X, TX_DESC_CSR = %08X, channel %d\n",
            mcb_read_reg (MCB_SWIC_TX_DATA_RUN (u->port)),
            mcb_read_reg (MCB_SWIC_TX_DESC_RUN (u->port)), u->port);

        completed += nbytes;
        size -= nbytes;
        if (size > 0) {
            mutex_wait (&u->tx_data_lock);
            mcb_read_reg(MCB_SWIC_TX_DATA_CSR (u->port));
        }
    }

    u->tx_packets++;
    u->tx_bytes += completed;

    PDEBUG ("=== spw_output exit, channel = %d, completed = %d\n", u->port, completed);
    return completed;
}

void spw_set_tx_speed (spw_t *u, unsigned mbit_per_sec)
{
    u->speed = mbit_per_sec;
    mcb_write_reg (MCB_SWIC_TX_SPEED (u->port),
        (mcb_read_reg (MCB_SWIC_TX_SPEED (u->port)) & ~MC_SWIC_TX_SPEED_PRM_MASK) |
        MC_SWIC_TX_SPEED_PRM (mbit_per_sec / 5));
}

unsigned spw_get_tx_speed (spw_t *u)
{
    return ((mcb_read_reg (MCB_SWIC_TX_SPEED (u->port)) &
        MC_SWIC_TX_SPEED_PRM_MASK) * 5);
}

unsigned spw_get_rx_speed (spw_t *u)
{
    return ((mcb_read_reg (MCB_SWIC_RX_SPEED (u->port)) * KHZ / 1000) >> 7);
}

//
// Обработчик прерывания по установке соединения
//
static void spw_connected_ih (void *arg)
{
    spw_t *u = arg;

    PDEBUG ("Connected, channel: %d, STATUS = %08X\n", u->port,
        mcb_read_reg (MCB_SWIC_STATUS (u->port)));

    /*
    if (MC_SWIC_STATUS (u->port) & 0xE) {
        MC_SWIC_STATUS (u->port) |= 0xF;
        PDEBUG ("Error detected, waiting reconnection\n");
        continue;
    }
    */
    PDEBUG ("Connected 1, channel: %d\n", u->port);
    mcb_write_reg (MCB_SWIC_STATUS (u->port),
        mcb_read_reg (MCB_SWIC_STATUS (u->port)) | MC_SWIC_CONNECTED);

    PDEBUG ("Connected 2, channel: %d\n", u->port);
    start_dma (&u->rx_desc_buf);
    PDEBUG ("Connected 3, channel: %d\n", u->port);
    start_dma (&u->rx_data_buf);
    PDEBUG ("Connected 4, channel: %d\n", u->port);
    spw_set_tx_speed (u, u->speed);
    PDEBUG ("Connected 5, channel: %d\n", u->port);

    // Закрываем прерывание, иначе оно будет возникать по приему каждого пакета
    mcb_write_reg (MCB_SWIC_MODE_CR (u->port),
        (mcb_read_reg (MCB_SWIC_MODE_CR (u->port)) & ~MC_SWIC_LINK_MASK) |
        MC_SWIC_ERR_MASK);
        
    PDEBUG ("Connected exit, channel: %d\n", u->port);
}

#if 0
static void spw_error_ih (void *arg)
{
    spw_t *u = arg;

    mutex_lock_irq (&u->err_lock, spw_err_irq(u->port), 0, 0);

    for (;;) {
        mutex_wait (&u->err_lock);

        PDEBUGG ("Error detected, restarting channel %d...\n", u->port);

        spw_clean_dma (u);
        spw_start (u);
    }
}

void dump (char *buf, int size)
{
    unsigned *ubuf = (unsigned *) buf;
    int i, j;

    for (i = 0; i < size / 4 / 8; ++i) {
        for (j = 0; j < 8; ++j) {
            debug_printf ("%08X ", *(ubuf + i * 8 + j));
        }
        debug_printf ("\n");
    }
}

void print_buffers (spw_t *u)
{
    int i;
    debug_printf ("DMA INDEX: %d, READER INDEX: %d, AVAIL SIZE: %d\n",
        u->rx_data_buf.dma_idx, u->rx_data_buf.reader_idx, avail_size (&u->rx_data_buf));
    for (i = 0; i < 2; ++i) {
        debug_printf ("RX DATA BUFFER %d:\n", i);
        debug_printf ("-----------------\n");
        debug_printf ("buf @ %08X, reader_p @ %08X, empty = %d\n",
            u->rx_data_buf.half[i].buf, u->rx_data_buf.half[i].reader_p,
            u->rx_data_buf.half[i].empty);
        dump (u->rx_data_buf.half[i].buf, u->rx_data_buf.size);
        debug_printf ("\n\n");
    }
}
#endif

static void spw_struct_init (spw_t *u, int port, int speed)
{
    int i, j;

    PDEBUGG ("spw_struct_init, port = %d\n", port);

    memset (u, 0, sizeof (spw_t));

    u->port = port;
    u->speed = speed;

    unsigned base_offset = 0;
    switch (port) {
    case 0: base_offset = 0; break;
    case 1: base_offset = SPW1_BASE_OFFSET; break;
    case 2: base_offset = SPW2_BASE_OFFSET; break;
    case 3: base_offset = SPW3_BASE_OFFSET; break;
    }

    // Инициализация буферов (двойной буферизации) принятых дескрипторов
    u->rx_desc_buf.size = SPW_ONE_BUFFER_DESC_CNT * sizeof (spw_descriptor_t);
    u->rx_desc_buf.dma_chan = MCB_SWIC_RX_DESC_CSR (port);
    u->rx_desc_buf.half[0].empty = 1;
    u->rx_desc_buf.half[1].empty = 1;

#ifdef SPW_RX_DESC_ADDR
    u->rx_desc_buf.half[0].reader_p = u->rx_desc_buf.half[0].buf =
        (char *) SPW_RX_DESC_ADDR + base_offset;
    u->rx_desc_buf.half[1].reader_p = u->rx_desc_buf.half[1].buf =
        (char *) SPW_RX_DESC_ADDR + base_offset + u->rx_desc_buf.size;
    u->rx_desc_chain[0] = (dma_params_t *) (SPW_RX_DESC_ADDR + base_offset +
        2 * u->rx_desc_buf.size);
    u->rx_desc_chain[1] = (dma_params_t *) (SPW_RX_DESC_ADDR + base_offset +
        2 * u->rx_desc_buf.size + SPW_ONE_BUFFER_DESC_CNT * sizeof (dma_params_t));
#else
    u->rx_desc_buf.half[0].reader_p = u->rx_desc_buf.half[0].buf = (char *) u->rxdescbuf[0];
    u->rx_desc_buf.half[1].reader_p = u->rx_desc_buf.half[1].buf = (char *) u->rxdescbuf[1];
    u->rx_desc_chain[0] = u->rxdescchain[0];
    u->rx_desc_chain[1] = u->rxdescchain[1];
#endif
    memset (u->rx_desc_buf.half[0].buf, 0, u->rx_desc_buf.size);
    memset (u->rx_desc_buf.half[1].buf, 0, u->rx_desc_buf.size);

    PDEBUG ("RX DESC: half[0] @ %08X, half[1] @ %08X, chain[0] @ %08X, chain[1] @ %08X\n",
        u->rx_desc_buf.half[0].buf, u->rx_desc_buf.half[1].buf,
        u->rx_desc_chain[0], u->rx_desc_chain[1]);

    PDEBUG ("RX DESC: buffer size = %d, nb of desc = %d\n", u->rx_desc_buf.size, SPW_ONE_BUFFER_DESC_CNT);

    // Инициализация цепочек самоинициализации RX DESC DMA для каждого
    // приёмного буфера двойной буферизации
    for (j = 0; j < 2; ++j) {
        for (i = 0; i < SPW_ONE_BUFFER_DESC_CNT; ++i) {
            u->rx_desc_chain[j][i].ir  = mips_virtual_addr_to_physical (
                (unsigned) (u->rx_desc_buf.half[j].buf + i * sizeof (spw_descriptor_t)));
            u->rx_desc_chain[j][i].csr = MC_DMA_CSR_IM | MC_DMA_CSR_CHEN |
                MC_DMA_CSR_WN(0) | MC_DMA_CSR_WCX(0) | MC_DMA_CSR_RUN;
            u->rx_desc_chain[j][i].cp  = mips_virtual_addr_to_physical ((unsigned) &u->rx_desc_chain[j][i + 1]);
        }
        u->rx_desc_chain[j][i - 1].csr = MC_DMA_CSR_IM | MC_DMA_CSR_WN(0) | MC_DMA_CSR_WCX(0) | MC_DMA_CSR_RUN;
        u->rx_desc_chain[j][i - 1].cp = 0;
        u->rx_desc_buf.half[j].chain_addr = (unsigned) u->rx_desc_chain[j];
    }

    PDEBUGG ("CSR in chain: %08X, chain addr = %08X\n", ((dma_params_t *) u->rx_desc_buf.half[0].chain_addr)->csr,
        u->rx_desc_buf.half[0].chain_addr);

    // Инициализация буферов (двойной буферизации) принятых данных
    u->rx_data_buf.size = (SPW_RX_DATA_BUFSZ / 2 + 7) & ~7;
    u->rx_data_buf.dma_chan = MCB_SWIC_RX_DATA_CSR (port);
    u->rx_data_buf.half[0].empty = 1;
    u->rx_data_buf.half[1].empty = 1;

#ifdef SPW_RX_DATA_ADDR
    u->rx_data_buf.half[0].reader_p = u->rx_data_buf.half[0].buf =
        (char *) SPW_RX_DATA_ADDR + base_offset;
    u->rx_data_buf.half[1].reader_p = u->rx_data_buf.half[1].buf =
        (char *) SPW_RX_DATA_ADDR + base_offset + u->rx_data_buf.size;
#else
    u->rx_data_buf.half[0].reader_p = u->rx_data_buf.half[0].buf = (char *) u->rxdatabuf[0];
    u->rx_data_buf.half[1].reader_p = u->rx_data_buf.half[1].buf = (char *) u->rxdatabuf[1];
#endif
    memset (u->rx_data_buf.half[0].buf, 0, u->rx_data_buf.size);
    memset (u->rx_data_buf.half[1].buf, 0, u->rx_data_buf.size);

    PDEBUGG ("RX DATA: half[0] @ %08X, half[1] @ %08X\n",
        u->rx_data_buf.half[0].buf, u->rx_data_buf.half[1].buf);


#ifdef SPW_TX_DESC_ADDR
    u->tx_desc = (spw_descriptor_t *) (SPW_TX_DESC_ADDR + base_offset);
#else
    u->tx_desc = &u->txdescbuf;
#endif

#ifdef SPW_TX_DATA_ADDR
    u->tx_data_buffer = (char *) (SPW_TX_DATA_ADDR + base_offset);
#else
    u->tx_data_buffer = u->txbuf;
#endif

    PDEBUG ("u->rx_desc_buf.reader_idx = %d\n", u->rx_desc_buf.reader_idx);
}

void spw_init (spw_t *u, int port, unsigned mbit_per_sec)
{
    PDEBUG ("spw_init, channel %d, u @ %p\n", port, u);
    spw_struct_init (u, port, mbit_per_sec);

#ifdef SPW_USE_MEM_DMA
    MC_CLKEN |= MC_CLKEN_DMA_MEM;
    mdelay (20);
    u->mem_dma_chan = (u->port == 0) ? SPW0_MEM_DMA_CHAN : SPW1_MEM_DMA_CHAN;
#endif

    //
    // Подключаем обработчики прерываний от MCB
    //
    memset (&u->mcb_irq_rx_data, 0, sizeof(u->mcb_irq_rx_data));
    u->mcb_irq_rx_data.mask0 = spw_rx_data_irq(u->port);
    u->mcb_irq_rx_data.handler = spw_dma_rx_data_ih;
    u->mcb_irq_rx_data.handler_arg = u;
    u->mcb_irq_rx_data.handler_lock = &u->rx_data_lock;
    mcb_register_interrupt_handler (&u->mcb_irq_rx_data);

    memset (&u->mcb_irq_connected, 0, sizeof(u->mcb_irq_connected));
    u->mcb_irq_connected.mask0 = spw_link_irq(u->port);
    u->mcb_irq_connected.handler = spw_connected_ih;
    u->mcb_irq_connected.handler_arg = u;
    u->mcb_irq_connected.handler_lock = &u->st_lock;
    mcb_register_interrupt_handler (&u->mcb_irq_connected);
    
    memset (&u->mcb_irq_rx_desc, 0, sizeof(u->mcb_irq_rx_desc));
    u->mcb_irq_rx_desc.mask0 = spw_rx_desc_irq(u->port);
    u->mcb_irq_rx_desc.handler = spw_dma_rx_desc_ih;
    u->mcb_irq_rx_desc.handler_arg = u;
    u->mcb_irq_rx_desc.handler_lock = &u->rx_desc_lock;
    mcb_register_interrupt_handler (&u->mcb_irq_rx_desc);
    
    memset (&u->mcb_irq_tx_data, 0, sizeof(u->mcb_irq_tx_data));
    u->mcb_irq_tx_data.mask0 = spw_tx_data_irq(u->port);
    u->mcb_irq_tx_data.handler = spw_dma_tx_data_ih;
    u->mcb_irq_tx_data.handler_arg = u;
    u->mcb_irq_tx_data.handler_lock = &u->tx_data_lock;
    mcb_register_interrupt_handler (&u->mcb_irq_tx_data);

    spw_start (u);
    PDEBUG ("spw_init done, channel %d\n", u->port);
}

