;
; Startup file with SRAM started at address 0x200.
; For ATmega256 and others.
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
	ldi	r28, lo8 (_edata-0x200)	; load size of .data image
	ldi	r29, hi8 (_edata-0x200)
	sbiw	r28, 0			; if (counter == 0)...
	breq	.clear_bss		; ...skip loop

	ldi	r26, lo8 (0x200)	; load start of ram
	ldi	r27, hi8 (0x200)

	ldi	r30, lo8 (_etext)	; load the end of .text segment
	ldi	r31, hi8 (_etext)	; this is a start of .data image

	ldi	r25, hh8 (_etext)
	out	RAMPZ, r25
	tst	r25
	breq	.copy_data_0_64
        subi    r25,  1
        breq	.copy_data_64_128
        subi    r25,  1
        breq	.copy_data_128_192
        subi    r25,  1
        breq	.copy_data_192_256

.copy_data_0_64:
	lpm				; load byte by Z pointer into r0
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	brcc	.copy_data_0_64		; if no overflow continue

.copy_data_64_128:
        ldi	r25, 1
	out	RAMPZ, r25
        elpm
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	brcc	.copy_data_64_128	; if no overflow continue

.copy_data_128_192:
        ldi	r25, 2
	out	RAMPZ, r25
        elpm
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	brcc	.copy_data_128_192	; if no overflow continue

.copy_data_192_256:
        ldi	r25, 3
	out	RAMPZ, r25
        elpm
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	brcc	.copy_data_192_256	; if no overflow continue

.clear_bss:
	;; avr-ld always put .bss after .data
	ldi	r26, lo8(__bss_start)
	ldi	r27, hi8(__bss_start)
	ldi	r16, hi8(__bss_end)
	rjmp	1f
2:
	st	X+, r1
1:	cpi	r26, lo8(__bss_end)
	cpc	r27, r16
	brne	2b

        ldi	r25, hh8(__progmem_start)       ; VK: for const string in .progmem
	out	RAMPZ, r25

	JMP(main)
.end_of_init:
	.endfunc
