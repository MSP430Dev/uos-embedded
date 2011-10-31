#include "asm-regs.h"
@======================================================================================
@
@======================================================================================
						.syntax unified
						.thumb
						.text
@======================================================================================
@ Read_Ethernet_Status();
@ r0 - Ethernet Status
@======================================================================================
						.align	2
						.global	Read_Ethernet_Status
						.thumb
						.thumb_func
						.type	Read_Ethernet_Status, %function
Read_Ethernet_Status:
						push	{r1,lr}
						ldr		r1,=(Base_Register + Register_3)
						ldr		r0,[r1,#0]
						ldr		r1,=Ethernet_Status
						and		r0,r0,r1
						lsr		r0,r0,#Etnernet_Status_Shift
						pop		{r1,pc}
						.size	Read_Ethernet_Status, .-Read_Ethernet_Status
@======================================================================================
@ Read_Ethernet_Register(r, &v);	r0 - Register Address	 r1 - Register Value
@ Read_Ethernet_Descriptor(d, &v);	r0 - Descriptor	Address	 r1 - Register Value
@ Read_Ethernet_Buffer(b, &v);		r0 - Buffer Address		 r1 - Register Value
@======================================================================================
						.align	2
						.global	Read_Ethernet_Register
						.thumb
						.thumb_func
						.type	Read_Ethernet_Register, %function
Read_Ethernet_Register:
						b		Read_Ethernet_Buffer
						.align	2
						.global	Read_Ethernet_Descriptor
						.thumb
						.thumb_func
						.type	Read_Ethernet_Descriptor, %function
Read_Ethernet_Descriptor:
						b		Read_Ethernet_Buffer
						.align	2
						.global	Read_Ethernet_Buffer
						.thumb
						.thumb_func
						.type	Read_Ethernet_Buffer, %function
Read_Ethernet_Buffer:
						push	{r0-r2,lr}
						lsl		r0,#2
						ldr		r2,=(((Base_Register + Register_1) | RW) & (~CSe))		@ CSe[A16] = 0 RW[A24] = 1
						add		r2,r2,r0
						ldr		r2,[r2,#0]
						nop
						nop
						nop
						nop
						nop
						nop
						nop
						nop
						ldr		r2,=(Base_Register + Register_4)						@ Register in
						add		r2,r2,r0
						ldr		r0,[r2,#0]
						strh	r0,[r1,#0]
						ldr		r2,=(((Base_Register + Register_1) & (~RW)) | (CSe))	@ CSe[A16] = 1 RW[A24] = 0
						ldr		r2,[r2,#0]
						pop		{r0-r2,pc}
						.size	Read_Ethernet_Register, .-Read_Ethernet_Register
@======================================================================================
@ Write_Ethernet_Register(r, &v);	r0 - Register Address	 r1 - Register Value
@ Write_Ethernet_Descriptor(d, &v);	r0 - Descriptor Address	 r1 - Register Value
@ Write_Ethernet_Buffer(b, &v);		r0 - Buffer Address		 r1 - Register Value
@======================================================================================
						.align	2
						.global	Write_Ethernet_Register
						.thumb
						.thumb_func
						.type	Write_Ethernet_Register, %function
Write_Ethernet_Register:
						b		Write_Ethernet_Buffer
						.align	2
						.global	Write_Ethernet_Descriptor
						.thumb
						.thumb_func
						.type	Write_Ethernet_Descriptor, %function
Write_Ethernet_Descriptor:
						b		Write_Ethernet_Buffer
						.align	2
						.global	Write_Ethernet_Buffer
						.thumb
						.thumb_func
						.type	Write_Ethernet_Buffer, %function
Write_Ethernet_Buffer:
						push	{r0-r6,lr}
						lsl		r6,r0,#2
						ldr		r3,=GPIOC_Port
						ldr		r4,=(~REL)
						ldr		r5,=REL
						ldr		r2,=(Base_Register + Register_0)						@ Register out
						add		r2,r2,r6
						ldr		r0,[r1,#0]
						str		r0,[r2,#0]
						ldr		r0,[r3,#0]												@ REL = 0
						and		r0,r0,r4
						str		r0,[r3,#0]
						ldr		r2,=((Base_Register + Register_1) & (~(CSe+RW)))		@ CSe[A16] = 0 RW[A24] = 0
						add		r2,r2,r6
						ldr		r2,[r2,#0]
						nop
						nop
						nop
						nop
						nop
						nop
						nop
						nop
						ldr		r2,=(((Base_Register + Register_1) & (~RW)) | (CSe))	@ CSe[A16] = 1 RW[A24] = 0
						add		r2,r2,r6
						ldr		r2,[r2,#0]
						ldr		r0,[r3,#0]												@ REL = 1
						orr		r0,r0,r5
						str		r0,[r3,#0]
						pop		{r0-r6,pc}
						.size	Write_Ethernet_Register, .-Write_Ethernet_Register
@======================================================================================
@
@======================================================================================

