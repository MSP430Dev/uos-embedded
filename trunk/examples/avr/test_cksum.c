/*
 * Testing UART.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "crc/crc16-inet.h"

ARRAY (task, 200);
uart_t uart;

void test (unsigned const char *buf, unsigned char len)
{
	unsigned char i;
	unsigned short sum;

	for (i=0; i<len; ++i)
		debug_printf ("%02x ", buf[i]);
	sum = crc16_inet (CRC16_INET_INIT, buf, len);
	debug_printf ("- %04x", sum);
	if (sum == CRC16_INET_GOOD)
		debug_printf (" - GOOD");
	debug_printf ("\n");
}

void test_header (unsigned char *src, unsigned char *dest,
	unsigned char proto, unsigned short proto_len)
{
	unsigned short sum;

	debug_printf ("%02x-%02x-%02x-%02x ", src[0], src[1], src[2], src[3]);
	debug_printf ("%02x-%02x-%02x-%02x ", dest[0], dest[1], dest[2], dest[3]);
	debug_printf ("%02x ", proto);
	debug_printf ("%04x ", proto_len);

	sum = crc16_inet_header (src, dest, proto, proto_len);

	debug_printf ("- %04x\n", sum);
}


void hello (void *data)
{
	unsigned char array [] = "\x55\xaa\xf0\x0f\x00\x00";
	unsigned short sum;
again:
	test ("\x00\x00", 2);
	test ("\x55\x00", 2);
	test ("\x55\x00\x00", 3);
	test ("\xaa\x55\x00\x00", 4);
	test ("\x55\xaa\x00\x00", 4);

	array[4] = 0;
	array[5] = 0;
	test (array, 6);
	sum = ~crc16_inet (CRC16_INET_INIT, array, 4);
	array[4] = (unsigned char) sum;
	array[5] = (unsigned char) (sum >> 8);
	test (array, 6);

	test ("\x55\xaa\xf0\x0f\x81\x18\x42\x24\x00\x11\x33\x77", 12);
	test_header ("\x55\xaa\xf0\x0f", "\x81\x18\x42\x24", 0x11, 0x3377);

	getchar (&uart);
	goto again;
}

void uos_init (void)
{
/*outb (((int) (KHZ * 1000L / 9600) + 8) / 16 - 1, UBRR);*/
	uart_init (&uart, 0, 90, KHZ, 9600);
	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
