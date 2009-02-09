#define __SFR_OFFSET 0
#include <runtime/avr/io.h>

#if WITH_UNDERSCORES
#	define _(x) _##x
#else
#	define _(x) x
#endif

#if FLASHEND > 0x1FFFF
#	define JMP(label) jmp _(label)
#elif FLASHEND > 0x2000
#	define JMP(label) jmp _(label)
#else
#	define JMP(label) rjmp _(label)
#endif

	.section	.init
	.func	__ld_entry
	.globl __ld_entry
__ld_entry:
	JMP(_init_)
	JMP(_interrupt0_)
	JMP(_interrupt1_)
	JMP(_interrupt2_)
	JMP(_interrupt3_)
	JMP(_interrupt4_)
	JMP(_interrupt5_)
	JMP(_interrupt6_)
	JMP(_interrupt7_)
        JMP(_pcinterrupt0_)
	JMP(_pcinterrupt1_)
	JMP(_pcinterrupt2_)
        JMP(_wdt_timeout_)
        JMP(_output_compare2a_)
	JMP(_output_compare2b_)
	JMP(_overflow2_)
	JMP(_input_capture1_)
	JMP(_output_compare1a_)
	JMP(_output_compare1b_)
        JMP(_output_compare1c_)
	JMP(_overflow1_)
        JMP(_output_compare0a_)
	JMP(_output_compare0b_)
	JMP(_overflow0_)
        JMP(_spi_)
        JMP(_uart_recv_)
	JMP(_uart_data_)
	JMP(_uart_trans_)
        JMP(_comparator_)
        JMP(_adc_)
        JMP(_eeprom_ready_)
        JMP(_input_capture3_)
	JMP(_output_compare3a_)
	JMP(_output_compare3b_)
	JMP(_output_compare3c_)
	JMP(_overflow3_)
        JMP(_uart1_recv_)
	JMP(_uart1_data_)
	JMP(_uart1_trans_)
        JMP(_twi_)
        JMP(_spm_ready_)
        JMP(_input_capture4_)
	JMP(_output_compare4a_)
	JMP(_output_compare4b_)
	JMP(_output_compare4c_)
	JMP(_overflow4_)
        JMP(_input_capture5_)
	JMP(_output_compare5a_)
	JMP(_output_compare5b_)
	JMP(_output_compare5c_)
	JMP(_overflow5_)
        JMP(_uart2_recv_)
	JMP(_uart2_data_)
	JMP(_uart2_trans_)
        JMP(_uart3_recv_)
	JMP(_uart3_data_)
	JMP(_uart3_trans_)

_(_init_):
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
