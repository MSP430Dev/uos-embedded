/*
 * Testing RAM on STK500 board.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include <timer/timer.h>
#include <mem/mem.h>
#include <watchdog/watchdog.h>

/*
 * Installed SRAM 64k*8.
 */
#define RAM_START	0x1000
#define RAM_END		0xffff	/* For STK300 use 0x8000 */

#define CTRL(c)  ((c) & 037)

ARRAY (stack_console, 200);	/* Task: menu on console */
ARRAY (stack_poll, 200);	/* Task: polling wathdog */
uart_t uart;
timer_t timer;
mem_pool_t pool;

/*
 * Reset the CPU.
 */
void reset ()
{
	asm volatile ("cli");
	for (;;)
		asm volatile ("sleep");
}

/*
 * Enter command from console.
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
}

void menu ()
{
	printf (&uart, "Free memory: %n bytes\n",
		mem_available (&pool));

	print_item (1, "Test All RAM");
	print_item (2, "Test Address 0xAAAA");
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
 * Poll watchdog periodically.
 */
void main_poll (void *data)
{
	for (;;) {
		/* Опрос через 30 мсек. */
		timer_delay (&timer, 30);
		watchdog_alive ();
	}
}

void main_console (void *data)
{
	printf (&uart, "\nSTK500 Starter Kit\n\n");

	/* Turn on watchdog with 2 second timeout. */
	watchdog_enable (7);

	task_create (main_poll, 0, "poll", 5, stack_poll, sizeof (stack_poll));
	for (;;)
		menu ();
}

void uos_init (void)
{
	extern char __bss_end;

	uart_init (&uart, 0, 90, KHZ, 9600);
	timer_init (&timer, KHZ, 10);

	/* Enable external RAM: port A - address/data, port C - address. */
	setb (SRE, MCUCR);

	/* Port B - LEDs. */
	DDRB = 0xff;

	/* Initialize memory allocator.
	 * External RAM first, in order of desreasing addresses. */
	mem_init (&pool, RAM_START, RAM_END);

	/* Internal CPU memory - 4000 bytes.
	 * Leave 128 bytes for idle task.
	 * GCC places a symbol __bss_end at the program end,
	 * all other memory is free. */
	mem_init (&pool, (unsigned short) &__bss_end, 0x1000 - 128);

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
