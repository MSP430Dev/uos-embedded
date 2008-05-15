/*
 * Testing RAM on STK500 board.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include <mem/mem.h>
#include <buf/buf.h>
#include <net/netif.h>
#include <s3c4530/eth.h>

/*
 * Установлена микросхема 1M x 16 - имеем 2 мегабайта памяти.
 */
#define RAM_START      0x02000000		/* SDRAM start address */
#define RAM_SIZE       (2*1024*1024)		/* SDRAM size (bytes) */
#define RAM_END        (RAM_START+RAM_SIZE)	/* SDRAM end address */
#define REFRESH_USEC	8			/* refresh period (usec) */
#define IO_START	0x03600000		/* address of i/o space */

#define ctl(c)		((c) & 037)

char stack_console [0x300];	/* Задача: меню на консоли */
char stack_test [0x300];	/* Задача: передача-прием пакетов */
uart_t uart;
mem_pool_t pool;
char group [sizeof(lock_group_t) + 4 * sizeof(lock_slot_t)];
eth_t *eth;
int packet_size = 1500;
int kbaud = 2500;
int local_loop;
unsigned char *data_pattern;
int run_test_flag;
volatile int tx_count, tx_limit;

/*
 * Ввод 16-битного целого числа, 5 разрядов.
 */
unsigned short get_short (unsigned short init_val)
{
	unsigned char i, first_touch, cmd;
	unsigned long val;

	printf (&uart, "%d", init_val);
	if      (init_val > 9999) i = 5;
	else if (init_val > 999)  i = 4;
	else if (init_val > 99)   i = 3;
	else if (init_val > 9)    i = 2;
	else if (init_val > 0)    i = 1;
	else {
		i = 0;
		putchar (&uart, '\b');
	}
	first_touch = 1;
	val = init_val;
	for (;;) {
		cmd = getchar (&uart);
		if (cmd >= '0' && cmd <= '9') {
			if (first_touch) {
				first_touch = 0;
				while (i-- > 0)
					puts (&uart, "\b \b");
				i = 0;
				val = 0;
			}
			if (i == 5) {
err:				putchar (&uart, 7);
				continue;
			}
			val = val * 10 + cmd - '0';
			if (val > 0xffff) {
				val /= 10;
				goto err;
			}
			putchar (&uart, cmd);
			++i;
			continue;
		}
		first_touch = 0;
		if (cmd == ctl('[') || cmd == '[')
			continue;
		if (cmd == ctl('C'))
			return init_val;
		if (cmd == '\n' || cmd == '\r')
			return val;
		if (cmd == ctl('H') || cmd == 0177) {
			if (i == 0)
				goto err;
			printf (&uart, "\b \b");
			--i;
			val /= 10;
			continue;
		}
		goto err;
	}
}

/*
 * Ввод 32-битного целого числа, 10 разрядов.
 */
unsigned long get_long (unsigned long init_val)
{
	unsigned char i, first_touch, cmd;
	unsigned long long val;

	printf (&uart, "%d", init_val);
	if      (init_val > 999999999) i = 10;
	else if (init_val > 99999999) i = 9;
	else if (init_val > 9999999) i = 8;
	else if (init_val > 999999) i = 7;
	else if (init_val > 99999) i = 6;
	else if (init_val > 9999) i = 5;
	else if (init_val > 999) i = 4;
	else if (init_val > 99) i = 3;
	else if (init_val > 9) i = 2;
	else if (init_val > 0) i = 1;
	else {
		i = 0;
		putchar (&uart, '\b');
	}
	first_touch = 1;
	val = init_val;
	for (;;) {
		cmd = getchar (&uart);
		if (cmd >= '0' && cmd <= '9') {
			if (first_touch) {
				first_touch = 0;
				while (i-- > 0)
					puts (&uart, "\b \b");
				i = 0;
				val = 0;
			}
			if (i == 10) {
err:				putchar (&uart, 7);
				continue;
			}
			val = val * 10 + cmd - '0';
			if (val > 0xffffffff) {
				val /= 10;
				goto err;
			}
			putchar (&uart, cmd);
			++i;
			continue;
		}
		first_touch = 0;
		if (cmd == ctl('[') || cmd == '[')
			continue;
		if (cmd == ctl('C'))
			return init_val;
		if (cmd == '\n' || cmd == '\r')
			return val;
		if (cmd == ctl('H') || cmd == 0177) {
			if (i == 0)
				goto err;
			printf (&uart, "\b \b");
			--i;
			val /= 10;
			continue;
		}
		goto err;
	}
}

void send_packets (int num)
{
	int i;
	buf_t *p;

	for (i=0; i<num; ++i) {
		p = buf_alloc (&pool, packet_size, 16);
		if (! p) {
			printf (&uart, "out of memory\n");
			break;
		}
		memcpy (p->payload, data_pattern, packet_size);
		if (! netif_output (&eth->netif, p, 0, 0))
			printf (&uart, "cannot send packet\n");
	}
}

void main_test ()
{
	buf_t *p;
	lock_group_t *g;

	/* Create a group of two locks: receive and transmit. */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &eth->netif.lock);
	lock_group_add (g, &eth->transmitter);
	lock_group_listen (g);

	for (;;) {
		/* Check received data. */
		p = netif_input (&eth->netif);
		if (p) {
#if 0
			if (memcmp (p->payload, data_pattern, packet_size) != 0)
				printf (&uart, "\npacket #%d: data error\n",
					eth->netif.in_packets);
#endif
			buf_free (p);
			continue;
		}
		/* Send packets - make transmit queue full. */
		if (run_test_flag && tx_count < tx_limit &&
		    eth_transmit_space (eth) > 0) {
			p = buf_alloc (&pool, packet_size, 16);
			if (p) {
#if 0
				memcpy (p->payload, data_pattern, packet_size);
#endif
				if (netif_output (&eth->netif, p, 0, 0)) {
					++tx_count;
					continue;
				}
			}
		}
		lock_group_wait (g, 0, 0);
	}
}

void run_test ()
{
	char c;

	printf (&uart, "\nRunning external loop test.\n");
	printf (&uart, "(press <Enter> to stop, `C' to clear counters)\n\n");
	run_test_flag = 1;
	lock_signal (&eth->transmitter, 0);
	for (;;) {
		/* Print results. */
		printf (&uart, "- out %d - in %d - errors un=%d ov=%d fr=%d crc=%d lost=%d -\r",
			eth->netif.out_packets, eth->netif.in_packets,
			eth->underrun, eth->overrun, eth->frame, eth->crc,
			eth->netif.in_discards);

		/* Break on any keyboard input. */
		if (peekchar (&uart) >= 0) {
			c = getchar (&uart);
			if (c == 'C' || c == 'c') {
				/* Clear the line. */
				printf (&uart, "\r\33[K");
				eth->netif.out_packets = 0;
				eth->netif.in_packets = 0;
				eth->netif.in_discards = 0;
				eth->underrun = 0;
				eth->overrun = 0;
				eth->frame = 0;
				eth->crc = 0;
				continue;
			}
			if (c == 'T' || c == 't') {
				printf (&uart, "\ntn=%d, te=%d, data[tn]=%08x, st[tn]=%04x\n",
					eth->tn, eth->te,
					eth->tdesc[eth->tn].data,
					eth->tdesc[eth->tn].status);
				printf (&uart, "BDMASTAT=%b\n",
					ARM_BDMASTAT, ARM_BDMASTAT_BITS);
				printf (&uart, "MACTXSTAT=%b\n",
					ARM_MACTXSTAT, ARM_MACTXSTAT_BITS);
				continue;
			}
			if (c == 'R' || c == 'r') {
				printf (&uart, "\nrn=%d, data[rn]=%08x, st[rn]=%04x\n",
					eth->rn,
					eth->rdesc[eth->rn].data,
					eth->rdesc[eth->rn].status);
				printf (&uart, "BDMASTAT=%b\n",
					ARM_BDMASTAT, ARM_BDMASTAT_BITS);
				printf (&uart, "MACRXSTAT=%b\n",
					ARM_MACRXSTAT, ARM_MACRXSTAT_BITS);
				continue;
			}
			break;
		}
		if (tx_limit <= tx_count + 500)
			tx_limit = tx_count + 2000;
		lock_signal (&eth->transmitter, 0);
	}
	run_test_flag = 0;
	puts (&uart, "\nDone.\n");
}

void menu ()
{
	char cmd;
	int speed, full_duplex;

	printf (&uart, "Free memory: %d bytes\n", mem_available (&pool));

	printf (&uart, "Ethernet: %s", eth_get_carrier (eth) ?
		"Cable OK" : "No cable");
	speed = eth_get_speed (eth, &full_duplex);
	if (speed) {
		printf (&uart, ", %s %s",
			speed == 100000000 ? "100Base-TX" : "10Base-TX",
			full_duplex ? "Full Duplex" : "Half Duplex");
	}
	printf (&uart, "\n");

	printf (&uart, "Transmit: %d packets, %d collisions, %d errors\n",
			eth->netif.out_packets, eth->netif.out_collisions,
			eth->netif.out_errors);

	printf (&uart, "Receive: %d packets, %d errors, %d lost\n",
			eth->netif.in_packets, eth->netif.in_errors,
			eth->netif.in_discards);

	printf (&uart, "\n  1. Transmit 1 packet");
	printf (&uart, "\n  2. Transmit 2 packets");
	printf (&uart, "\n  3. Transmit 100 packets");
	printf (&uart, "\n  4. Run send/receive test");
	printf (&uart, "\n  5. Start auto-negotiation");
	printf (&uart, "\n  6. Packet size: %d bytes", packet_size);
	printf (&uart, "\n  7. Local loopback: %s",
			local_loop ? "Enabled" : "Disabled");
	puts (&uart, "\n\n");
	for (;;) {
		/* Ввод команды. */
		puts (&uart, "Command: ");
		cmd = getchar (&uart);
		putchar (&uart, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			send_packets (1);
			break;
		}
		if (cmd == '2') {
			send_packets (2);
			break;
		}
		if (cmd == '3') {
			send_packets (100);
			break;
		}
		if (cmd == '4') {
			run_test ();
			break;
		}
		if (cmd == '5') {
			eth_start_negotiation (eth);
			break;
		}
		if (cmd == '6') {
try_again:		printf (&uart, "Enter packet size (1-1600): ");
			packet_size = get_short (packet_size);
			if (packet_size <= 0 || packet_size > 1600) {
				printf (&uart, "Invalid value, try again.");
				goto try_again;
			}
			putchar (&uart, '\n');
			data_pattern = mem_realloc (data_pattern, packet_size);
			if (! data_pattern) {
				printf (&uart, "No memory for data_pattern\n");
				abort ();
			}
			memset (data_pattern, 0xFF, packet_size);
			if (packet_size >= 12)
				memcpy (data_pattern+6, eth->netif.ethaddr, 6);
			break;
		}
		if (cmd == '7') {
			local_loop = ! local_loop;
			eth_set_loop (eth, local_loop);
			break;
		}
	}
}

void configure_ram (unsigned long ram_start, unsigned long ram_end,
	int refresh_usec, unsigned long io_start)
{
	arm_memory_regs_t reg;

	arm_get_memory_regs (&reg);

	/* DRAM bank 0: 16-bit bus width. */
	reg.extdbwth |= ARM_EXTDBWTH_16BIT << ARM_EXTDBWTH_DSD0_shift;

	/* DRAM bank 0 address, size and timings. */
	reg.dramcon0 = ARM_DRAMCON_BASE (ram_start) |
		ARM_DRAMCON_NEXT (ram_end) |
		ARM_DRAMCON_CAN_8B |	/* column address = 8-bit */
		ARM_DRAMCON_TCS_1C |	/* CAS strobe time = 1 cycle */
					/* CAS pre-charge time = 1 cycle */
		ARM_DRAMCON_TRC_2C |	/* RAS to CAS delay = 2 cycles */
		ARM_DRAMCON_TRP_4C;	/* RAS pre-charge time = 4 cycles */

	/* Setup DRAM refresh cycle = 8 usec. */
	reg.refextcon = ARM_REFEXTCON_BASE (io_start) |
		ARM_REFEXTCON_VSF |	/* validity of special regs */
		ARM_REFEXTCON_REN |	/* refresh enable */
		ARM_REFEXTCON_TCHR_4C | /* CAS hold time = 4 cycles */
					/* CAS setup time = 1 cycle */
		ARM_REFEXTCON_RFR (refresh_usec, KHZ);

	/* Disable write buffer and cache. */
	ARM_SYSCFG &= ~(ARM_SYSCFG_WE | ARM_SYSCFG_CE);

	/* Sync DRAM mode. */
	ARM_SYSCFG |= ARM_SYSCFG_SDM;

	/* Set memory configuration registers, all at once. */
	arm_set_memory_regs (&reg);

	/* Enable write buffer and cache. */
	ARM_SYSCFG |= ARM_SYSCFG_WE | ARM_SYSCFG_CE;
#if 0
        /* Invalidate the entire cache.
         * Clear 1-kbyte tag memory. */
        memset ((void*) ARM_CACHE_TAG_ADDR, 0, 1024);
#endif
}

void main_console (void *data)
{
	printf (&uart, "\nTesting Ethernet\n\n");
	eth_set_promisc (eth, 1, 1);

	data_pattern = mem_alloc (&pool, packet_size);
	if (! data_pattern) {
		printf (&uart, "No memory for data_pattern\n");
		abort ();
	}
	memset (data_pattern, 0xFF, packet_size);
	if (packet_size >= 12)
		memcpy (data_pattern+6, eth->netif.ethaddr, 6);
	for (;;)
		menu ();
}

void uos_init (void)
{
	/* Baud 9600. */
	ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
	ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	configure_ram (RAM_START, RAM_END, REFRESH_USEC, IO_START);
	mem_init (&pool, RAM_START, RAM_END);
	uart_init (&uart, 0, 90, KHZ, 9600);
	eth = mem_alloc (&pool, sizeof (eth_t));
	if (! eth) {
		printf (&uart, "No memory for eth_t\n");
		abort ();
	}
	eth_init (eth, "eth0", 80, 70, &pool, 0);
	task_create (main_test, 0, "test", 5,
		stack_test, sizeof (stack_test));
	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
