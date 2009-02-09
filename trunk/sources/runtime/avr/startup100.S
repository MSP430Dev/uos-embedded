;
; Startup file with SRAM started at address 0x100.
; For ATmega128, ATmega161, ATmega168 and others.
;
#define __SFR_OFFSET 0
#include <runtime/avr/io.h>

#if FLASHEND > 0x2000
#   define JMP(label) jmp label
#else
#   define JMP(label) rjmp label
#endif

	.section	.init
	.func	__ld_entry
	.globl __ld_entry
__ld_entry:
	JMP(_init_)

#include "vectors.S"

_init_:
	; Clear zero register
	clr	r1

	; Setup stack to end of internal memory
	ldi	r16, lo8(__stack)
	out	SPL, r16
	ldi	r16, hi8(__stack)
	out	SPH, r16

	; Copy the .data image from flash to ram.
	; Linker places it at the end of .text segment.
	; Register usage:
	; X (r26:27) - destination pointer (ram)
	; Y (r28:29) - byte counter (size of .data segment)
	; Z (r30:31:RAMPZ) - source pointer (flash)
	;
	ldi	r28, lo8 (_edata-0x100)	; load size of .data image
	ldi	r29, hi8 (_edata-0x100)
	sbiw	r28, 0			; if (counter == 0)...
	breq	.clear_bss		; ...skip loop

	ldi	r26, lo8 (0x100)	; load start of ram
	ldi	r27, hi8 (0x100)

	ldi	r30, lo8 (_etext)	; load the end of .text segment
	ldi	r31, hi8 (_etext)	; this is a start of .data image
#ifdef RAMPZ
	ldi	r25, hh8 (_etext)
	out	RAMPZ, r25
	tst	r25			; if high memory...
	brne	.copy_data_high		; go to loop #2
#endif
	;
	; Loop #1: source in lower flash memory.
	;
.copy_data:
	lpm				; load byte by Z pointer into r0
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
#ifndef RAMPZ
	rjmp	.copy_data		; continue
#else
	brcc	.copy_data		; if no overflow continue

	;
	; Loop #2: source in high flash memory.
	;
	ldi	r25, 1
	out	RAMPZ, r25
.copy_data_high:
	elpm				; load byte by Z pointer into r0
	adiw	r30, 1			; increment Z pointer
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	brne	.copy_data_high		; if (counter != 0) continue
#endif

.clear_bss:
	;; avr-ld always put .bss after .data
#ifdef RAMPZ
	out	RAMPZ, r1
#endif
	ldi	r26, lo8(__bss_start)
	ldi	r27, hi8(__bss_start)
	ldi	r16, hi8(__bss_end)
	rjmp	1f
2:
	st	X+, r1
1:	cpi	r26, lo8(__bss_end)
	cpc	r27, r16
	brne	2b

	JMP(main)
.end_of_init:
	.endfunc
