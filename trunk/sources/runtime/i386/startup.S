/*
 * uOS startup.
 * Called from GRUB boot loader.
 */
#include <runtime/i386/io.h>

#if WITH_UNDERSCORES
#define _(x) _##x
#else
#define _(x) x
#endif

/* The magic number for the Multiboot header. */
#define HEADER_MAGIC	0x1BADB002

/* The flags for the Multiboot header. */
#ifdef __ELF__
#define HEADER_FLAGS	0x00000003
#else
#define HEADER_FLAGS	0x00010003
#endif

/* The size of our stack (64KB). */
#define STACKSZ		0x10000

		.section .text
_start:		.globl	_start
		jmp	entry

		/* Multiboot header. */
header:		.align	4		/* Align 32 bits boundary. */
		.long	HEADER_MAGIC	/* magic */
		.long	HEADER_FLAGS	/* flags */
		.long	-(HEADER_MAGIC + HEADER_FLAGS)	/* checksum */
#ifndef __ELF__
		.long	header		/* header_addr */
		.long	_start		/* load_addr */
		.long	_edata		/* load_end_addr */
		.long	_end		/* bss_end_addr */
		.long	entry		/* entry_addr */
#endif
		.comm	stack, STACKSZ	/* Our stack area. */
entry:
		movl	$(stack + STACKSZ), %esp	/* Set stack pointer */

		pushl	$0
		popf			/* Reset EFLAGS. */

		clts			/* Clear task switch bit in CR0. */
		fninit			/* Initialize float point unit. */
		movl	%cr0, %eax
		orb 	$2, %al		/* Set MP bit in CR0. */
		movl	%eax, %cr0

		pushl	%ebx		/* Pointer to the Multiboot info. */
		call	_(_init_)	/* Now enter the C main function... */

1:		hlt			/* Halted. */
		jmp	1b
