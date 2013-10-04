//
// Пока заглушка для отладки USB-загрузчика uuloader.
// После подключения в системе должно появиться устройство USB-Device 
// производства UOS-embedded.
// В Linux dmesg возвращает что-то вроде этого:
//[262157.153560] usb 4-5: New USB device found, idVendor=04d8, idProduct=003e
//[262157.153571] usb 4-5: New USB device strings: Mfr=1, Product=2, SerialNumber=0
//[262157.153580] usb 4-5: Product: uuloader
//[262157.153587] usb 4-5: Manufacturer: UOS-embedded
//

#include "runtime/lib.h"
#include "kernel/uos.h"
#include <timer/timer.h>
#include <mem/mem.h>
#include <usb/usbdev.h>
#include <usb/usb_struct.h>
#include <usb/usb_const.h>
#include <milandr/usbdevhal.h>

#ifndef UULOAD_FLASH_BLOCK_SIZE
#define UULOAD_FLASH_BLOCK_SIZE     4096
#endif

#define UULOAD_REQ_FEATURES         0
#define UULOAD_REQ_ERASE            1
#define UULOAD_REQ_ERASE_ALL        2
#define UULOAD_REQ_UPLOAD_DATA      3
#define UULOAD_REQ_DOWNLOAD_DATA    4
#define UULOAD_REQ_PROGRAM          5
#define UULOAD_REQ_WRITE            6
#define UULOAD_REQ_READ             7
#define UULOAD_REQ_RESET            8
#define UULOAD_REQ_RUN              9

ARRAY (task_space, 2000);

usbdev_t usb;
mem_pool_t pool;
timer_t timer;
mutex_t usb_lock;

static const usb_dev_desc_t device_descriptor = {
    sizeof (usb_dev_desc_t),
    USB_DESC_TYPE_DEVICE,
    0x0200,
    0,
    0,
    0,
    USBDEV_EP0_MAX_SIZE,
    0x04d8,
    0x003e,
    0x0100,
    1,
    2,
    0,
    1
};

typedef struct __attribute__ ((packed)) _this_conf_desc_t
{
    usb_conf_desc_t     conf;
    usb_iface_desc_t    iface;
    //usb_ep_desc_t       ep;
} this_conf_desc_t;

static const this_conf_desc_t config_descriptor = {
    // Configuration descriptor
    {
        .bLength            = sizeof (usb_conf_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_CONFIG,
        .wTotalLength       = sizeof (this_conf_desc_t),
        .bNumInterfaces     = 1,
        .bConfigurationValue= 1,
        .iConfiguration     = 0,
        .bmAttributes       = USB_CONF_ATTR_REQUIRED | USB_CONF_ATTR_SELF_PWR,
        .bMaxPower          = 50,
    },
    
    // Interface descriptor
    {
        .bLength            = sizeof (usb_iface_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_IFACE,
        .bInterfaceNumber   = 0,
        .bAlternateSettings = 0,
        .bNumEndpoints      = 0,
        .bInterfaceClass    = 0,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface         = 0
    },

    /*
    // Endpoint descriptor
    {
        .bLength            = sizeof (usb_ep_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
        .bEndpointAddress   = EP_OUT_NUMBER(1),
        .bmAttributes       = EP_ATTR_BULK,
        .wMaxPacketSize     = EP_MAX_PKT_BULK_FS,
        .bInterval          = 0
    }
    */
};

// Language code string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [1];
} sd000 = {
    sizeof(sd000),
    USB_DESC_TYPE_STRING,
    { 0x0409 }
};

// Manufacturer string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [12];
} sd001 = {
    sizeof(sd001),
    USB_DESC_TYPE_STRING,
{   'u','O','S','-','e','m','b','e','d','d','e','d'
}};

// Product string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [8];
} sd002 = {
    sizeof(sd002),
    USB_DESC_TYPE_STRING,
{   'u','u','l','o','a','d','e','r'
}};

const void *usb_strings[] = {
    (const void *) &sd000,
    (const void *) &sd001,
    (const void *) &sd002
};


typedef uint32_t UULOAD_DEV_ID;

typedef struct _dev_features_t {
    UULOAD_DEV_ID           id;
    unsigned                block_size;
} dev_features_t;

typedef struct _mem_cmd_t {
    uint32_t    addr;
    uint32_t    size;
} mem_cmd_t;

static unsigned bytes_received = 0;
static uint8_t data_buf[UULOAD_FLASH_BLOCK_SIZE];
static dev_features_t dev_features = {0, UULOAD_FLASH_BLOCK_SIZE};

static int get_dev_id ()
{
    return 5;
}

static int correct_flash_address (unsigned addr)
{
    return (addr >= 0x08005000 && addr < 0x08020000);
}

static inline int not_adjusted (unsigned addr)
{
    return (addr & (UULOAD_FLASH_BLOCK_SIZE - 1));
}

static void reset_handler (usbdev_t *u, unsigned ep, int dir, void *tag)
{
    mdelay(1);
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_WWDT;
	ARM_WWDG->CFR = 0;
	ARM_WWDG->CR = 0xFF;
	ARM_WWDG->CR = 0xFF;
}

static void run_handler (usbdev_t *u, unsigned ep, int dir, void *tag)
{
}

static void loader_specific (usbdev_t *u, void *tag, usb_setup_pkt_t *setup, uint8_t **data, int *size)
{
    mem_cmd_t *pcmd = (mem_cmd_t *) *data;
    
    bytes_received += setup->wLength;
    
    if (USB_REQ_GET_TYPE(setup->bmRequestType) == USB_REQ_TYPE_SPECIFIC) {
        switch (setup->bRequest) {
        case UULOAD_REQ_FEATURES:
debug_printf ("REQUEST FEATURES\n");
            *data = (uint8_t *)&dev_features;
            *size = sizeof(dev_features);
            break;
        case UULOAD_REQ_ERASE:
debug_printf ("REQUEST ERASE address = 0x%08X\n", pcmd->addr);
            if (not_adjusted(pcmd->addr)) {
                *size = -1;
                break;
            }
            *size = 0;
            break;
        case UULOAD_REQ_ERASE_ALL:
debug_printf ("REQUEST ERASE ALL\n");
            *size = 0;
            break;
        case UULOAD_REQ_UPLOAD_DATA:
debug_printf ("REQUEST UPLOAD size = %d\n", *size);
            if (*size > sizeof(data_buf)) {
                *size = -1;
                break;
            }
            memcpy (data_buf, *data, *size);
            *size = 0;
            break;
        case UULOAD_REQ_DOWNLOAD_DATA:
debug_printf ("REQUEST DOWNLOAD size = %d\n", *size);
            if (*size > sizeof(data_buf)) {
                *size = -1;
                break;
            }
            memcpy (*data, data_buf, *size);
            break;
        case UULOAD_REQ_PROGRAM:
debug_printf ("REQUEST PROGRAM address = 0x%08X, size = %d\n", pcmd->addr, pcmd->size);
            if (not_adjusted(pcmd->addr) || !correct_flash_address(pcmd->addr)) {
                *size = -1;
                break;
            }
            *size = 0;
            break;
        case UULOAD_REQ_WRITE:
debug_printf ("REQUEST WRITE address = 0x%08X, size = %d\n", pcmd->addr, pcmd->size);
            if (*size > sizeof(data_buf)) {
                *size = -1;
                break;
            }
            memcpy ((void *) pcmd->addr, data_buf, pcmd->size);
            *size = 0;
            break;
        case UULOAD_REQ_READ:
debug_printf ("REQUEST READ address = 0x%08X, size = %d\n", pcmd->addr, pcmd->size);
            if (*size > sizeof(data_buf)) {
                *size = -1;
                break;
            }
            memcpy (data_buf, (void *) pcmd->addr, pcmd->size);
            *size = 0;
            break;
        case UULOAD_REQ_RESET:
debug_printf ("REQUEST RESET\n");
            usbdev_set_ack_handler (u, 0, USBDEV_DIR_OUT, reset_handler, 0);
            *size = 0;
            break;
        case UULOAD_REQ_RUN:
debug_printf ("REQUEST RUN\n");
            usbdev_set_ack_handler (u, 0, USBDEV_DIR_OUT, run_handler, 0);
            *size = 0;
            break;
        default:
            *size = -1;
        }
    }
}

static void task (void *arg)
{
    unsigned prev_bytes = 0;
    
    debug_printf ("Free memory: %d bytes\n", mem_available (&pool));
    for (;;) {
        //timer_delay (&timer, 1000);
        mdelay (1000);

        debug_printf ("disc: %d, bad_rq: %d, bad_tr: %d, bad_l: %d, ctl_fail: %d, out_mem: %d, rcv: %d, rate: %d\n",
            usb.rx_discards, usb.rx_bad_req, usb.rx_bad_trans, usb.rx_bad_len, usb.ctrl_failed, 
            usb.out_of_memory, bytes_received, bytes_received - prev_bytes);
        prev_bytes = bytes_received;
    }
}

void uos_init (void)
{
    extern unsigned __bss_end[], _estack[];

    debug_printf ("=============== USB LOADER ================\n");	
	
    mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
	
    //timer_init (&timer, KHZ, 100);

    usbdev_init (&usb, &pool, &device_descriptor);
    usbdev_add_config_desc (&usb, &config_descriptor);
    usbdev_set_string_table (&usb, usb_strings);
    
    dev_features.id = get_dev_id();
    
    mldr_usbdev_init (&usb, 10, &pool, &usb_lock);
    
    usbdev_set_iface_specific_handler (&usb, 0, loader_specific, 0);
    
    task_create (task, "task", "task", 1, task_space, sizeof (task_space));
}
