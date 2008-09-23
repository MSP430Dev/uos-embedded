/*
 * Testing NVRAM.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "nvram/nvram.h"

char task [200];
uart_t uart;
nvram_t nvram;

void test (void *data)
{
	int i;
	unsigned char c;

	printf (&uart, "Test of NVRAM, total %d bytes", E2END+1);
	for (;;) {
		puts (&uart, "\nWriting: ");
		for (i=0; i<=E2END; ++i) {
			nvram_write (&nvram, i, ~i);
			if ((i & 63) == 63)
				putchar (&uart, '.');
		}
		puts (&uart, "\nReading: ");
		for (i=0; i<=E2END; ++i) {
			c = nvram_read (&nvram, i);
			if (c != (unsigned char) ~i) {
				printf (&uart, "error on byte 0x%x: written 0x%x, read 0x%x\n",
					i, (unsigned char) ~i, c);
				continue;
			}
			if ((i & 63) == 63)
				putchar (&uart, '.');
		}
	}
}

void uos_init (void)
{
	uart_init (&uart, 0, 90, KHZ, 9600);
	nvram_init (&nvram);
	task_create (test, 0, "test", 1, task, sizeof (task));
}
