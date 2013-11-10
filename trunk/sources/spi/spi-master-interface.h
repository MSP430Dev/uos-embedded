#ifndef __SPI_MASTER_INTERFACE_H__
#define __SPI_MASTER_INTERFACE_H__

#define SPI_ERR_OK          0
#define SPI_ERR_BAD_BITS    -1
#define SPI_ERR_BAD_FREQ    -2
#define SPI_ERR_BAD_CS      -3
#define SPI_ERR_SMALL_BUF   -4

#define SPI_MODE_CPOL           (1 << 0)
#define SPI_MODE_CPHA           (1 << 1)
#define SPI_MODE_CS_HIGH        (1 << 2)
#define SPI_MODE_LSB_FIRST      (1 << 3)
#define SPI_MODE_CS_HOLD        (1 << 4)
#define SPI_MODE_GET_MODE(x)    ((x) & 0xFF)
#define SPI_MODE_CS_NUM(x)      ((x) << 8)
#define SPI_MODE_GET_CS_NUM(x)  (((x) >> 8) & 0xFF)
#define SPI_MODE_NB_BITS(x)     ((x) << 16)
#define SPI_MODE_GET_NB_BITS(x) (((x) >> 16) & 0xFF)

typedef struct _spimif_t spimif_t;
typedef struct _spi_message_t spi_message_t;

struct _spimif_t
{
    mutex_t     lock;

    int (* trx)(spimif_t *spi, spi_message_t *msg);
};

struct _spi_message_t {
    void        *tx_data;
    void        *rx_data;
    unsigned    word_count;
    unsigned    freq;
    unsigned    mode;
};

static inline __attribute__((always_inline)) 
int spim_trx(spimif_t *spi, spi_message_t *msg)
{
    return spi->trx(spi, msg);
}

#endif
