#define __SFR_OFFSET 0
#include <runtime/avr/io.h>

#if WITH_UNDERSCORES
#	define _(x) _##x
#else
#	define _(x) x
#endif

#if FLASHEND > 0x1FFFF
#	define JMP(label) ejmp _(label)
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
	JMP(_uart_data_)
	JMP(_uart_trans_)
	JMP(_adc_)
	JMP(_eeprom_ready_)
	JMP(_comparator_)

	JMP(_output_compare1c_)
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
	ldi	r28, lo8 (_edata-0x100)	; load size of .data image
	ldi	r29, hi8 (_edata-0x100)
	sbiw	r28,0			; if (counter == 0)...
	breq	.clear_bss		; ...skip loop

	ldi	r26, lo8 (0x100)	; load start of ram
	ldi	r27, hi8 (0x100)

	ldi	r30, lo8 (_etext)	; load the end of .text segment
	ldi	r31, hi8 (_etext)	; this is a start of .data image
#if FLASHEND > 0x2000
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
#if FLASHEND <= 0x2000
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
	clr	r1
#if FLASHEND > 0x2000
	out	RAMPZ, r1
#endif
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

#if 0 /* LY: будут использоваться обработчики из kernel/avr/machdep.c,
       *     они выполняют маскирование необрабатываемых прерываний.
       *     это более ясное, простое и надежное решение для общего случая,
       *     при необходимости код приложения может явно пределить
       *     свои обработчики в виде 'reti'.
       */
.section .text
	.func	.weaks		; stay avr-as happy with -gstabs
.weaks:
	.weak	_(_interrupt0_)
	.weak	_(_interrupt1_)
	.weak	_(_interrupt2_)
	.weak	_(_interrupt3_)
	.weak	_(_interrupt4_)
	.weak	_(_interrupt5_)
	.weak	_(_interrupt6_)
	.weak	_(_interrupt7_)
	.weak	_(_output_compare2_)
	.weak	_(_overflow2_)
	.weak	_(_input_capture1_)
	.weak	_(_output_compare1a_)
	.weak	_(_output_compare1b_)
	.weak	_(_overflow1_)
	.weak	_(_output_compare0_)
	.weak	_(_overflow0_)
	.weak	_(_spi_)
#if 0  /* vch: no default handler for UARTs, uOS or superfast handler used  */
	.weak	_(_uart_recv_)
	.weak	_(_uart1_recv_)
	.weak	_(_uart_data_)
	.weak	_(_uart1_data_)
	.weak	_(_uart_trans_)
	.weak	_(_uart1_trans_)
#endif
	.weak	_(_adc_)
	.weak	_(_eeprom_ready_)
	.weak	_(_comparator_)
	.weak   _(_output_compare1c_)
	.weak   _(_input_capture3_)
	.weak   _(_output_compare3a_)
	.weak   _(_output_compare3b_)
	.weak   _(_output_compare3c_)
	.weak   _(_overflow3_)
	.weak   _(_twi_)
	.weak   _(_spm_ready_)

_(_interrupt0_):
_(_interrupt1_):
_(_interrupt2_):
_(_interrupt3_):
_(_interrupt4_):
_(_interrupt5_):
_(_interrupt6_):
_(_interrupt7_):
_(_output_compare2_):
_(_overflow2_):
_(_input_capture1_):
_(_output_compare1a_):
_(_output_compare1b_):
_(_overflow1_):
_(_output_compare0_):
_(_overflow0_):
_(_spi_):
#if 0  /* vch: no default handler for UARTs, uOS or superfast handler used */
_(_uart_recv_):
_(_uart1_recv_):
_(_uart_data_):
_(_uart1_data_):
_(_uart_trans_):
_(_uart1_trans_):
#endif
_(_adc_):
_(_eeprom_ready_):
_(_comparator_):
_(_output_compare1c_):
_(_input_capture3_):
_(_output_compare3a_):
_(_output_compare3b_):
_(_output_compare3c_):
_(_overflow3_):
_(_twi_):
_(_spm_ready_):
	reti
#endif /* LY: end of 'if 0' */
