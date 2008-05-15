#include <runtime/lib.h>
#include <runtime/i386/int86.h>
#include <runtime/i386/i8259.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

/*
 * Small subroutine, to be placed at address 500 (see call86.S).
 */
static unsigned char call86 [] = {
#include <runtime/i386/call86.h>
};

static i386_dtr_t idtr16;

static int int86_initialized;

/*
 * Interrupt mask for BIOS calls.
 */
unsigned short int86_irqmask;

static void int86_init ()
{
	/* Prepare data for int86() call.  */
	memcpy (INT86_CALL, call86, sizeof (call86));

	/* Set BIOS interrupt mask, for int86() calls. */
	int86_irqmask = 0xffff;

	/* Set up the interrupt descriptor table for 16-bit mode. */
	idtr16.len = 4*256 - 1;
	idtr16.addrlow = 0;
	idtr16.addrhigh = 0;

	int86_initialized = 1;
}

/*
 * Make a 16-bit BIOS call. Put input registers to inregs,
 * and take output registers and flags from outregs.
 * Both inregs and outregs are optional.
 */
void
int86 (int intnum, int86_regs_t *inregs, int86_regs_t *outregs)
{
	int x, irqmask;
	i386_dtr_t idtr_saved;

	if (! int86_initialized)
		int86_init ();

	MACHDEP_INTR_DISABLE (&x);

	/* Put parameters for INT86_CALL(). */
	if (inregs)
		INT86_REGS = *inregs;
	else
		memset (&INT86_REGS, 0, sizeof (INT86_REGS));

	/* Opcode: int $N */
	INT86_CODE[0] = 0x909000cd | (intnum & 0xff) << 8;
	INT86_CODE[1] = 0x90909090;

	/* Save current interrupt mask. */
	irqmask = inb (PIC1_MASK);
	irqmask |= inb (PIC2_MASK) << 8;

	/* Restore BIOS interrupt mask. */
	outb (int86_irqmask & 0xff, PIC1_MASK);
	outb (int86_irqmask >> 8, PIC2_MASK);

	/* Set up the IDT for real mode. */
	sidt (&idtr_saved);
	lidt (&idtr16);

	INT86_CALL ();

	/* Restore the IDT for protected mode. */
	lidt (&idtr_saved);

	/* Restore saved interrupt mask. */
	outb (irqmask & 0xff, PIC1_MASK);
	outb (irqmask >> 8, PIC2_MASK);

	/* Get parameters from INT86_CALL(). */
	if (outregs)
		*outregs = INT86_REGS;

	MACHDEP_INTR_RESTORE (x);
}

/*
 * Reboot the system via jump to BIOS.
 * Code must be: 0x1234 - for warm boot, or 0 for cold boot.
 */
void
i386_reboot (int code)
{
	int x;

	if (! int86_initialized)
		int86_init ();

	MACHDEP_INTR_DISABLE (&x);

	/* Opcode: ljmp $ffff, $0000 */
	INT86_CODE[0] = 0xff0000ea;
	INT86_CODE[1] = 0x909090ff;

	/* Restore BIOS interrupt mask. */
	outb (int86_irqmask & 0xff, PIC1_MASK);
	outb (int86_irqmask >> 8, PIC2_MASK);

	/* Write zero to CMOS register number 0x0f, which the BIOS POST
	 * routine will recognize as telling it to do a proper reboot.
	 * At the same time, disable NMIs by setting the top bit in
	 * the CMOS address register, as we're about to do peculiar
	 * things to the CPU. */
	outb (0x8f, 0x70);
	outb (0, 0x71);

        /* Write 0x1234 to absolute memory location 0x472. The BIOS reads this
	 * on booting to tell it to "Bypass memory test (also warm boot)". */
	*((short*) 0x472) = code;

	/* Set up the IDT for real mode. */
	lidt (&idtr16);

	INT86_CALL ();

	/* No return! */
}
