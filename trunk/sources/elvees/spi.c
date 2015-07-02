#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <elvees/spi.h>

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#if defined(ELVEES_NVCOM02T)
#   define IRQ_SHIFT        3
#   define SPI_IRQ(port_id)    (41 + ((port_id) << IRQ_SHIFT))
#elif defined(ELVEES_MC0428)
#   define IRQ_SHIFT        3
#   define SPI_IRQ(port_id)    (62 + ((port_id) << IRQ_SHIFT))
#elif defined(ELVEES_MC24R2)
#   define IRQ_SHIFT        2
#   define SPI_IRQ(port_id)    (30 + ((port_id) << IRQ_SHIFT))
#elif defined(ELVEES_MCT03P)
#   define IRQ_SHIFT        3
#   define SPI_IRQ(port_id)    (80 + ((port_id) << IRQ_SHIFT))
#elif defined(ELVEES_MC30SF6)
#   define IRQ_SHIFT        2
#   define SPI_IRQ(port_id)    (50 + ((port_id) << IRQ_SHIFT))
#endif

static inline void start_rx(int port)
{
#if defined(ELVEES_MC24R2)
    MC_MFBSP_RCTR(port) |= MC_MFBSP_REN;
#else
    MC_MFBSP_RSTART(port) = 1;
#endif
}

static inline void stop_rx(int port)
{
#if defined(ELVEES_MC24R2)
    MC_MFBSP_RCTR(port) &= ~MC_MFBSP_REN;
#else
    MC_MFBSP_RSTART(port) = 0;
#endif
}

static inline void start_tx(int port)
{
#if defined(ELVEES_MC24R2)
    MC_MFBSP_TCTR(port) |= MC_MFBSP_TEN;
#else
    MC_MFBSP_TSTART(port) = 1;
#endif
}

static inline void stop_tx(int port)
{
#if defined(ELVEES_MC24R2)
    MC_MFBSP_TCTR(port) &= ~MC_MFBSP_TEN;
#else
    MC_MFBSP_TSTART(port) = 0;
#endif
}

static inline void
cs_activate(int port, int cs_num, int cs_high)
{
    if (cs_high)
        MC_MFBSP_TCTR(port) |= MC_MFBSP_SS(cs_num);
    else
        MC_MFBSP_TCTR(port) &= ~MC_MFBSP_SS(cs_num);
}

static inline void
cs_deactivate(int port, int cs_num, int cs_high)
{
    if (cs_high)
        MC_MFBSP_TCTR(port) &= ~MC_MFBSP_SS(cs_num);
    else
        MC_MFBSP_TCTR(port) |= MC_MFBSP_SS(cs_num);
}

static inline int
init_hw(elvees_spim_t *spi, unsigned freq, unsigned bits_per_word,
    unsigned cs_num, unsigned mode)
{
    if (freq != spi->last_freq) {
        stop_rx(spi->port);
        stop_tx(spi->port);

        // Максимальная частота, поддерживаемая контроллером - половина
        // частоты процессора
        unsigned div = DIV_ROUND_UP(KHZ * 1000 / 2, freq) - 1;
        if (div > 0x3FF) {
            debug_printf ("SPI Master %d: too low frequency!\n", spi->port);
            spi->last_freq = 0;
            return SPI_ERR_BAD_FREQ;
        }

        MC_MFBSP_TCTR_RATE(spi->port) = MC_MFBSP_TCLK_RATE(div);
        MC_MFBSP_RCTR_RATE(spi->port) = MC_MFBSP_RCLK_RATE(div);

        spi->last_freq = freq;
    }

    if (cs_num > 1) {
        debug_printf ("SPI Master %d: bad chipselect: %d\n",
            spi->port, cs_num);
        return SPI_ERR_BAD_CS;
    }
    if (mode != spi->last_mode) {
        stop_rx(spi->port);
        stop_tx(spi->port);

        if (mode & SPI_MODE_CPOL) {
            MC_MFBSP_RCTR(spi->port) |= MC_MFBSP_RNEG;
            MC_MFBSP_TCTR(spi->port) |= MC_MFBSP_TNEG;
        } else {
            MC_MFBSP_RCTR(spi->port) &= ~MC_MFBSP_RNEG;
            MC_MFBSP_TCTR(spi->port) &= ~MC_MFBSP_TNEG;
        }

        if (mode & SPI_MODE_CPHA) {
            MC_MFBSP_RCTR(spi->port) |= MC_MFBSP_RDEL;
            MC_MFBSP_TCTR(spi->port) |= MC_MFBSP_TDEL;
        } else {
            MC_MFBSP_RCTR(spi->port) &= ~MC_MFBSP_RDEL;
            MC_MFBSP_TCTR(spi->port) &= ~MC_MFBSP_TDEL;
        }

        if (mode & SPI_MODE_LSB_FIRST) {
            MC_MFBSP_RCTR(spi->port) &= ~MC_MFBSP_RMBF;
            MC_MFBSP_TCTR(spi->port) &= ~MC_MFBSP_TMBF;
        } else {
            MC_MFBSP_RCTR(spi->port) |= MC_MFBSP_RMBF;
            MC_MFBSP_TCTR(spi->port) |= MC_MFBSP_TMBF;
        }            

        spi->last_mode = mode;
    }
    if (bits_per_word != spi->last_bits) {
        stop_rx(spi->port);
        stop_tx(spi->port);

        if (bits_per_word > 32) {
            debug_printf ("SPI Master %d: too many bits per word: %d\n",
                spi->port, bits_per_word);
            return SPI_ERR_BAD_BITS;
        }
        MC_MFBSP_RCTR(spi->port) = (MC_MFBSP_RCTR(spi->port) &
            ~MC_MFBSP_RWORDLEN_MASK) | MC_MFBSP_RWORDLEN(bits_per_word-1);
        MC_MFBSP_TCTR(spi->port) = (MC_MFBSP_TCTR(spi->port) &
            ~MC_MFBSP_TWORDLEN_MASK) | MC_MFBSP_TWORDLEN(bits_per_word-1);

        spi->last_bits = bits_per_word;
    }

    start_rx(spi->port);
    start_tx(spi->port);

    return SPI_ERR_OK;
}

#ifndef SPI_NO_DMA

static void
repack_to_dma(void *dma_buf, const void *src_buf, unsigned words, unsigned bits)
{
    const uint8_t  *pu8 = (uint8_t *) src_buf;
    const uint16_t *pu16 = (uint16_t *) src_buf;
    uint32_t       *pdma = (uint32_t *) dma_buf;
    int       i;

    if (bits <= 8) {
        if (!pu8)
            memset(pdma, 0, words << 2);
        else
            for (i = 0; i < words; ++i) {
                *pdma++ = *pu8++;
            }
    } else if (bits > 8 && bits <= 16) {
        if (!pu16)
            memset(pdma, 0, words << 2);
        else
            for (i = 0; i < words; ++i)
                *pdma++ = *pu16++;
    } else {
        memcpy(dma_buf, src_buf, words << 2);
    }
}

static void
repack_from_dma(const void *dma_buf, void *dst_buf, unsigned words, unsigned bits)
{
    uint8_t         *pu8 = dst_buf;
    uint16_t        *pu16 = dst_buf;
    const uint32_t  *pdma = dma_buf;
    int         i;

    if (!dst_buf)
        return;

    if (bits <= 8) {
        for (i = 0; i < words; ++i) {
            *pu8++ = *pdma++;
        }
    } else if (bits > 8 && bits <= 16) {
        for (i = 0; i < words; ++i)
            *pu16++ = *pdma++;
    } else {
        memcpy(dst_buf, dma_buf, words << 2);
    }
}

static int trx(spimif_t *spimif, spi_message_t *msg)
{
    elvees_spim_t   *spi = (elvees_spim_t *) spimif;
    unsigned        bits_per_word = SPI_MODE_GET_NB_BITS(msg->mode);
    unsigned        cs_num = SPI_MODE_GET_CS_NUM(msg->mode);
    unsigned        mode = SPI_MODE_GET_MODE(msg->mode);
    uint32_t        *pdma_buf;
    unsigned        bytes = msg->word_count * ((bits_per_word + 7) / 8);
    int             res;

    if (msg->word_count == 0)
        return SPI_ERR_OK;
    
    if (bytes > SPI_DMA_BUFSZ) {
        /*
        debug_printf("DMA buffers are too short: \
                %d bytes, shall be at least: %d bytes \
                (change SPI_DMA_BUFSZ parameter!)\n", \
                SPI_DMA_BUFSZ, bytes);
        */
        return SPI_ERR_SMALL_BUF;
    }

    mutex_lock(&spimif->lock);

    res = init_hw(spi, msg->freq, bits_per_word, cs_num, mode);
    if (res != SPI_ERR_OK) {
        mutex_unlock(&spimif->lock);
        return res;
    }

    if (msg->word_count == 1) {
        cs_activate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);
        if (msg->tx_data)
            if (bits_per_word <= 8) {
                MC_MFBSP_TX(spi->port) = *((uint8_t *)msg->tx_data);
            }
            else if (bits_per_word > 8 && bits_per_word <= 16)
                MC_MFBSP_TX(spi->port) = *((uint16_t *)msg->tx_data);
            else
                MC_MFBSP_TX(spi->port) = *((uint32_t *)msg->tx_data);
        else
            MC_MFBSP_TX(spi->port) = 0;
        // Ожидаем завершения выдачи слова (и приёма ответного слова)
        while (MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE);
        // Выбираем из приёмной очереди слово
        if (msg->rx_data)
            if (bits_per_word <= 8)
                *((uint8_t *)msg->rx_data) = MC_MFBSP_RX(spi->port);
            else if (bits_per_word > 8 && bits_per_word <= 16)
                *((uint16_t *)msg->rx_data) = MC_MFBSP_RX(spi->port);
            else
                *((uint32_t *)msg->rx_data) = MC_MFBSP_RX(spi->port);
        else
            MC_MFBSP_RX(spi->port);

        if (!(mode & SPI_MODE_CS_HOLD))
            cs_deactivate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

        mutex_unlock(&spimif->lock);

        return SPI_ERR_OK;
    }

    repack_to_dma(spi->dma_txbuf, msg->tx_data, msg->word_count, bits_per_word);

    cs_activate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

    mutex_lock_irq (&spi->irq_lock, SPI_IRQ(spi->port), 0, 0);
    // Запускаем DMA на приём и на выдачу
    MC_IR_MFBSP_RX(spi->port) = (unsigned) spi->dma_rxbuf & 0x1FFFFFFF;
    MC_CSR_MFBSP_RX(spi->port) = MC_DMA_CSR_WN(0) | 
        MC_DMA_CSR_WCX((msg->word_count >> 1) - 1) | MC_DMA_CSR_RUN;
    MC_IR_MFBSP_TX(spi->port) = (unsigned) spi->dma_txbuf & 0x1FFFFFFF;
    MC_CSR_MFBSP_TX(spi->port) = MC_DMA_CSR_WN(0) | 
        MC_DMA_CSR_WCX((msg->word_count >> 1) - 1) | MC_DMA_CSR_RUN;

    mutex_wait(&spi->irq_lock);
    MC_CSR_MFBSP_RX(spi->port);
    mutex_unlock(&spi->irq_lock);

    if (msg->word_count & 1) {
        // Нечётное число слов в передаче -
        // последнее слово досылаем "вручную"
        pdma_buf = (uint32_t *) spi->dma_txbuf;
        MC_MFBSP_TX(spi->port) = pdma_buf[msg->word_count - 1];
        // Ожидаем завершения выдачи слова (и приёма ответного слова)
        while (MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE);
        // Выбираем из приёмной очереди слово
        pdma_buf = (uint32_t *) spi->dma_rxbuf;
        pdma_buf[msg->word_count - 1] = MC_MFBSP_RX(spi->port);
    }

    if (!(mode & SPI_MODE_CS_HOLD))
        cs_deactivate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

#ifdef ENABLE_DCACHE
    MC_CSR |= MC_CSR_FLUSH_D;
#endif

    repack_from_dma(spi->dma_rxbuf, msg->rx_data, msg->word_count, bits_per_word);

    mutex_unlock(&spimif->lock);

    return SPI_ERR_OK;
}

#else   //!SPI_NO_DMA

static int trx(spimif_t *spimif, spi_message_t *msg)
{
    elvees_spim_t       *spi = (elvees_spim_t *) spimif;
    uint8_t             *rxp_8bit;
    uint8_t             *txp_8bit;
    uint16_t            *rxp_16bit;
    uint16_t            *txp_16bit;
    uint32_t            *rxp_32bit;
    uint32_t            *txp_32bit;
    unsigned            i, j;
    unsigned            bits_per_word = SPI_MODE_GET_NB_BITS(msg->mode);
    unsigned            cs_num = SPI_MODE_GET_CS_NUM(msg->mode);
    unsigned            mode = SPI_MODE_GET_MODE(msg->mode);
    int                 res;

    if (msg->word_count == 0)
        return SPI_ERR_OK;
        
    mutex_lock(&spimif->lock);

    res = init_hw(spi, msg->freq, bits_per_word, cs_num, mode);
    if (res != SPI_ERR_OK) {
        mutex_unlock(&spimif->lock);
        return res;
    }

    for (j = 0; j < 16; ++j)
        MC_MFBSP_RX(spi->port);

    cs_activate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

    if (bits_per_word <= 8) {
        rxp_8bit = (uint8_t *) msg->rx_data;
        txp_8bit = (uint8_t *) msg->tx_data;

        i = 0;
        while (i < msg->word_count) {
            while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TBE));
            for (j = 0; j < 16; ++j) {
                if (txp_8bit)
                    MC_MFBSP_TX(spi->port) = *txp_8bit++;
                else
                    MC_MFBSP_TX(spi->port) = 0;

                ++i;
                if (i >= msg->word_count)
                    break;

                if (rxp_8bit) {
                    while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                        *rxp_8bit = MC_MFBSP_RX(spi->port);
                        rxp_8bit++;
                    }
                }
            }
        }

        while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TSBE));

        if (rxp_8bit) {
            while (rxp_8bit - (uint8_t *) msg->rx_data < msg->word_count) {
                while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                    *rxp_8bit = MC_MFBSP_RX(spi->port);
                    rxp_8bit++;
                }
            }
        }
    } else if (bits_per_word > 8 && bits_per_word <= 16) {
        rxp_16bit = (uint16_t *) msg->rx_data;
        txp_16bit = (uint16_t *) msg->tx_data;

        i = 0;
        while (i < msg->word_count) {
            while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TBE));
            for (j = 0; j < 16; ++j) {
                if (txp_16bit) {
                    MC_MFBSP_TX(spi->port) = *txp_16bit++;
                } else {
                    MC_MFBSP_TX(spi->port) = 0;
                }
                ++i;
                if (i >= msg->word_count)
                    break;

                if (rxp_16bit) {
                    while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                        *rxp_16bit = MC_MFBSP_RX(spi->port);
                        rxp_16bit++;
                    }
                }
            }
        }

        while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TSBE));

        if (rxp_16bit) {
            while (rxp_16bit - (uint16_t *) msg->rx_data < msg->word_count) {
                 while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                    *rxp_16bit = MC_MFBSP_RX(spi->port);
                    rxp_16bit++;
                }
            }
        }
    } else if (bits_per_word > 16 && bits_per_word <= 32) {
        rxp_32bit = (uint32_t *) msg->rx_data;
        txp_32bit = (uint32_t *) msg->tx_data;

        i = 0;
        while (i < msg->word_count) {
            while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TBE));
            for (j = 0; j < 16; ++j) {
                if (txp_32bit) {
                    MC_MFBSP_TX(spi->port) = *txp_32bit++;
                } else {
                    MC_MFBSP_TX(spi->port) = 0;
                }
                ++i;
                if (i >= msg->word_count)
                    break;

                if (rxp_32bit) {
                    while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                        *rxp_32bit = MC_MFBSP_RX(spi->port);
                        rxp_32bit++;
                    }
                }
            }
        }

        while (!(MC_MFBSP_TSR(spi->port) & MC_MFBSP_TSR_TSBE));

        if (rxp_32bit) {
            while (rxp_32bit - (uint32_t *) msg->rx_data < msg->word_count) {
                while (!(MC_MFBSP_RSR(spi->port) & MC_MFBSP_RSR_RBE)) {
                    *rxp_32bit = MC_MFBSP_RX(spi->port);
                    rxp_32bit++;
                }
            }
        }
    } else {
        mutex_unlock(&spimif->lock);
        return SPI_ERR_BAD_BITS;
    }

    if (!(mode & SPI_MODE_CS_HOLD))
        cs_deactivate(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

    mutex_unlock(&spimif->lock);

    return SPI_ERR_OK;
}

#endif

int spim_init(elvees_spim_t *spi, unsigned port, unsigned io_mode)
{
    memset(spi, 0, sizeof(elvees_spim_t));

    spi->port = port;
    spi->spimif.trx = trx;

    MC_CLKEN |= MC_CLKEN_MFBSP;

    MC_MFBSP_CSR(spi->port) = MC_MFBSP_CSR_SPI_I2S_EN;

    unsigned dir;
    dir = MC_MFBSP_RCLK_DIR | MC_MFBSP_TCLK_DIR | MC_MFBSP_TCS_DIR | 
        MC_MFBSP_RCS_DIR;
    if (io_mode & SPI_MOSI_OUT) dir |= MC_MFBSP_TD_DIR;
    if (io_mode & SPI_MISO_OUT) dir |= MC_MFBSP_RD_DIR;
    if (io_mode & SPI_SS0_OUT)  dir |= MC_MFBSP_TCS_DIR;
    if (io_mode & SPI_SS1_OUT)  dir |= MC_MFBSP_RCS_DIR;
    if (io_mode & SPI_TSCK_OUT) dir |= MC_MFBSP_TCLK_DIR;
    if (io_mode & SPI_RSCK_OUT) dir |= MC_MFBSP_RCLK_DIR;

    MC_MFBSP_DIR(spi->port) = dir;

    MC_MFBSP_RCTR(spi->port) = MC_MFBSP_RMODE | MC_MFBSP_RCLK_CP | 
        MC_MFBSP_RCS_CP | MC_MFBSP_RMBF;
    MC_MFBSP_TCTR(spi->port) = MC_MFBSP_TMODE | MC_MFBSP_TMBF | MC_MFBSP_SS_DO;
    
#ifdef SPI_NO_DMA
	spi->spimif.data_align = 0;
#else
	spi->spimif.data_align = 7;
#endif

    return 0;
}

