		.section .init
		.code	32
		
		.extern	_etext
		.extern _init_
		
		.org	0x0
		.global	_start_
		
		@
		@ Configuration header for OMAP 44xx
		@
		.word	0x40		@ CHSETTINGS item offset
		.word	0x0c		@ CHSETTINGS item size
		.word	0x00		@ unused
		.word	0x00		@ unused
		.word	0x00		@ unused
		.string	"CHSETTINGS"	@ section name, 12 bytes
		.byte	0x00
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		.word	0xffffffff	@ other sections not used
		
		@ CHSETTINGS item
		.word	0xc0c0c0c1	@ section key
		.byte	0x00		@ disabled section
		.byte	0x01		@ version
		.byte	0x00		@ reserved
		.byte	0x00		@ reserved
		.word	0x00		@ clocking section (not used here)
		
		.org	0x0200		@ GP header
		.word	_etext-_start_	@ size of image
		.word	_start_		@ image destination
		
_start_:	b	_init_		

