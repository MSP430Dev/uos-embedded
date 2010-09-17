/*
 * Testing Ethernet.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <elvees/eth.h>

#define CTL(c)		((c) & 037)

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x00000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_console, 1000);		/* Task: menu on console */
ARRAY (stack_test, 1000);		/* Task: transmit/receive packets */
mem_pool_t pool;
eth_t eth;
timer_t timer;
int packet_size = 1500;
int local_loop;
unsigned char *data_pattern;
volatile int run_test_flag;

/*
 * Enter 16-bit integer value, 5 digits.
 */
unsigned short get_short (unsigned short init_val)
{
	small_uint_t i, first_touch, cmd;
	unsigned long val;

	printf (&debug, "%d", init_val);
	if      (init_val > 9999) i = 5;
	else if (init_val > 999)  i = 4;
	else if (init_val > 99)   i = 3;
	else if (init_val > 9)    i = 2;
	else if (init_val > 0)    i = 1;
	else {
		i = 0;
		putchar (&debug, '\b');
	}
	first_touch = 1;
	val = init_val;
	for (;;) {
		cmd = getchar (&debug);
		if (cmd >= '0' && cmd <= '9') {
			if (first_touch) {
				first_touch = 0;
				while (i-- > 0)
					puts (&debug, "\b \b");
				i = 0;
				val = 0;
			}
			if (i == 5) {
err:				putchar (&debug, 7);
				continue;
			}
			val = val * 10 + cmd - '0';
			if (val > 0xffff) {
				val /= 10;
				goto err;
			}
			putchar (&debug, cmd);
			++i;
			continue;
		}
		first_touch = 0;
		if (cmd == CTL('[') || cmd == '[')
			continue;
		if (cmd == CTL('C'))
			return init_val;
		if (cmd == '\n' || cmd == '\r')
			return val;
		if (cmd == CTL('H') || cmd == 0177) {
			if (i == 0)
				goto err;
			printf (&debug, "\b \b");
			--i;
			val /= 10;
			continue;
		}
		goto err;
	}
}

/*
 * Enter 32-bit integer value, 10 digits.
 */
unsigned long get_long (unsigned long init_val)
{
	small_uint_t i, first_touch, cmd;
	unsigned long long val;

	printf (&debug, "%d", init_val);
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
		putchar (&debug, '\b');
	}
	first_touch = 1;
	val = init_val;
	for (;;) {
		cmd = getchar (&debug);
		if (cmd >= '0' && cmd <= '9') {
			if (first_touch) {
				first_touch = 0;
				while (i-- > 0)
					puts (&debug, "\b \b");
				i = 0;
				val = 0;
			}
			if (i == 10) {
err:				putchar (&debug, 7);
				continue;
			}
			val = val * 10 + cmd - '0';
			if (val > 0xffffffff) {
				val /= 10;
				goto err;
			}
			putchar (&debug, cmd);
			++i;
			continue;
		}
		first_touch = 0;
		if (cmd == CTL('[') || cmd == '[')
			continue;
		if (cmd == CTL('C'))
			return init_val;
		if (cmd == '\n' || cmd == '\r')
			return val;
		if (cmd == CTL('H') || cmd == 0177) {
			if (i == 0)
				goto err;
			printf (&debug, "\b \b");
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
			printf (&debug, "out of memory\n");
			break;
		}
		memcpy (p->payload, data_pattern, packet_size);
		if (! netif_output (&eth.netif, p, 0, 0))
			printf (&debug, "cannot send packet\n");
	}
}

void main_test ()
{
	buf_t *p;

	for (;;) {
		/* Check received data. */
		p = netif_input (&eth.netif);
		if (p) {
			if (memcmp (p->payload, data_pattern, packet_size) != 0)
				printf (&debug, "\npacket #%ld: data error\n",
					eth.netif.in_packets);
			buf_free (p);
			continue;
		}
		/* Send packets - make transmit queue full. */
		if (run_test_flag) {
			p = buf_alloc (&pool, packet_size, 16);
			if (p) {
				memcpy (p->payload, data_pattern, packet_size);
				netif_output (&eth.netif, p, 0, 0);
			}
		}
		timer_delay (&timer, 20);
	}
}

void run_test ()
{
	small_uint_t c;

	printf (&debug, "\nRunning external loop test.\n");
	printf (&debug, "(press <Enter> to stop, `C' to clear counters)\n\n");
	run_test_flag = 1;
	for (;;) {
		/* Print results. */
		printf (&debug, "- out %ld - in %ld - errors lost=%ld -\r",
			eth.netif.out_packets, eth.netif.in_packets,
			eth.netif.in_discards);

		/* Break on any keyboard input. */
		if (peekchar (&debug) >= 0) {
			c = getchar (&debug);
			if (c == 'C' || c == 'c') {
				/* Clear the line. */
				printf (&debug, "\r\33[K");
				eth.netif.out_packets = 0;
				eth.netif.in_packets = 0;
				eth.netif.in_discards = 0;
				continue;
			}
			if (c == 'D' || c == 'd') {
				eth_debug (&eth, &debug);
				continue;
			}
			break;
		}
		timer_delay (&timer, 100);
	}
	run_test_flag = 0;
	puts (&debug, "\nDone.\n");
}

void menu ()
{
	small_uint_t cmd;
	long speed;
	int full_duplex;

	printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));

	printf (&debug, "Ethernet: %s",
		eth_get_carrier (&eth) ? "Cable OK" : "No cable");
	speed = eth_get_speed (&eth, &full_duplex);
	if (speed) {
		printf (&debug, ", %s %s",
			speed == 100000000 ? "100Base-TX" : "10Base-TX",
			full_duplex ? "Full Duplex" : "Half Duplex");
	}
	printf (&debug, ", %u interrupts\n", eth.intr);

	printf (&debug, "Transmit: %ld packets, %ld collisions, %ld errors\n",
			eth.netif.out_packets, eth.netif.out_collisions,
			eth.netif.out_errors);

	printf (&debug, "Receive: %ld packets, %ld errors, %ld lost\n",
			eth.netif.in_packets, eth.netif.in_errors,
			eth.netif.in_discards);

	printf (&debug, "\n  1. Transmit 1 packet");
	printf (&debug, "\n  2. Transmit 2 packets");
	printf (&debug, "\n  3. Transmit 100 packets");
	printf (&debug, "\n  4. Run send/receive test");
	printf (&debug, "\n  5. Packet size: %d bytes", packet_size);
	printf (&debug, "\n  6. Local loopback: %s",
			local_loop ? "Enabled" : "Disabled");
/*	printf (&debug, "\n  0. Start auto-negotiation");*/
	puts (&debug, "\n\n");
	for (;;) {
		/* Ввод команды. */
		puts (&debug, "Command: ");
		cmd = getchar (&debug);
		putchar (&debug, '\n');

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
try_again:		printf (&debug, "Enter packet size (1-1518): ");
			packet_size = get_short (packet_size);
			if (packet_size <= 0 || packet_size > 1518) {
				printf (&debug, "Invalid value, try again.");
				goto try_again;
			}
			putchar (&debug, '\n');
			data_pattern = mem_realloc (data_pattern, packet_size);
			if (! data_pattern) {
				printf (&debug, "No memory for data_pattern\n");
				uos_halt (1);
			}
			memset (data_pattern, 0xFF, packet_size);
			if (packet_size >= 12)
				memcpy (data_pattern+6, eth.netif.ethaddr, 6);
			break;
		}
		if (cmd == '6') {
			local_loop = ! local_loop;
			eth_set_loop (&eth, local_loop);
			break;
		}
/*		if (cmd == '0') {
			eth_start_negotiation (&eth);
			break;
		} */
		if (cmd == CTL('E')) {
			/* Регистры Ethernet. */
			putchar (&debug, '\n');
			eth_debug (&eth, &debug);
			putchar (&debug, '\n');
			continue;
		}
		if (cmd == CTL('T')) {
			/* Список задач uOS. */
			printf (&debug, "\nFree memory: %u bytes\n\n",
				mem_available (&pool));
			task_print (&debug, 0);
			task_print (&debug, (task_t*) stack_console);
			task_print (&debug, (task_t*) stack_test);
			task_print (&debug, (task_t*) eth.stack);
			putchar (&debug, '\n');
			continue;
		}
	}
}

void main_console (void *data)
{
	printf (&debug, "\nTesting Ethernet\n\n");
	eth_set_promisc (&eth, 1, 1);

	data_pattern = mem_alloc (&pool, packet_size);
	if (! data_pattern) {
		printf (&debug, "No memory for data_pattern\n");
		uos_halt (1);
	}
	memset (data_pattern, 0xFF, packet_size);
	if (packet_size >= 12)
		memcpy (data_pattern+6, eth.netif.ethaddr, 6);
	for (;;)
		menu ();
}

void uos_init (void)
{
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (3);		/* Wait states  */

	/* Configure 64 Mbytes of external 32-bit SDRAM memory at nCS0. */
	MC_CSCON0 = MC_CSCON_E |		/* Enable nCS0 */
		MC_CSCON_T |			/* Sync memory */
		MC_CSCON_CSBA (0x00000000) |	/* Base address */
		MC_CSCON_CSMASK (0xF8000000);	/* Address mask */

	MC_SDRCON = MC_SDRCON_PS_512 |		/* Page size 512 */
		MC_SDRCON_CL_3 |		/* CAS latency 3 cycles */
		MC_SDRCON_RFR (64000000/8192, MPORT_KHZ); /* Refresh period */

	MC_SDRTMR = MC_SDRTMR_TWR(2) |		/* Write recovery delay */
		MC_SDRTMR_TRP(2) |		/* Минимальный период Precharge */
		MC_SDRTMR_TRCD(2) |		/* Между Active и Read/Write */
		MC_SDRTMR_TRAS(5) |		/* Между * Active и Precharge */
		MC_SDRTMR_TRFC(15);		/* Интервал между Refresh */

	MC_SDRCSR = 1;				/* Initialize SDRAM */
        udelay (2);

	mem_init (&pool, SDRAM_START, SDRAM_START + SDRAM_SIZE);

	timer_init (&timer, KHZ, 50);
	eth_init (&eth, "eth0", 80, &pool, 0);

	task_create (main_test, 0, "test", 5,
		stack_test, sizeof (stack_test));
	task_create (main_console, 0, "console", 2,
		stack_console, sizeof (stack_console));
}
