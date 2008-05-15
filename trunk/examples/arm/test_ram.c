/*
 * Testing RAM on STK500 board.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"

/*
 * Установлена микросхема 1M x 16 - имеем 2 мегабайта памяти.
 */
#define RAM_START	0x02000000
#define RAM_SIZE	(2*1024*1024)
#define RAM_END		(RAM_START+RAM_SIZE)
#define REFRESH_USEC	8
#define IO_START	0x03600000

char stack_console [0x300];	/* Задача: меню на консоли */

void test_location (unsigned long addr, unsigned short val)
{
	volatile unsigned short *p = (unsigned short*) addr;
	unsigned short rval;

	*p = val;
	rval = *p;
	if (rval != val)
		debug_printf ("\nAddress 0x%08lx written 0x%04x read 0x%04x ",
			addr, val, rval);
}

void test_address (unsigned long addr)
{
	unsigned char i, n;

	debug_printf ("\nTesting address 0x%08x... ", addr);
	for (i=0; ; ++i) {
		for (n=1; n; ++n) {
			test_location (addr, 0xAAAA);
			test_location (addr, 0x5555);
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

void test_ram (void)
{
	unsigned long addr;

	debug_puts ("\nTesting all RAM ");
	for (;;) {
		debug_puts ("--------------------------------");
		debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		for (addr=RAM_START; addr<RAM_END; addr+=2) {
			test_location (addr, 0x5555);
			test_location (addr, 0xAAAA);
			if ((addr & 0xfffe) == 0xfffe)
				debug_putchar (0, '.');
			if (debug_peekchar () >= 0) {
				debug_getchar ();
				debug_puts ("\nDone.\n");
				return;
			}
		}
		debug_puts ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
}

void inline __attribute__ ((always_inline))
write_external_ram (unsigned long addr, unsigned short val)
{
	*(volatile unsigned short*) (RAM_START + addr) = val;
}

unsigned short inline __attribute__ ((always_inline))
read_external_ram (unsigned long addr)
{
	return *(volatile unsigned short*) (RAM_START + addr);
}

static void
check_external_ram (unsigned long addr, unsigned short val)
{
	unsigned short rval;

	rval = read_external_ram (addr);
	if (rval != val)
		debug_printf ("\nAddress 0x%06lx written 0x%04x read 0x%04x ",
			addr, val, rval);
}

void test_address_signals ()
{
	int i;

	debug_printf ("\nTesting address signals... ");
	for (i=0; ; ++i) {
		write_external_ram (0x000000, 0x5555);
		write_external_ram (0x000002, 0x0001);
		write_external_ram (0x000004, 0x0002);
		write_external_ram (0x000008, 0x0004);
		write_external_ram (0x000010, 0x0008);
		write_external_ram (0x000020, 0x0010);
		write_external_ram (0x000040, 0x0020);
		write_external_ram (0x000080, 0x0040);
		write_external_ram (0x000100, 0x0080);
		write_external_ram (0x000200, 0x0100);
		write_external_ram (0x000400, 0x0200);
		write_external_ram (0x000800, 0x0400);
		write_external_ram (0x001000, 0x0800);
		write_external_ram (0x002000, 0x1000);
		write_external_ram (0x004000, 0x2000);
		write_external_ram (0x008000, 0x4000);
		write_external_ram (0x010000, 0x8000);
		write_external_ram (0x020000, 0x1111);
		write_external_ram (0x040000, 0x2222);
		write_external_ram (0x080000, 0x4444);
		write_external_ram (0x100000, 0x8888);

		check_external_ram (0x000000, 0x5555);
		check_external_ram (0x000002, 0x0001);
		check_external_ram (0x000004, 0x0002);
		check_external_ram (0x000008, 0x0004);
		check_external_ram (0x000010, 0x0008);
		check_external_ram (0x000020, 0x0010);
		check_external_ram (0x000040, 0x0020);
		check_external_ram (0x000080, 0x0040);
		check_external_ram (0x000100, 0x0080);
		check_external_ram (0x000200, 0x0100);
		check_external_ram (0x000400, 0x0200);
		check_external_ram (0x000800, 0x0400);
		check_external_ram (0x001000, 0x0800);
		check_external_ram (0x002000, 0x1000);
		check_external_ram (0x004000, 0x2000);
		check_external_ram (0x008000, 0x4000);
		check_external_ram (0x010000, 0x8000);
		check_external_ram (0x020000, 0x1111);
		check_external_ram (0x040000, 0x2222);
		check_external_ram (0x080000, 0x4444);
		check_external_ram (0x100000, 0x8888);

		debug_putchar (0, "|/-\\" [i&3]);
		debug_putchar (0, '\b');
		if (debug_peekchar () >= 0) {
			debug_getchar ();
			break;
		}
	}
	debug_puts ("done.\n");
}

void menu ()
{
	char cmd;

	debug_printf ("\n  1. Test address signals");
	debug_printf ("\n  2. Test address 0x%08x", RAM_START + 0xaaaaa);
	debug_printf ("\n  3. Test all RAM (%d Mbytes)", RAM_SIZE/1024/1024);
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
			test_address (RAM_START + 0xaaaaa);
			break;
		}
		if (cmd == '3') {
			test_ram ();
			break;
		}
	}
}

void configure_ram (void)
{
	arm_memory_regs_t reg;

	arm_get_memory_regs (&reg);

	/* DRAM bank 0: 16-bit bus width. */
	reg.extdbwth |= ARM_EXTDBWTH_16BIT << ARM_EXTDBWTH_DSD0_shift;

	/* DRAM bank 0 address, size and timings. */
	reg.dramcon0 = ARM_DRAMCON_BASE (RAM_START) |
		ARM_DRAMCON_NEXT (RAM_END) |
		ARM_DRAMCON_CAN_8B |	/* column address = 8-bit */
		ARM_DRAMCON_TCS_1C |	/* CAS strobe time = 1 cycle */
					/* CAS pre-charge time = 1 cycle */
		ARM_DRAMCON_TRC_2C | 	/* RAS to CAS delay = 2 cycles */
		ARM_DRAMCON_TRP_4C;	/* RAS pre-charge time = 4 cycles */

	/* Setup DRAM refresh cycle = 8 usec. */
	reg.refextcon = ARM_REFEXTCON_BASE (IO_START) |
		ARM_REFEXTCON_VSF |	/* validity of special regs */
		ARM_REFEXTCON_REN |	/* refresh enable */
		ARM_REFEXTCON_TCHR_4C |	/* CAS hold time = 4 cycles */
					/* CAS setup time = 1 cycle */
		ARM_REFEXTCON_RFR (REFRESH_USEC, KHZ);

	/* Disable write buffer and cache. */
	ARM_SYSCFG &= ~(ARM_SYSCFG_WE | ARM_SYSCFG_CE);

	/* Sync DRAM mode. */
	ARM_SYSCFG |= ARM_SYSCFG_SDM;

	/* Set memory configuration registers, all at once. */
	arm_set_memory_regs (&reg);
}

void main_console (void *data)
{
	debug_printf ("\nTesting RAM\n");

	configure_ram ();
	for (;;)
		menu ();
}

void uos_init (void)
{
	/* Baud 9600 at 50/2 MHz. */
	ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
	ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
