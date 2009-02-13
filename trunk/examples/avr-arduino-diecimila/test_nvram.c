/*
 * Testing NVRAM.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include <nvram/eeprom.h>

uart_t uart;
ARRAY (task, 280);

void test (void *data)
{
	int i;
	unsigned char c;

	for (;;) {
		printf (&uart, "\n\nTesting NVRAM, total %d bytes.", E2END+1);
		puts (&uart, "\nWriting: ");
		for (i=0; i<=E2END; ++i) {
			eeprom_write_byte (i, ~i);
			if ((i & 63) == 63)
				putchar (&uart, '.');
		}
		puts (&uart, "\nReading: ");
		for (i=0; i<=E2END; ++i) {
			c = eeprom_read_byte (i);
			if (c != (unsigned char) ~i) {
				printf (&uart, "error on byte 0x%x: written 0x%x, read 0x%x\n",
					i, (unsigned char) ~i, c);
				continue;
			}
			if ((i & 63) == 63)
				putchar (&uart, '.');
		}
		puts (&uart, "\nPress <Enter> to continue: ");
		getchar (&uart);
	}
}

void uos_init (void)
{
	uart_init (&uart, 0, 90, KHZ, 38400);
	eeprom_init ();
	task_create (test, 0, "test", 1, task, sizeof (task));
}
