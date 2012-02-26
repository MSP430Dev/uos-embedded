/*
 * Testing SDRAM.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <uart/uart.h>
#include <elvees/mct-02.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#   define CRAMS_START	0x98400000
#else
#   define SDRAM_START	0xA0000000
#   define CRAMS_START	0xb8400000
#endif

/* 64 Мб SDRAM */
#define SDRAM_SIZE	(64*1024*1024)

/* 2 Мб внутренней памяти (CRAMS) */
#define CRAMS_SIZE	(2*1024*1024)

ARRAY (stack_console, 2000);	/* Task: menu on console */
extern void _etext();

uart_t uart;

MEM test_mem[]={
	{ 0x00000000, 0x55555555 },
	{ 0x00000004, 0x00000001 },
	{ 0x00000008, 0x00000002 },
	{ 0x00000010, 0x00000004 },
	{ 0x00000020, 0x00000008 },
	{ 0x00000040, 0x00000010 },
	{ 0x00000080, 0x00000020 },
	{ 0x00000100, 0x00000040 },
	{ 0x00000200, 0x00000080 },
	{ 0x00000400, 0x00000100 },
	{ 0x00000800, 0x00000200 },
	{ 0x00001000, 0x00000400 },
	{ 0x00002000, 0x00000800 },
	{ 0x00004000, 0x00001000 },
	{ 0x00008000, 0x00002000 },
	{ 0x00010000, 0x00004000 },
	{ 0x00020000, 0x00008000 },
	{ 0x00040000, 0x00010000 },
	{ 0x00080000, 0x00020000 },
	{ 0x00100000, 0x00040000 },
	{ 0x00200000, 0x00080000 },
	{ 0x00400000, 0x00100000 },
	{ 0x00800000, 0x00200000 },
	{ 0x01000000, 0x00400000 },
	{ 0x02000000, 0x00800000 },
	{ 0x00000000, 0x00000000 },
};

MEM test_mem_crams[]={
	{ 0x00000000, 0x55555555 },
	{ 0x00000004, 0x00000001 },
	{ 0x00000008, 0x00000002 },
	{ 0x00000010, 0x00000004 },
	{ 0x00000020, 0x00000008 },
	{ 0x00000040, 0x00000010 },
	{ 0x00000080, 0x00000020 },
	{ 0x00000100, 0x00000040 },
	{ 0x00000200, 0x00000080 },
	{ 0x00000400, 0x00000100 },
	{ 0x00000800, 0x00000200 },
	{ 0x00001000, 0x00000400 },
	{ 0x00002000, 0x00000800 },
	{ 0x00004000, 0x00001000 },
	{ 0x00008000, 0x00002000 },
	{ 0x00010000, 0x00004000 },
	{ 0x00020000, 0x00008000 },
	{ 0x00040000, 0x00010000 },
	{ 0x00080000, 0x00020000 },
	{ 0x00100000, 0x00040000 },
	{ 0x00000000, 0x00000000 },
};

/**
 * Memory configuration
 */
void set_mem(MEM* table)
{
	unsigned i;
	for (i=0;table[i].addr|table[i].val;i++) {
		*(unsigned*)(table[i].addr)=table[i].val;		
	};
	udelay (2);
};

unsigned word_check (unsigned addr, unsigned val)
{
	volatile unsigned *p = (unsigned*) addr;
	unsigned rval;

	*p = val;
	rval = *p;
	if (rval == val)
		return 0;
	printf (&uart, "\nAddress %08X written %08X read %08X ",
		addr, val, rval);
	return 1;
}

void word_test (unsigned addr)
{
	unsigned i, n;

	printf (&uart, "\nTesting address %08X... ", addr);
	for (i=0; ; ++i) {
		for (n=1; n; n<<=1) {
			word_check (addr, n);
			word_check (addr, ~n);
		}
		putchar (&uart, "|/-\\" [i&3]);
		putchar (&uart, '\b');
		if (peekchar (&uart) >= 0) {
			getchar (&uart);
			break;
		}
	}
	puts (&uart, "done.\n");
}

void sdram_test (void)
{
	unsigned addr, nerrors;

	puts (&uart, "\nTesting all SDRAM ");
	puts (&uart, "--------------------------------");
	puts (&uart, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=SDRAM_START; addr<SDRAM_START+SDRAM_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			puts (&uart, "\nToo many errors, test stopped.\n");
			return;
		}
		/* Заменяем '-' на '.' каждые 2 Мб */
		if ((addr & 0x1ffffc) == 0x1ffffc)
			putchar (&uart, '.');
	}
	puts (&uart, " done.\n");
}

void crams_test (void)
{
	unsigned addr, nerrors;

	puts (&uart, "\nTesting all CRAMS ");
	puts (&uart, "--------------------------------");
	puts (&uart, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=CRAMS_START; addr<CRAMS_START+CRAMS_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			puts (&uart, "\nToo many errors, test stopped.\n");
			return;
		}
		/* Заменяем '-' на '.' каждые 64 Кб */
		if ((addr & 0xfffc) == 0xfffc)
			putchar (&uart, '.');
	}
	puts (&uart, " done.\n");
}


void inline __attribute__ ((always_inline))
sdram_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (SDRAM_START + addr) = val;
}

void inline __attribute__ ((always_inline))
crams_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (CRAMS_START + addr) = val;
}

static void
sdram_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (SDRAM_START + addr);
	if (rval != val)
		printf(&uart, "\nAddress %08X written %08X read %08X ",
			SDRAM_START + addr, val, rval);
}

static void
crams_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (CRAMS_START + addr);
	if (rval != val)
		printf(&uart, "\nAddress %08X written %08X read %08X ",
			CRAMS_START + addr, val, rval);
}

void sdram_test_address_bus ()
{
	int i,j;

	printf (&uart, "\nTesting SDRAM address signals ");
	puts (&uart, "----------------");
	puts (&uart, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		for (j=0;test_mem[j].addr|test_mem[j].val;j++) {
			sdram_write (test_mem[j].addr, test_mem[j].val);		
		};

		for (j=0;test_mem[j].addr|test_mem[j].val;j++) {
			sdram_check (test_mem[j].addr, test_mem[j].val);		
		};

		putchar (&uart, '.');
	}
	puts (&uart, " done.\n");
}

void crams_test_address_bus ()
{
	int i,j;

	printf (&uart, "\nTesting CRAMS address signals ");
	puts (&uart, "----------------");
	puts (&uart, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		for (j=0;test_mem_crams[j].addr|test_mem_crams[j].val;j++) {
			crams_write (test_mem_crams[j].addr, test_mem_crams[j].val);		
		};

		for (j=0;test_mem_crams[j].addr|test_mem_crams[j].val;j++) {
			crams_check (test_mem_crams[j].addr, test_mem_crams[j].val);		
		};

		putchar (&uart, '.');
	}
	puts (&uart, " done.\n");
}

void menu ()
{
	int cmd;

	for (;;) {
		printf (&uart, "\33[H\33[2J");
		printf (&uart, "\nTesting memory on LDE-Vega board\n");
		printf (&uart, "Generator %d.%d MHz, CPU clock %d.%d MHz, bus clock %d.%d MHz\n",
			ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10,
			MPORT_KHZ/1000, MPORT_KHZ/100%10);

#ifdef ENABLE_ICACHE
		puts (&uart, "Instruction cache enabled\n");
#else
		puts (&uart, "Instruction cache disabled\n");
#endif
#ifdef ENABLE_DCACHE
		puts (&uart, "Data cache enabled\n");
#else
		puts (&uart, "Data cache disabled\n");
#endif
		printf (&uart, "  CRPLL  = %08X\n", MC_CRPLL);
		printf (&uart, "  CSCON0 = %08X\n", MC_CSCON0);
		printf (&uart, "  CSCON3 = %08X\n", MC_CSCON3);
		printf (&uart, "  SDRCON = %08X\n", MC_SDRCON);
		printf (&uart, "  SDRTMR = %08X\n", MC_SDRTMR);
		printf (&uart, "  SRTMR  = %08X\n", MC_SRTMR);

		printf(&uart, "\n  1. Test SDRAM address signals");
		printf(&uart, "\n  2. Test SDRAM address %08X", SDRAM_START + 0x02aaaaa8);
		printf(&uart, "\n  3. Test SDRAM address %08X", SDRAM_START + 0x02555554);
		printf(&uart, "\n  4. Test all SDRAM (%d Mbytes)", SDRAM_SIZE/1024/1024);
		printf(&uart, "\n  5. Test CRAMS address signals");
		printf(&uart, "\n  6. Test CRAMS address %08X", CRAMS_START + 0x001aaaa8);
		printf(&uart, "\n  7. Test CRAMS address %08X", CRAMS_START + 0x00155554);
		printf(&uart, "\n  8. Test all CRAMS (%d Mbytes)", CRAMS_SIZE/1024/1024);
		puts (&uart, "\n\n");

		/* Ввод команды. */
		puts (&uart, "Command: ");
		cmd = getchar (&uart);
		putchar (&uart, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			sdram_test_address_bus ();
		}
		if (cmd == '2') {
			word_test (SDRAM_START + 0x02aaaaa8);
		}
		if (cmd == '3') {
			word_test (SDRAM_START + 0x02555554);
		}
		if (cmd == '4') {
			sdram_test ();
		}
		if (cmd == '5') {
			crams_test_address_bus ();
		}
		if (cmd == '6') {
			word_test (CRAMS_START + 0x001aaaa8);
		}
		if (cmd == '7') {
			word_test (CRAMS_START + 0x00155554);
		}
		if (cmd == '8') {
			crams_test ();
		}
		puts (&uart, "Press any key");
		getchar (&uart);
	}
}

void main_console (void *data)
{
	for (;;)
		menu ();
}

void uos_init (void)
{
	set_mem(cscon);
	uart_init (&uart, 0, 90, KHZ, 115200);
#if 0
	/* Стираем экран. */
	printf (&uart, "\33[H\33[2J");
	printf (&uart, "\nTesting memory on LDE-Vega board\n");
	printf (&uart, "Generator %d.%d MHz, CPU clock %d.%d MHz, bus clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10,
		MPORT_KHZ/1000, MPORT_KHZ/100%10);

#ifdef ENABLE_ICACHE
	puts (&uart, "Instruction cache enabled\n");
#else
	puts (&uart, "Instruction cache disabled\n");
#endif
#ifdef ENABLE_DCACHE
	puts (&uart, "Data cache enabled\n");
#else
	puts (&uart, "Data cache disabled\n");
#endif
	printf (&uart, "  CRPLL  = %08X\n", MC_CRPLL);
	printf (&uart, "  CSCON0 = %08X\n", MC_CSCON0);
	printf (&uart, "  CSCON3 = %08X\n", MC_CSCON3);
	printf (&uart, "  SDRCON = %08X\n", MC_SDRCON);
	printf (&uart, "  SDRTMR = %08X\n", MC_SDRTMR);
	printf (&uart, "  SDRTMR = %08X\n", MC_SDRTMR);
#endif
	task_create (main_console, 0, "console", 1,	stack_console, sizeof (stack_console));
}
