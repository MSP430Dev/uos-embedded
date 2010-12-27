/*
 * Testing RAM on Elvees MC-24R_EM board.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

#define SDRAM_START	0xA0000000
#define SDRAM_SIZE	(128*1024*1024)
#define FLASH_START	0xBFC00000

static inline unsigned word_check (unsigned addr, unsigned val)
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

	debug_puts ("\nTesting all SDRAM ... 0%");
	nerrors = 0;
	for (addr=SDRAM_START; addr<SDRAM_START+SDRAM_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			debug_puts ("\nToo many errors, test stopped.\n");
			return;
		}
		if ((addr & 0xfffc) == 0xfffc) {
			debug_printf ("\b\b\b%2d%%",
				(((addr-SDRAM_START) >> 16) * 100) / (SDRAM_SIZE >> 16));
			if (debug_peekchar () >= 0) {
				debug_getchar ();
				break;
			}
		}
	}
	debug_puts ("\b\b\b done.\n");
}

void inline __attribute__ ((always_inline))
sdram_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (SDRAM_START + addr) = val;
}

static int
sdram_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (SDRAM_START + addr);
	if (rval == val)
		return 0;
	debug_printf ("\nAddress %08X written %08X read %08X ",
		SDRAM_START + addr, val, rval);
	return 1;
}

/*
 * Вычисляем "бегущую единицу" - слово, в котором установлен
 * единственный бит, соответствующий указанному номеру.
 */
unsigned running_one (unsigned bitno)
{
	return 1 << (bitno & 31);
}

void sdram_test_address_bus ()
{
	int d, a, nerrors;

	debug_printf ("\nTesting SDRAM address signals ");
	debug_puts ("----------------");
	debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (d=0; d<32; ++d) {
		sdram_write (0x0000000, 0x55555555);
		for (a=2; a<27; ++a) {
			/* Пробегаем единицей по всем битам адреса,
			 * записывая разные значения. */
			sdram_write (1 << a, running_one (a+d));
		}

		/* Проверяем, не затёрлись ли данные. */
		nerrors += sdram_check (0x0000000, 0x55555555);
		for (a=2; a<27; ++a) {
			nerrors += sdram_check (1 << a, running_one (a+d));
		}

		if (nerrors) {
			debug_puts ("\nTest aborted.\n");
			return;
		}
		debug_putchar (0, '.');
	}
	debug_puts (" done.\n");
}

void test_boot_flash ()
{
	unsigned id;

	debug_printf ("Testing boot flash: 32-bit width.\n");
	for (;;) {
		*(volatile unsigned*) (FLASH_START + 0x555*4) = 0xAAAAAAAA;
		*(volatile unsigned*) (FLASH_START + 0x2AA*4) = 0x55555555;
		*(volatile unsigned*) (FLASH_START + 0x555*4) = 0x90909090;

		id = *(volatile unsigned*) (FLASH_START + 4);

		*(volatile unsigned*) FLASH_START = 0xF0F0F0F0;

		debug_printf ("id = %08x\n", id);
		if (debug_peekchar () >= 0) {
			debug_getchar ();
			break;
		}
	}
}

void menu ()
{
	int cmd;

	debug_printf ("\n  0. Show memory registers");
	debug_printf ("\n  1. Test SDRAM address signals");
	debug_printf ("\n  2. Test SDRAM address %08X", SDRAM_START + 0x02aaaaa8);
	debug_printf ("\n  3. Test SDRAM address %08X", SDRAM_START + 0x05555554);
	debug_printf ("\n  4. Test all SDRAM (%d Mbytes)", SDRAM_SIZE/1024/1024);
/*	debug_printf ("\n  5. Test flash");*/
	debug_puts ("\n\n");
	for (;;) {
		/* Ввод команды. */
		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_putchar (0, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '0') {
			debug_printf ("  CSR    = %08X\n", MC_CSR);
			debug_printf ("  CSCON0 = %08X\n", MC_CSCON0);
			debug_printf ("  CSCON1 = %08X\n", MC_CSCON1);
			debug_printf ("  CSCON2 = %08X\n", MC_CSCON2);
			debug_printf ("  CSCON3 = %08X\n", MC_CSCON3);
			debug_printf ("  SDRCON = %08X\n", MC_SDRCON);
			break;
		}
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
/*		if (cmd == '5') {
			test_boot_flash ();
			break;
		} */
	}
}

int main (void)
{
	debug_printf ("\nTesting memory on MC-24R_EM board\n");
	debug_printf ("Generator %d.%d MHz, CPU clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10);

	/* Configure 16 Mbytes of external flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (3);		/* Wait states  */

	/* Configure 128 Mbytes of external 64-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_WS (0) |		/* Wait states  */
		MC_CSCON_T |			/* Sync memory */
		MC_CSCON_W64 |			/* 64-bit data width */
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xF8000000);	/* Address mask */
	MC_SDRCON = MC_SDRCON_INIT |		/* Initialize SDRAM */
		MC_SDRCON_BL_PAGE |		/* Bursh full page */
		MC_SDRCON_RFR (64000000/8192, KHZ) |	/* Refresh period */
		MC_SDRCON_PS_512;		/* Page size 512 */
	udelay (2);

	for (;;)
		menu ();
}
