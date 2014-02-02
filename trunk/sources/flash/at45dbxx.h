#ifndef __M25PXX_H__
#define __M25PXX_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

struct _at45dbxx_t
{
    flashif_t       flashif;
    spimif_t       *spi;
    spi_message_t   msg;
    uint8_t         databuf[5];
};
typedef struct _at45dbxx_t at45dbxx_t;

void at45dbxx_init(at45dbxx_t *m, spimif_t *s, unsigned freq, unsigned mode);

#endif
