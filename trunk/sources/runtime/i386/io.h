/* Conventional GDT indexes. */
#define I386_CS		0x08		/* system code */
#define I386_DS		0x10		/* system data/stack */
#define I386_RM_CS	0x18		/* real mode code */
#define I386_RM_DS	0x20		/* real mode data/stack */

/* Trap types. */
#define I386_TRAP_DIVZERO	0	/* fault */
#define I386_TRAP_DEBUG		1	/* trap/fault */
#define I386_TRAP_NMI		2	/* nonmaskable interrupt */
#define I386_TRAP_BREAK		3	/* trap */
#define I386_TRAP_OVFLOW	4	/* trap */
#define I386_TRAP_BOUNDS	5	/* fault */
#define I386_TRAP_INVOP		6	/* fault */
#define I386_TRAP_NO387		7	/* fault */
#define I386_TRAP_DBLFAULT	8	/* abort, error code=0 */
#define I386_TRAP_SEG387	9	/* fault, error code? */
#define I386_TRAP_INVTSS	10	/* fault, error code */
#define I386_TRAP_NOSEGM	11	/* fault, error code */
#define I386_TRAP_NOSTACK	12	/* fault, error code */
#define I386_TRAP_PROT		13	/* trap/fault, error code */
#define I386_TRAP_PAGEFAULT	14	/* fault, spec error code */
#define I386_TRAP_STRAY		15	/* unknown interrupt */
#define I386_TRAP_FPE		16	/* fault */
#define I386_TRAP_ALIGN		17	/* fault, error code=0 */

/*
 * Page descriptor bits.
 */
#define PG_SHIFT        12              /* phys page frame */
#define PG_DIRTY        0x40            /* data written */
#define PG_ACCESSED     0x20            /* data read/written */
#define PG_NOCACHE      0x10            /* don't cache this page */
#define PG_WTHRU        0x08            /* write through */
#define PG_USER         0x04            /* enable access from cpl=3 */
#define PG_WRITABLE     0x02            /* enable data write */
#define PG_AVAIL        0x01            /* page is available */

/* Segment descriptor fields. */
#define SEG_DATA_RO     0x90            /* data segment, read only */
#define SEG_DATA_RW     0x92            /* data segment, read/write */
#define SEG_STACK_RW    0x96            /* stack segment, read/write */
#define SEG_CODE_XO     0x98            /* code segment, exec only */
#define SEG_CODE_RX     0x9a            /* code segment, read/exec */
#define SEG_CODE_RXC    0x9e            /* code segment, read/exec/call */
#define SEG_CALL        0x8c            /* call gate */
#define SEG_INTR        0x8e            /* interrupt gate */
#define SEG_TRAP        0x8f            /* trap gate */
#define SEG_TSS         0x89            /* TSS segment */

#define SEG_DPL1        0x20            /* descriptor protection level 1 */
#define SEG_DPL2        0x40            /* descriptor protection level 2 */
#define SEG_DPL3        0x60            /* descriptor protection level 3 */

#define SEG_4K          0x80            /* G-bit - 4k granularity */
#define SEG_32          0x40            /* D-bit - use 32-bit operands */

#define TSSSZ           0x68            /* task segment size in bytes */

#ifndef __ASSEMBLER__

typedef struct {
	unsigned short	limit16;
	unsigned short	base16;
	unsigned char	base24;
	unsigned char	type;
	unsigned char	gran;
	unsigned char	base32;
} i386_gdt_t;

typedef struct {
	unsigned short	offset16;
	unsigned short	segm;
	unsigned char	reserved;
	unsigned char	type;
	unsigned short	offset32;
} i386_idt_t;

typedef struct {
	unsigned short	len;
	unsigned short	addrlow;
	unsigned short	addrhigh;
} i386_dtr_t;

typedef struct {
	unsigned long	link;
	unsigned long	esp0;
	unsigned long	ss0;
	unsigned long	esp1;
	unsigned long	ss1;
	unsigned long	esp2;
	unsigned long	ss2;
	unsigned long	cr3;
	unsigned long	eip;
	unsigned long	eflags;
	unsigned long	eax;
	unsigned long	ecx;
	unsigned long	edx;
	unsigned long	ebx;
	unsigned long	esp;
	unsigned long	ebp;
	unsigned long	esi;
	unsigned long	edi;
	unsigned long	es;
	unsigned long	cs;
	unsigned long	ss;
	unsigned long	ds;
	unsigned long	fs;
	unsigned long	gs;
	unsigned long	ldtr;
	unsigned short	traceflag;
	unsigned short	iobase;
} i386_tss_t;

/*
 * Exception/trap/interrupt stack frame.
 */
typedef struct trapframe {
	unsigned long     es;		/* SEG_SAVE */
	unsigned long     ds;

	unsigned long     edi;		/* pushal */
	unsigned long     esi;
	unsigned long     ebp;
	unsigned long     esp;
	unsigned long     ebx;
	unsigned long     edx;
	unsigned long     ecx;
	unsigned long     eax;

	unsigned long     err;		/* saved by 386 hardware */
	unsigned long     eip;
	unsigned long     cs;
	unsigned long     eflags;
} trapframe_t;

/*
 * Set value of stack pointer register.
 */
static void inline __attribute__ ((always_inline))
i386_set_stack_pointer (void *x)
{
	asm volatile (
	"movl	%0, %%esp"
	: : "r" (x));
}

/*
 * Get value of stack pointer register.
 */
static inline __attribute__ ((always_inline))
void *i386_get_stack_pointer ()
{
        void *x;

	asm volatile (
	"movl	%%esp, %0"
	: "=r" (x));
        return x;
}

/*
 * Get value of stack segment register.
 */
static inline __attribute__ ((always_inline))
unsigned int i386_get_stack_segment ()
{
	int x;

	asm volatile (
	"movl	%%ss, %0"
	: "=r" (x));
        return x;
}

/*
 * Disable and restore the hardware interrupts,
 * saving the interrupt enable flag into the supplied variable.
 */
static void inline __attribute__ ((always_inline))
i386_intr_disable (int *x)
{
	asm volatile (
	"pushf \n"
"	pop %0 \n"
"	cli"
	: "=r" (*(x)) : : "memory");
}

/*
 * Restore the hardware interrupt mode using the saved interrupt state.
 */
static void inline __attribute__ ((always_inline))
i386_intr_restore (int x)
{
	asm volatile (
	"push %0 \n"
"	popf"
	: : "r" (x) : "memory");
}

/*
 * Enable hardware interrupts.
 */
static void inline __attribute__ ((always_inline))
i386_intr_enable ()
{
	asm volatile (
	"sti"
	: : : "memory");
}

static __inline int
ffs (int mask)
{
	int	result;
	/*
	 * bsfl turns out to be not all that slow on 486's.  It can beaten
	 * using a binary search to reduce to 4 bits and then a table lookup,
	 * but only if the code is inlined and in the cache, and the code
	 * is quite large so inlining it probably busts the cache.
	 *
	 * Note that gcc-2's builtin ffs would be used if we didn't declare
	 * this inline or turn off the builtin.  The builtin is faster but
	 * broken in gcc-2.4.5 and slower but working in gcc-2.5 and 2.6.
	 */
	__asm __volatile("testl %0,%0; je 1f; bsfl %0,%0; incl %0; 1:"
			 : "=r" (result) : "0" (mask));
	return (result);
}

/*
 * The following complications are to get around gcc not having a
 * constraint letter for the range 0..255.  We still put "d" in the
 * constraint because "i" isn't a valid constraint when the port
 * isn't constant.  This only matters for -O0 because otherwise
 * the non-working version gets optimized away.
 *
 * Use an expression-statement instead of a conditional expression
 * because gcc-2.6.0 would promote the operands of the conditional
 * and produce poor code for "if ((inb(var) & const1) == const2)".
 */
#define	inb(port)	({						\
	unsigned char	_data;							\
	if (__builtin_constant_p((int) (port)) && (port) < 256ul)	\
		_data = inbc(port);					\
	else								\
		_data = inbv(port);					\
	_data; })

#define	outb(data, port) \
	(__builtin_constant_p((int) (port)) && (port) < 256ul \
	 ? outbc(data, port) : outbv(data, port))

static __inline unsigned char
inbc(unsigned int port)
{
	unsigned char	data;

	__asm __volatile("inb %1,%0" : "=a" (data) : "id" ((unsigned short)(port)));
	return (data);
}

static __inline void
outbc(unsigned char data, unsigned int port)
{
	__asm __volatile("outb %0,%1" : : "a" (data), "id" ((unsigned short)(port)));
}

static __inline unsigned char
inbv(unsigned int port)
{
	unsigned char	data;
	/*
	 * We use %%dx and not %1 here because i/o is done at %dx and not at
	 * %edx, while gcc generates inferior code (movw instead of movl)
	 * if we tell it to load (unsigned short) port.
	 */
	__asm __volatile("inb %%dx,%0" : "=a" (data) : "d" (port));
	return (data);
}

static __inline unsigned long
inl(unsigned int port)
{
	unsigned long	data;

	__asm __volatile("inl %%dx,%0" : "=a" (data) : "d" (port));
	return (data);
}

static __inline void
insb(unsigned int port, void *addr, unsigned long cnt)
{
	__asm __volatile("cld; rep; insb"
			 : : "d" (port), "D" (addr), "c" (cnt)
			 : "di", "cx", "memory");
}

static __inline void
insw(unsigned int port, void *addr, unsigned long cnt)
{
	__asm __volatile("cld; rep; insw"
			 : : "d" (port), "D" (addr), "c" (cnt)
			 : "di", "cx", "memory");
}

static __inline void
insl(unsigned int port, void *addr, unsigned long cnt)
{
	__asm __volatile("cld; rep; insl"
			 : : "d" (port), "D" (addr), "c" (cnt)
			 : "di", "cx", "memory");
}

static __inline unsigned short
inw(unsigned int port)
{
	unsigned short	data;

	__asm __volatile("inw %%dx,%0" : "=a" (data) : "d" (port));
	return (data);
}

static __inline unsigned
loadandclear(unsigned int *addr)
{
	unsigned int	result;

	__asm __volatile("xorl %0,%0; xchgl %1,%0"
			 : "=&r" (result) : "m" (*addr));
	return (result);
}

static __inline void
outbv(unsigned char data, unsigned int port)
{
	unsigned char	al;
	/*
	 * Use an unnecessary assignment to help gcc's register allocator.
	 * This make a large difference for gcc-1.40 and a tiny difference
	 * for gcc-2.6.0.  For gcc-1.40, al had to be ``asm("ax")'' for
	 * best results.  gcc-2.6.0 can't handle this.
	 */
	al = data;
	__asm __volatile("outb %0,%%dx" : : "a" (al), "d" (port));
}

static __inline void
outl(unsigned long data, unsigned int port)
{
	/*
	 * outl() and outw() aren't used much so we haven't looked at
	 * possible micro-optimizations such as the unnecessary
	 * assignment for them.
	 */
	__asm __volatile("outl %0,%%dx" : : "a" (data), "d" (port));
}

static __inline void
outsb(void *addr, unsigned long cnt, unsigned int port)
{
	__asm __volatile("cld; rep; outsb"
			 : : "d" (port), "S" (addr), "c" (cnt)
			 : "si", "cx");
}

static __inline void
outsw(void *addr, unsigned long cnt, unsigned int port)
{
	__asm __volatile("cld; rep; outsw"
			 : : "d" (port), "S" (addr), "c" (cnt)
			 : "si", "cx");
}

static __inline void
outsl(void *addr, unsigned long cnt, unsigned int port)
{
	__asm __volatile("cld; rep; outsl"
			 : : "d" (port), "S" (addr), "c" (cnt)
			 : "si", "cx");
}

static __inline void
outw(unsigned short data, unsigned int port)
{
	__asm __volatile("outw %0,%%dx" : : "a" (data), "d" (port));
}

static __inline void
pmap_update(void)
{
	unsigned long	temp;
	/*
	 * This should be implemented as load_cr3(rcr3()) when load_cr3()
	 * is inlined.
	 */
	__asm __volatile("movl %%cr3, %0; movl %0, %%cr3" : "=r" (temp)
			 : : "memory");
}

static __inline unsigned long
rcr0(void)
{
	unsigned long	data;

	__asm __volatile("movl %%cr0,%0" : "=r" (data));
	return (data);
}

static __inline unsigned long
rcr2(void)
{
	unsigned long	data;

	__asm __volatile("movl %%cr2,%0" : "=r" (data));
	return (data);
}

static __inline unsigned long
rcr3(void)
{
	unsigned long	data;

	__asm __volatile("movl %%cr3,%0" : "=r" (data));
	return (data);
}

static __inline unsigned long
read_eflags(void)
{
	unsigned long	ef;

	__asm __volatile("pushfl; popl %0" : "=r" (ef));
	return (ef);
}

static __inline void
write_eflags(unsigned long ef)
{
	__asm __volatile("pushl %0; popfl" : : "r" (ef));
}

static __inline void
ltr(unsigned short sel)
{
	__asm __volatile("ltr %%ax"::"a" (sel));
}

static __inline void
lidt(i386_dtr_t *idtr)
{
	__asm __volatile("lidt (%0)"::"r" (idtr));
}

static __inline void
sidt(i386_dtr_t *idtr)
{
	__asm __volatile("sidt (%0)"::"r" (idtr):"memory");
}

static __inline void
hlt()
{
	__asm __volatile("hlt");
}

#endif /* __ASSEMBLER__ */
