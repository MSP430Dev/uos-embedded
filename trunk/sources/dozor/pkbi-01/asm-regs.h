//======================================================================================
// Board Registers
//======================================================================================
#define Base_Register			0x80000000		// Base ARINC-429 MIL-STD-1553 Ethernet Register
#define Register_0				0x00000000		// Write Data
#define Register_1				0x03FF8000		// Control Pins
#define Register_2				0x04000000		// Write_Command Word
#define Register_3				0x06000000		// Read Status
#define Register_4				0x08000000		// Read Data
//======================================================================================
// Register_1 Bits
//======================================================================================
#define CSm						0x00008000		// A15 MIL-STD-1553 Chip Select
#define CSe						0x00010000		// A16 Ethernet Chip Select
#define EN1						0x00020000		// A17 ARINC-429 Enable Receiver 1 data to outputs
#define EN2						0x00040000		// A18 ARINC-429 Enable Receiver 2 data to outputs
#define PL1						0x00080000		// A19 ARINC-429 Latch Enable for Byte 1 to transmitter FIFO
#define PL2						0x00100000		// A20 ARINC-429 Latch Enable for Byte 2 to transmitter FIFO
#define CWSTR					0x00200000		// A21 ARINC-429 Clock for Control Word Register
#define ENTAX					0x00400000		// A22 ARINC-429 Enable Transmission
#define SEL						0x00800000		// A23 ARINC-429 Receiver data Byte selection [0 = Byte 1, 1 = Byte 2]
#define RW						0x01000000		// A24 Read / Write
//======================================================================================
// Processor Pin Constants
//======================================================================================
#define GPIOC_Port				0x400B8000		// Port Address
#define REL						0x00000001		// REL = Pin PC00
//======================================================================================
// Ethernet Constants [Status Register]
//======================================================================================
#define Ethernet_Status			0x00400000		// Ethernet Status Mask
#define Etnernet_Status_Shift	22
