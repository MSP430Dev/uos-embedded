#ifndef __USB_DESC_H__
#define __USB_DESC_H__

//
// Device descriptor
//
typedef struct __attribute__ ((packed)) _usb_dev_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Device descriptor type (must be USB_DESC_TYPE_DEVICE).
    uint16_t bcdUSB;                // USB Spec Release Number (BCD, 0x0200 for USB 2.0).
    uint8_t  bDeviceClass;          // Class code. 0xFF - Vendor specific.
    uint8_t  bDeviceSubClass;       // Subclass code.
    uint8_t  bDeviceProtocol;       // Protocol code. 0xFF - Vendor specific.
    uint8_t  bMaxPacketSize0;       // Maximum packet size for endpoint 0.
    uint16_t idVendor;              // Vendor ID.
    uint16_t idProduct;             // Product ID.
    uint16_t bcdDevice;             // Device release number (BCD).
    uint8_t  iManufacturer;         // Index of manufacturer string descriptor.
    uint8_t  iProduct;              // Index of product string descriptor.
    uint8_t  iSerialNumber;         // Index of serial number string descriptor.
    uint8_t  bNumConfigurations;    // Number of configurations of this device.
} usb_dev_desc_t;

//
// Configuration descriptor
//
typedef struct __attribute__ ((packed)) _usb_conf_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Configuration descriptor type (must be USB_DESC_TYPE_CONFIG or USB_DESC_TYPE_OTHER_SPEED).
    uint16_t wTotalLength;          // Total length of all descriptors for this configuration.
    uint8_t  bNumInterfaces;        // Number of interfaces in this configuration.
    uint8_t  bConfigurationValue;   // Self number of this configuration (1 based).
    uint8_t  iConfiguration;        // Index of configuration string descriptor.
    uint8_t  bmAttributes;          // Configuration characteristics.
    uint8_t  bMaxPower;             // Maximum power for this configuration.
} usb_conf_desc_t;

//
// String descriptor
//
//typedef struct __attribute__ ((packed)) _usb_string_desc_t
//{
//    uint8_t  bLength;               // Length of this descriptor.
//    uint8_t  bDescriptorType;       // String descriptor type (must be USB_DESC_TYPE_STRING).
//} usb_string_desc_t;

//
// Interface descriptor
//
typedef struct __attribute__ ((packed)) _usb_iface_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Interface descriptor type (must be USB_DESC_TYPE_IFACE).
    uint8_t  bInterfaceNumber;      // Number of this interface (0 based).
    uint8_t  bAlternateSettings;    // Value of this alternate interface setting.
    uint8_t  bNumEndpoints;         // Number of endpoints in this interface (excluding EP0).
    uint8_t  bInterfaceClass;       // Class code (assigned by the USB-IF).  0xFF-Vendor specific.
    uint8_t  bInterfaceSubClass;    // Subclass code (assigned by the USB-IF).
    uint8_t  bInterfaceProtocol;    // Protocol code (assigned by the USB-IF).  0xFF-Vendor specific.
    uint8_t  iInterface;            // Index of String Descriptor describing the interface.
} usb_iface_desc_t;

//
// Endpoint descriptor
//
typedef struct __attribute__ ((packed)) _usb_ep_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Endpoint descriptor type (must be USB_DESC_TYPE_ENDPOINT).
    uint8_t  bEndpointAddress;      // Endpoint address. Bit 7 indicates direction (0=OUT, 1=IN).
    uint8_t  bmAttributes;          // Endpoint transfer type.
    uint16_t wMaxPacketSize;        // Maximum packet size.
    uint8_t  bInterval;             // Polling interval in frames.
} usb_ep_desc_t;

//
// Device qualifier descriptor
//
typedef struct __attribute__ ((packed)) _usb_qualifier_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // Device qualifier descriptor type (must be USB_DESC_TYPE_QUALIFIER).
    uint16_t bcdUSB;                // USB Spec Release Number (BCD, 0x0200 for USB 2.0).
    uint8_t  bDeviceClass;          // Device class code.
    uint8_t  bDeviceSubClass;       // Device sub-class code.
    uint8_t  bDeviceProtocol;       // Device protocol.
    uint8_t  bMaxPacketSize0;       // Maximum packet size for endpoint 0.
    uint8_t  bNumConfigurations;    // Number of "other-speed" configurations.
    uint8_t  bReserved;             // Always zero.
} usb_qualifier_desc_t;

//
// OTG descriptor
//
typedef struct __attribute__ ((packed)) _usb_otg_desc_t
{
    uint8_t  bLength;               // Length of this descriptor.
    uint8_t  bDescriptorType;       // OTG descriptor type (must be USB_DESC_TYPE_OTG).
    uint8_t  bmAttributes;          // OTG attributes.
} usb_otg_desc_t;

//
// Setup packet
//
typedef struct __attribute__ ((packed)) _usb_setup_pkt_t
{
    uint8_t  bmRequestType;         // Request type.
    uint8_t  bRequest;              // Request.
    uint16_t wValue;                // Depends on bRequest.
    uint16_t wIndex;                // Depends on bRequest.
    uint16_t wLength;               // Depends on bRequest.
} usb_setup_pkt_t;


#endif
