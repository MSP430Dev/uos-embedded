/*
 * Testing Ethernet on easyWeb2 board.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <cs8900/cs8900.h>
#include <lcd2/lcd.h>

ARRAY (stack_poll, 0x100);	/* Task: polling CS8900 */
mem_pool_t pool;
cs8900_t eth;
lcd_t line1, line2;

void poll_eth (void *data)
{
	buf_t *p;
	unsigned long rx_packets, rx_bytes;

	rx_packets = 0;
	rx_bytes = 0;
	for (;;) {
		cs8900_poll (&eth);
		p = eth.netif.interface->input (&eth.netif);
		if (p) {
			++rx_packets;
			rx_bytes += p->tot_len;
			lcd_clear_all (&line1, &line2);
			printf (&line1, "Packets: %ld", rx_packets);
			printf (&line2, "Bytes: %ld", rx_bytes);

			debug_printf ("Packet #%ld:\n", rx_packets);
			buf_print_ethernet (p);
			buf_free (p);
		}
	}
}

void uos_init (void)
{
	extern char __bss_end, __stack;

	lcd_init (&line1, &line2, 0);
	debug_printf ("\nTesting Ethernet.\n");
	if (cs8900_probe ()) {
		debug_printf ("Chip detected successfully.\n");
		printf (&line1, "Testing CS8900A");
	} else {
		debug_printf ("Chip not detected!\n");
		printf (&line1, "CS8900A error!");
	}
	/* Initialize memory pool, leave 128 bytes for idle task stack. */
	mem_init (&pool, (unsigned) &__bss_end, (unsigned) &__stack - 128);
	printf (&line2, "Free %d bytes", mem_available (&pool));

	cs8900_init (&eth, "eth0", 80, &pool, 0);
	task_create (poll_eth, 0, "eth", 1, stack_poll, sizeof (stack_poll));
}
