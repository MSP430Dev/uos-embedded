#include <runtime/lib.h>

bool_t debug_onlcr = 1;

static int debug_char = -1;

static void (*hook) (void *arg, short c);
static void *hook_arg;

/*
 * Redirect debug output.
 */
void
debug_redirect (void (*func) (void*, short), void *arg)
{
	hook = func;
	hook_arg = arg;
}

#ifdef ARM_S3C4530
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	arm_intr_disable (&x);

	if (hook) {
		hook (hook_arg, c);
		arm_intr_restore (x);
		return;
	}

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
	if (debug_onlcr && c == '\n') {
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
	ARM_UCON(0) = (ARM_UCON(0) & ~ARM_UCON_RMODE_MASK) | ARM_UCON_RMODE_IRQ;

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
	}
	/* TODO: utf8 to unicode conversion. */
	c = ARM_URXBUF(0);

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

	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_S3C4530 */

#ifdef ARM_AT91SAM
#ifdef AT91BOOTSTRAP
#   define AT91C_CONSOLE_CSR AT91C_DBGU_CSR
#   define AT91C_CONSOLE_THR AT91C_DBGU_THR
#   define AT91C_CONSOLE_RHR AT91C_DBGU_RHR
#else
#   define AT91C_CONSOLE_CSR AT91C_US0_CSR
#   define AT91C_CONSOLE_THR AT91C_US0_THR
#   define AT91C_CONSOLE_RHR AT91C_US0_RHR
#endif

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	arm_intr_disable (&x);

	if (hook) {
		hook (hook_arg, c);
		arm_intr_restore (x);
		return;
	}

	/* Wait for transmitter holding register empty. */
	while (! (*AT91C_CONSOLE_CSR & AT91C_US_TXRDY))
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	*AT91C_CONSOLE_THR = (unsigned char) c;

	/* Wait for transmitter holding register empty. */
	while (! (*AT91C_CONSOLE_CSR & AT91C_US_TXRDY))
		continue;

	if (debug_onlcr && c == '\n') {
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

	/* Wait until receive data available. */
	while (! (*AT91C_CONSOLE_CSR & AT91C_US_RXRDY)) {
/*debug_printf ("<%x> ", *AT91C_CONSOLE_CSR);*/
		arm_intr_restore (x);
		arm_intr_disable (&x);
	}

	/* Get byte. */
	/* TODO: utf8 to unicode conversion. */
	c = *AT91C_CONSOLE_RHR;

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

	/* Wait until receive data available. */
	if (! (*AT91C_CONSOLE_CSR & AT91C_US_RXRDY)) {
		arm_intr_restore (x);
		return -1;
	}

	/* Get byte. */
	/* TODO: utf8 to unicode conversion. */
	c = *AT91C_CONSOLE_RHR;

	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_AT91SAM */


#ifdef ARM_PANDABOARD
/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
	int x;

	arm_intr_disable (&x);

	if (hook) {
		hook (hook_arg, c);
		arm_intr_restore (x);
		return;
	}

again:
	/* Wait for transmitter holding register empty. */
	while (UART_SSR(3) & UART_SSR_TX_FIFO_FULL);

	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	UART_THR(3) = (unsigned char) c;

	if (debug_onlcr && c == '\n') {
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
		return c;
	}
	arm_intr_disable (&x);

	/* Wait until receive data available. */
	while (! (UART_LSR(3) & UART_LSR_RX_FIFO_E)) {
		arm_intr_restore (x);
		arm_intr_disable (&x);
	}

	/* Get byte. */
	/* TODO: utf8 to unicode conversion. */
	c = UART_RHR(3);

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

	/* Wait until receive data available. */
	if (! (UART_LSR(3) & UART_LSR_RX_FIFO_E)) {
		arm_intr_restore (x);
		return -1;
	}

	/* Get byte. */
	/* TODO: utf8 to unicode conversion. */
	c = UART_RHR(3);

	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_OMAP44XX */

void
debug_puts (const char *p)
{
	int x;

	arm_intr_disable (&x);
	for (; *p; ++p)
		debug_putchar (0, *p);
	arm_intr_restore (x);
}
