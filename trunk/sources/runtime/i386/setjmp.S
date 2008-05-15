#define JB_BX	0
#define JB_SI	1
#define JB_DI	2
#define JB_BP	3
#define JB_SP	4
#define JB_PC	5
#define JB_SIZE 24

		.section .text
setjmp:		.globl	setjmp

		movl	4(%esp), %edx

		/* Save registers */
		movl	%ebx, (JB_BX*4)(%edx)
		movl	%esi, (JB_SI*4)(%edx)
		movl	%edi, (JB_DI*4)(%edx)
		leal	4(%esp), %ecx		/* SP as it will be after we return */
		movl	%ecx, (JB_SP*4)(%edx)
		movl	(%esp), %ecx		/* PC we are returning to now */
		movl	%ecx, (JB_PC*4)(%edx)
		movl	%ebp, (JB_BP*4)(%edx)	/* Caller's frame pointer */

		ret
