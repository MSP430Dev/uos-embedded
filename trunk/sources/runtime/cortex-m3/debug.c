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

#ifdef ARM_1986BE9
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
	while (ARM_UART2->FR & ARM_UART_FR_TXFF)
		continue;
again:
	/* Send byte. */
	/* TODO: unicode to utf8 conversion. */
	ARM_UART2->DR = c;

	/* Wait for transmitter holding register empty. */
	while (ARM_UART2->FR & ARM_UART_FR_TXFF)
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
	unsigned c;
	int x;

	if (debug_char >= 0) {
		c = debug_char;
		debug_char = -1;
/*debug_printf ("getchar -> 0x%02x\n", c);*/
		return c;
	}
	arm_intr_disable (&x);

	/* Enable receiver. */
	ARM_UART2->CTL |= ARM_UART_CTL_RXE;

	/* Wait until receive data available. */
	while (ARM_UART2->FR & ARM_UART_FR_RXFE) {
		watchdog_alive ();
		arm_intr_restore (x);
		arm_intr_disable (&x);
	}
	/* TODO: utf8 to unicode conversion. */
	c = ARM_UART2->DR & ARM_UART_DR_DATA;

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
	ARM_UART2->CTL |= ARM_UART_CTL_RXE;

	/* Wait until receive data available. */
	if (ARM_UART2->FR & ARM_UART_FR_RXFE) {
		arm_intr_restore (x);
		return -1;
	}
	/* TODO: utf8 to unicode conversion. */
	c = ARM_UART2->DR & ARM_UART_DR_DATA;

	arm_intr_restore (x);
	debug_char = c;
	return c;
}
#endif /* ARM_1986BE9 */

void
debug_puts (const char *p)
{
	int x;

	arm_intr_disable (&x);
	for (; *p; ++p)
		debug_putchar (0, *p);
	arm_intr_restore (x);
}
