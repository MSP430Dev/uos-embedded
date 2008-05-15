#include <runtime/lib.h>

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void
debug_putchar (void *arg, short c)
{
again:
	/* Send byte. */
#asm
	mov	ax,[bp+6]
	mov	ah,#$0E
	mov	bx,#7
	int	$10
#endasm
	if (c == '\n') {
		c = '\r';
		goto again;
	}
}

/*
 * Wait for the byte to be received and return it.
 */
unsigned short
debug_getchar (void)
{
	unsigned char c;
#asm
	xor	ax,ax
	int	$16
#endasm
	c = _AX;
	if (c == 3)
		abort ();
}

/*
 * Get the received byte without waiting.
 */
int
debug_peekchar (void)
{
#asm
	mov	ah,#1
	int	$16
	jz	nokey
	cmp	ax,#$ffff
	jnz	dort
	mov	ax,#3
dort:
	ret
nokey:
	mov	ax,#$ffff
#endasm
}

void
debug_puts (char *p)
{
	char c;

	for (;;) {
		c = *p;
		if (! c)
			return;
		debug_putchar (0, c);
		++p;
	}
}
