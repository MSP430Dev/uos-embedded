#ifndef __SPI_MASTER_MILANDR_H__
#define __SPI_MASTER_MILANDR_H__

#include <spi/spi-master-interface.h>

#ifndef SPI_DMA_BUFSZ
#define SPI_DMA_BUFSZ   4096
#endif

typedef void (* spi_cs_control_func)(unsigned port, unsigned cs_num, int level);

struct _milandr_spim_t {
    spimif_t            spimif;
    unsigned            port;
    unsigned            last_freq;
    unsigned            last_bits;
    unsigned            last_mode;
    
    SSP_t               *reg;
    spi_cs_control_func cs_control;    
#ifndef SPI_NO_DMA
    uint8_t             dma_txbuf[SPI_DMA_BUFSZ] __attribute__((aligned(8)));
    uint8_t             dma_rxbuf[SPI_DMA_BUFSZ] __attribute__((aligned(8)));
#endif
};
typedef struct _milandr_spim_t milandr_spim_t;

int milandr_spim_init(milandr_spim_t *spi, unsigned port, spi_cs_control_func csc);

#endif
