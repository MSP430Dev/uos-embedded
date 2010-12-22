/*
    USB Hardware Abstraction Layer (HAL)  (Header File)

Summary:
    This file abstracts the hardware interface.  The USB stack firmware can be
    compiled to work on different USB microcontrollers, such as PIC18 and PIC24.
    The USB related special function registers and bit names are generally very
    similar between the device families, but small differences in naming exist.

Description:
    This file abstracts the hardware interface.  The USB stack firmware can be
    compiled to work on different USB microcontrollers, such as PIC18 and PIC24.
    The USB related special function registers and bit names are generally very
    similar between the device families, but small differences in naming exist.

    In order to make the same set of firmware work accross the device families,
    when modifying SFR contents, a slightly abstracted name is used, which is
    then "mapped" to the appropriate real name in the usb_hal_picxx.h header.

    Make sure to include the correct version of the usb_hal_picxx.h file for
    the microcontroller family which will be used.
*/
/*
 File Description:

 This file defines the interface to the USB hardware abstraction layer.

 Filename:        usb_hal.h
 Dependancies:    none
 Processor:       PIC18, PIC24, or PIC32 USB Microcontrollers
 Hardware:        The code is natively intended to be used on the following
   		  hardware platforms: PICDEM™ FS USB Demo Board,
     		  PIC18F87J50 FS USB Plug-In Module, or
     		  Explorer 16 + PIC24 USB PIM.  The firmware may be
     		  modified for use on other USB platforms by editing the
     		  HardwareProfile.h file.
 Compiler:        Microchip C18 (for PIC18) or C30 (for PIC24)
 Company:         Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PICmicro® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */

#if !defined(USB_HAL_PIC32_H)
#define USB_HAL_PIC32_H

#define USBTransactionCompleteIE	(U1IE & PIC32_U1I_TRN)
#define USBTransactionCompleteIF	(U1IR & PIC32_U1I_TRN)
#define USBTransactionCompleteIFReg	(BYTE*)&U1IR
#define USBTransactionCompleteIFBitNum	3

#define USBResetIE			(U1IE & PIC32_U1I_URST)
#define USBResetIF			(U1IR & PIC32_U1I_URST)
#define USBResetIFReg			(BYTE*)&U1IR
#define USBResetIFBitNum		0

#define USBIdleIE			(U1IE & PIC32_U1I_IDLE)
#define USBIdleIF			(U1IR & PIC32_U1I_IDLE)
#define USBIdleIFReg			(BYTE*)&U1IR
#define USBIdleIFBitNum			4

#define USBActivityIE			(U1OTGIE & PIC32_U1OTGI_ACTV)
#define USBActivityIF			(U1OTGIR & PIC32_U1OTGI_ACTV)
#define USBActivityIFReg		(BYTE*)&U1OTGIR
#define USBActivityIFBitNum		4

#define USBSOFIE			(U1IE & PIC32_U1I_SOF)
#define USBSOFIF			(U1IR & PIC32_U1I_SOF)
#define USBSOFIFReg			(BYTE*)&U1IR
#define USBSOFIFBitNum			2

#define USBStallIE			(U1IE & PIC32_U1I_STALL)
#define USBStallIF			(U1IR & PIC32_U1I_STALL)
#define USBStallIFReg			(BYTE*)&U1IR
#define USBStallIFBitNum		7

#define USBErrorIE			(U1IE & PIC32_U1I_UERR)
#define USBErrorIF			(U1IR & PIC32_U1I_UERR)
#define USBErrorIFReg			(BYTE*)&U1IR
#define USBErrorIFBitNum		1

//#define USBResumeControl		U1CONbits.RESUME

/* Buffer Descriptor Status Register Initialization Parameters */

//The _BSTALL definition is changed from 0x04 to 0x00 to
// fix a difference in the PIC18 and PIC24 definitions of this
// bit.  This should be changed back once the definitions are
// synced.
#define _BSTALL     0x04        //Buffer Stall enable
#define _DTSEN      0x08        //Data Toggle Synch enable
#define _DAT0       0x00        //DATA0 packet expected next
#define _DAT1       0x40        //DATA1 packet expected next
#define _DTSMASK    0x40        //DTS Mask
#define _USIE       0x80        //SIE owns buffer
#define _UCPU       0x00        //CPU owns buffer

#define _STAT_MASK  0xFC

// Buffer Descriptor Status Register layout.
typedef union __attribute__ ((packed)) _BD_STAT
{
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    BSTALL  :1;     //Buffer Stall Enable
        unsigned    DTSEN   :1;     //Data Toggle Synch Enable
        unsigned            :2;     //Reserved - write as 00
        unsigned    DTS     :1;     //Data Toggle Synch Value
        unsigned    UOWN    :1;     //USB Ownership
    };
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    PID0    :1;
        unsigned    PID1    :1;
        unsigned    PID2    :1;
        unsigned    PID3    :1;

    };
    struct __attribute__ ((packed)){
        unsigned            :2;
        unsigned    PID     :4;         //Packet Identifier
    };
    WORD           Val;
} BD_STAT;

// BDT Entry Layout
typedef union __attribute__ ((packed))__BDT
{
    struct __attribute__ ((packed))
    {
        BD_STAT     STAT;
        WORD       CNT:10;
        BYTE*       ADR;                      //Buffer Address
    };
    struct __attribute__ ((packed))
    {
        DWORD       res  :16;
        DWORD       count:10;
    };
    DWORD           w[2];
    WORD            v[4];
    QWORD           Val;
} BDT_ENTRY;


#define USTAT_EP0_PP_MASK   ~0x04
#define USTAT_EP_MASK       0xFC
#define USTAT_EP0_OUT       0x00
#define USTAT_EP0_OUT_EVEN  0x00
#define USTAT_EP0_OUT_ODD   0x04

#define USTAT_EP0_IN        0x08
#define USTAT_EP0_IN_EVEN   0x08
#define USTAT_EP0_IN_ODD    0x0C

typedef union
{
    WORD UEP[16];
} _UEP;

#define UEP_STALL 0x0002
#define USB_VOLATILE

typedef union _POINTER
{
    struct
    {
        BYTE bLow;
        BYTE bHigh;
        //byte bUpper;
    };
    WORD _word;                         // bLow & bHigh

    //pFunc _pFunc;                       // Usage: ptr.pFunc(); Init: ptr.pFunc = &<Function>;

    BYTE* bRam;                         // Ram byte pointer: 2 bytes pointer pointing
                                        // to 1 byte of data
    WORD* wRam;                         // Ram word poitner: 2 bytes poitner pointing
                                        // to 2 bytes of data

    ROM BYTE* bRom;                     // Size depends on compiler setting
    ROM WORD* wRom;
    //rom near byte* nbRom;               // Near = 2 bytes pointer
    //rom near word* nwRom;
    //rom far byte* fbRom;                // Far = 3 bytes pointer
    //rom far word* fwRom;
} POINTER;

 //* Depricated: v2.2 - will be removed at some point of time ***
#define _LS         0x00            // Use Low-Speed USB Mode
#define _FS         0x00            // Use Full-Speed USB Mode
#define _TRINT      0x00            // Use internal transceiver
#define _TREXT      0x00            // Use external transceiver
#define _PUEN       0x00            // Use internal pull-up resistor
#define _OEMON      0x00            // Use SIE output indicator
//*

#define USB_PULLUP_ENABLE 0x10
#define USB_PULLUP_DISABLE 0x00

#define USB_INTERNAL_TRANSCEIVER 0x00
#define USB_EXTERNAL_TRANSCEIVER 0x01

#define USB_FULL_SPEED 0x00
//USB_LOW_SPEED not currently supported in PIC24F USB products

//#define USB_PING_PONG__NO_PING_PONG	0x00
//#define USB_PING_PONG__EP0_OUT_ONLY	0x01
#define USB_PING_PONG__FULL_PING_PONG	0x02
//#define USB_PING_PONG__ALL_BUT_EP0	0x03

#define ConvertToPhysicalAddress(a) mips_virtual_addr_to_physical((unsigned)a)

/*
    Function:
        void USBModuleDisable(void)

    Description:
        This macro is used to disable the USB module

    Parameters:
        None

    Return Values:
        None

    Remarks:
        None

  */
#define USBModuleDisable() {\
	U1CON = 0;\
	U1IE = 0;\
	U1OTGIE = 0;\
	U1PWR |= PIC32_U1PWR_USBPWR;\
	USBDeviceState = DETACHED_STATE;\
}
#endif
