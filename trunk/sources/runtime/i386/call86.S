#include "io.h"

/*
 * This program is loaded at address 0x500.
 * Input registers must be placed at 7E0 and 8 bytes of 16-bit code - at 560.
 * Output parameters will be put at 7E0.
 * It must be called with interrupts disabled.
 */
code_offset	=	0x60
regs		=	0x7e0

		.text
		.code32
_start:		.globl	_start
		pushal

		movl	%esp, %eax		/* Save stack pointer */
		movl	%eax, savedstack

		movl 	$regs, %esp		/* Set up internal stack */

		xorl	%eax, %eax		/* Clear long registers */
		movl	%eax, %ebx
		movl	%eax, %ecx
		movl	%eax, %edx
		movl	%eax, %esi
		movl	%eax, %edi
		movl	%eax, %ebp

		ljmp	$I386_RM_CS, $1f	/* Jump to a 16 bit segment */
1:		.code16

		movl	%cr0, %eax		/* Clear PE bit of CR0 */
		andb 	$0xfe, %al
		movl	%eax, %cr0

		DATA32	ljmp	$0, $2f		/* Reload CS */
2:
		/*
		 * We are in real mode now.
		 */
		xorl	%eax, %eax		/* Set up real mode segments */
		movw	%ax, %ss
		movw	%ax, %fs
		movw	%ax, %gs

		popw	%ax			/* Fetch input params */
		popw	%bx
		popw	%cx
		popw	%dx
		popw	%si
		popw	%di
		popw	%bp
		pop	%ds
		pop	%es
		addw	$2, %sp

		jmp	code0

		/*
		 * Leave some free space for future modifications.
		 */
		. = _start + code_offset
code0:		nop				/* Put 16-bit code here */
		nop
		nop
		nop
code1:		nop
		nop
		nop
		nop

		cli				/* Disable interrupts */

		pushf				/* Store output params */
		push	%es
		push	%ds
		pushw	%bp
		pushw	%di
		pushw	%si
		pushw	%dx
		pushw	%cx
		pushw	%bx
		pushw	%ax

		movl	%cr0, %eax		/* Turn on protected mode */
		orb	$1, %al
		movl	%eax, %cr0

		DATA32	ljmp	$I386_CS, $3f	/* Reload CS */
3:		.code32

		/*
		 * Now in 32-bit mode.
		 */
		movl	$I386_DS, %eax		/* Set up 32-bit segments */
		movw	%ax, %ds
		movw	%ax, %es
		movw	%ax, %fs
		movw	%ax, %gs
		movw	%ax, %ss

		movl	savedstack, %eax	/* Get protected mode stack */
		movl	%eax, %esp

		popal
		ret				/* Return on the old stack! */

		.align 	4
savedstack:	.long	0
