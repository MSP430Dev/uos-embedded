#ifndef __USB_DEV_HAL_H__
#define __USB_DEV_HAL_H__

#include <mem/mem.h>

#ifndef USBDEV_MAX_EP_NB
#define USBDEV_MAX_EP_NB   16
#endif

/* Struct of a buffer descriptor */
typedef struct _bd_t {
    uint32_t    status;
    uint32_t    address;
} bd_t;

typedef struct _ep_bd_t {
    bd_t    out[2];
    bd_t    in[2];
} ep_bd_t;

typedef struct _ep_state_t {
    int     max_size[2];
    int     cur_txbuf;
    int     cur_inbuf;
    int     cur_outbuf;
    int     in_busy[2];
} ep_state_t;


void pic32_usbdev_init (usbdev_t *owner, int io_prio, mem_pool_t *pool, mutex_t *m);

#endif
