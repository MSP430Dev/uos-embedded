/*
 * The software supplied herewith by Microchip Technology Incorporated
 * (the 'Company') for its PIC® Microcontroller is intended and
 * supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <microchip/usb.h>
#include <microchip/usb_function_hid.h>
#include "devcfg.h"

/*
 * Process the received data (hid_report_out) and
 * prepare the data for transmit (hid_report_in).
 */
void process_data()
{
    unsigned int count;

    for (count=0; count<HID_OUTPUT_REPORT_BYTES; count++) {
        hid_report_in[count] = hid_report_out[count];
    }
}

/*
 * This routine will poll for a received Input report, process it
 * and send an Output report to the host.
 * Both directions use interrupt transfers.
 * The ownership of the USB buffers will change according
 * to the required operation.
 */
void send_receive()
{
    static int usb_state = 'r';
    static USB_HANDLE last_transmit = 0;
    static USB_HANDLE last_receive = 0;

    switch (usb_state) {
    case 'r':
        if (! HIDRxHandleBusy (last_receive)) {
            // The CPU owns the endpoint. Start receiving data.
            last_receive = HIDRxPacket (HID_EP,
                (unsigned char*) &hid_report_out,
                HID_INT_OUT_EP_SIZE);
            usb_state = 'p';
        }
        break;
    case 'p':
        if (! HIDRxHandleBusy (last_receive)) {
            // The CPU owns the endpoint.
            if (last_receive->CNT > 0) {
                // Data was received. Copy it to the output buffer for sending.
                process_data();

                // Ready to transmit the received data back to the host.
                usb_state = 't';
            } else {
                // No data was received. Return to checking for new received data.
                usb_state = 'r';
            }
        }
        break;
    case 't':
        if (! HIDTxHandleBusy (last_transmit)) {
            // The CPU owns the endpoint. Start sending data.
            last_transmit = HIDTxPacket (HID_EP,
                (unsigned char*) &hid_report_in,
                HID_INPUT_REPORT_BYTES);

            // Return to checking for new received data.
            usb_state = 'r';
        }
        break;
    default:
        // Cannot happen.
        break;
    }
}

/*
 * Check USB status, receive and send data.
 */
void poll_usb()
{
    // Check bus status and service USB interrupts.
    USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
                      // this function periodically.  This function will take care
                      // of processing and responding to SETUP transactions
                      // (such as during the enumeration process when you first
                      // plug in).  USB hosts require that USB devices should accept
                      // and process SETUP packets in a timely fashion.  Therefore,
                      // when using polling, this function should be called
                      // frequently (such as once about every 100 microseconds) at any
                      // time that a SETUP packet might reasonably be expected to
                      // be sent by the host to your device.  In most cases, the
                      // USBDeviceTasks() function does not take very long to
                      // execute (~50 instruction cycles) before it returns.

    // When USB link established - receive and send data
    if (USBDeviceState >= CONFIGURED_STATE &&
        ! (U1PWRC & PIC32_U1PWRC_USUSPEND))
    {
        send_receive();
    }
}

/*
 * Display a picture on LoL shield.
 * Duration in milliseconds is specified.
 */
void lol (msec, data)
    unsigned msec;
    const short *data;
{
    /* Number of control pins for LoL Shield. */
    #define LOL_NPINS   12

    /* Number of rows on LoL Shield. */
    #define LOL_NROW    9

    /* Number of columns on LoL Shield. */
    #define LOL_NCOL    14

    /* Sequence of pins to set high during refresh cycle. */
    static const unsigned high [LOL_NPINS] = {
        1 << 15,        /* RB15 - labeled D13 on board */
        1 << 13,        /* RB13 - D12 */
        1 << 5,         /* RB5  - D11 */
        1 << 1,         /* RA1  - D10 */
        1 << 10,        /* RA10 - D9 */
        1 << 7,         /* RB7  - D8 */
        0x10000 << 7,   /* RC7  - D7 */
        0x10000 << 6,   /* RC6  - D6 */
        0x10000 << 5,   /* RC5  - D5 */
        0x10000 << 4,   /* RC4  - D4 */
        0x10000 << 3,   /* RC3  - D3 */
        0x10000 << 2,   /* RC2  - D2 */
    };

    /* Remap pixels to pin indexes. */
    static const unsigned char lol_map [LOL_NROW*LOL_NCOL*2] =
    {
        0,8,0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,9,9,0,0,10,10,0,0,11,11,0,
        1,8,1,7,1,6,1,5,1,4,1,3,1,2,1,0,1,9,9,1,1,10,10,1,1,11,11,1,
        2,8,2,7,2,6,2,5,2,4,2,3,2,1,2,0,2,9,9,2,2,10,10,2,2,11,11,2,
        3,8,3,7,3,6,3,5,3,4,3,2,3,1,3,0,3,9,9,3,3,10,10,3,3,11,11,3,
        4,8,4,7,4,6,4,5,4,3,4,2,4,1,4,0,4,9,9,4,4,10,10,4,4,11,11,4,
        5,8,5,7,5,6,5,4,5,3,5,2,5,1,5,0,5,9,9,5,5,10,10,5,5,11,11,5,
        6,8,6,7,6,5,6,4,6,3,6,2,6,1,6,0,6,9,9,6,6,10,10,6,6,11,11,6,
        7,8,7,6,7,5,7,4,7,3,7,2,7,1,7,0,7,9,9,7,7,10,10,7,7,11,11,7,
        8,7,8,6,8,5,8,4,8,3,8,2,8,1,8,0,8,9,9,8,8,10,10,8,8,11,11,8,
    };

    unsigned row, mask, amask, bmask, cmask;
    const unsigned char *map;
    unsigned low [LOL_NPINS];

    /* Clear pin masks. */
    for (row = 0; row < LOL_NPINS; row++)
        low [row] = 0;

    /* Convert image to array of pin masks. */
    for (row = 0; row < LOL_NROW; row++) {
        mask = *data++ & ((1 << LOL_NCOL) - 1);
        map = &lol_map [row * LOL_NCOL * 2];
        while (mask != 0) {
            if (mask & 1) {
                low [map[0]] |= high [map[1]];
            }
            map += 2;
            mask >>= 1;
        }
    }
    amask = high[3] | high[4];
    bmask = high[0] | high[1] | high[2] | high[5];
    cmask = (high[6] | high[7] | high[8] | high[9] | high[10] |
             high[11]) >> 16;

    /* Display the image. */
    if (msec < 1)
        msec = 20;
    while (msec-- > 0) {
        for (row = 0; row < LOL_NPINS; row++) {
            /* Set all pins to tristate. */
            TRISASET = amask;
            TRISBSET = bmask;
            TRISCSET = cmask;

            /* Set one pin to high. */
            mask = high [row];
            if (row >= 6) {
                mask >>= 16;
                TRISCCLR = mask;
                LATCSET = mask;
            } else if (mask & amask) {
                TRISACLR = mask;
                LATASET = mask;
            } else {
                TRISBCLR = mask;
                LATBSET = mask;
            }

            /* Set other pins to low. */
            mask = low [row];
            TRISACLR = mask & amask;
            LATACLR = mask & amask;
            TRISBCLR = mask & bmask;
            LATBCLR = mask & bmask;
            mask >>= 16;
            TRISCCLR = mask;
            LATCCLR = mask;

            /* Pause to make it visible. */
            udelay (1000 / LOL_NPINS);
            poll_usb();

            /* Set all low. */
            LATACLR = amask;
            LATBCLR = bmask;
            LATCCLR = cmask;
        }
    }

    /* Turn display off. */
    TRISASET = amask;
    TRISBSET = bmask;
    TRISCSET = cmask;
}

#define ROW(a,b,c,d,e,f,g) (a | b<<1 | c<<2 | d<<3 | e<<4 | f<<5 | g<<6)
#define _ 0
#define O 1

#define __  ROW (_,_,_,_,_,_,_)
#define _space __,__,__
#define _comma \
            ROW (O,O,_,_,_,_,_), \
            ROW (_,O,O,_,_,_,_), \
            __,__
#define _exclam __,\
            ROW (O,_,O,O,O,O,O), \
            __,__
#define _dot \
            ROW (O,O,_,_,_,_,_), \
            ROW (O,O,_,_,_,_,_), \
            __,__,__
#define _A  ROW (O,O,O,O,O,_,_), \
            ROW (_,_,O,_,_,O,_), \
            ROW (_,_,O,_,_,_,O), \
            ROW (_,_,O,_,_,O,_), \
            ROW (O,O,O,O,O,_,_), \
            __
#define _B  ROW (O,O,O,O,O,O,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (_,O,O,_,O,O,_), \
            __
#define _C  ROW (_,O,O,O,O,O,_), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (_,O,_,_,_,O,_), \
            __
#define _D  ROW (O,O,O,O,O,O,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (_,O,_,_,_,O,_), \
            ROW (_,_,O,O,O,_,_), \
            __
#define _E  ROW (O,O,O,O,O,O,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            __
#define _F  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,_,_,_,O), \
            __
#define _G  ROW (_,O,O,O,O,O,_), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (_,O,O,O,_,O,_), \
            __
#define _H  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,O,_,_,_), \
            ROW (_,_,_,O,_,_,_), \
            ROW (_,_,_,O,_,_,_), \
            ROW (O,O,O,O,O,O,O), \
            __
#define _I  ROW (O,_,_,_,_,_,O), \
            ROW (O,O,O,O,O,O,O), \
            ROW (O,_,_,_,_,_,O), \
            __
#define _K  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,O,_,_,_), \
            ROW (_,_,_,O,O,_,_), \
            ROW (_,_,O,_,_,O,_), \
            ROW (O,O,_,_,_,_,O), \
            __
#define _L  ROW (O,O,O,O,O,O,O), \
            ROW (O,_,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            __
#define _M  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,_,_,O,_), \
            ROW (_,_,_,_,O,_,_), \
            ROW (_,_,_,_,_,O,_), \
            ROW (O,O,O,O,O,O,O), \
            __
#define _N  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,_,O,_,_), \
            ROW (_,_,_,O,_,_,_), \
            ROW (_,_,O,_,_,_,_), \
            ROW (O,O,O,O,O,O,O), \
            __
#define _O  ROW (_,O,O,O,O,O,_), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (O,_,_,_,_,_,O), \
            ROW (_,O,O,O,O,O,_), \
            __
#define _P  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,_,_,O,O,_), \
            __
#define _R  ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,O,_,_,O), \
            ROW (_,_,O,O,_,_,O), \
            ROW (_,O,_,O,_,_,O), \
            ROW (O,_,_,_,O,O,_), \
            __
#define _S  ROW (_,_,_,_,O,O,_), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (O,_,_,O,_,_,O), \
            ROW (_,O,O,_,_,_,_), \
            __
#define _T  ROW (_,_,_,_,_,_,O), \
            ROW (_,_,_,_,_,_,O), \
            ROW (O,O,O,O,O,O,O), \
            ROW (_,_,_,_,_,_,O), \
            ROW (_,_,_,_,_,_,O), \
            __
#define _U  ROW (_,O,O,O,O,O,O), \
            ROW (O,_,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (_,O,O,O,O,O,O), \
            __
#define _V  ROW (_,_,O,O,O,O,O), \
            ROW (_,O,_,_,_,_,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (_,O,_,_,_,_,_), \
            ROW (_,_,O,O,O,O,O), \
            __
#define _W  ROW (_,O,O,O,O,O,O), \
            ROW (O,_,_,_,_,_,_), \
            ROW (_,O,O,O,O,O,_), \
            ROW (O,_,_,_,_,_,_), \
            ROW (_,O,O,O,O,O,O), \
            __
#define _X  ROW (O,O,_,_,_,O,O), \
            ROW (_,_,O,_,O,_,_), \
            ROW (_,_,_,O,_,_,_), \
            ROW (_,_,O,_,O,_,_), \
            ROW (O,O,_,_,_,O,O), \
            __
#define _Y  ROW (_,_,_,_,_,O,O), \
            ROW (_,_,_,_,O,_,_), \
            ROW (O,O,O,O,_,_,_), \
            ROW (_,_,_,_,O,_,_), \
            ROW (_,_,_,_,_,O,O), \
            __
//
// One, two, buckle my shoe.
// Three, four, knock on the door.
// Five, six, pick up sticks.
// Seven, eight, lay them straight.
// Nine, ten, do it again!
//
static const unsigned char phrase [] = {
        __,_O,_N,_E,_comma,_T,_W,_O,_comma,_B,_U,_C,_K,_L,_E,_space,_M,_Y,_space,_S,_H,_O,_E,_dot,
        _T,_H,_R,_E,_E,_comma,_F,_O,_U,_R,_comma,_K,_N,_O,_C,_K,_space,_O,_N,_space,_T,_H,_E,_space,_D,_O,_O,_R,_dot,
        _F,_I,_V,_E,_comma,_S,_I,_X,_comma,_P,_I,_C,_K,_space,_U,_P,_space,_S,_T,_I,_C,_K,_S,_dot,
        _S,_E,_V,_E,_N,_comma,_E,_I,_G,_H,_T,_comma,_L,_A,_Y,_space,_T,_H,_E,_M,_space,_S,_T,_R,_A,_I,_G,_H,_T,_dot,
        _N,_I,_N,_E,_comma,_T,_E,_N,_comma,_D,_O,_space,_I,_T,_space,_A,_G,_A,_I,_N,_exclam,
        __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	0xff
};

void shift_picture (unsigned short *picture, int next)
{
    int y;

    for (y=0; y<9; y++) {
        picture[y] >>= 1;
        if (y > 0 && ((next << y) & 0x80))
            picture[y] |= 1 << 13;
    }
}

/*
 * Scrolling text.
 */
void scrolling_text()
{
    static unsigned short picture[9];
    const unsigned char *next;

    for (;;) {
        memset (picture, 0, sizeof(picture));
        for (next=phrase; *next!=0xff; next++) {
            shift_picture (picture, *next);
            lol (70, picture);
        }
    }
}

/*
 * Main program entry point.
 */
int main()
{
    /* Unlock CFGCON register. */
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCON &= (1 << 13);            // clear IOLOCK

    /* Disable JTAG ports, to make more pins available. */
    CFGCON &= (1 << 3);             // clear JTAGEN

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

    USBDeviceInit();        //usb_device.c.  Initializes USB module SFRs and firmware
                //variables to known states.
    PMCON = 0;

    for (;;) {
        scrolling_text();
    }
}

/*
 * USB Callback Functions
 *
 * The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
 * events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
 * packets to your device.  In response to this, all USB devices are supposed to decrease their power
 * consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
 * to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
 * function.  You should modify these callback functions to take appropriate actions for each of these
 * conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
 * consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
 * microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
 * add code that undoes the power saving things done in the USBCBSuspend() function.
 *
 * The USBCBSendResume() function is special, in that the USB stack will not automatically call this
 * function.  This function is meant to be called from the application firmware instead.  See the
 * additional comments near the function.
 */

/*
 * This function is called when the device becomes
 * initialized, which occurs after the host sends a
 * SET_CONFIGURATION (wValue not = 0) request.  This
 * callback function should initialize the endpoints
 * for the device's usage according to the current
 * configuration.
 */
void USBCBInitEP()
{
    USBEnableEndpoint (HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED |
        USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
}

/*
 * When SETUP packets arrive from the host, some
 * firmware must process the request and respond
 * appropriately to fulfill the request.  Some of
 * the SETUP packets will be for standard
 * USB "chapter 9" (as in, fulfilling chapter 9 of
 * the official USB specifications) requests, while
 * others may be specific to the USB device class
 * that is being implemented.  For example, a HID
 * class device needs to be able to respond to
 * "GET REPORT" type of requests.  This
 * is not a standard USB chapter 9 request, and
 * therefore not handled by usb_device.c.  Instead
 * this request should be handled by class specific
 * firmware, such as that contained in usb_function_hid.c.
 */
void USBCBCheckOtherReq()
{
    USBCheckHIDRequest();
}

/*
 * The USBCBStdSetDscHandler() callback function is
 * called when a SETUP, bRequest: SET_DESCRIPTOR request
 * arrives.  Typically SET_DESCRIPTOR requests are
 * not used in most applications, and it is
 * optional to support this type of request.
 */
void USBCBStdSetDscHandler()
{
    /* Must claim session ownership if supporting this request */
}

/*
 * The host may put USB peripheral devices in low power
 * suspend mode (by "sending" 3+ms of idle).  Once in suspend
 * mode, the host may wake the device back up by sending non-
 * idle state signalling.
 *
 * This call back is invoked when a wakeup from USB suspend is detected.
 */
void USBCBWakeFromSuspend()
{
    // If clock switching or other power savings measures were taken when
    // executing the USBCBSuspend() function, now would be a good time to
    // switch back to normal full power run mode conditions.  The host allows
    // a few milliseconds of wakeup time, after which the device must be
    // fully back to normal, and capable of receiving and processing USB
    // packets.  In order to do this, the USB module must receive proper
    // clocking (IE: 48MHz clock must be available to SIE for full speed USB
    // operation).
}

/*
 * Call back that is invoked when a USB suspend is detected
 */
void USBCBSuspend()
{
}

/*
 * The USB host sends out a SOF packet to full-speed
 * devices every 1 ms. This interrupt may be useful
 * for isochronous pipes. End designers should
 * implement callback routine as necessary.
 */
void USBCB_SOF_Handler()
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*
 * The purpose of this callback is mainly for
 * debugging during development. Check UEIR to see
 * which error causes the interrupt.
 */
void USBCBErrorHandler()
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

    // Typically, user firmware does not need to do anything special
    // if a USB error occurs.  For example, if the host sends an OUT
    // packet to your device, but the packet gets corrupted (ex:
    // because of a bad connection, or the user unplugs the
    // USB cable during the transmission) this will typically set
    // one or more USB error interrupt flags.  Nothing specific
    // needs to be done however, since the SIE will automatically
    // send a "NAK" packet to the host.  In response to this, the
    // host will normally retry to send the packet again, and no
    // data loss occurs.  The system will typically recover
    // automatically, without the need for application firmware
    // intervention.

    // Nevertheless, this callback function is provided, such as
    // for debugging purposes.
}

/*
 * The USB specifications allow some types of USB
 * peripheral devices to wake up a host PC (such
 * as if it is in a low power suspend to RAM state).
 * This can be a very useful feature in some
 * USB applications, such as an Infrared remote
 * control receiver.  If a user presses the "power"
 * button on a remote control, it is nice that the
 * IR receiver can detect this signalling, and then
 * send a USB "command" to the PC to wake up.
 *
 * The USBCBSendResume() "callback" function is used
 * to send this special USB signalling which wakes
 * up the PC.  This function may be called by
 * application firmware to wake up the PC.  This
 * function should only be called when:
 *
 * 1.  The USB driver used on the host PC supports
 *     the remote wakeup capability.
 * 2.  The USB configuration descriptor indicates
 *     the device is remote wakeup capable in the
 *     bmAttributes field.
 * 3.  The USB host PC is currently sleeping,
 *     and has previously sent your device a SET
 *     FEATURE setup packet which "armed" the
 *     remote wakeup capability.
 *
 * This callback should send a RESUME signal that
 * has the period of 1-15ms.
 *
 * Note: Interrupt vs. Polling
 * -Primary clock
 * -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 * > Can switch to primary first by calling USBCBWakeFromSuspend()
 *
 * The modifiable section in this routine should be changed
 * to meet the application needs. Current implementation
 * temporary blocks other functions from executing for a
 * period of 1-13 ms depending on the core frequency.
 *
 * According to USB 2.0 specification section 7.1.7.7,
 * "The remote wakeup device must hold the resume signaling
 * for at lest 1 ms but for no more than 15 ms."
 * The idea here is to use a delay counter loop, using a
 * common value that would work over a wide range of core
 * frequencies.
 * That value selected is 1800. See table below:
 * ==========================================================
 * Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 * ==========================================================
 *     48              12          1.05
 *      4              1           12.6
 * ==========================================================
 *  * These timing could be incorrect when using code
 *    optimization or extended instruction mode,
 *    or when having other interrupts enabled.
 *    Make sure to verify using the MPLAB SIM's Stopwatch
 *    and verify the actual signal on an oscilloscope.
 */
void USBCBSendResume()
{
    static unsigned delay_count;

    // Start RESUME signaling
    U1CON |= PIC32_U1CON_RESUME;

    // Set RESUME line for 1-13 ms
    delay_count = 1800U;
    do {
        delay_count--;
    } while (delay_count);

    U1CON &= ~PIC32_U1CON_RESUME;
}

/*
 * This function is called whenever a EP0 data
 * packet is received.  This gives the user (and
 * thus the various class examples a way to get
 * data that is received via the control endpoint.
 * This function needs to be used in conjunction
 * with the USBCBCheckOtherReq() function since
 * the USBCBCheckOtherReq() function is the apps
 * method for getting the initial control transfer
 * before the data arrives.
 *
 * PreCondition: ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 * defined already (in usb_config.h)
 */
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived()
{
}
#endif

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR device_dsc = {
    0x12,                   // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usbcfg.h
    0x04d8,                 // Vendor ID: Microchip
    0x003F,                 // Product ID: HID Demo
    0x0001,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration 1 Descriptor */
const unsigned char configDescriptor1[] = {
    /* Configuration Descriptor */
    0x09,                           // sizeof(USB_CFG_DSC)
    USB_DESCRIPTOR_CONFIGURATION,   // CONFIGURATION descriptor type
    0x29, 0x00,                     // Total length of data for this cfg
    1,                              // Number of interfaces in this cfg
    1,                              // Index value of this configuration
    0,                              // Configuration string index
    _DEFAULT | _SELF,               // Attributes, see usbd.h
    50,                             // Max power consumption (2X mA)

    /* Interface Descriptor */
    0x09,                           // sizeof(USB_INTF_DSC)
    USB_DESCRIPTOR_INTERFACE,       // INTERFACE descriptor type
    0,                              // Interface Number
    0,                              // Alternate Setting Number
    2,                              // Number of endpoints in this intf
    HID_INTF,                       // Class code
    0,                              // Subclass code
    0,                              // Protocol code
    0,                              // Interface string index

    /* HID Class-Specific Descriptor */
    0x09,                           // sizeof(USB_HID_DSC)+3
    DSC_HID,                        // HID descriptor type
    0x11, 0x01,                     // HID Spec Release Number in BCD format (1.11)
    0x00,                           // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,                 // Number of class descriptors, see usbcfg.h
    DSC_RPT,                        // Report descriptor type
    47, 0x00,                       // sizeof(hid_rpt01)

    /* Endpoint Descriptor */
    0x07,                           // sizeof(USB_EP_DSC)
    USB_DESCRIPTOR_ENDPOINT,        // Endpoint Descriptor
    HID_EP | _EP_IN,                // EndpointAddress
    _INTERRUPT,                     // Attributes
    0x08, 0x00,                     // size
    0x01,                           // Interval

    /* Endpoint Descriptor */
    0x07,                           // sizeof(USB_EP_DSC)
    USB_DESCRIPTOR_ENDPOINT,        // Endpoint Descriptor
    HID_EP | _EP_OUT,               // EndpointAddress
    _INTERRUPT,                     // Attributes
    0x08, 0x00,                     // size
    0x01                            // Interval
};

// Language code string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [1];
} sd000 = {
    sizeof(sd000),
    USB_DESCRIPTOR_STRING,
    { 0x0409 }
};

// Manufacturer string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [25];
} sd001 = {
    sizeof(sd001),
    USB_DESCRIPTOR_STRING,
{       'M','i','c','r','o','c','h','i','p',' ',
    'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'
}};

// Product string descriptor
const struct {
    unsigned char bLength;
    unsigned char bDscType;
    uint16_t string [11];
} sd002 = {
    sizeof(sd002),
    USB_DESCRIPTOR_STRING,
{       'G','e','n','e','r','i','c',' ','H','I','D'
}};

// Class specific descriptor - Generic HID

// To change the number of bytes in a report, under Input report, Output report,
// or Feature report below, change Report Count from 0x02 bytes to a value
// from 0x01 to 0x08.
// In usb_config.h, edit these values to match the new report size(s):
// #define HID_INPUT_REPORT_BYTES   2
// #define HID_OUTPUT_REPORT_BYTES  2
// #define HID_FEATURE_REPORT_BYTES 2

// This firmware version doesn't support reports > 8 bytes.

const unsigned char hid_rpt01 [HID_RPT01_SIZE] = {
    0x06, 0xA0, 0xFF,       // Usage page (vendor defined)
    0x09, 0x01,             // Usage ID (vendor defined)
    0xA1, 0x01,             // Collection (application)

    // The Input report
    0x09, 0x03,             // Usage ID - vendor defined
    0x15, 0x00,             // Logical Minimum (0)
    0x26, 0xFF, 0x00,       // Logical Maximum (255)
    0x75, 0x08,             // Report Size (8 bits)
    0x95, 0x02,             // Report Count (2 fields)
    0x81, 0x02,             // Input (Data, Variable, Absolute)

    // The Output report
    0x09, 0x04,             // Usage ID - vendor defined
    0x15, 0x00,             // Logical Minimum (0)
    0x26, 0xFF, 0x00,       // Logical Maximum (255)
    0x75, 0x08,             // Report Size (8 bits)
    0x95, 0x02,             // Report Count (2 fields)
    0x91, 0x02,             // Output (Data, Variable, Absolute)

    // The Feature report
    0x09, 0x05,             // Usage ID - vendor defined
    0x15, 0x00,             // Logical Minimum (0)
    0x26, 0xFF, 0x00,       // Logical Maximum (255)
    0x75, 0x08,             // Report Size (8 bits)
    0x95, 0x02,             // Report Count (2 fields)
    0xB1, 0x02,             // Feature (Data, Variable, Absolute)

    0xC0
};

//Array of congiruation descriptors
const unsigned char *const USB_CD_Ptr[] = {
    (const unsigned char *const) &configDescriptor1
};

//Array of string descriptors
const unsigned char *const USB_SD_Ptr[] = {
    (const unsigned char *const) &sd000,
    (const unsigned char *const) &sd001,
    (const unsigned char *const) &sd002
};
