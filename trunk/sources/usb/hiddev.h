#ifndef __HIDDEV_H__
#define __HIDDEV_H__

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include "usb_const.h"
#include "usb_struct.h"
#include "usbdev.h"
#include "hid_const.h"

#ifndef HIDDEV_NB_REPORTS
#define HIDDEV_NB_REPORTS       1
#endif

#ifndef HIDDEV_NB_PHYSICALS
#define HIDDEV_NB_PHYSICALS     1
#endif

#define HIDDEV_BOOT_PROTOCOL    0
#define HIDDEV_REPORT_PROTOCOL  1

typedef struct __attribute__ ((packed)) _hid_1desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Device descriptor type (must be USB_DESC_TYPE_HID).
    uint16_t bcdHID;                // HID Class Specification release number.
    uint8_t  bCountryCode;          // Hardware target country.
    uint8_t  bNumDescriptors;       // Number of HID class descriptors to follow.
    uint8_t  bReportType;           // Report descriptor type.
    uint16_t wReportLength;         // Total length of Report descriptor.
} hid_1desc_t;

struct _hiddev_t;
typedef struct _hiddev_t hiddev_t;

typedef int (*hiddev_set_report_handler_t) (hiddev_t *h, uint8_t *report, int size);
typedef int (*hiddev_set_idle_handler_t) (hiddev_t *h, unsigned idle_rate);
typedef int (*hiddev_set_protocol_handler_t) (hiddev_t *h, uint8_t protocol);

struct _hiddev_t
{
    mutex_t *           lock;
    usbdev_t *          usb;
    unsigned            if_n;
    unsigned            ep_n;
    mem_pool_t *        pool;
    
    uint8_t *           rpt_desc[HIDDEV_NB_REPORTS];
    unsigned            rpt_desc_sz[HIDDEV_NB_REPORTS];
    uint8_t *           in_rpt[HIDDEV_NB_REPORTS];
    unsigned            in_rpt_sz[HIDDEV_NB_REPORTS];
    int                 in_rpt_ready[HIDDEV_NB_REPORTS];
    uint8_t *           out_rpt[HIDDEV_NB_REPORTS];
    unsigned            out_rpt_sz[HIDDEV_NB_REPORTS];
    uint8_t *           feature_rpt[HIDDEV_NB_REPORTS];
    unsigned            feature_rpt_sz[HIDDEV_NB_REPORTS];
    int                 feature_rpt_ready[HIDDEV_NB_REPORTS];
    uint8_t *           phys_desc[HIDDEV_NB_PHYSICALS];
    unsigned            phys_desc_sz[HIDDEV_NB_PHYSICALS];
    
    uint8_t             idle_rate_ms;
    uint8_t             cur_prot;
    
    hiddev_set_report_handler_t     set_rpt;
    hiddev_set_idle_handler_t       set_idle;
    hiddev_set_protocol_handler_t   set_prt;
};

void hiddev_init (hiddev_t *h, usbdev_t *u, unsigned if_n,
    unsigned ep_n, mem_pool_t *pool, mutex_t *m);

void hiddev_set_idle_rate (hiddev_t *h, unsigned rate);
void hiddev_set_protocol (hiddev_t *h, uint8_t protocol);

void hiddev_set_report_handler (hiddev_t *h,
    hiddev_set_report_handler_t handler);
void hiddev_set_idle_handler (hiddev_t *h,
    hiddev_set_idle_handler_t handler);
void hiddev_set_protocol_handler (hiddev_t *h,
    hiddev_set_protocol_handler_t handler);

void hiddev_set_report_desc (hiddev_t *h, unsigned rpt_id, 
    uint8_t *rpt_desc, unsigned desc_size, unsigned in_rpt_size,
    unsigned out_rpt_size, unsigned feature_rpt_size);
    
void hiddev_set_physical_desc (hiddev_t *h, unsigned rpt_id,
    uint8_t *phys_desc, unsigned desc_size);

void hiddev_output_report (hiddev_t *h, unsigned rpt_id,
    const uint8_t *report);
void hiddev_input_report (hiddev_t *h, unsigned rpt_id, uint8_t *report);

#endif
