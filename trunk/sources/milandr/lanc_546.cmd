/*
 * Пример определения адресного пространства для контроллеров TMS320C5XX.
 */

MEMORY {
PAGE 0:							/* program memory */
	PROG_RAM (RWX):	origin = 0x1480, length = 0x2C00
	PROG_EXT (RWX):	origin = 0x8000, length = 0x4000
	VECTORS (RWX):	origin = 0x1400, length = 0x0080 /* boot interrupt vector table location */

PAGE 1:							/* data memory, addresses 0-7Fh are reserved */
	DATA_RAM (RW):	origin = 0x0080, length = 0x7f80
	DATA_EXT (RW):	origin = 0x8000, length = 0x6000
	MAC_RXBF (RW):	origin = 0xE000, length = 0x0800
	MAC_RXBD (RW):	origin = 0xE800, length = 0x0100
	MAC_TXBF (RW):	origin = 0xF000, length = 0x0800
	MAC_TXBD (RW):	origin = 0xF800, length = 0x0100
	MAC_RG (RW):	origin = 0xFFC0, length = 0x0020
} /* MEMORY */

SECTIONS {
	.text	 > PROG_RAM | PROG_EXT	PAGE 0	/* code */
	.switch  > PROG_RAM		PAGE 0	/* switch table info */
	.cinit	 > PROG_RAM		PAGE 0

	.vectors > VECTORS		PAGE 0	/* interrupt vectors */

	.cio	 > DATA_RAM		PAGE 1	/* C I/O */
	.data	 > DATA_RAM | DATA_EXT	PAGE 1	/* initialized data */
	.bss	 > DATA_RAM | DATA_EXT	PAGE 1	/* global & static variables */
	.const	 > DATA_RAM		PAGE 1	/* constant data */
	.sysmem  > DATA_RAM | DATA_EXT	PAGE 1	/* heap */
	.stack	 > DATA_RAM | DATA_EXT	PAGE 1	/* stack */
	.csldata > DATA_RAM		PAGE 1

	.RG_sect > MAC_RG		PAGE 1
	.XB_sect > MAC_TXBF		PAGE 1
	.RB_sect > MAC_RXBF		PAGE 1
	.XD_sect > MAC_TXBD		PAGE 1
	.RD_sect > MAC_RXBD	PAGE 1
} /* SECTIONS */
