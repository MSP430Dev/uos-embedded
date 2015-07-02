/*
 * Testing SDRAM.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_console, 1000);	/* Task: menu on console */
extern void _etext();

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

void sdram_test (void)
{
	unsigned addr, nerrors;

	debug_puts ("\nTesting all SDRAM ");
	debug_puts ("--------------------------------");
	debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=SDRAM_START; addr<SDRAM_START+SDRAM_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			debug_puts ("\nToo many errors, test stopped.\n");
			return;
		}
		if ((addr & 0x1ffffc) == 0x1ffffc)
			debug_putchar (0, '.');
	}
	debug_puts (" done.\n");
}

void inline __attribute__ ((always_inline))
sdram_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (SDRAM_START + addr) = val;
}

static void
sdram_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (SDRAM_START + addr);
	if (rval != val)
		debug_printf ("\nAddress %08X written %08X read %08X ",
			SDRAM_START + addr, val, rval);
}

void sdram_test_address_bus ()
{
	int i;

	debug_printf ("\nTesting SDRAM address signals ");
	debug_puts ("----------------");
	debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		sdram_write (0x0000000, 0x55555555);
		sdram_write (0x0000004, 0x00000001);
		sdram_write (0x0000008, 0x00000002);
		sdram_write (0x0000010, 0x00000004);
		sdram_write (0x0000020, 0x00000008);
		sdram_write (0x0000040, 0x00000010);
		sdram_write (0x0000080, 0x00000020);
		sdram_write (0x0000100, 0x00000040);
		sdram_write (0x0000200, 0x00000080);
		sdram_write (0x0000400, 0x00000100);
		sdram_write (0x0000800, 0x00000200);
		sdram_write (0x0001000, 0x00000400);
		sdram_write (0x0002000, 0x00000800);
		sdram_write (0x0004000, 0x00001000);
		sdram_write (0x0008000, 0x00002000);
		sdram_write (0x0010000, 0x00004000);
		sdram_write (0x0020000, 0x00008000);
		sdram_write (0x0040000, 0x00010000);
		sdram_write (0x0080000, 0x00020000);
		sdram_write (0x0100000, 0x00040000);
		sdram_write (0x0200000, 0x00080000);
		sdram_write (0x0400000, 0x00100000);
		sdram_write (0x0800000, 0x00200000);
		sdram_write (0x1000000, 0x00400000);
		sdram_write (0x2000000, 0x00800000);

		sdram_check (0x0000000, 0x55555555);
		sdram_check (0x0000004, 0x00000001);
		sdram_check (0x0000008, 0x00000002);
		sdram_check (0x0000010, 0x00000004);
		sdram_check (0x0000020, 0x00000008);
		sdram_check (0x0000040, 0x00000010);
		sdram_check (0x0000080, 0x00000020);
		sdram_check (0x0000100, 0x00000040);
		sdram_check (0x0000200, 0x00000080);
		sdram_check (0x0000400, 0x00000100);
		sdram_check (0x0000800, 0x00000200);
		sdram_check (0x0001000, 0x00000400);
		sdram_check (0x0002000, 0x00000800);
		sdram_check (0x0004000, 0x00001000);
		sdram_check (0x0008000, 0x00002000);
		sdram_check (0x0010000, 0x00004000);
		sdram_check (0x0020000, 0x00008000);
		sdram_check (0x0040000, 0x00010000);
		sdram_check (0x0080000, 0x00020000);
		sdram_check (0x0100000, 0x00040000);
		sdram_check (0x0200000, 0x00080000);
		sdram_check (0x0400000, 0x00100000);
		sdram_check (0x0800000, 0x00200000);
		sdram_check (0x1000000, 0x00400000);
		sdram_check (0x2000000, 0x00800000);
		debug_putchar (0, '.');
	}
	debug_puts (" done.\n");
}

void menu ()
{
	int cmd;

	debug_printf ("\n  1. Test SDRAM address signals");
	debug_printf ("\n  2. Test SDRAM address %08X", SDRAM_START + 0x02aaaaa8);
	debug_printf ("\n  3. Test SDRAM address %08X", SDRAM_START + 0x05555554);
	debug_printf ("\n  4. Test all SDRAM (%d Mbytes)", SDRAM_SIZE/1024/1024);
	debug_puts ("\n\n");
	for (;;) {
		/* Ввод команды. */
		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_putchar (0, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			sdram_test_address_bus ();
			break;
		}
		if (cmd == '2') {
			word_test (SDRAM_START + 0x02aaaaa8);
			break;
		}
		if (cmd == '3') {
			word_test (SDRAM_START + 0x05555554);
			break;
		}
		if (cmd == '4') {
			sdram_test ();
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
	debug_printf ("\nTesting memory on NVCom-EM board\n");
	debug_printf ("Generator %d.%d MHz, CPU clock %d.%d MHz, bus clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10,
		MPORT_KHZ/1000, MPORT_KHZ/100%10);
#ifdef ENABLE_ICACHE
	debug_printf ("Instruction cache enabled\n");
#else
	debug_printf ("Instruction cache disabled\n");
#endif
#ifdef ENABLE_DCACHE
	debug_printf ("Data cache enabled\n");
#else
	debug_printf ("Data cache disabled\n");
#endif

#if defined(ELVEES_MC24) || defined(ELVEES_MC0226) || defined(ELVEES_MC30SF6)
	/* Configure 128 Mbytes of external 64-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_WS (0) |		/* Wait states  */
		MC_CSCON_T |			/* Sync memory */
		MC_CSCON_W64 |			/* 64-bit data width */
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xF8000000);	/* Address mask */
	MC_SDRCON = MC_SDRCON_INIT |		/* Initialize SDRAM */
		MC_SDRCON_BL_PAGE |		/* Bursh full page */
		MC_SDRCON_RFR (64000000/8192, MPORT_KHZ) |	/* Refresh period */
		MC_SDRCON_PS_512;		/* Page size 512 */
	udelay (2);
	
	debug_printf ("  CSR    = %08X\n", MC_CSR);
	debug_printf ("  CSCON0 = %08X\n", MC_CSCON0);
	debug_printf ("  CSCON3 = %08X\n", MC_CSCON3);
	debug_printf ("  SDRCON = %08X\n", MC_SDRCON);
#else
	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
	MC_CSCON_T |			/* Sync memory */
	MC_CSCON_CSBA (0x00000000) |	/* Base address */
	MC_CSCON_CSMASK (0xF8000000);	/* Address mask */

	MC_SDRCON = MC_SDRCON_PS_512 |				/* Page size 512 */
		MC_SDRCON_CL_3 |				/* CAS latency 3 cycles */
		MC_SDRCON_RFR (64000000/8192, MPORT_KHZ); 	/* Refresh period */

	MC_SDRTMR = MC_SDRTMR_TWR(2) |		/* Write recovery delay */
		MC_SDRTMR_TRP(2) |		/* Минимальный период Precharge */
		MC_SDRTMR_TRCD(2) |		/* Между Active и Read/Write */
		MC_SDRTMR_TRAS(5) |		/* Между * Active и Precharge */
		MC_SDRTMR_TRFC(15);		/* Интервал между Refresh */

	MC_SDRCSR = 1;				/* Initialize SDRAM */
	udelay (2);
	
	debug_printf ("  CRPLL  = %08X\n", MC_CRPLL);
	debug_printf ("  CSCON0 = %08X\n", MC_CSCON0);
	debug_printf ("  CSCON3 = %08X\n", MC_CSCON3);
	debug_printf ("  SDRCON = %08X\n", MC_SDRCON);
	debug_printf ("  SDRTMR = %08X\n", MC_SDRTMR);
#endif
	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
