/*
 * Testing RAM on STK500 board.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "mem/mem.h"
#include "kernel/internal.h"
#include "watchdog/watchdog.h"

/*
 * Установлена микросхема 16kx8 - имеем 60 килобайт памяти.
 */
#define RAM_START	0x1000
#define RAM_END		0xffff	/* For STK300 use 0x8000 */

#define CTRL(c)  ((c) & 037)

ARRAY (stack_console, 0x100);	/* Задача: меню на консоли */
ARRAY (stack_poll, 0x100);	/* Задача: опрос по таймеру */
uart_t uart;
timer_t timer;
mem_pool_t pool;

void main_console (void *data);
void main_poll (void *data);
void menu (void);

void uos_init (void)
{
	extern char __bss_end;

/*outb (25, UBRR);*/
	uart_init (&uart, 0, 90, KHZ, 9600);
	timer_init (&timer, 100, KHZ, 10);

	/*
	 * Инициализируем все, что относится к процессору, без периферии.
	 */
	/* Разрешаем внешнюю память: порты A - адрес/данные, C - адрес. */
	setb (SRE, MCUCR);

	/* Порт B - светодиоды. */
	outb (0xff, DDRB);

	/* Инициализируем драйвер памяти.
	 * Сначала внешняя память (в порядке убывания адресов).
	 * Установлена микросхема 62256 - имеем 32 килобайта памяти. */
	mem_init (&pool, RAM_START, RAM_END);

	/* Память внутри процессора - 4000 байт.
	 * Оставляем 128 байт для idle task.
	 * У компилятора GCC адрес специальной переменной __bss_end отмечает
	 * конец программы, с этого места остальная память свободна. */
	mem_init (&pool, (unsigned short) &__bss_end, 0x1000 - 128);

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}

void main_console (void *data)
{
	printf (&uart, "\nSTK500 Starter Kit\n\n");

	/* Включаем сторожевой таймер на 2 секунды. */
	watchdog_enable (7);

	task_create (main_poll, 0, "poll", 5, stack_poll, sizeof (stack_poll));
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

	puts (&uart, "Command: ");
	cmd = getchar (&uart);
	putchar (&uart, '\r');
	if (cmd == CTRL('L'))
		cmd = CTRL('R');
	if (cmd == '\r')
		cmd = '\n';
	return cmd;
}

void print_tasks ()
{
	task_t *t;
	lock_t *m;

	t = task_active.head;
	if (t)
		puts (&uart, "\n\nTask\tAddress\tPrio\tSP\tSt Av\tMsg\n");
	else
		puts (&uart, "\n\nNo tasks.\n");
	for (; t; t=t->next) {
		printf (&uart, "%s\t%p\t", t->name, t);
		printf (&uart, t == task_current ? "*%d*" : "%d", t->prio);
		printf (&uart, "\t%p\t%n\t%p\n",
			t->stack_context, task_stack_avail (t),
			t->message);
		if (t->lock)
			printf (&uart, "\tLocked by %p\n", t->lock);
		if (t->wait)
			printf (&uart, "\tWaiting for %p\n", t->lock);
		if (t->slaves.head) {
			puts (&uart, "\tOwning:");
			for (m=t->slaves.head; m; m=m->next)
				printf (&uart, " %p", m);
			putchar (&uart, '\n');
		}
	}
	putchar (&uart, '\n');
}

void print_item (unsigned char num, const char *name)
{
	printf (&uart, "\n  %d. ", num);
	if (name)
		puts (&uart, name);
}

static inline void test_location (unsigned short addr, unsigned char val)
{
	volatile unsigned char *p = (unsigned char*) addr;
	unsigned char rval;

	*p = val;
	rval = *p;
	if (rval != val)
		printf (&uart, "\nAddress %#x written %#x read %#x\n",
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
			printf (&uart, "\nAddress %#x written %#x read %#x\n",
				p, val, rval);
			return;
		}
	}
}

void test_address (unsigned short addr)
{
	unsigned char i, n;

	puts (&uart, "\n\nTesting... ");
	for (i=0; ; ++i) {
		for (n=1; n; ++n) {
			test_location (addr, 0xAA);
			test_location (addr, 0x55);
		}
		putchar (&uart, "|/-\\" [i&3]);
		putchar (&uart, '\b');
		fflush (&uart);
		if (peekchar (&uart) >= 0) {
			getchar (&uart);
			break;
		}
	}
	puts (&uart, "done.\n\n");
}

void test_ram_old (void)
{
	unsigned short addr;
	unsigned char i;

	puts (&uart, "\n\nTesting: ");
	for (i=0; i<32; ++i) {
		for (addr=RAM_START; addr<RAM_END; ++addr) {
			test_location (addr, 0xAA);
			test_location (addr, 0x55);
		}
		test_range (RAM_START, RAM_END, 0xAA);
		test_range (RAM_START, RAM_END, 0x55);
		putchar (&uart, '.');
		fflush (&uart);
	}
	puts (&uart, " done.\n\n");
}

void test_ram (void)
{
	unsigned short addr;
	unsigned char i;

	puts (&uart, "\n\nTesting --------------------------------\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	fflush (&uart);
	for (addr=RAM_START; addr; addr+=2) {
		*(volatile unsigned short*) addr = addr;
	}
	for (i=0; i<32; ++i) {
		for (addr=RAM_START; addr; addr+=2) {
			if (*(volatile unsigned short*) addr != addr) {
				printf (&uart, "\nAddress %#x written %#x read %#x\n",
					addr, addr, *(volatile unsigned short*) addr);
			}
		}
		putchar (&uart, '.');
		fflush (&uart);
	}
	puts (&uart, " done.\n");
#if 0
	puts (&uart, "Testing --------------------------------\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	fflush (&uart);
	for (addr=1; addr; ++addr) {
		unsigned short a = addr << 8 | addr >> 8;
		if (a<RAM_START || a>RAM_END)
			continue;
		*(volatile unsigned char*) a = (unsigned char) a;
	}
	for (i=0; i<32; ++i) {
		for (addr=1; addr; ++addr) {
			unsigned short a = addr << 8 | addr >> 8;
			unsigned char val;
			if (a<RAM_START || a>RAM_END)
				continue;
			val = (unsigned char) a;
			if (*(volatile unsigned char*) a != val) {
				printf (&uart, "\nAddress %#x written %#x read %#x\n",
					a, val, *(volatile unsigned char*) a);
			}
		}
		putchar (&uart, '.');
		fflush (&uart);
	}
	puts (&uart, " done.\n\n");
#endif
}

void menu ()
{
	printf (&uart, "Free memory: %n bytes\n",
		mem_available (&pool));

	print_item (1, "Tasks");
	print_item (2, "Test All RAM");
	print_item (3, "Test Address 0xAAAA");
	print_item (0, "Reset\n\n");

	for (;;) {
		char cmd = get_cmd ();
		if (cmd == CTRL('R') || cmd == '\n')
			break;
		if (cmd == '1') {
			print_tasks ();
			break;
		}
		if (cmd == '2') {
			test_ram ();
			break;
		}
		if (cmd == '3') {
			test_address (0xAAAA);
			break;
		}
		if (cmd == '0') {
			printf (&uart, "Reset...\n");
			fflush (&uart);
			reset ();
		}
	}
}

/*
 * Задача периодического опроса.
 */
void main_poll (void *data)
{
	for (;;) {
		/* Опрос через 30 мсек. */
		timer_delay (&timer, 30);
		watchdog_alive ();

/*		lock_take (&periph);*/

		/* TODO */

/*		lock_release (&periph);*/
	}
}
