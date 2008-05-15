	cli				; LY: disable interrupt in case jmp0

	in	r30, 0x3D		; LY: save SP-value on reset/jmp0
	in	r31, 0x3E
	sts	_(broken_stack), r30
	sts	_(broken_stack) + 1, r31

	lds	r30, _(task_current)	; LY: save task_current
	lds	r31, _(task_current) + 1
	sts	_(task_broken), r30
	sts	_(task_broken) + 1, r31
					; LY: save task_current->stack_context
	ldd	r28, Z + _(task_stack_context_offset)
	ldd	r29, Z + _(task_stack_context_offset) + 1
	sts	_(task_broken_stack), r28
	sts	_(task_broken_stack) + 1, r29

	ldi	r28, lo8(__stack)	; LY: setup stack to end of internal SRAM
	ldi	r29, hi8(__stack)
	out	0x3E, r29
	out	0x3D, r28
