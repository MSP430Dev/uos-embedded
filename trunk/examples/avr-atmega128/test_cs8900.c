/*
 * Testing RAM on STK500 board.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "kernel/internal.h"
#include "mem/mem.h"
#include "buf/buf.h"
#include "cs8900/cs8900.h"

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0x8000	/* For STK300 use 0x8000 */

#define CTRL(c)  ((c) & 037)

ARRAY (stack_console, 0x100);	/* Задача: меню на консоли */
ARRAY (stack_poll, 0x100);	/* Задача: опрос по таймеру */
mem_pool_t pool;
cs8900_t eth;

void main_console (void *data);
void menu (void);

void uos_init (void)
{
	extern char __bss_end;

/* Baud 19200. */
outb (((int) (KHZ * 1000L / 19200) + 8) / 16 - 1, UBRR);

	/*
	 * Инициализируем все, что относится к процессору, без периферии.
	 */
	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);

	/* Инициализируем память в порядке убывания адресов. */
	mem_init (&pool, RAM_START, RAM_END);

	/* Оставляем 128 байт для idle task. */
	mem_init (&pool, (unsigned short) &__bss_end, 0x1000 - 128);

	/* Reset чипа CS8900A заведен на порт G3.
	 * Он в прямой логике, надо подать туда 0. */
	outb_far (0x08, DDRG);
	clearb_far (3, PORTG);

	/* Добавляем один wait state, т.к. иначе cs8900 не успевает. */
	setb (SRW, MCUCR);
	cs8900_init (&eth, "eth0", 80, &pool, 0);

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));

	/* Define an address of chip CS8900. */
	ASSIGN_VIRTUAL_ADDRESS (device_cs8900, 0x7f00);
}

void main_console (void *data)
{
	debug_printf ("\nSTK500 Starter Kit\n\n");
	for (;;)
		menu ();
}

/*
 * Программный сброс устройства.
 */
void reset ()
{
	cli ();
	for (;;)
		asm volatile ("sleep");
}

/*
 * Ввод команды.
 */
char get_cmd ()
{
	char cmd;

	debug_puts ("Command: ");
	cmd = debug_getchar ();
	debug_putchar (0, '\r');
	if (cmd == CTRL('L'))
		cmd = CTRL('R');
	if (cmd == '\r')
		cmd = '\n';
	return cmd;
}

void print_item (unsigned char num, const char *name)
{
	debug_printf ("\n  %d. ", num);
	if (name)
		debug_puts (name);
}

static inline void test_location (unsigned short addr, unsigned char val)
{
	volatile unsigned char *p = (unsigned char*) addr;
	unsigned char rval;

	*p = val;
	rval = *p;
	if (rval != val)
		debug_printf ("\nAddress %#x written %#x read %#x\n",
			addr, val, rval);
}

static inline void test_range (unsigned short addr0, unsigned short addr1,
	unsigned char val)
{
	volatile unsigned char *p;
	unsigned char rval;

	for (p=(unsigned char*)addr0; p<(unsigned char*)addr1; ++p)
		*p = val;
	for (p=(unsigned char*)addr0; p<(unsigned char*)addr1; ++p) {
		rval = *p;
		if (rval != val) {
			debug_printf ("\nAddress %#x written %#x read %#x\n",
				p, val, rval);
			return;
		}
	}
}

void test_address (unsigned short addr)
{
	unsigned char i, n;

	debug_puts ("\n\nTesting... ");
	for (i=0; ; ++i) {
		for (n=1; n; ++n) {
			test_location (addr, 0xAA);
			test_location (addr, 0x55);
		}
		debug_putchar (0, "|/-\\" [i&3]);
		debug_putchar (0, '\b');
		if (debug_peekchar () >= 0) {
			debug_getchar ();
			break;
		}
	}
	debug_puts ("done.\n\n");
}

void test_ram (void)
{
	unsigned short addr;
	unsigned char i;

	debug_puts ("\n\nTesting --------------------------------\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (addr=RAM_START; addr<RAM_END; addr+=2) {
		*(volatile unsigned short*) addr = addr;
	}
	for (i=0; i<32; ++i) {
		for (addr=RAM_START; addr<RAM_END; addr+=2) {
			if (*(volatile unsigned short*) addr != addr) {
				debug_printf ("\nAddress %#x written %#x read %#x\n",
					addr, addr, *(volatile unsigned short*) addr);
			}
		}
		debug_putchar (0, '.');
	}
	debug_puts (" done.\n\n");
}

void test_eth ()
{
	buf_t *p;

	debug_printf ("\n*** Testing Ethernet CS8900A ***\n");
	debug_printf ("(press any key to break)\n");
	for (;;) {
		if (debug_peekchar () >= 0)
			break;

		cs8900_poll (&eth);

/*		lock_wait (&eth.netif.lock);*/
		p = netif_input (&eth.netif);
		if (p) {
			buf_print_ethernet (p);
			buf_free (p);
		}
	}
}

void menu ()
{
	debug_printf ("Free memory: %n bytes\n",
		mem_available (&pool));

	print_item (1, "Test All RAM");
	print_item (2, "Test Address 0x5555");
	print_item (3, "Test Ethernet CS8900A");
	print_item (0, "Reset\n\n");

	for (;;) {
		char cmd = get_cmd ();
		if (cmd == CTRL('R') || cmd == '\n')
			break;
		if (cmd == '1') {
			test_ram ();
			break;
		}
		if (cmd == '2') {
			test_address (0x5555);
			break;
		}
		if (cmd == '3') {
			test_eth ();
			break;
		}
		if (cmd == '0') {
			debug_printf ("Reset...\n");
			reset ();
		}
	}
}
