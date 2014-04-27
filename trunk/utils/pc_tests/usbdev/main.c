#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>

// Device USB vendor ID
#define VENDOR_ID        0x04d8

// Device USB product ID
#define PRODUCT_ID       0x003e

//
// Debug modes (higher the level, more messages).
//
typedef enum {
    LUSB_DEBUG_SILENT,        // no debug messages
    LUSB_DEBUG_ERROR,         // only error messages
    LUSB_DEBUG_WARNING,       // error and warning
    LUSB_DEBUG_INFO,          // error, warning and info
    LUSB_DEBUG_MORE_INFO,     // all of above with more info
    LUSB_DEBUG_LIBUSB         // all of above plus turn on libusb debug messages
} lusb_debug_mode_t;

enum {
    PROBE,
    RX,
    TX
};

int device_initialized = 0;
libusb_device_handle *handle;
lusb_debug_mode_t debug_mode = LUSB_DEBUG_LIBUSB;
uint8_t databuf[4096];


void device_deinit();

void finish (int res, const char *msg)
{
    if (msg)
        fprintf (stderr, "%s\n", msg);
    if (device_initialized)
        device_deinit();
    exit (res);
}


void device_init()
{
    libusb_device **devs;
    libusb_device *dev;
    struct libusb_device_descriptor desc;
    int r;

    if (debug_mode >= LUSB_DEBUG_MORE_INFO)
        fprintf(stderr, "Initialization\n");

    r = libusb_init(NULL);
    if (r < 0)
        finish(-2, "Failed to initialize libusb!");

    if (debug_mode >= LUSB_DEBUG_LIBUSB)
        libusb_set_debug(NULL, 3);

    r = libusb_get_device_list(NULL, &devs);
    if (r < 0)
        finish(-2, "Failed to list USB devices!");

    int i = 0;
    while ((dev = devs[i++]) != NULL) {
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            if (debug_mode >= LUSB_DEBUG_WARNING)
                fprintf(stderr, "Failed to get USB device descriptor, libusb error code: %d!\n", r);
            continue;
        }

        if (debug_mode >= LUSB_DEBUG_MORE_INFO)
            fprintf(stderr, "Vendor: 0x%04X, product 0x%04X\n", desc.idVendor, desc.idProduct);

        if (desc.idVendor == VENDOR_ID &&
                desc.idProduct == PRODUCT_ID) {
            r = libusb_open(dev, &handle);
            if (r < 0) {
                if (debug_mode >= LUSB_DEBUG_ERROR)
                    fprintf(stderr, "Failed to open USB device, libusb error code: %d!\n", r);
                continue;
            }
        }
    }

    libusb_free_device_list(devs, 1);

    device_initialized = 1;
}

void device_deinit()
{
    if (debug_mode >= LUSB_DEBUG_MORE_INFO)
        fprintf(stderr, "Deinitialization\n");

    libusb_close(handle);
}

void do_test(int up, unsigned packet_size)
{
    int r;

    unsigned char i = 0;
    for (;;) {
        if (up)
            memset(databuf, i++, packet_size);

        r = libusb_control_transfer(handle, ((up) ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN) |
                                    LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE,
                                    0, 0, 0, databuf, packet_size, 1000);
        if (r < 0) {
            if (debug_mode >= LUSB_DEBUG_ERROR) {
                if (up)
                    fprintf(stderr, "Failed to send data to the device, libusb error code: %d!\n", r);
                else
                    fprintf(stderr, "Failed to receive data from the device, libusb error code: %d!\n", r);
            }
            finish(-10, 0);
        }
    }
}

void usage()
{
    printf ("Probe:\n");
    printf ("       test-usbdev\n");
    printf ("\nTest rx speed:\n");
    printf ("       test-usbdev -r <packet_size>\n");
    printf ("\nTest tx speed:\n");
    printf ("       test-usbdev -t <packet_size>\n");
    printf ("\nMaximum packet_size is 4096 bytes.\n");
    printf ("\n");
}


int main(int argc, char **argv)
{
    int mode = PROBE;
    unsigned packet_size = 4096;

    static const struct option long_options[] = {
        { "help",        0, 0, 'h' },
        { NULL,          0, 0, 0 },
    };

    printf ("\nTest of USB device performance through libusb\n\n");

    char ch;
    while ((ch = getopt_long (argc, argv, "r:t:h",
      long_options, 0)) != -1) {
        switch (ch) {
        case 'r':
            mode = RX;
            packet_size = strtoul (optarg, 0, 0);
            continue;
        case 't':
            mode = TX;
            packet_size = strtoul (optarg, 0, 0);
            continue;
        case 'h':
            usage();
            finish (0, 0);
        }
    }

    if (packet_size > 4096)
        finish (-1, "Wrong size of packet. Must be no more than 4096!");

    device_init();

    if (mode == PROBE) finish (0, "Device for test detected successfully!");

    switch (mode) {
    case RX:
        do_test(0, packet_size);
        break;
    case TX:
        do_test(1, packet_size);
        break;
    default:
        usage();
        finish (-11, 0);
    }

    printf("Done successfully!\n");
    device_deinit();
    return 0;
}

