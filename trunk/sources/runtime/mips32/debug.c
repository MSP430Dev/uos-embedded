#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

static int debug_char = -1;

#ifdef ELVEES_MC24
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	mips32_intr_disable (&x);

	/* Enable transmitter. */
	ARM_UCON(0) = (ARM_UCON(0) & ~ARM_UCON_TMODE_MASK) | ARM_UCON_TMODE_IRQ;

	/* Wait for transmitter holding register empty. */
	while (! (ARM_USTAT(0) & ARM_USTAT_TC))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	ARM_UTXBUF(0) = c;

	/* Wait for transmitter holding register empty. */
	while (! (ARM_USTAT(0) & ARM_USTAT_TC))
		continue;

/*	watchdog_alive ();*/
	if (c == '\n') {
		c = '\r';
		goto again;
	}
	mips32_intr_restore (x);
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
/*debug_printf ("getchar -> 0x%02x\n", c);*/
		return c;
	}
	mips32_intr_disable (&x);

	/* Enable receiver. */
	ARM_UCON(0) = (ARM_UCON(0) & ~ARM_UCON_RMODE_MASK) | ARM_UCON_RMODE_IRQ;
	for (;;) {
		/* Wait until receive data available. */
		while (! (ARM_USTAT(0) & ARM_USTAT_RDV)) {
			if (ARM_USTAT(0) & (ARM_USTAT_FER | ARM_USTAT_PER |
			    ARM_USTAT_OER | ARM_USTAT_ROVFF)) {
/*debug_printf ("ustat 0x%x\n", ARM_USTAT(0));*/
				ARM_USTAT(0) = ARM_USTAT_FER | ARM_USTAT_PER |
				    ARM_USTAT_OER | ARM_USTAT_ROVFF;
			}
/*			watchdog_alive ();*/
			mips32_intr_restore (x);
			mips32_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = ARM_URXBUF(0);
		break;
	}
	mips32_intr_restore (x);
	return c;
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
	unsigned char c;
	int x;

	if (debug_char >= 0)
		return debug_char;

	mips32_intr_disable (&x);

	/* Enable receiver. */
	ARM_UCON(0) = (ARM_UCON(0) & ~ARM_UCON_RMODE_MASK) | ARM_UCON_RMODE_IRQ;
	/* Wait until receive data available. */
	if (! (ARM_USTAT(0) & ARM_USTAT_RDV)) {
		if (ARM_USTAT(0) & (ARM_USTAT_FER | ARM_USTAT_PER |
		    ARM_USTAT_OER | ARM_USTAT_ROVFF)) {
			ARM_USTAT(0) = ARM_USTAT_FER | ARM_USTAT_PER |
			    ARM_USTAT_OER | ARM_USTAT_ROVFF;
		}
		mips32_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = ARM_URXBUF(0);
	mips32_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ELVEES_MC24 */

void
debug_puts (const char *p)
{
	int x;

	for (; *p; ++p)
		debug_putchar (0, *p);
}
