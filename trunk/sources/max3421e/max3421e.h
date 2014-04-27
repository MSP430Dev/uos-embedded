#ifndef __MAX3421E_H__
#define __MAX3421E_H__

//
// SPI Command Byte
//
#define MAX3421_REG_NUM(x)  ((x) << 3)
#define MAX3421_DIR_RD      0x00
#define MAX3421_DIR_WR      (1 << 1)
#define MAX3421_ACKSTAT     0x01

//
// Registers
//
#define EP0FIFO     0
#define EP1OUTFIFO  1
#define RCVFIFO     1
#define EP2INFIFO   2
#define SNDFIFO     2
#define EP3INFIFO   3
#define SUDFIFO     4
#define EP0BC       5
#define EP1OUTBC    6
#define RCVBC       6
#define EP2INBC     7
#define SNDBC       7
#define EP3INBC     8
#define EPSTALLS    9
#define CLRTOGS     10
#define EPIRQ       11
#define EPIEN       12
#define USBIRQ      13
#define USBIEN      14
#define USBCTL      15
#define CPUCTL      16
#define PINCTL      17
#define REVISION    18
#define FNADDR      19
#define IOPINS1     20
#define IOPINS2     21
#define GPINIRQ     22
#define GPINIEN     23
#define GPINPOL     24
#define HIRQ        25
#define HIEN        26
#define MODE        27
#define PERADDR     28
#define HCTL        29
#define HXFR        30
#define HRSL        31

//
// Register values
//

// EPSTALLS
#define STLEP0IN        (1 << 0)    // Stall endpoint 0 IN
#define STLEP0OUT       (1 << 1)    // Stall endpoint 0 OUT
#define STLEP1OUT       (1 << 2)    // Stall endpoint 1 OUT
#define STLEP2IN        (1 << 3)    // Stall endpoint 2 IN
#define STLEP3IN        (1 << 4)    // Stall endpoint 3 IN
#define STLSTAT         (1 << 5)    // Stall the STATUS stage of CONTROL transfer
#define ACKSTAT         (1 << 6)    // Acknowledge the STATUS stage of CONTROL transfer

// CLRTOGS
#define CTGEP1OUT       (1 << 2)    // Clear Data Toggle for endpoint 1 OUT to DATA0
#define CTGEP2IN        (1 << 3)    // Clear Data Toggle for endpoint 2 IN to DATA0
#define CTGEP3IN        (1 << 4)    // Clear Data Toggle for endpoint 3 IN to DATA0
#define EP1DISAB        (1 << 5)    // Disable Endpoint 1
#define EP2DISAB        (1 << 6)    // Disable Endpoint 2
#define EP3DISAB        (1 << 7)    // Disable Endpoint 3

// EPIRQ
#define IN0BAVIRQ       (1 << 0)    // Endpoint 0 IN Buffer Available Interrupt Request
#define OUT0BAVIRQ      (1 << 1)    // Endpoint 0 OUT Buffer Available Interrupt Request
#define OUT1BAVIRQ      (1 << 2)    // Endpoint 1 OUT Buffer Available Interrupt Request
#define IN2BAVIRQ       (1 << 3)    // Endpoint 2 IN Buffer Available Interrupt Request
#define IN3BAVIRQ       (1 << 4)    // Endpoint 3 IN Buffer Available Interrupt Request
#define SUDAVIRQ        (1 << 5)    // SETUP Data Available Interrupt Request

// EPIEN
#define IN0BAVIE        (1 << 0)
#define OUT0BAVIE       (1 << 1)
#define OUT1BAVIE       (1 << 2)
#define IN2BAVIE        (1 << 3)
#define IN3BAVIE        (1 << 4)
#define SUDAVIE         (1 << 5)

// USBIRQ
#define OSCOKIRQ        (1 << 0)    // Oscillator OK Interrupt Request
#define RWUDNIRQ        (1 << 1)    // Remote Wakeup Signaling Done Interrupt Request
#define BUSACTIRQ       (1 << 2)    // Bus Active Interrupt Request
#define URESIRQ         (1 << 3)    // USB bus reset interrupt request
#define SUSPIRQ         (1 << 4)    // SUSPEND Interrupt Request
#define NOVBUSIRQ       (1 << 5)    // No VBUS interrupt request
#define VBUSIRQ         (1 << 6)    // VBUS present interrupt request
#define URESDNIRQ       (1 << 7)    // USB Bus Reset Done Interrupt Request

// USBIEN
#define OSCOKIE         (1 << 0)
#define RWUDNIE         (1 << 1)
#define BUSACTIE        (1 << 2)
#define URESIE          (1 << 3)
#define SUSPIE          (1 << 4)
#define NOVBUSIE        (1 << 5)
#define VBUSIE          (1 << 6)
#define URESDNIE        (1 << 7)

// USBCTL
#define SIGRWU          (1 << 2)    // Signal Remote Wakeup
#define CONNECT         (1 << 3)    // Connect an internal 1500 Ohm resistor between the DPLUS line and VCC
#define PWRDOWN         (1 << 4)    // Switch power-down mode (to low power state)
#define CHIPRES         (1 << 5)    // Chip reset
#define VBGATE          (1 << 6)    // Make operation of the CONNECT bit conditional on VBUS being present
#define HOSCSTEN        (1 << 7)    // Host Oscillator Start Enable (in power-down mode)

// CPUCTL
#define IE              (1 << 0)    // Global interrupt enable
#define PULSEWID0       (1 << 6)    // Interrupt pulse width (0,0 => 10.6 us;
#define PULSEWID1       (1 << 7)    // 0,1 => 5.3 us; 1,0 => 2.6 us; 1,1 => 1.3 us)

// PINCTL
#define GPXA            (1 << 0)    // The two bits GPXB:GPXA determine the output of the GPX pin
#define GPXB            (1 << 1)    // The two bits GPXB:GPXA determine the output of the GPX pin
#define POSINT          (1 << 2)    // Interrupt edge polarity
#define INTLEVEL        (1 << 3)    // When set, interrupt is level-active
#define FDUPSPI         (1 << 4)    // Full Duplex SPI port operation
#define EP0INAK         (1 << 5)    // EP0-IN NAK
#define EP2INAK         (1 << 6)    // EP2-IN NAK
#define EP3INAK         (1 << 7)    // EP3-IN NAK

// IOPINS1
#define GPOUT0          (1 << 0)
#define GPOUT1          (1 << 1)
#define GPOUT2          (1 << 2)
#define GPOUT3          (1 << 3)
#define GPIN0           (1 << 4)
#define GPIN1           (1 << 5)
#define GPIN2           (1 << 6)
#define GPIN3           (1 << 7)

// IOPINS2
#define GPOUT4          (1 << 0)
#define GPOUT5          (1 << 1)
#define GPOUT6          (1 << 2)
#define GPOUT7          (1 << 3)
#define GPIN4           (1 << 4)
#define GPIN5           (1 << 5)
#define GPIN6           (1 << 6)
#define GPIN7           (1 << 7)

// GPINIRQ
#define GPINIRQ0        (1 << 0)
#define GPINIRQ1        (1 << 1)
#define GPINIRQ2        (1 << 2)
#define GPINIRQ3        (1 << 3)
#define GPINIRQ4        (1 << 4)
#define GPINIRQ5        (1 << 5)
#define GPINIRQ6        (1 << 6)
#define GPINIRQ7        (1 << 7)

// GPINIEN
#define GPINIEN0        (1 << 0)
#define GPINIEN1        (1 << 1)
#define GPINIEN2        (1 << 2)
#define GPINIEN3        (1 << 3)
#define GPINIEN4        (1 << 4)
#define GPINIEN5        (1 << 5)
#define GPINIEN6        (1 << 6)
#define GPINIEN7        (1 << 7)

// GPINPOL
#define GPINPOL0        (1 << 0)
#define GPINPOL1        (1 << 1)
#define GPINPOL2        (1 << 2)
#define GPINPOL3        (1 << 3)
#define GPINPOL4        (1 << 4)
#define GPINPOL5        (1 << 5)
#define GPINPOL6        (1 << 6)
#define GPINPOL7        (1 << 7)

// HIRQ
#define BUSEVENTIRQ     (1 << 0)    // The SIE sets the BUSEVENTIRQ bit when it completes signaling of
                                    // Bus Reset or Bus Resume
#define RSMREQIRQ       (1 << 1)    // Remote Wakeup Interrupt Request
#define RCVDAVIRQ       (1 << 2)    // Receive FIFO Data Available Interrupt Request
#define SNDDAVIRQ       (1 << 3)    // Send FIFO Data Available Interrupt Request
#define SUSDNIRQ        (1 << 4)    // Suspend operation Done IRQ
#define CONDETIRQ       (1 << 5)    // Peripheral Connect/Disconnect Interrupt Request
#define FRAMEIRQ        (1 << 6)    // Frame Generator Interrupt Request
#define HXFRDNIRQ       (1 << 7)    // Host Transfer Done Interrupt Request

// HIE
#define BUSEVENTIE      (1 << 0)
#define RSMREQIE        (1 << 1)
#define RCVDAVIE        (1 << 2)
#define SNDDAVIE        (1 << 3)
#define SUSDNIE         (1 << 4)
#define CONDETIE        (1 << 5)
#define FRAMEIE         (1 << 6)
#define HXFRDNIE        (1 << 7)

// MODE
#define HOST            (1 << 0)    // When 1, MAX3421E operates in host mode
#define LOWSPEED        (1 << 1)    // Sets the host for low-speed operation
#define HUBPRE          (1 << 2)    // Send the PRE PID to a LS device operating through a USB hub
#define SOFKAENAB       (1 << 3)    // Enable automatic generation of full-speed SOF packets or low-speed Keep-Alive pulses
#define SEPIRQ          (1 << 4)    // Provides the GPIN IRQS on a separate pin (GPX)
#define DELAYISO        (1 << 5)    // Delay data transfer to an ISOCHRONOUS endpoint until the next SOF
#define DMPULLDN        (1 << 6)    // Connect internal 15kΩ resistors from D- to ground
#define DPPULLDN        (1 << 7)    // Connect internal 15kΩ resistors from D+ to ground

// HCTL
#define BUSRST          (1 << 0)    // Issue a Bus Reset to a USB peripheral
#define FRMRST          (1 << 1)    // Reset the SOF frame counter
#define SAMPLEBUS       (1 << 2)    // Sample the state of the USB bus
#define SIGRSM          (1 << 3)    // Signal a bus resume event
#define RCVTOG0         (1 << 4)    // Set or clear the data toggle value for a data transfer
#define RCVTOG1         (1 << 5)    // Set or clear the data toggle value for a data transfer
#define SNDTOG0         (1 << 6)    // Set or clear the data toggle value for a data transfer
#define SNDTOG1         (1 << 7)    // Set or clear the data toggle value for a data transfer

//
// HXFR
//
// The CPU writes this register to launch a host transfer This register is load-sensitive.
// This means that when the CPU writes it, the SIE launches a transfer.
//
// Xfr Type      HS      ISO     OUTNIN      SETUP
// -------------------------------------------------
// SETUP         0        0         0          1
// BULK-IN       0        0         0          0
// BULK-OUT      0        0         1          0
// HS-IN         1        0         0          0
// HS-OUT        1        0         1          0
// ISO-IN        0        1         0          0
// ISO-OUT       0        1         1          0
// 
// EP[3:0] set the endpoint
//
#define EP0             (1 << 0)
#define EP1             (1 << 1)
#define EP2             (1 << 2)
#define EP3             (1 << 3)
#define SETUP           (1 << 4)
#define OUTNIN          (1 << 5)
#define ISO             (1 << 6)
#define HS              (1 << 7)

// HRSL
#define HRSLT0          (1 << 0)    // HRSLT[3:0] indicate the result code
#define HRSLT1          (1 << 1)    // HRSLT[3:0] indicate the result code
#define HRSLT2          (1 << 2)    // HRSLT[3:0] indicate the result code
#define HRSLT3          (1 << 3)    // HRSLT[3:0] indicate the result code
#define RCVTOGRD        (1 << 4)    // SNDTOGRD indicates the resulting data toggle values for OUT transfers
#define SNDTOGRD        (1 << 5)    // RCVTOGRD indicates the resulting data toggle values for IN transfers
#define KSTATUS         (1 << 6)    // Indicate the state (K=0,J=0 => SE0;
#define JSTATUS         (1 << 7)    // K=1,J=0 => K; K=0,J=1 => J; K=1,J=1 => N/A)


#endif
