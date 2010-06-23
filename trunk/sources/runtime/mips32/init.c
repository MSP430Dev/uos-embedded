#include <runtime/lib.h>

extern void _etext();
extern unsigned __data_start, _edata, _end;
extern int main ();

/*
 * Initialize the system configuration, cache, intermal SRAM,
 * and set up the stack. Then call main().
 * _init_ is called from startup.S.
 * Attribute "naked" skips function prologue.
 */
void __attribute ((noreturn))_init_ (void)
{
	unsigned *src, *dest, *limit;

#ifdef ELVEES_MC24
	unsigned int divisor;

	/* Clear CAUSE register. Use special irq vector. */
	mips32_write_c0_register (C0_CAUSE, CA_IV);

	/* Initialize STATUS register: CP0 usable, ROM vectors used,
	 * internal interrupts enabled, master interrupt disable. */
	mips32_write_c0_register (C0_STATUS, ST_BEV | ST_IM_MCU | ST_CU0
#ifdef ARCH_HAVE_FPU
		 | ST_CU1
#endif
		);

#ifdef ENABLE_ICACHE
	/* Enable cache for kseg0 segment. */
	mips32_write_c0_register (C0_CONFIG, 3);
	MC_CSR |= MC_CSR_FLUSH_I | MC_CSR_FLUSH_D;
	asm volatile (
		"la	$k0, 1f \n"
		"jr	$k0 \n"
	"1:");
#else
	/* Disable cache for kseg0 segment. */
	mips32_write_c0_register (C0_CONFIG, 2);
#endif

#ifdef ARCH_HAVE_FPU
	/* Clear all FPU registers. */
	mips32_write_fpu_control (C1_FCSR, 0);
	mips32_write_fpu_register (0, 0);
	mips32_write_fpu_register (1, 0);
	mips32_write_fpu_register (2, 0);
	mips32_write_fpu_register (3, 0);
	mips32_write_fpu_register (4, 0);
	mips32_write_fpu_register (5, 0);
	mips32_write_fpu_register (6, 0);
	mips32_write_fpu_register (7, 0);
	mips32_write_fpu_register (8, 0);
	mips32_write_fpu_register (9, 0);
	mips32_write_fpu_register (10, 0);
	mips32_write_fpu_register (11, 0);
	mips32_write_fpu_register (12, 0);
	mips32_write_fpu_register (13, 0);
	mips32_write_fpu_register (14, 0);
	mips32_write_fpu_register (15, 0);
	mips32_write_fpu_register (16, 0);
	mips32_write_fpu_register (17, 0);
	mips32_write_fpu_register (18, 0);
	mips32_write_fpu_register (19, 0);
	mips32_write_fpu_register (20, 0);
	mips32_write_fpu_register (21, 0);
	mips32_write_fpu_register (22, 0);
	mips32_write_fpu_register (23, 0);
	mips32_write_fpu_register (24, 0);
	mips32_write_fpu_register (25, 0);
	mips32_write_fpu_register (26, 0);
	mips32_write_fpu_register (27, 0);
	mips32_write_fpu_register (28, 0);
	mips32_write_fpu_register (29, 0);
	mips32_write_fpu_register (30, 0);
	mips32_write_fpu_register (31, 0);
#endif

	/*
	 * Setup all essential system registers.
	 */
	/* Fixed mapping, clock multiply from CLKIN to KHZ. */
	MC_CSR = MC_CSR_FM | MC_CSR_CLK(KHZ/ELVEES_CLKIN) | MC_CSR_CLKEN;
	MC_MASKR = 0;
	MC_ITCSR = 0;
	MC_RTCSR = 0;
	MC_WTCSR = 0;
	MC_SWIC_RX_DESC_CSR(0) = 0;
	MC_SWIC_RX_DESC_CSR(1) = 0;
	MC_SWIC_RX_DATA_CSR(0) = 0;
	MC_SWIC_RX_DATA_CSR(1) = 0;
	MC_SWIC_TX_DESC_CSR(0) = 0;
	MC_SWIC_TX_DESC_CSR(1) = 0;
	MC_SWIC_TX_DATA_CSR(0) = 0;
	MC_SWIC_TX_DATA_CSR(1) = 0;
	MC_CSR_LPCH(0) = 0;
	MC_CSR_LPCH(1) = 0;
	MC_CSR_LPCH(2) = 0;
	MC_CSR_LPCH(3) = 0;
	MC_CSR_MEMCH(0) = 0;
	MC_CSR_MEMCH(1) = 0;
	MC_CSR_MEMCH(2) = 0;
	MC_CSR_MEMCH(3) = 0;
	MC_STCTL(0) = 0;
	MC_STCTL(1) = 0;
	MC_SRCTL(0) = 0;
	MC_SRCTL(1) = 0;
	MC_LCSR(0) = 0;
	MC_LCSR(1) = 0;
	MC_LCSR(2) = 0;
	MC_LCSR(3) = 0;
	MC_LDIR(0) = 0;
	MC_LDIR(1) = 0;
	MC_LDIR(2) = 0;
	MC_LDIR(3) = 0;

	/* Disable all external memory except nCS3.
	 * Set to default values. */
	MC_CSCON0 = MC_CSCON_WS (15);
	MC_CSCON1 = MC_CSCON_WS (15);
	MC_CSCON2 = MC_CSCON_WS (15);
#ifdef BOOT_FLASH_8BIT
	MC_CSCON3 = MC_CSCON_WS (15) | MC_CSCON3_BYTE;
#else
	MC_CSCON3 = MC_CSCON_WS (15);
#endif
	MC_CSCON4 = MC_CSCON_WS (15);
	MC_SDRCON = 0;

	/*
	 * Setup UART registers.
	 * Compute the divisor for 115.2 kbaud.
	 * Assume we have 80 MHz cpu clock.
	 */
	divisor = MC_DL_BAUD (ELVEES_CLKIN /*KHZ*/ * 1000, 115200);

	MC_LCR = MC_LCR_8BITS | MC_LCR_DLAB;
	MC_DLM = divisor >> 8;
	MC_DLL = divisor;
	MC_LCR = MC_LCR_8BITS;
	MC_SCLR = 0;
	MC_SPR = 0;
	MC_IER = 0;
	MC_MSR = 0;
	MC_MCR = MC_MCR_DTR | MC_MCR_RTS | MC_MCR_OUT2;
	MC_FCR = MC_FCR_RCV_RST | MC_FCR_XMT_RST | MC_FCR_ENABLE;

	/* Clear pending status, data and irq. */
	(void) MC_LSR;
	(void) MC_MSR;
	(void) MC_RBR;
	(void) MC_IIR;

#endif /* ELVEES_MC24 */

	/* Copy the .data image from flash to ram.
	 * Linker places it at the end of .text segment. */
	src = (unsigned*) &_etext;
	dest = &__data_start;
	limit = &_edata;
	while (dest < limit)
		*dest++ = *src++;

	/* Initialize .bss segment by zeroes. */
	dest = &_edata;
	limit = &_end;
	while (dest < limit)
		*dest++ = 0;

	for (;;)
		main ();
}

bool_t __attribute__((weak))
uos_valid_memory_address (void *ptr)
{
	unsigned address = (unsigned) ptr;

#ifdef ELVEES_MC24
	/* Internal SRAM. */
	if (address >= 0xb8000000 && address < 0xb8008000)
		return 1;

#ifdef BOOT_SRAM_SIZE
	/* Boot SRAM. */
	if (address >= 0xbfc00000 && address < 0xbfc00000+BOOT_SRAM_SIZE)
		return 1;
#endif /* BOOT_SRAM_SIZE */

#endif /* ELVEES_MC24 */
	return 0;
}

static void dump_of_death (unsigned int context[])
{
	debug_printf ("                t0 = %8x   s0 = %8x   t8 = %8x   lo = %8x\n",
		context [CONTEXT_R8], context [CONTEXT_R16],
		context [CONTEXT_R24], context [CONTEXT_LO]);
	debug_printf ("at = %8x   t1 = %8x   s1 = %8x   t9 = %8x   hi = %8x\n",
		context [CONTEXT_R1], context [CONTEXT_R9], context [CONTEXT_R17],
		context [CONTEXT_R25], context [CONTEXT_HI]);
	debug_printf ("v0 = %8x   t2 = %8x   s2 = %8x               status = %8x\n",
		context [CONTEXT_R2], context [CONTEXT_R10],
		context [CONTEXT_R18], context [CONTEXT_STATUS]);
	debug_printf ("v1 = %8x   t3 = %8x   s3 = %8x                  epc = %8x\n",
		context [CONTEXT_R3], context [CONTEXT_R11],
		context [CONTEXT_R19], context [CONTEXT_PC]);
	debug_printf ("a0 = %8x   t4 = %8x   s4 = %8x   gp = %8x\n",
		context [CONTEXT_R4], context [CONTEXT_R12],
		context [CONTEXT_R20], context [CONTEXT_GP]);
	debug_printf ("a1 = %8x   t5 = %8x   s5 = %8x   sp = %8x\n",
		context [CONTEXT_R5], context [CONTEXT_R13],
		context [CONTEXT_R21], context + CONTEXT_WORDS);
	debug_printf ("a2 = %8x   t6 = %8x   s6 = %8x   fp = %8x\n",
		context [CONTEXT_R6], context [CONTEXT_R14],
		context [CONTEXT_R22], context [CONTEXT_FP]);
	debug_printf ("a3 = %8x   t7 = %8x   s7 = %8x   ra = %8x\n",
		context [CONTEXT_R7], context [CONTEXT_R15],
		context [CONTEXT_R23], context [CONTEXT_RA]);

	debug_printf ("\nHalt...\n\n");
	asm volatile ("1: j 1b; nop");
}

void _exception_handler_ (unsigned int context[])
{
	unsigned int cause, badvaddr, config, prid;
	const char *code = 0;

	debug_printf ("\n\n*** 0x%08x: exception ", context [CONTEXT_PC]);

	cause = mips32_read_c0_register (C0_CAUSE);
	switch (cause >> 2 & 31) {
	case 0:	code = "Interrupt"; break;
	case 1: code = "TLB Modification"; break;
	case 2: code = "TLB Load"; break;
	case 3: code = "TLB Save"; break;
	case 4: code = "Address Load"; break;
	case 5: code = "Address Save"; break;
	case 8: code = "System"; break;
	case 9: code = "BBreakpoint"; break;
	case 10: code = "Reserved Instruction"; break;
	case 11: code = "Coprocessor Unavailable"; break;
	case 12: code = "Integer Overflow"; break;
	case 13: code = "Trap"; break;
	case 15: code = "FPU"; break;
	case 24: code = "MCheck"; break;
	}
	if (code)
		debug_printf ("'%s'\n", code);
	else
		debug_printf ("%d\n", cause >> 2 & 31);

	badvaddr = mips32_read_c0_register (C0_BADVADDR);
	config = mips32_read_c0_register (C0_CONFIG);
	prid = mips32_read_c0_register (C0_PRID);
	debug_printf ("*** cause=0x%08x, badvaddr=0x%08x, config=0x%08x, prid=0x%08x\n",
		cause, badvaddr, config, prid);

	dump_of_death (context);
}

void _pagefault_handler_ (unsigned int context[])
{
	unsigned int cause, badvaddr, config, prid;

	debug_printf ("\n\n*** 0x%08x: page fault\n", context [CONTEXT_PC]);

	cause = mips32_read_c0_register (C0_CAUSE);
	badvaddr = mips32_read_c0_register (C0_BADVADDR);
	config = mips32_read_c0_register (C0_CONFIG);
	prid = mips32_read_c0_register (C0_PRID);
	debug_printf ("*** cause=0x%08x, badvaddr=0x%08x, config=0x%08x, prid=0x%08x\n",
		cause, badvaddr, config, prid);

	dump_of_death (context);
}
