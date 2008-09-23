#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"

uart_t uart;
timer_t timer;

/*extern unsigned char crc8_slow (unsigned const char *buf, unsigned char len);*/

char stack_console [0x200];		/* Задача: меню на консоли */

static void main_console (void *data);

void uos_init (void)
{
/*outb (25, UBRR);*/
	uart_init (&uart, 90, KHZ, 9600);
	timer_init (&timer, 100, KHZ, 10);

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console), 0);
}

void test (unsigned const char *buf, unsigned char len)
{
	unsigned char i;

	for (i=0; i<len-1; ++i)
		printf (&uart, "%02x ", buf[i]);
	printf (&uart, "(%02x) - %02x\n",
		buf[len-1], crc8 (buf, len));
}

static void main_console (void *data)
{
	unsigned char array [] = "\x10\x20\x00";
	char c;

	for (;;) {
		test ("\x00", 1);
		test ("\x07", 1);
		test ("\x01", 2);
		test ("\x02", 2);
		test ("\x04", 2);
		test ("\x08", 2);
		test ("\x10", 2);
		test ("\x20", 2);
		test ("\x40", 2);
		test ("\x80", 2);

		test ("\x00\x01", 3);
		test ("\x00\x02", 3);
		test ("\x00\x04", 3);
		test ("\x00\x08", 3);
		test ("\x00\x10", 3);
		test ("\x00\x20", 3);
		test ("\x00\x40", 3);
		test ("\x00\x80", 3);

		test ("\x01\x00", 3);
		test ("\x02\x00", 3);
		test ("\x04\x00", 3);
		test ("\x08\x00", 3);
		test ("\x10\x00", 3);
		test ("\x20\x00", 3);
		test ("\x40\x00", 3);
		test ("\x80\x00", 3);

		test (array, 3);
		array[2] = crc8 (array, 3);
		test (array, 3);

		printf (&uart, "> ");
		c = getchar (&uart);
		putchar (&uart, '\r');
	}
}
