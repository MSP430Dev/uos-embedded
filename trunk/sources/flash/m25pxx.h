#ifndef __M25PXX_H__
#define __M25PXX_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

#define M25PXX_CMD_WREN         0x06
#define M25PXX_CMD_WRDI         0x04
#define M25PXX_CMD_RDID         0x9F
#define M25PXX_CMD_RDSR         0x05
#define M25PXX_CMD_WRSR         0x01
#define M25PXX_CMD_READ         0x03
#define M25PXX_CMD_FAST_READ    0x0B
#define M25PXX_CMD_PP           0x02
#define M25PXX_CMD_SE           0xD8
#define M25PXX_CMD_BE           0xC7
#define M25PXX_CMD_DP           0xB9
#define M25PXX_CMD_RES          0xAB

#define M25PXX_STATUS_WIP       (1 << 0)
#define M25PXX_STATUS_WEL       (1 << 1)
#define M25PXX_STATUS_BP0       (1 << 2)
#define M25PXX_STATUS_BP1       (1 << 3)
#define M25PXX_STATUS_BP2       (1 << 4)
#define M25PXX_STATUS_SRWD      (1 << 7)

struct _m25pxx_t
{
    flashif_t       flashif;
    spimif_t       *spi;
    spi_message_t   msg;
    uint8_t         databuf[4];
};
typedef struct _m25pxx_t m25pxx_t;

void m25pxx_init(m25pxx_t *m, spimif_t *s, unsigned freq, unsigned mode);

#endif
