/*
 * Testing Ethernet.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <milandr/k5600bg1.h>

#define CTL(c)		((c) & 037)

ARRAY (stack_console, 1500);		/* Task: menu on console */
ARRAY (stack_test, 1000);		/* Task: transmit/receive packets */
mem_pool_t pool;
k5600bg1_t eth;
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

void test_task ()
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
		k5600bg1_poll (&eth);
//if (ARM_GPIOB->DATA >> 10 & 1) debug_printf ("!");
//else debug_printf (".");
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
				k5600bg1_debug (&eth, &debug);
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
	int full_duplex;

	printf (&debug, "Free memory: %d bytes\n", mem_available (&pool));

	printf (&debug, "Ethernet: %s",
		k5600bg1_get_carrier (&eth) ? "Cable OK" : "No cable");
	if (k5600bg1_get_speed (&eth, &full_duplex)) {
		printf (&debug, ", %s", full_duplex ? "Full Duplex" : "Half Duplex");
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
	printf (&debug, "\n  3. Transmit 8 packets");
	printf (&debug, "\n  4. Run send/receive test");
	printf (&debug, "\n  5. Packet size: %d bytes", packet_size);
	printf (&debug, "\n  6. Local loopback: %s",
			local_loop ? "Enabled" : "Disabled");
	puts (&debug, "\n\n");
	for (;;) {
		/* Ввод команды. */
		puts (&debug, "Command: ");
		while (peekchar (&debug) < 0)
			timer_delay (&timer, 50);
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
			send_packets (8);
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
			int i;
			for (i=0; i<packet_size; i++)
				data_pattern[i] = i;
			if (packet_size >= 6)
				memset (data_pattern, 0xFF, 6);
			if (packet_size >= 12)
				memcpy (data_pattern+6, eth.netif.ethaddr, 6);
			break;
		}
		if (cmd == '6') {
			local_loop = ! local_loop;
			k5600bg1_set_loop (&eth, local_loop);
			break;
		}
		if (cmd == CTL('E')) {
			/* Регистры Ethernet. */
			putchar (&debug, '\n');
			k5600bg1_debug (&eth, &debug);
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

void console_task (void *data)
{
	printf (&debug, "\nTesting Ethernet\n\n");
	k5600bg1_set_promisc (&eth, 1, 1);

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
	/* Используем только внутреннюю память.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);

	timer_init (&timer, KHZ, 50);

	const unsigned char my_macaddr[] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	k5600bg1_init (&eth, "eth0", 80, &pool, 0, my_macaddr);

	task_create (test_task, 0, "test", 5,
		stack_test, sizeof (stack_test));
	task_create (console_task, 0, "console", 10,
		stack_console, sizeof (stack_console));
}
