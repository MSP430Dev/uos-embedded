#ifndef __USB_DEV_HAL_H__
#define __USB_DEV_HAL_H__

#include <mem/mem.h>

#ifndef USBDEV_MAX_EP_NB
#define USBDEV_MAX_EP_NB   4
#endif

typedef struct _ep_state_t {
    uint8_t max_size[2];
    int8_t  last_in_bytes;
    uint8_t busy;
} ep_state_t;


void mldr_usbdev_init (usbdev_t *owner, int io_prio, mem_pool_t *pool, mutex_t *m);

#endif
