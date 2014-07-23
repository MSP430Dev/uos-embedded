#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <time.h>

// Device USB vendor ID
#define VENDOR_ID        0x0111

// Device USB product ID
#define PRODUCT_ID       0x0001

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
lusb_debug_mode_t debug_mode = LUSB_DEBUG_INFO;
uint8_t databuf[8192];

unsigned bulk_in_ep = 0;
unsigned bulk_in_size;
unsigned bulk_out_ep = 0;

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
    struct libusb_device_descriptor dev_desc;
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
        int r = libusb_get_device_descriptor(dev, &dev_desc);
        if (r < 0) {
            if (debug_mode >= LUSB_DEBUG_WARNING)
                fprintf(stderr, "Failed to get USB device descriptor, libusb error code: %d!\n", r);
            continue;
        }

        if (debug_mode >= LUSB_DEBUG_MORE_INFO)
            fprintf(stderr, "Vendor: 0x%04X, product 0x%04X\n", dev_desc.idVendor, dev_desc.idProduct);

        if (dev_desc.idVendor == VENDOR_ID &&
                dev_desc.idProduct == PRODUCT_ID) {
            struct libusb_config_descriptor *pconf_desc;
            r = libusb_get_active_config_descriptor(dev, &pconf_desc);
            if (r < 0) {
                if (debug_mode >= LUSB_DEBUG_WARNING)
                    fprintf(stderr, "Failed to read configuration descriptor, libusb error code: %d!\n", r);
                continue;
            }
            const struct libusb_interface_descriptor *piface_desc;
            piface_desc = &pconf_desc->interface->altsetting[0];
            int i;
            for (i = 0; i < piface_desc->bNumEndpoints; ++i) {
                const struct libusb_endpoint_descriptor *pep_desc = &piface_desc->endpoint[i];
                if ((pep_desc->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK) {
                    if (pep_desc->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                        bulk_in_ep = pep_desc->bEndpointAddress;
                        bulk_in_size = pep_desc->wMaxPacketSize;
                    } else {
                        bulk_out_ep = pep_desc->bEndpointAddress;
                    }
                }
            }
            if (debug_mode >= LUSB_DEBUG_INFO) {
                fprintf(stderr, "BULK IN endpoint: ");
                if (bulk_in_ep)
                    fprintf(stderr, "%d\n", bulk_in_ep & 0xF);
                else fprintf(stderr, "not found!\n");
                fprintf(stderr, "BULK OUT endpoint: ");
                if (bulk_out_ep)
                    fprintf(stderr, "%d\n", bulk_out_ep & 0xF);
                else fprintf(stderr, "not found!\n");
            }
            r = libusb_open(dev, &handle);
            if (r < 0) {
                if (debug_mode >= LUSB_DEBUG_WARNING)
                    fprintf(stderr, "Failed to open USB device, libusb error code: %d!\n", r);
                continue;
            }
#ifndef WIN32
            r = libusb_detach_kernel_driver(handle, 0);
            if (r < 0 && r != LIBUSB_ERROR_NOT_FOUND)
                continue;
#endif
            r = libusb_claim_interface(handle, 0);
            if (r < 0) {
                if (debug_mode >= LUSB_DEBUG_ERROR)
                    fprintf(stderr, "Failed to claim interface, libusb error code: %d!\n", r);
                continue;
            }
        }
    }

    if (handle == 0)
        finish(-3, "Device for test not found!");

    libusb_free_device_list(devs, 1);

    device_initialized = 1;
}

void device_deinit()
{
    if (debug_mode >= LUSB_DEBUG_MORE_INFO)
        fprintf(stderr, "Deinitialization\n");

    libusb_close(handle);
}

void do_test(int up, int bulk, unsigned packet_size)
{
    int r;
    unsigned char v = 0;
    unsigned i;
    int actual_len;
    time_t t0, t1;
    double seconds;
    unsigned trx_size = 0, cum_trx_size = 0;
    unsigned bad_data = 0;
    int work_mode = 0;

    if (bulk && !up)
        packet_size -= packet_size % bulk_in_size;

    time(&t0);

    for (;;) {
        if (up) {
            for (i = 0; i < packet_size; ++i)
                databuf[i] = v++;
        }

        if (bulk) {
            r = libusb_bulk_transfer(handle, (up) ? bulk_out_ep : bulk_in_ep,
                                     databuf, packet_size, &actual_len, 1000);
        } else {
            r = libusb_control_transfer(handle, ((up) ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN) |
                                        LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE,
                                        0, 0, 0, databuf, packet_size, 1000);
        }
        if (r < 0) {
            if (debug_mode >= LUSB_DEBUG_ERROR) {
                if (up)
                    fprintf(stderr, "Failed to send data to the device, libusb error code: %d!\n", r);
                else
                    fprintf(stderr, "Failed to receive data from the device, libusb error code: %d!\n", r);
            }
            finish(-10, 0);
        }
        trx_size += packet_size;
        cum_trx_size += packet_size;

        if (!up) {
            for (i = 0; i < packet_size; ++i)
                if (databuf[i] != v++) {
                    if (work_mode) {
                        bad_data++;
                        work_mode = 1;
                    }
                    v = databuf[i] + 1;
                }
        }

        time(&t1);
        seconds = difftime(t1, t0);
        if (seconds > 1.0) {
            if (up) fprintf(stderr, "Sent: ");
            else fprintf(stderr, "Received: ");
            fprintf(stderr, "%12u bytes, rate: %u bytes/sec",
                    cum_trx_size, (unsigned)(trx_size / seconds));
            if (up) fprintf(stderr, "\n");
            else fprintf(stderr, ", bad data: %d\n", bad_data);
            memcpy(&t0, &t1, sizeof(time_t));
            trx_size = 0;
        }
    }
}

void usage()
{
    printf ("Probe:\n");
    printf ("       test-usbdev\n");
    printf ("\nTest rx speed:\n");
    printf ("       test-usbdev [-b] -r <packet_size>\n");
    printf ("\nTest tx speed:\n");
    printf ("       test-usbdev [-b] -t <packet_size>\n");
    printf ("\n -b   -   test BULK transmission, otherwise use endpoint 0.");
    printf ("\n\nNote:");
    printf ("\nFor BULK IN transmission (rx) <packet_size> will be");
    printf ("\ntruncated to be multiply of endpoint maximum size.");
    printf ("\n");
}


int main(int argc, char **argv)
{
    int mode = PROBE;
    unsigned packet_size = 4096;
    int use_bulk = 0;

    static const struct option long_options[] = {
        { "help",        0, 0, 'h' },
        { NULL,          0, 0, 0 },
    };

    printf ("\nTest of USB device performance through libusb\n\n");

    char ch;
    while ((ch = getopt_long (argc, argv, "br:t:h",
      long_options, 0)) != -1) {
        switch (ch) {
        case 'b':
            use_bulk++;
            continue;
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

    //if (packet_size > 4096)
    //    finish (-1, "Wrong size of packet. Must be no more than 4096!");

    device_init();

    if (mode == PROBE) finish (0, "Device for test detected successfully!");

    switch (mode) {
    case RX:
        do_test(0, use_bulk, packet_size);
        break;
    case TX:
        do_test(1, use_bulk, packet_size);
        break;
    default:
        usage();
        finish (-11, 0);
    }

    printf("Done successfully!\n");
    device_deinit();
    return 0;
}

