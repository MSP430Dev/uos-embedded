#ifndef AT91BOOTSTRAP
		.section .init
		.code	32
_start_:	.global	_start_
		b	_reset_
		b	_undefined_
		b	_swi_
		b	_prefetch_
		b	_abort_
		b	_undefined_		@ reserved
		b	_irq_
		b	_fiq_
_reset_:
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
#ifdef AT91BOOTSTRAP
_start_:	.global	_start_
#endif
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
