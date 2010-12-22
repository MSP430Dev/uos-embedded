/*
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PIC� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include "usb_config.h"
#include <microchip/usb.h>
#include <microchip/usb_function_cdc.h>

/*
 * Board definition.
 *
 * These defintions will tell the main() function which board is
 * currently selected.  This will allow the application to add
 * the correct configuration bits as wells use the correct
 * initialization functions for the board.  These defitions are only
 * required in the stack provided demos.  They are not required in
 * final application design.
 */
#define mLED_1_On()		LATECLR = (1 << 3)
#define mLED_USB_On()		LATECLR = (1 << 3)
#define mLED_2_On()		LATECLR = (1 << 2)
#define mLED_3_On()		LATECLR = (1 << 1)
#define mLED_4_On()		LATECLR = (1 << 0)

#define mLED_1_Off()		LATESET = (1 << 3)
#define mLED_USB_Off()		LATESET = (1 << 3)
#define mLED_2_Off()		LATESET = (1 << 2)
#define mLED_3_Off()		LATESET = (1 << 1)
#define mLED_4_Off()		LATESET = (1 << 0)

#define mLED_1_Toggle()		LATEINV = (1 << 3)
#define mLED_USB_Toggle()	LATEINV = (1 << 3)
#define mLED_2_Toggle()		LATEINV = (1 << 2)
#define mLED_3_Toggle()		LATEINV = (1 << 1)
#define mLED_4_Toggle()		LATEINV = (1 << 0)

// Let compile time pre-processor calculate the CORE_TICK_PERIOD
//#define CORE_TICK_RATE	(KHZ / 2)

// Decrements every 1 ms.
//volatile static unsigned int OneMSTimer;

/*
 * BlinkUSBStatus turns on and off LEDs
 * corresponding to the USB device state.
 *
 * mLED macros can be found in HardwareProfile.h
 * USBDeviceState is declared and updated in usb_device.c.
 */
void BlinkUSBStatus (void)
{
	static unsigned led_count = 0;

	if (led_count == 0) {
		led_count = 50000U;
	}
	led_count--;

	if (USBDeviceState == CONFIGURED_STATE) {
		if (led_count == 0) {
			mLED_USB_Toggle();
		}
	}
}

#if 0
void /*__ISR(_CORE_TIMER_VECTOR, ipl2)*/ CoreTimerHandler(void)
{
	// clear the interrupt flag
	mCTClearIntFlag();

	if (OneMSTimer) {
		OneMSTimer--;
	}

	// update the period
	UpdateCoreTimer (CORE_TICK_RATE);
}
#endif

/*
 * Main program entry point.
 */
int main (void)
{
	AD1PCFG = 0xFFFF;

	//Initialize all of the LED pins
	LATE |= 0x000F;
	TRISE &= 0xFFF0;

	USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    				//variables to known states.
	PMCON = 0;

	for (;;) {
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

		// Application-specific tasks.
		// Blink the LEDs according to the USB device status
		BlinkUSBStatus();

		// User Application USB tasks
		if (USBDeviceState >= CONFIGURED_STATE && ! (U1PWRC & PIC32_U1PWRC_USUSPEND)) {
			unsigned char numBytesRead;
			static unsigned char USB_In_Buffer[64];
			static unsigned char USB_Out_Buffer[64];

			// Pull in some new data if there is new data to pull in
			numBytesRead = getsUSBUSART ((char*) USB_In_Buffer, 64);
			if (numBytesRead != 0) {
				snprintf (USB_Out_Buffer, sizeof (USB_Out_Buffer),
					"Received USB bytes. Hello!\r\n");
				putUSBUSART ((char*) USB_Out_Buffer, strlen (USB_Out_Buffer));
				mLED_2_Toggle();
				mLED_3_On();
				//OneMSTimer = 1000;
			}

//			if (! OneMSTimer) {
				mLED_3_Off();
//			}

			CDCTxService();
		}
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
void USBCBInitEP (void)
{
	CDCInitEP();
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
void USBCBCheckOtherReq (void)
{
	USBCheckCDCRequest();
}

/*
 * The USBCBStdSetDscHandler() callback function is
 * called when a SETUP, bRequest: SET_DESCRIPTOR request
 * arrives.  Typically SET_DESCRIPTOR requests are
 * not used in most applications, and it is
 * optional to support this type of request.
 */
void USBCBStdSetDscHandler(void)
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
void USBCBWakeFromSuspend(void)
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
void USBCBSuspend (void)
{
}

/*
 * The USB host sends out a SOF packet to full-speed
 * devices every 1 ms. This interrupt may be useful
 * for isochronous pipes. End designers should
 * implement callback routine as necessary.
 */
void USBCB_SOF_Handler(void)
{
	// No need to clear UIRbits.SOFIF to 0 here.
	// Callback caller is already doing that.
}

/*
 * The purpose of this callback is mainly for
 * debugging during development. Check UEIR to see
 * which error causes the interrupt.
 */
void USBCBErrorHandler(void)
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
void USBCBSendResume(void)
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
void USBCBEP0DataReceived(void)
{
}
#endif
