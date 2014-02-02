#ifndef __M25PXX_H__
#define __M25PXX_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

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
