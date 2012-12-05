		.section .init
		.code	32

#ifdef AT91BOOTSTRAP
_start_:	.global	_start_

		@
		@ Branch instruction could address only lower 64 Mbytes.
		@ We use LDR instruction to jump to higher flash memory.
		@
		ldr	pc, [pc, #24]
		ldr	pc, [pc, #24]
		ldr	pc, [pc, #24]
		ldr	pc, [pc, #24]
		ldr	pc, [pc, #24]
		.word	_edata - _start_	@ length of code+data
		ldr	pc, [pc, #24]
		ldr	pc, [pc, #24]
		.word	_reset_
		.word	_undefined_
		.word	_swi_
		.word	_prefetch_
		.word	_abort_
		.word	_reset_			@ cannot happen
		.word	_irq_
		.word	_fiq_
		
#elif defined(ARM_OMAP44XX)

		.extern	_etext
		.extern _init_
		
		.org	0x0
		.global	_start_
		.global _irq_
		.global _undefined_
		.global _swi_
		.global _prefetch_
		.global _fiq_
		.global _abort_
		
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
		
_start_:	b	_reset_

#else
_start_:	.global	_start_

		@
		@ Jump to exception handlers.
		@
		b	_reset_
		b	_undefined_
		b	_swi_
		b	_prefetch_
		b	_abort_
		b	_reset_			@ cannot happen
		b	_irq_
		b	_fiq_
#endif

		@
		@ Space for irq stack. We need 2 words here,
		@ to save LR and SPSR before switching
		@ from IRQ to supervisor mode.
		@
		.comm	_irq_stack_, 12

		.section .text
		@
		@ Startup initialization.
		@
_reset_:
		mrs	r0, cpsr
		bic	r0, r0, #0x1f
		orr	r0, r0, #0xd2
		msr	cpsr, r0		@ switch to IRQ mode

		ldr	sp, =_irq_stack_+12	@ set irq stack

		orr	r0, r0, #0xd3
		msr	cpsr, r0		@ switch to supervisor mode
#if __thumb__
		@ Jump to _init_ in thumb mode.
		ldr	lr, =_init_+1
		bx	lr
#else
		b	_init_
#endif
		@
		@ Interrupt handler.
		@ When entering interrupt or switching task,
		@ we save the following data in the stack:
		@	PC	- at SP-4
		@	CPSR	- at SP-8
		@	LR	- at SP-12
		@	R12	- at SP-16
		@	...
		@	R0	- at SP-64
		@ Total 16 words or 64 bytes.
		@
_irq_:
		stmfd	sp, {r0-r2}		@ save R0-R2 to irq stack
		sub	r1, lr, #4		@ adjust saved PC, move to R1
		mrs	r0, spsr		@ move SPSR to r0

		mrs	r2, cpsr
		orr	r2, r2, #1
		msr	cpsr, r2		@ switch to supervisor mode

		stmfd	sp!, {r0-r1}		@ save SPSR, saved PC
		ldr	r0, =_irq_stack_
		ldmfd	r0, {r0-r2}		@ load saved R0-R1
		stmfd	sp!, {r0-r12,lr}	@ save R0-R12,LR to task stack
#if __thumb__
		@ Jump to _irq_handler_ in thumb mode.
		ldr	lr, =_irq_handler_+1
		bx	lr
#else
		b	_irq_handler_
#endif
		@
		@ Stubs for exception handlers.
		@
		.comm   _dump_stack_, 13*4
_undefined_:
		ldr	sp, =_dump_stack_+13*4
		stmfd	sp, {r0-r12}		@ save r0-r12
		sub	r0, lr, #0
		mrs	r1, spsr

		mrs	r2, cpsr
		bic	r2, r2, #0x1f
		orr	r2, r2, #0xd3
		msr	cpsr, r2		@ switch to supervisor mode

		mov	r2, lr
#if __thumb__
		ldr	lr, =_undef_handler_+1
		bx	lr
#else
		b	_undef_handler_
#endif
_swi_:
		ldr	sp, =_dump_stack_+13*4
		stmfd	sp, {r0-r12}		@ save r0-r12
		sub	r0, lr, #0
		mrs	r1, spsr

		mrs	r2, cpsr
		bic	r2, r2, #0x1f
		orr	r2, r2, #0xd3
		msr	cpsr, r2		@ switch to supervisor mode

		mov	r2, lr
#if __thumb__
		ldr	lr, =_swi_handler_+1
		bx	lr
#else
		b	_swi_handler_
#endif
_prefetch_:
		ldr	sp, =_dump_stack_+13*4
		stmfd	sp, {r0-r12}		@ save r0-r12
		sub	r0, lr, #4
		mrs	r1, spsr

		mrs	r2, cpsr
		bic	r2, r2, #0x1f
		orr	r2, r2, #0xd3
		msr	cpsr, r2		@ switch to supervisor mode

		mov	r2, lr
#if __thumb__
		ldr	lr, =_prefetch_handler_+1
		bx	lr
#else
		b	_prefetch_handler_
#endif
_fiq_:
		ldr	sp, =_dump_stack_+13*4
		stmfd	sp, {r0-r12}		@ save r0-r12
		sub	r0, lr, #4
		mrs	r1, spsr

		mrs	r2, cpsr
		bic	r2, r2, #0x1f
		orr	r2, r2, #0xd3
		msr	cpsr, r2		@ switch to supervisor mode

		mov	r2, lr
#if __thumb__
		ldr	lr, =_fiq_handler_+1
		bx	lr
#else
		b	_fiq_handler_
#endif
_abort_:
		ldr	sp, =_dump_stack_+13*4
		stmfd	sp, {r0-r12}		@ save r0-r12
		sub	r0, lr, #8
		mrs	r1, spsr

		mrs	r2, cpsr
		bic	r2, r2, #0x1f
		orr	r2, r2, #0xd3
		msr	cpsr, r2		@ switch to supervisor mode

		mov	r2, lr
#if __thumb__
		ldr	lr, =_abort_handler_+1
		bx	lr
#else
		b	_abort_handler_
#endif
