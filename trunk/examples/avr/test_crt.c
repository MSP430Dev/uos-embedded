/*
 * Testing runtime library.
 * Use buttons on port A and LEDs on port B.
 */
#include <runtime/lib.h>

void uos_init (void)
{
	unsigned char val, old_val;

	outb (((int) (KHZ * 1000L / 9600) + 8) / 16 - 1, UBRR);
        setb (TXEN, UCR);

	outb (0xff, DDRB);
	val = old_val = 0;
	for (;;) {
		val = inb (PINA);
		outb (val, PORTB);
		if (val != old_val) {
			outb ('z', UDR);
			old_val = val;
		}
	}
}
