/*
 * Testing SRAM on Starterkit SAM9-S3E board.
 * It is connected to FPGA via 8-bit data bus.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>

#define	SRAM_START	0x10000000	/* NCS0 */
#define	SRAM_SIZE	(512*1024)	/* 512 kbytes */

ARRAY (stack_console, 400);		/* Task: menu on console */

void inline __attribute__ ((always_inline))
write_external_ram (unsigned long offset, unsigned char val)
{
	*(volatile unsigned char*) (SRAM_START + offset) = val;
}

void
check_external_ram (unsigned long offset, unsigned char val)
{
	unsigned char rval;

	rval = *(volatile unsigned char*) (SRAM_START + offset);
	if (rval != val)
		debug_printf ("\nAddress 0x%08lx written 0x%02x read 0x%02x ",
			SRAM_START + offset, val, rval);
}

void
test_byte (unsigned long offset)
{
	unsigned char i, n;

	debug_printf ("\nTesting address 0x%08lx... ", SRAM_START + offset);
	for (i=0; ; ++i) {
		for (n=1; n; ++n) {
			write_external_ram (offset, 0xAA);
			check_external_ram (offset, 0xAA);

			write_external_ram (offset, 0x55);
			check_external_ram (offset, 0x55);

			if (debug_peekchar () >= 0) {
				debug_getchar ();
				debug_puts ("\nStopped.\n");
				return;
			}
		}
		debug_putchar (0, "|/-\\" [i&3]);
		debug_putchar (0, '\b');
	}
}

void
test_ram (void)
{
	unsigned long offset;

	debug_puts ("\nTesting all SRAM ");
	for (;;) {
		debug_puts ("--------------------------------");
		debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		for (offset=0; offset<SRAM_SIZE; offset++) {
			write_external_ram (offset, 0x55);
			check_external_ram (offset, 0x55);

			write_external_ram (offset, 0xAA);
			check_external_ram (offset, 0xAA);

			if ((offset & 0x3fff) == 0x3fff)
				debug_putchar (0, '.');
			if (debug_peekchar () >= 0) {
				debug_getchar ();
				debug_puts ("\nStopped.\n");
				return;
			}
		}
		debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
}

void
test_address_signals ()
{
	int i;

	debug_printf ("\nTesting address signals... ");
	for (i=0; ; ++i) {
		write_external_ram (0x000000, 0x55);
		write_external_ram (0x000002, 0x01);
		write_external_ram (0x000004, 0x02);
		write_external_ram (0x000008, 0x04);
		write_external_ram (0x000010, 0x08);
		write_external_ram (0x000020, 0x10);
		write_external_ram (0x000040, 0x20);
		write_external_ram (0x000080, 0x40);
		write_external_ram (0x000100, 0x80);
		write_external_ram (0x000200, 0x81);
		write_external_ram (0x000400, 0x42);
		write_external_ram (0x000800, 0x24);
		write_external_ram (0x001000, 0x18);
		write_external_ram (0x002000, 0x21);
		write_external_ram (0x004000, 0x42);
		write_external_ram (0x008000, 0x84);
		write_external_ram (0x010000, 0x09);
		write_external_ram (0x020000, 0x11);
		write_external_ram (0x040000, 0x22);

		check_external_ram (0x000000, 0x55);
		check_external_ram (0x000002, 0x01);
		check_external_ram (0x000004, 0x02);
		check_external_ram (0x000008, 0x04);
		check_external_ram (0x000010, 0x08);
		check_external_ram (0x000020, 0x10);
		check_external_ram (0x000040, 0x20);
		check_external_ram (0x000080, 0x40);
		check_external_ram (0x000100, 0x80);
		check_external_ram (0x000200, 0x81);
		check_external_ram (0x000400, 0x42);
		check_external_ram (0x000800, 0x24);
		check_external_ram (0x001000, 0x18);
		check_external_ram (0x002000, 0x21);
		check_external_ram (0x004000, 0x42);
		check_external_ram (0x008000, 0x84);
		check_external_ram (0x010000, 0x09);
		check_external_ram (0x020000, 0x11);
		check_external_ram (0x040000, 0x22);

		debug_putchar (0, "|/-\\" [i&3]);
		debug_putchar (0, '\b');
		if (debug_peekchar () >= 0) {
			debug_getchar ();
			debug_puts ("\nStopped.\n");
			break;
		}
	}
}

void display_reg32 (volatile unsigned int *addr, char *title)
{
	debug_printf ("%08x: %08x  - %s\n",
		(unsigned) addr, *addr, title);
}

void show_smc_registers ()
{
	debug_printf ("\nStatic memory controller registers for NCS0:\n");
	display_reg32 (AT91C_SMC_SETUP0,	"AT91C_SMC_SETUP0");
	display_reg32 (AT91C_SMC_PULSE0,	"AT91C_SMC_PULSE0");
	display_reg32 (AT91C_SMC_CYCLE0,	"AT91C_SMC_CYCLE0");
	display_reg32 (AT91C_SMC_CTRL0,		"AT91C_SMC_CTRL0");
}

void
menu ()
{
	char cmd;

	debug_printf ("\n  1. Test address signals");
	debug_printf ("\n  2. Test address 0x%08X", SRAM_START + 0x2aaaa);
	debug_printf ("\n  3. Test all SRAM (%d kbytes)", SRAM_SIZE/1024);
	debug_printf ("\n  4. Show SMC NCS0 registers");
	debug_puts ("\n\n");
	for (;;) {
		/* Ввод команды. */
		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_putchar (0, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			test_address_signals ();
			break;
		}
		if (cmd == '2') {
			test_byte (0x2aaaa);
			break;
		}
		if (cmd == '3') {
			test_ram ();
			break;
		}
		if (cmd == '4') {
			show_smc_registers ();
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
	debug_puts ("\nTesting SRAM addressed via FPGA\n");

	/* On Starterkit SAM9-S3E board a signal NCS0 addresses
	 * an FPGA chip. Configure it as READ_MODE=0, WRITE_MODE=0, BAT=0.
	 * Address is AT91C_EBIC_CS0 == 0x10000000. */
#if 0
	/* Incorrect. */
	*AT91C_SMC_SETUP0 = 0x00000000;
	*AT91C_SMC_PULSE0 = 0x03020101;
	*AT91C_SMC_CYCLE0 = 0x00010001;
#endif
	/* Works fine. */
	*AT91C_SMC_SETUP0 = 0x00000002;
	*AT91C_SMC_PULSE0 = 0x0A0A0A06;
	*AT91C_SMC_CYCLE0 = 0x000A000A;
	*AT91C_SMC_CTRL0  = AT91C_SMC_DBW_WIDTH_EIGTH_BITS;

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
