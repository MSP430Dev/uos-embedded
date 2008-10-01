#include <runtime/lib.h>
#include <runtime/i386/multiboot.h>
#include <runtime/i386/i8259.h>
#include <runtime/i386/int86.h>

static const unsigned char gdt [] = {
	0,    0,    0, 0, 0,    0,    0,    0,	/* Null descriptor */
	0xff, 0xff, 0, 0, 0,    0x9b, 0xcf, 0,	/* Command segment */
	0xff, 0xff, 0, 0, 0,    0x93, 0xcf, 0,	/* Data segment */
	0xff, 0xff, 0, 0, 0,	0x9b, 0,    0,	/* Real mode commands */
	0xff, 0xff, 0, 0, 0,	0x93, 0,    0,	/* Real mode data */
};

const struct {
	unsigned short unused;
	unsigned short length;
	const unsigned char *address;
} i386_gdtr [] = {
	{ 0, sizeof (gdt) - 1, gdt }
};

static volatile i386_idt_t idt [256];
unsigned long i386_highmem_addr;
unsigned long i386_highmem_len;
unsigned long i386_lowmem_len;

extern void main (void);

static void
intr_setidt (int vector, int user, void (*proc) ())
{
	idt[vector].offset16 = (unsigned short) (unsigned long) proc;
	idt[vector].offset32 = (unsigned long) proc >> 16;
	idt[vector].segm     = I386_CS;
	idt[vector].reserved = 0;
	idt[vector].type     = SEG_INTR;
	if (user)
		idt[vector].type |= SEG_DPL3;
}

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from gcrt1.S.
 */
void
_init_ (multiboot_info_t *mbi)
{
	unsigned long *dest, *limit;
	int i;
	extern unsigned long _edata, _end;
	i386_dtr_t idtr;
	void i386_intr_divzero (), i386_intr_debug (), i386_intr_nmi ();
	void i386_intr_break (), i386_intr_ovflow (), i386_intr_bounds ();
	void i386_intr_invop (), i386_intr_no387 (), i386_intr_dblfault ();
	void i386_intr_seg387 (), i386_intr_invtss (), i386_intr_nosegm ();
	void i386_intr_nostack (), i386_intr_prot (), i386_intr_pagefault ();
	void i386_intr_fpe (), i386_intr_align (), i386_intr_0 ();
	void i386_intr_1 (), i386_intr_2 (), i386_intr_3 (), i386_intr_4 ();
	void i386_intr_5 (), i386_intr_6 (), i386_intr_7 (), i386_intr_8 ();
	void i386_intr_9 (), i386_intr_10 (), i386_intr_11 (), i386_intr_12 ();
	void i386_intr_13 (), i386_intr_14 (), i386_intr_15 ();
	void i386_intr_stray ();

	/* Set up GDTR. */
	asm (
"	lgdt	i386_gdtr + 2 \n"
"	movl	$0x10, %eax \n"
"	mov	%ax, %ds \n"
"	mov	%ax, %ss \n"
"	mov	%ax, %es \n"
"	mov	%ax, %gs \n"
"	mov	%ax, %fs \n"
"	ljmp	$0x08, $1f \n"
"1:");
	/* Initialize .bss segment by zeroes. */
	dest = &_edata;
	limit = &_end;
	while (dest < limit)
		*dest++ = 0;

	/* Are mem_* valid? */
	if (mbi->flags & MB_INFO_MEMORY)
		debug_printf ("mem_lower = %uKB, mem_upper = %uKB\n",
		    (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

	/* Is boot_device valid? */
	if (mbi->boot_device >> 24 == 0xff)
		mbi->flags &= ~MB_INFO_BOOTDEV;
	if (mbi->flags & MB_INFO_BOOTDEV)
		debug_printf ("boot_device = 0x%x, partition = 0x%x\n",
			(unsigned)mbi->boot_device >> 24,
			(unsigned)mbi->boot_device & 0xffffff);

	/* Is the command line passed? */
	if (mbi->flags & MB_INFO_CMDLINE)
		debug_printf ("cmdline = %s\n", (char*) mbi->cmdline);

	/* Are mmap_* valid? */
	if (mbi->flags & MB_INFO_MEM_MAP) {
		memory_map_t *mmap;

		for (mmap = (memory_map_t*) mbi->mmap_addr;
		    (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
		    mmap = (memory_map_t*) ((unsigned long)mmap
			+ mmap->size + sizeof (mmap->size))) {
			if (mmap->type == 1) {
				/* RAM */
				if (mmap->base_addr_low == 0) {
					/* Low 640 kbytes of memory. */
					i386_lowmem_len = mmap->length_low;
				} else if (! i386_highmem_addr) {
					/* Upper memory - use only
					 * the first upper memory region. */
					i386_highmem_addr = mmap->base_addr_low;
					i386_highmem_len = mmap->length_low;
				}
			}
		}
	}

	/* Stop floppy. When loading from floppy, it would continue rotating.
	 * To prevent damage, we stop it here. */
	if ((mbi->boot_device >> 24) <= 1)
		outb (0x0c, 0x3f2);

	/*
	 * Initialize 8259's.
	 */
	/* Reset and program the devices. */
	outb (PIC_CMD_INIT, PIC1_CTL);
	outb (PIC_CMD_INIT, PIC2_CTL);

	/* The first 32 entries are reserved by the processor.
	 * Use indexes 32-47 for the hardware interrupts. */
	outb (PIC_INIT2_VECTOR (32), PIC1_MASK);
	outb (PIC_INIT2_VECTOR (32 + 8), PIC2_MASK);

	/* Slave attached to master at line 2. */
	outb (PIC_INIT3_MASTER (2), PIC1_MASK);
	outb (PIC_INIT3_SLAVE (2),  PIC2_MASK);

	/* Set 8086 mode. */
	outb (PIC_INIT4_8086, PIC1_MASK);
	outb (PIC_INIT4_8086, PIC2_MASK);

	/* Leave interrupts masked, except IRQ2, which is cascaded. */
	outb (0xff & ~PIC_MASK(2), PIC1_MASK);
	outb (0xff, PIC2_MASK);

	/*
	 * Initialize the interrupt descriptors and IDT.
	 */
	memset ((void*) idt, 0, sizeof (idt));

	for (i=0; i<256; ++i)
		intr_setidt (i, 0, i386_intr_stray);

	/* Processor interrupts. */
	intr_setidt (0,  0, i386_intr_divzero);      /* fault */
	intr_setidt (1,  0, i386_intr_debug);        /* trap/fault */
	intr_setidt (2,  0, i386_intr_nmi);          /* nonmaskable interrupt */
	intr_setidt (3,  1, i386_intr_break);        /* trap */
	intr_setidt (4,  1, i386_intr_ovflow);       /* trap */
	intr_setidt (5,  0, i386_intr_bounds);       /* fault */
	intr_setidt (6,  0, i386_intr_invop);        /* fault */
	intr_setidt (7,  0, i386_intr_no387);        /* fault */
	intr_setidt (8,  0, i386_intr_dblfault);     /* abort, error code=0 */
	intr_setidt (9,  0, i386_intr_seg387);       /* fault, error code? */
	intr_setidt (10, 0, i386_intr_invtss);       /* fault, error code */
	intr_setidt (11, 0, i386_intr_nosegm);       /* fault, error code */
	intr_setidt (12, 0, i386_intr_nostack);      /* fault, error code */
	intr_setidt (13, 0, i386_intr_prot);         /* trap/fault, error code */
	intr_setidt (14, 0, i386_intr_pagefault);    /* fault, spec error code */
	intr_setidt (16, 0, i386_intr_fpe);          /* fault */
	intr_setidt (17, 0, i386_intr_align);        /* fault, error code=0 */

	/* Hardware interrupts. */
	intr_setidt (32, 0, i386_intr_0);
	intr_setidt (33, 0, i386_intr_1);
	intr_setidt (34, 0, i386_intr_2);
	intr_setidt (35, 0, i386_intr_3);
	intr_setidt (36, 0, i386_intr_4);
	intr_setidt (37, 0, i386_intr_5);
	intr_setidt (38, 0, i386_intr_6);
	intr_setidt (39, 0, i386_intr_7);
	intr_setidt (40, 0, i386_intr_8);
	intr_setidt (41, 0, i386_intr_9);
	intr_setidt (42, 0, i386_intr_10);
	intr_setidt (43, 0, i386_intr_11);
	intr_setidt (44, 0, i386_intr_12);
	intr_setidt (45, 0, i386_intr_13);
	intr_setidt (46, 0, i386_intr_14);
	intr_setidt (47, 0, i386_intr_15);

	/* Set up the interrupt descriptor table. */
	idtr.len = sizeof (idt) - 1;
	idtr.addrlow = (unsigned short) (unsigned long) idt;
	idtr.addrhigh = (unsigned short) ((unsigned long) idt >> 16);
	lidt (&idtr);

	main ();

	/* Halted. */
	asm (
"1:	hlt \n"
"	jmp 1b");

	/*
	 * Interrupt handlers.
	 */
#define HANDLE_IRQ(n,mask2)\
	asm ("i386_intr_"#n": \n"\
	"push	$"#n" \n"\
	"pushal \n"\
	"movb $0x20, %al \n"\
	mask2 "\n"\
	"outb %al, $0x20 \n"\
	"push %ds \n"\
	"push %es \n"\
	"mov %ss, %ax \n"\
	"mov %ax, %ds \n"\
	"mov %ax, %es \n"\
	"push %esp \n"\
	"call i386_intr")

	HANDLE_IRQ (0,	"");
	HANDLE_IRQ (1,	"");
	HANDLE_IRQ (2,	"");
	HANDLE_IRQ (3,	"");
	HANDLE_IRQ (4,	"");
	HANDLE_IRQ (5,	"");
	HANDLE_IRQ (6,	"");
	HANDLE_IRQ (7,	"");
	HANDLE_IRQ (8,	"outb %al, $0xA0");
	HANDLE_IRQ (9,	"outb %al, $0xA0");
	HANDLE_IRQ (10,	"outb %al, $0xA0");
	HANDLE_IRQ (11,	"outb %al, $0xA0");
	HANDLE_IRQ (12,	"outb %al, $0xA0");
	HANDLE_IRQ (13,	"outb %al, $0xA0");
	HANDLE_IRQ (14,	"outb %al, $0xA0");
	HANDLE_IRQ (15,	"outb %al, $0xA0");

	/*
	 * Trap handlers.
	 */
#define HANDLE_TRAP(n,name)\
	asm ("i386_"#name": \n"\
	"pushal \n"\
	"push %ds \n"\
	"push %es \n"\
	"mov %ss, %ax \n"\
	"mov %ax, %ds \n"\
	"mov %ax, %es \n"\
	"push %esp \n"\
	"push $"#n" \n"\
	"call i386_trap")

#define HANDLE_TRAP0(n,name)\
	asm ("i386_"#name": \n"\
	"push	$0 \n"\
	"pushal \n"\
	"push %ds \n"\
	"push %es \n"\
	"mov %ss, %ax \n"\
	"mov %ax, %ds \n"\
	"mov %ax, %es \n"\
	"push %esp \n"\
	"push $"#n" \n"\
	"call i386_trap")

	HANDLE_TRAP0 (0,  intr_divzero);
	HANDLE_TRAP0 (1,  intr_debug);
	HANDLE_TRAP0 (2,  intr_nmi);
	HANDLE_TRAP0 (3,  intr_break);
	HANDLE_TRAP0 (4,  intr_ovflow);
	HANDLE_TRAP0 (5,  intr_bounds);
	HANDLE_TRAP0 (6,  intr_invop);
	HANDLE_TRAP0 (7,  intr_no387);
	HANDLE_TRAP  (8,  intr_dblfault);
	HANDLE_TRAP  (9,  intr_seg387);
	HANDLE_TRAP  (10, intr_invtss);
	HANDLE_TRAP  (11, intr_nosegm);
	HANDLE_TRAP  (12, intr_nostack);
	HANDLE_TRAP  (13, intr_prot);
	HANDLE_TRAP  (14, intr_pagefault);
	HANDLE_TRAP0 (15, intr_stray);
	HANDLE_TRAP0 (16, intr_fpe);
	HANDLE_TRAP  (17, intr_align);
}

void
__gccmain (void)
{
	/* Implicitly called just after entering main(). */
}

/*
 * Call global C++ constructors.
 */
void uos_call_global_initializers (void)
{
	typedef void (*funcptr_t) ();
	extern funcptr_t __CTOR_LIST__[];
	extern funcptr_t __CTOR_END__[];
	funcptr_t *func;

	for (func = __CTOR_END__-1; func >= __CTOR_LIST__; --func)
		(*func) ();
}

/*
 * Call global C++ destructors.
 */
void uos_call_global_destructors (void)
{
	typedef void (*funcptr_t) ();
	extern funcptr_t __DTOR_LIST__[];
	extern funcptr_t __DTOR_END__[];
	funcptr_t *func;

	for (func = __DTOR_LIST__; func < __DTOR_END__; ++func)
		(*func) ();
}
