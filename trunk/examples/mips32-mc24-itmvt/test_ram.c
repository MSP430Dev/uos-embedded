/*
 * Testing RAM on PKI board.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

#define SRAM_START	0xA0000000
#define SRAM_SIZE	(4*1024*1024)

ARRAY (stack_console, 1000);	/* Task: menu on console */

unsigned word_check (unsigned addr, unsigned val)
{
	volatile unsigned *p = (unsigned*) addr;
	unsigned rval;

	*p = val;
	rval = *p;
	if (rval == val)
		return 0;
	debug_printf ("\nAddress %08X written %08X read %08X ",
		addr, val, rval);
	return 1;
}

void word_test (unsigned addr)
{
	unsigned i, n;

	debug_printf ("\nTesting address %08X... ", addr);
	for (i=0; ; ++i) {
		for (n=1; n; n<<=1) {
			word_check (addr, n);
			word_check (addr, ~n);
		}
		debug_putchar (0, "|/-\\" [i&3]);
		debug_putchar (0, '\b');
		if (debug_peekchar () >= 0) {
			debug_getchar ();
			break;
		}
	}
	debug_puts ("done.\n");
}

void sram_test (void)
{
	unsigned addr, nerrors;

	debug_puts ("\nTesting all SRAM ");
	debug_puts ("--------------------------------");
	debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=SRAM_START; addr<SRAM_START+SRAM_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			debug_puts ("\nToo many errors, test stopped.\n");
			return;
		}
		if ((addr & 0x7ffc) == 0x7ffc)
			debug_putchar (0, '.');
	}
	debug_puts (" done.\n");
}

static void inline __attribute__ ((always_inline))
sram_write (unsigned addr, unsigned val)
{
	addr += SRAM_START;
	*(volatile unsigned*) addr = val;
}

static void
sram_check (unsigned addr, unsigned val)
{
	unsigned rval;

	addr += SRAM_START;
	rval = *(volatile unsigned*) addr;
	if (rval != val)
		debug_printf ("\nAddress %08X written %08X read %08X ",
			addr, val, rval);
}

void sram_test_address_bus ()
{
	int i;

	debug_printf ("\nTesting SRAM address signals ");
	debug_puts ("----------------");
	debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		sram_write (0x000000, 0x55555555);
		sram_write (0x000004, 0x00000001);
		sram_write (0x000008, 0x00000002);
		sram_write (0x000010, 0x00000004);
		sram_write (0x000020, 0x00000008);
		sram_write (0x000040, 0x00000010);
		sram_write (0x000080, 0x00000020);
		sram_write (0x000100, 0x00000040);
		sram_write (0x000200, 0x00000080);
		sram_write (0x000400, 0x00000100);
		sram_write (0x000800, 0x00000200);
		sram_write (0x001000, 0x00000400);
		sram_write (0x002000, 0x00000800);
		sram_write (0x004000, 0x00001000);
		sram_write (0x008000, 0x00002000);
		sram_write (0x010000, 0x00004000);
		sram_write (0x020000, 0x00008000);
		sram_write (0x040000, 0x00010000);
		sram_write (0x080000, 0x00020000);
		sram_write (0x100000, 0x00040000);
		sram_write (0x200000, 0x00080000);

		sram_check (0x000000, 0x55555555);
		sram_check (0x000004, 0x00000001);
		sram_check (0x000008, 0x00000002);
		sram_check (0x000010, 0x00000004);
		sram_check (0x000020, 0x00000008);
		sram_check (0x000040, 0x00000010);
		sram_check (0x000080, 0x00000020);
		sram_check (0x000100, 0x00000040);
		sram_check (0x000200, 0x00000080);
		sram_check (0x000400, 0x00000100);
		sram_check (0x000800, 0x00000200);
		sram_check (0x001000, 0x00000400);
		sram_check (0x002000, 0x00000800);
		sram_check (0x004000, 0x00001000);
		sram_check (0x008000, 0x00002000);
		sram_check (0x010000, 0x00004000);
		sram_check (0x020000, 0x00008000);
		sram_check (0x040000, 0x00010000);
		sram_check (0x080000, 0x00020000);
		sram_check (0x100000, 0x00040000);
		sram_check (0x200000, 0x00080000);
		debug_putchar (0, '.');
	}
	debug_puts (" done.\n");
}

void menu ()
{
	int cmd;

	debug_printf ("\n  1. Test SRAM address signals");
	debug_printf ("\n  2. Test SRAM address %08X", SRAM_START + 0x00055554);
	debug_printf ("\n  3. Test SRAM address %08X", SRAM_START + 0x000aaaa8);
	debug_printf ("\n  4. Test all SRAM (%d kbytes)", SRAM_SIZE/1024);
	debug_puts ("\n\n");
	for (;;) {
		/* Ввод команды. */
		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_putchar (0, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			sram_test_address_bus ();
			break;
		}
		if (cmd == '2') {
			word_test (SRAM_START + 0x00055554);
			break;
		}
		if (cmd == '3') {
			word_test (SRAM_START + 0x000aaaa8);
			break;
		}
		if (cmd == '4') {
			sram_test ();
			break;
		}
	}
}

void main_console (void *data)
{
	for (;;)
		menu ();
}

void uos_init (void)
{
	debug_printf ("\nTesting memory on PKI board\n");
	debug_printf ("Generator %d.%d MHz, CPU clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10);

	/* Configure 1 Mbyte of external SRAM memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (7);		/* Wait states  */

	/* Configure 128 Mbytes of external 64-bit SRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_WS (15) |		/* Wait states  */
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xFF000000);	/* Address mask */
	udelay (2);

	debug_printf ("  CSR    = %08X\n", MC_CSR);
	debug_printf ("  CSCON0 = %08X\n", MC_CSCON0);
	debug_printf ("  CSCON3 = %08X\n", MC_CSCON3);

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
