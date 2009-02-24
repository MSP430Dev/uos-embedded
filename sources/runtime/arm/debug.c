#include <runtime/lib.h>

static int debug_char = -1;

#ifdef ARM_S3C4530
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	arm_intr_disable (&x);

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

	watchdog_alive ();
	if (c == '\n') {
		c = '\r';
		goto again;
	}

#if 0 /*ndef NDEBUG*/
	if (ARM_USTAT(0) & ARM_USTAT_RDV) {
		debug_char = ARM_URXBUF(0);
		if (debug_char == 3) {
			debug_char = -1;
			breakpoint ();
		}
	}
#endif
	arm_intr_restore (x);
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
	arm_intr_disable (&x);

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
			watchdog_alive ();
			arm_intr_restore (x);
			arm_intr_disable (&x);
			continue;
		}
		/* TODO: utf8 to unicode conversion. */
		c = ARM_URXBUF(0);
#if 0 /*ndef NDEBUG*/
		if (c == 3) {
			breakpoint ();
			continue;
		}
#endif
		break;
	}
	arm_intr_restore (x);
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

	arm_intr_disable (&x);

	/* Enable receiver. */
	ARM_UCON(0) = (ARM_UCON(0) & ~ARM_UCON_RMODE_MASK) | ARM_UCON_RMODE_IRQ;
	/* Wait until receive data available. */
	if (! (ARM_USTAT(0) & ARM_USTAT_RDV)) {
		if (ARM_USTAT(0) & (ARM_USTAT_FER | ARM_USTAT_PER |
		    ARM_USTAT_OER | ARM_USTAT_ROVFF)) {
			ARM_USTAT(0) = ARM_USTAT_FER | ARM_USTAT_PER |
			    ARM_USTAT_OER | ARM_USTAT_ROVFF;
		}
		arm_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = ARM_URXBUF(0);
#if 0 /*ndef NDEBUG*/
	if (c == 3) {
		breakpoint ();
		arm_intr_restore (x);
		return -1;
	}
#endif
	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_S3C4530 */

#ifdef ARM_AT91SAM
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	arm_intr_disable (&x);

	/* Enable transmitter. */
	/* TODO */

	/* Wait for transmitter holding register empty. */
	/* TODO */
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	/* TODO */

	/* Wait for transmitter holding register empty. */
	/* TODO */

	if (c == '\n') {
		c = '\r';
		goto again;
	}
	arm_intr_restore (x);
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
	arm_intr_disable (&x);

	/* Enable receiver. */
	/* TODO */
	for (;;) {
		/* Wait until receive data available. */
		/* TODO */

		/* Get byte. */
		/* TODO */
		c = '?';
		break;
	}
	arm_intr_restore (x);
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

	arm_intr_disable (&x);

	/* Enable receiver. */
	/* TODO */

	/* Wait until receive data available. */
	/* TODO */

	/* Get byte. */
	/* TODO: utf8 to unicode conversion. */
	/* TODO */
	c = 0;

	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_AT91SAM */

void
debug_puts (const char *p)
{
	int x;

	arm_intr_disable (&x);
	for (; *p; ++p)
		debug_putchar (0, *p);
	arm_intr_restore (x);
}