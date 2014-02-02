#ifndef __SD_SPI_H__
#define __SD_SPI_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

#define SD_BAD_CARD     0
#define SD_VER_1_XX     (1 << 0)
#define SD_VER_2_00     (2 << 0)
#define SD_VER_MASK     0x3
#define SDSC            (1 << 2)
#define SDHC            (2 << 2)
#define SDXC            (3 << 2)

struct _sdhc_spi_t
{
    flashif_t       flashif;
    spimif_t       *spi;
    spi_message_t   msg;
    uint8_t         databuf[42];
    uint8_t         ncr;
    uint8_t         version;
};
typedef struct _sdhc_spi_t sdhc_spi_t;

void sd_spi_init(sdhc_spi_t *m, spimif_t *s, unsigned mode);

#endif
