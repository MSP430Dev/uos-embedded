//
// Тест демонстрирует подключение простого устройства USB с использованием MAX342x.
// После подключения в системе должно появиться устройство USB-Device 
// производства UOS-embedded.
// В Linux dmesg возвращает что-то вроде этого:
// [262157.153560] usb 4-5: New USB device found, idVendor=04d8, idProduct=003e
// [262157.153571] usb 4-5: New USB device strings: Mfr=1, Product=2, SerialNumber=0
// [262157.153580] usb 4-5: Product: USB-Device
// [262157.153587] usb 4-5: Manufacturer: UOS-embedded
//
// Производительность передач типа CONTROL и BULK с контролем целостности передачи
// можно померять с помощью программы test-usbdev (utils/pc_tests/usbdev).
//

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/spi.h>
#include <mem/mem.h>
#include <usb/usbdev.h>
#include <usb/usb_struct.h>
#include <usb/usb_const.h>
#include <milandr/usbdevhal.h>
#include <timer/timer.h>

#define SPI_NUM     1
#define SPI_CS      0
#define SPI_FREQUENCY   15000000

#define USB_IRQ     0

ARRAY (task, 1000);
elvees_spim_t spi;
spi_message_t msg;
usbdev_t usb;
mem_pool_t pool;
mutex_t usb_lock;
timer_t timer;

uint8_t buf[8192] __attribute__((aligned(8)));


static const usb_dev_desc_t device_descriptor = {
    sizeof (usb_dev_desc_t),
    USB_DESC_TYPE_DEVICE,
    0x0200,
    0,
    0,
    0,
    USBDEV_EP0_MAX_SIZE,
    0x0111,
    0x0001,
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
    usb_ep_desc_t       ep_out;
    usb_ep_desc_t       ep_in;
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
        .bNumEndpoints      = 2,
        .bInterfaceClass    = 0,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface         = 0
    },
    
    
    // Endpoint OUT descriptor
    {
        .bLength            = sizeof (usb_ep_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
        .bEndpointAddress   = EP_OUT_NUMBER(1),
        .bmAttributes       = EP_ATTR_BULK | EP_ATTR_NO_SYNC | EP_ATTR_DATA,
        .wMaxPacketSize     = EP_MAX_PKT_BULK_FS,
        .bInterval          = 0
    },
    
    // Endpoint IN descriptor
    {
        .bLength            = sizeof (usb_ep_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
        .bEndpointAddress   = EP_IN_NUMBER(2),
        .bmAttributes       = EP_ATTR_BULK | EP_ATTR_NO_SYNC | EP_ATTR_DATA,
        .wMaxPacketSize     = EP_MAX_PKT_BULK_FS,
        .bInterval          = 0
    }
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
{   'U','O','S','-','e','m','b','e','d','d','e','d'
}};

// Product string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [10];
} sd002 = {
    sizeof(sd002),
    USB_DESC_TYPE_STRING,
{   'U','S','B','-','D','e','v','i','c','e'
}};

const void *usb_strings[] = {
    (const void *) &sd000,
    (const void *) &sd001,
    (const void *) &sd002
};


static unsigned bytes_received = 0;
static unsigned bytes_transmitted = 0;
static unsigned bad_data = 0;

static int ep0_specific_handler (usbdev_t *u, void *tag, 
    usb_setup_pkt_t *setup, uint8_t **data, int *size)
{
    static uint8_t v = 0;
    unsigned i;

    if (setup->bmRequestType & USB_REQ_FROM_DEV) {
        bytes_transmitted += setup->wLength;
        for (i = 0; i < setup->wLength; ++i)
            buf[i] = v++;
        *data = buf;
        *size = setup->wLength;
    } else {
        uint8_t *databuf = *data;
        bytes_received += setup->wLength;
        *size = 0;
        for (i = 0; i < setup->wLength; ++i) {
            if (databuf[i] != v++) {
                bad_data++;
                v = databuf[i] + 1;
            }
        }
    }
    return USBDEV_ACK;
}

static int bulk_in_handler (usbdev_t *u, void *tag, 
    usb_setup_pkt_t *setup, uint8_t **data, int *size)
{
    static uint8_t v = 0;
    unsigned i;
    
    bytes_transmitted += 4096;
    for (i = 0; i < 4096; ++i)
        buf[i] = v++;
    *data = buf;
    *size = 4096;
    return USBDEV_ACK;
}

static int bulk_out_handler (usbdev_t *u, void *tag, 
    usb_setup_pkt_t *setup, uint8_t **data, int *size)
{
    static uint8_t v = 0;
    uint8_t *databuf = *data;
    unsigned i;
    
//debug_printf("out_handler, size = %d\n", *size);
    bytes_received += *size;
    for (i = 0; i < *size; ++i)
        if (databuf[i] != v++) {
            bad_data++;
            v = databuf[i] + 1;
        }
    return USBDEV_ACK;
}

static void hello (void *arg)
{
    unsigned prev_bytes_rx = 0;
    unsigned prev_bytes_tx = 0;
    
    debug_printf ("Free memory: %d bytes\n", mem_available (&pool));
    usbdev_ack_in (&usb, config_descriptor.ep_in.bEndpointAddress & 0xF, buf, sizeof(buf));
    for (;;) {
        //timer_delay (&timer, 1000);
        mdelay (1000);

        debug_printf ("disc: %d, bad_rq: %d, bad_tr: %d, bad_l: %d, ctl_fail: %d, out_mem: %d, rx: %d, rx rate: %d, tx: %d, tx rate: %d, bad_data: %d\n",
            usb.rx_discards, usb.rx_bad_req, usb.rx_bad_trans, usb.rx_bad_len, usb.ctrl_failed, 
            usb.out_of_memory, bytes_received, bytes_received - prev_bytes_rx, bytes_transmitted, bytes_transmitted - prev_bytes_tx, bad_data);
        prev_bytes_rx = bytes_received;
        prev_bytes_tx = bytes_transmitted;
    }
}

void uos_init (void)
{
	debug_printf("\n\nTesting MAX3421E\n");
    
	/* Выделяем место для динамической памяти */
	extern unsigned __bss_end[], _estack[];
    mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
    
    timer_init(&timer, KHZ, 1);
    
    usbdev_init (&usb, &pool, &device_descriptor);
    usbdev_add_config_desc (&usb, &config_descriptor);
    usbdev_set_string_table (&usb, usb_strings);
    usbdev_set_iface_specific_handler (&usb, 0, ep0_specific_handler, 0);
    usbdev_set_ep_specific_handler (&usb, config_descriptor.ep_out.bEndpointAddress, 
        USBDEV_DIR_OUT, bulk_out_handler, 0);
    usbdev_set_ep_specific_handler (&usb, config_descriptor.ep_in.bEndpointAddress & 0xF, 
        USBDEV_DIR_IN, bulk_in_handler, 0);

    mldr_usbdev_init (&usb, 10, &pool, &usb_lock);    
    task_create (hello, 0, "hello", 1, task, sizeof (task));
}
