;
; Startup code for MSP430 microcontroller.
;

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
		VEC vector_fffa
		VEC vector_fffc

;---------------------------------------
; Reset vector at FFFE.
;
		.extern	_init_
		.word _init_
