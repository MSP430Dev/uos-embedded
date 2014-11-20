#ifndef __M25PXX_H__
#define __M25PXX_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

struct _s25fl_t
{
    flashif_t       flashif;
    spimif_t       *spi;
    spi_message_t   msg;
    uint8_t         databuf[5];
};
typedef struct _s25fl_t s25fl_t;

void s25fl_init(s25fl_t *m, spimif_t *s, unsigned freq, unsigned mode);

#endif
