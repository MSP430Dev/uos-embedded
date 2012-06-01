#ifndef __USB_CONST_H__
#define __USB_CONST_H__

// === USB Specification Constants ===

//
// USB descriptor types
//
#define USB_DESC_TYPE_DEVICE         0x01    // bDescriptorType for a device descriptor.
#define USB_DESC_TYPE_CONFIG         0x02    // bDescriptorType for a configuration descriptor.
#define USB_DESC_TYPE_STRING         0x03    // bDescriptorType for a string descriptor.
#define USB_DESC_TYPE_IFACE          0x04    // bDescriptorType for an interface descriptor.
#define USB_DESC_TYPE_ENDPOINT       0x05    // bDescriptorType for an endpoint descriptor.
#define USB_DESC_TYPE_QUALIFIER      0x06    // bDescriptorType for a device qualifier.
#define USB_DESC_TYPE_OTHER_SPEED    0x07    // bDescriptorType for a other speed configuration.
#define USB_DESC_TYPE_IFACE_POWER    0x08    // bDescriptorType for interface power.
#define USB_DESC_TYPE_OTG            0x09    // bDescriptorType for an OTG descriptor.

//
// Attributes bits
//
#define USB_CONF_ATTR_REQUIRED       0x80                             // Required attribute
#define USB_CONF_ATTR_SELF_PWR       (0x40 | USB_CONF_ATTR_REQUIRED)  // Device is self powered.
#define USB_CONF_ATTR_REM_WAKE       (0x20 | USB_CONF_ATTR_REQUIRED)  // Device can request remote wakeup

//
// Endpoint attributes
//
#define EP_ATTR_IN          0x80        // Data flows from device to host
#define EP_ATTR_OUT         0x00        // Data flows from host to device
// Transfer Types
#define EP_ATTR_CONTROL     (0 << 0)    // Control endpoint
#define EP_ATTR_ISOCH       (1 << 0)    // Isochronous endpoint
#define EP_ATTR_BULK        (2 << 0)    // Bulk endpoint
#define EP_ATTR_INTR        (3 << 0)    // Interrupt endpoint
#define EP_ATTR_TRANSFER_MASK (3 << 0)
// Synchronization types (only for isochronous enpoints)
#define EP_ATTR_NO_SYNC     (0 << 2)    // No synchronization
#define EP_ATTR_ASYNC       (1 << 2)    // Asynchronous
#define EP_ATTR_ADAPT       (2 << 2)    // Adaptive synchronization
#define EP_ATTR_SYNC        (3 << 2)    // Synchronous
#define EP_ATTR_SYNC_MASK (3 << 2)
// Usage types (only for isochronous endpoints)
#define EP_ATTR_DATA        (0 << 4)    // Data Endpoint
#define EP_ATTR_FEEDBACK    (1 << 4)    // Feedback endpoint
#define EP_ATTR_IMP_FB      (2 << 4)    // Implicit Feedback data EP
#define EP_ATTR_USAGE_MASK (3 << 4)
// Max Packet Sizes
#define EP_MAX_PKT_INTR_LS  8       // Max low-speed interrupt packet
#define EP_MAX_PKT_INTR_FS  64      // Max full-speed interrupt packet
#define EP_MAX_PKT_ISOCH_FS 1023    // Max full-speed isochronous packet
#define EP_MAX_PKT_BULK_FS  64      // Max full-speed bulk packet
#define EP_LG_PKT_BULK_FS   32      // Large full-speed bulk packet
#define EP_MED_PKT_BULK_FS  16      // Medium full-speed bulk packet
#define EP_SM_PKT_BULK_FS   8       // Small full-speed bulk packet


#define EP_IN_NUMBER(n)		(0x80 | n)
#define EP_OUT_NUMBER(n)	(n)

//
// Standard requests
//

// bmRequestType field
#define USB_REQ_FROM_DEV        (1 << 7)
// Standard request types
#define USB_REQ_GET_TYPE(r)	(((r) >> 5) & 3)
#define USB_REQ_TYPE_STANDARD   0
#define USB_REQ_TYPE_CLASS      1
#define USB_REQ_TYPE_SPECIFIC   2
#define USB_REQ_TYPE_RESERVED   3
// Standard request receivers
#define USB_REQ_GET_RCV(r)	((r) & 5)
#define USB_REQ_RCV_DEVICE      0
#define USB_REQ_RCV_IFACE       1
#define USB_REQ_RCV_ENDPOINT    2
#define USB_REQ_RCV_OTHER       3

// bRequest field
#define USB_SR_GET_STATUS           0
#define USB_SR_CLEAR_FEATURE        1
#define USB_SR_SET_FEATURE          3
#define USB_SR_SET_ADDRESS          5
#define USB_SR_GET_DESCRIPTOR       6
#define USB_SR_SET_DESCRIPTOR       7
#define USB_SR_GET_CONFIGURATION    8
#define USB_SR_SET_CONFIGURATION    9
#define USB_SR_GET_INTERFACE        10
#define USB_SR_SET_INTERFACE        11
#define USB_SR_SYNCH_FRAME          12

//
// PID Values
//
#define PID_OUT                                 0x1     // PID for an OUT token
#define PID_ACK                                 0x2     // PID for an ACK handshake
#define PID_DATA0                               0x3     // PID for DATA0 data
#define PID_PING                                0x4     // Special PID PING
#define PID_SOF                                 0x5     // PID for a SOF token
#define PID_NYET                                0x6     // PID for a NYET handshake
#define PID_DATA2                               0x7     // PID for DATA2 data
#define PID_SPLIT                               0x8     // Special PID SPLIT
#define PID_IN                                  0x9     // PID for a IN token
#define PID_NAK                                 0xA     // PID for a NAK handshake
#define PID_DATA1                               0xB     // PID for DATA1 data
#define PID_PRE                                 0xC     // Special PID PRE (Same as PID_ERR)
#define PID_ERR                                 0xC     // Special PID ERR (Same as PID_PRE)
#define PID_SETUP                               0xD     // PID for a SETUP token
#define PID_STALL                               0xE     // PID for a STALL handshake
#define PID_MDATA                               0xF     // PID for MDATA data

#define PID_MASK_DATA                           0x03    // Data PID mask
#define PID_MASK_DATA_SHIFTED                  (PID_MASK_DATA << 2) // Data PID shift to proper position

//
// OTG Descriptor Constants
//
#define OTG_HNP_SUPPORT                         0x02    // OTG Descriptor bmAttributes - HNP support flag
#define OTG_SRP_SUPPORT                         0x01    // OTG Descriptor bmAttributes - SRP support flag

//
// Endpoint Directions
//
#define USB_IN_EP                               0x80    // IN endpoint mask
#define USB_OUT_EP                              0x00    // OUT endpoint mask

//
// Standard Device Requests
//
#define USB_REQUEST_GET_STATUS                  0       // Standard Device Request - GET STATUS
#define USB_REQUEST_CLEAR_FEATURE               1       // Standard Device Request - CLEAR FEATURE
#define USB_REQUEST_SET_FEATURE                 3       // Standard Device Request - SET FEATURE
#define USB_REQUEST_SET_ADDRESS                 5       // Standard Device Request - SET ADDRESS
#define USB_REQUEST_GET_DESCRIPTOR              6       // Standard Device Request - GET DESCRIPTOR
#define USB_REQUEST_SET_DESCRIPTOR              7       // Standard Device Request - SET DESCRIPTOR
#define USB_REQUEST_GET_CONFIGURATION           8       // Standard Device Request - GET CONFIGURATION
#define USB_REQUEST_SET_CONFIGURATION           9       // Standard Device Request - SET CONFIGURATION
#define USB_REQUEST_GET_INTERFACE               10      // Standard Device Request - GET INTERFACE
#define USB_REQUEST_SET_INTERFACE               11      // Standard Device Request - SET INTERFACE
#define USB_REQUEST_SYNCH_FRAME                 12      // Standard Device Request - SYNCH FRAME

#define USB_FEATURE_ENDPOINT_HALT               0       // CLEAR/SET FEATURE - Endpoint Halt
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP        1       // CLEAR/SET FEATURE - Device remote wake-up
#define USB_FEATURE_TEST_MODE                   2       // CLEAR/SET FEATURE - Test mode

//
// Setup Data Constants
//
#define USB_SETUP_HOST_TO_DEVICE                0x00    // Device Request bmRequestType transfer direction - host to device transfer
#define USB_SETUP_DEVICE_TO_HOST                0x80    // Device Request bmRequestType transfer direction - device to host transfer
#define USB_SETUP_TYPE_STANDARD                 0x00    // Device Request bmRequestType type - standard
#define USB_SETUP_TYPE_CLASS                    0x20    // Device Request bmRequestType type - class
#define USB_SETUP_TYPE_VENDOR                   0x40    // Device Request bmRequestType type - vendor
#define USB_SETUP_RECIPIENT_DEVICE              0x00    // Device Request bmRequestType recipient - device
#define USB_SETUP_RECIPIENT_INTERFACE           0x01    // Device Request bmRequestType recipient - interface
#define USB_SETUP_RECIPIENT_ENDPOINT            0x02    // Device Request bmRequestType recipient - endpoint
#define USB_SETUP_RECIPIENT_OTHER               0x03    // Device Request bmRequestType recipient - other

//
// OTG SET FEATURE Constants
//
#define OTG_FEATURE_B_HNP_ENABLE                3       // SET FEATURE OTG - Enable B device to perform HNP
#define OTG_FEATURE_A_HNP_SUPPORT               4       // SET FEATURE OTG - A device supports HNP
#define OTG_FEATURE_A_ALT_HNP_SUPPORT           5       // SET FEATURE OTG - Another port on the A device supports HNP

//
// Endpoint Transfer Types
//
#define USB_TRANSFER_TYPE_CONTROL               0x00    // Endpoint is a control endpoint.
#define USB_TRANSFER_TYPE_ISOCHRONOUS           0x01    // Endpoint is an isochronous endpoint.
#define USB_TRANSFER_TYPE_BULK                  0x02    // Endpoint is a bulk endpoint.
#define USB_TRANSFER_TYPE_INTERRUPT             0x03    // Endpoint is an interrupt endpoint.

//
// Standard Feature Selectors for CLEAR_FEATURE Requests
//
#define USB_FEATURE_ENDPOINT_STALL              0       // Endpoint recipient
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP        1       // Device recipient
#define USB_FEATURE_TEST_MODE                   2       // Device recipient

//
// Class Code Definitions
//
#define USB_HUB_CLASSCODE                       0x09    //  Class code for a hub.

enum {
    USB_STATE_DETACHED,
    USB_STATE_ATTACHED,
    USB_STATE_POWERED,
    USB_STATE_DEFAULT,
    USB_STATE_ADDRESSED,
    USB_STATE_CONFIGURED,
    USB_STATE_SUSPENDED
};

#endif
