//
// Тест демонстрирует работу стека USB на примере НID-устройства (мышки).
// После подключения в системе должно появиться устройство USB-Device 
// производства UOS-embedded, и указатель мышки должен описывать на экране
// равнобедренный треугольник.
//

#include "runtime/lib.h"
#include "kernel/uos.h"
#include <timer/timer.h>
#include <mem/mem.h>
#include <usb/usbdev.h>
#include <usb/usb_struct.h>
#include <usb/usb_const.h>
#include <usb/hid_const.h>
#include <usb/hiddev.h>
#include <milandr/usbdevhal.h>

ARRAY (task_space, 0x400);

usbdev_t usb;
hiddev_t hid;
mem_pool_t pool;
timer_t timer;
mutex_t usb_lock;

static const usb_dev_desc_t device_descriptor = {
    .bLength                = sizeof (usb_dev_desc_t),
    .bDescriptorType        = USB_DESC_TYPE_DEVICE,
    .bcdUSB                 = 0x0200,
    .bDeviceClass           = 0,
    .bDeviceSubClass        = 0,
    .bDeviceProtocol        = 0,
    .bMaxPacketSize0        = USBDEV_EP0_MAX_SIZE,
    .idVendor               = 0x04d8,
    .idProduct              = 0x003e,
    .bcdDevice              = 0x0100,
    .iManufacturer          = 1,
    .iProduct               = 2,
    .iSerialNumber          = 0,
    .bNumConfigurations     = 1
};

unsigned char mouse_report[] =
{
    HID_USAGE_PAGE_1B (HUP_GENERIC_DESKTOP),
    HID_USAGE_1B (HU_MOUSE),
    HID_COLLECTION (HID_APPLICATION),
        HID_USAGE_1B (HU_POINTER),
        HID_COLLECTION (HID_PHYSICAL),
            HID_USAGE_PAGE_1B (HUP_BUTTONS),
            HID_USAGE_MINIMUM (1),
            HID_USAGE_MAXIMUM (3),
            HID_LOGICAL_MINIMUM_1B (0),
            HID_LOGICAL_MAXIMUM_1B (1),
            HID_REPORT_COUNT (3),
            HID_REPORT_SIZE (1),
            HID_INPUT (HID_DATA | HID_VARIABLE | HID_ABSOLUTE),
            HID_REPORT_COUNT (1),
            HID_REPORT_SIZE (5),
            HID_INPUT (HID_CONSTANT),
            HID_USAGE_PAGE_1B (HUP_GENERIC_DESKTOP),
            HID_USAGE_1B (HU_X),
            HID_USAGE_1B (HU_Y),
            HID_LOGICAL_MINIMUM_1B (-127),
            HID_LOGICAL_MAXIMUM_1B (127),
            HID_REPORT_SIZE (8),
            HID_REPORT_COUNT (2),
            HID_INPUT (HID_DATA | HID_VARIABLE | HID_RELATIVE),
        HID_END_COLLECTION,
    HID_END_COLLECTION
};

typedef struct __attribute__ ((packed)) _this_conf_desc_t
{
    usb_conf_desc_t     conf;
    usb_iface_desc_t    iface;
    hid_1desc_t         hid;
    usb_ep_desc_t       ep;
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
        .bNumEndpoints      = 1,
        .bInterfaceClass    = USB_HID_INTERFACE_CLASS,
        .bInterfaceSubClass = USB_HID_BOOT_SUBCLASS,
        .bInterfaceProtocol = USB_HID_PROT_MOUSE,
        .iInterface         = 0
    },
    
    // HID descriptor
    {
        .bLength            = sizeof (hid_1desc_t),
        .bDescriptorType    = USB_DESC_TYPE_HID,
        .bcdHID             = 0x111,
        .bCountryCode       = 0,
        .bNumDescriptors    = 1,
        .bReportType        = USB_DESC_TYPE_REPORT,
        .wReportLength      = sizeof (mouse_report)
    },
    
    // Endpoint descriptor
    {
        .bLength            = sizeof (usb_ep_desc_t),
        .bDescriptorType    = USB_DESC_TYPE_ENDPOINT,
        .bEndpointAddress   = EP_IN_NUMBER(1),
        .bmAttributes       = EP_ATTR_INTR | EP_ATTR_NO_SYNC | EP_ATTR_DATA,
        .wMaxPacketSize     = 8,
        .bInterval          = 10
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


char out_rep[] = {0, 0, 0};

static void task (void *arg)
{
#define LEFT 0
#define RIGHT 1
#define BOTTOM 2
    int side = LEFT;
#define LENGTH 100
    int step = 0;
    
    debug_printf ("Free memory: %d bytes\n", mem_available (&pool));
    for (;;) {
        switch (side) {
        case LEFT:
            out_rep[1] = -1;
            out_rep[2] = 1;
            if (++step >= LENGTH) {
                side = BOTTOM;
                step = 0;
            }
        break;
        case RIGHT:
            out_rep[1] = -1;
            out_rep[2] = -1;
            if (++step >= LENGTH) {
                side = LEFT;
                step = 0;
            }
        break;
        case BOTTOM:
            out_rep[1] = 1;
            out_rep[2] = 0;
            if (++step >= 2 * LENGTH) {
                side = RIGHT;
                step = 0;
            }
        break;
        }
        hiddev_output_report (&hid, 0, (uint8_t *) out_rep);
    }
}

void uos_init (void)
{
    extern unsigned __bss_end[], _estack[];

    debug_printf ("=============== TEST-HID ================\n");	
	
    mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);
	
    timer_init (&timer, KHZ, 128);

    usbdev_init (&usb, &pool, &device_descriptor);
    usbdev_add_config_desc (&usb, &config_descriptor);
    usbdev_set_string_table (&usb, usb_strings);
    
    hiddev_init (&hid, &usb, 0, config_descriptor.ep.bEndpointAddress, &pool, &usb_lock);
    hiddev_set_report_desc (&hid, 0, mouse_report, sizeof (mouse_report), 3, 0, 0);
    //hiddev_output_report (&hid, 0, out_rep);
    
    mldr_usbdev_init (&usb, 10, &pool, &usb_lock);
    task_create (task, "task", "task", 1, task_space, sizeof (task_space));
}
