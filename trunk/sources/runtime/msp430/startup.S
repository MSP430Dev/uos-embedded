;
; Startup code for MSP430 microcontroller.
;
		.extern	_init_

;---------------------------------------
; Handler for unexpected interrupt.
;
		.weak   _unexpected_interrupt_
_unexpected_interrupt_:
		reti

;---------------------------------------
; Macro: setup an interrupt vector.
; By default, an _unexpected_interrupt_() routine is called.
;
		.macro  VEC name
		.weak   \name
		.set    \name, _unexpected_interrupt_
		.word   \name
		.endm

;---------------------------------------
; Interrupt vectors at FFE0-FFFC.
;
		.section .vectors, "ax", @progbits
#if defined(__MSP430_5418__) || defined(__MSP430_5419__) || \
    defined(__MSP430_5435__) || defined(__MSP430_5436__) || \
    defined(__MSP430_5437__) || defined(__MSP430_5438__)

		VEC vector_ff80
		VEC vector_ff82
		VEC vector_ff84
		VEC vector_ff86
		VEC vector_ff88
		VEC vector_ff8a
		VEC vector_ff8c
		VEC vector_ff8e
		VEC vector_ff90
		VEC vector_ff92
		VEC vector_ff94
		VEC vector_ff96
		VEC vector_ff98
		VEC vector_ff9a
		VEC vector_ff9c
		VEC vector_ff9e

		VEC vector_ffa0
		VEC vector_ffa2
		VEC vector_ffa4
		VEC vector_ffa6
		VEC vector_ffa8
		VEC vector_ffaa
		VEC vector_ffac
		VEC vector_ffae
		VEC vector_ffb0
		VEC vector_ffb2
		VEC vector_ffb4
		VEC vector_ffb6
		VEC vector_ffb8
		VEC vector_ffba
		VEC vector_ffbc
		VEC vector_ffbe

		VEC vector_ffc0
		VEC vector_ffc2
		VEC vector_ffc4
		VEC vector_ffc6
		VEC vector_ffc8
		VEC vector_ffca
		VEC vector_ffcc
		VEC vector_ffce
		VEC vector_ffd0
		VEC vector_ffd2
		VEC vector_ffd4
		VEC vector_ffd6
		VEC vector_ffd8
		VEC vector_ffda
		VEC vector_ffdc
		VEC vector_ffde
#endif
		VEC vector_ffe0
		VEC vector_ffe2
		VEC vector_ffe4
		VEC vector_ffe6
		VEC vector_ffe8
		VEC vector_ffea
		VEC vector_ffec
		VEC vector_ffee
		VEC vector_fff0
		VEC vector_fff2
		VEC vector_fff4
		VEC vector_fff6
		VEC vector_fff8
#if defined(__MSP430_5418__) || defined(__MSP430_5419__) || \
    defined(__MSP430_5435__) || defined(__MSP430_5436__) || \
    defined(__MSP430_5437__) || defined(__MSP430_5438__)
		.word _init_
		.word _init_
#else
		VEC vector_fffa
		VEC vector_fffc
#endif

;---------------------------------------
; Reset vector at FFFE.
;
		.word _init_
