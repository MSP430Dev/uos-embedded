#define __SFR_OFFSET 0
#include <runtime/avr/io.h>

#if WITH_UNDERSCORES
#define _(x) _##x
#else
#define _(x) x
#endif

#if FLASHEND > 0x2000
#define JMP(label) jmp _(label)
#else
#define JMP(label) rjmp _(label)
#endif

	.section	.init
	.func	__ld_entry
	.globl __ld_entry
__ld_entry:
	JMP(_init_)
	JMP(_interrupt0_)
	JMP(_interrupt1_)
	JMP(_interrupt2_)
	JMP(_output_compare2_)
	JMP(_overflow2_)
	JMP(_input_capture1_)
	JMP(_output_compare1a_)
	JMP(_output_compare1b_)
	JMP(_overflow1_)
	JMP(_output_compare0_)
	JMP(_overflow0_)
	JMP(_spi_)
	JMP(_uart_recv_)
	JMP(_uart1_recv_)
	JMP(_uart_data_)
	JMP(_uart1_data_)
	JMP(_uart_trans_)
	JMP(_uart1_trans_)
	JMP(_eeprom_ready_)
	JMP(_comparator_)

_(_init_):
#	include "avr-asm-init.inc"

	;
	; Copy the .data image from flash to ram.
	; Linker places it at the end of .text segment.
	; Register usage:
	; X (r26:27) - destination pointer (ram)
	; Y (r28:29) - byte counter (size of .data segment)
	; Z (r30:31:RAMPZ) - source pointer (flash)
	;
	ldi	r28, lo8 (_edata-0x60)	; load size of .data image
	ldi	r29, hi8 (_edata-0x60)
	sbiw	r28,0			; if (counter == 0)...
	breq	.clear_bss		; ...skip loop

	ldi	r26, 0x60		; load start of ram
	ldi	r27, 0

	ldi	r30, lo8 (_etext)	; load the end of .text segment
	ldi	r31, hi8 (_etext)	; this is a start of .data image

	;
	; Loop #1: source in lower flash memory.
	;
.copy_data:
	lpm				; load byte by Z pointer into r0
	st	X+, r0			; store byte by X++
	sbiw	r28, 1			; decrement counter
	breq	.clear_bss		; if (counter == 0) done
	adiw	r30, 1			; increment Z pointer
	rjmp	.copy_data		; continue

.clear_bss:
	;; avr-ld always put .bss after .data
	clr	r1
	ldi	r26, lo8(__bss_start)
	ldi	r27, hi8(__bss_start)
	ldi	r16, hi8(__bss_end)
	rjmp	1f
2:
	st	X+,r1
1:	cpi	r26, lo8(__bss_end)
	cpc	r27, r16
	brne	2b

	JMP(main)
.end_of_init:
	.endfunc
