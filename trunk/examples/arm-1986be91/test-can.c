/*
 * Testing CAN.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>
#include <milandr/can.h>
#include "board-1986be91.h"

#define CTL(c)		((c) & 037)

ARRAY (stack_console, 1000);		/* Task: menu on console */
ARRAY (stack_transmit, 1000);		/* Task: transmit packets */
ARRAY (stack_receive, 1000);		/* Task: receive packets */

gpanel_t display;
timer_t timer;
can_t can;
int local_loop;
volatile int transmit_enable;
const can_frame_t pattern = { 0, 8, { 0xabcdef89, 0x12345678 } };

extern gpanel_font_t font_fixed6x8;

void send_packets (int num)
{
	int i;

	for (i=0; i<num; ++i) {
		can_output (&can, &pattern);
	}
}

void task_receive ()
{
	can_frame_t fr;

	for (;;) {
		/* Check received data. */
		can_input (&can, &fr);
		if (fr.id != pattern.id ||
		    fr.dlc != pattern.dlc ||
		    fr.data[0] != pattern.data[0] ||
		    fr.data[1] != pattern.data[1])
			printf (&debug, "\npacket #%ld: data error\n",
				can.in_packets);
	}
}

void task_transmit ()
{
	for (;;) {
		/* Send packets - make transmit queue full. */
		if (transmit_enable) {
			send_packets (100);
		}
		timer_delay (&timer, 10);
	}
}

void run_test ()
{
	small_uint_t c;

	printf (&debug, "\nRunning external loop test.\n");
	printf (&debug, "(press <Enter> to stop, `C' to clear counters)\n\n");
	transmit_enable = 1;
	for (;;) {
		/* Print results. */
		printf (&debug, "- out %ld - in %ld - errors lost=%ld -\r",
			can.out_packets, can.in_packets, can.in_discards);

		/* Break on any keyboard input. */
		if (peekchar (&debug) >= 0) {
			c = getchar (&debug);
			if (c == 'C' || c == 'c') {
				/* Clear the line. */
				printf (&debug, "\r\33[K");
				can.out_packets = 0;
				can.in_packets = 0;
				can.in_discards = 0;
				continue;
			}
			break;
		}
		timer_delay (&timer, 100);
	}
	transmit_enable = 0;
	puts (&debug, "\nDone.\n");
}

void task_console (void *data)
{
	small_uint_t cmd;
again:
	printf (&debug, "\nTesting CAN\n\n");
	printf (&debug, "CAN: %u kbit/sec, %u interrupts\n", can.kbitsec, can.intr);

	printf (&debug, "Transmit: %ld packets, %ld collisions, %ld errors\n",
			can.out_packets, can.out_collisions, can.out_errors);
	printf (&debug, "Receive: %ld packets, %ld errors, %ld lost\n",
			can.in_packets, can.in_errors, can.in_discards);

	printf (&debug, "\n  1. Transmit 1 packet");
	printf (&debug, "\n  2. Transmit 2 packets");
	printf (&debug, "\n  3. Transmit 100 packets");
	printf (&debug, "\n  4. Run send/receive test");
	printf (&debug, "\n  5. Local loopback: %s",
			local_loop ? "Enabled" : "Disabled");
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
			local_loop = ! local_loop;
			can_set_loop (&can, local_loop);
			break;
		}
		if (cmd == CTL('T')) {
			/* Список задач uOS. */
			printf (&debug, "\n\n");
			task_print (&debug, 0);
			task_print (&debug, (task_t*) stack_console);
			task_print (&debug, (task_t*) stack_receive);
			task_print (&debug, (task_t*) stack_transmit);
			task_print (&debug, (task_t*) can.stack);
			putchar (&debug, '\n');
			continue;
		}
	}
	goto again;
}

/*
 * Redirect debug output.
 */
void gpanel_putchar (void *arg, short c)
{
	putchar ((stream_t*) arg, c);
}

void uos_init (void)
{
	buttons_init ();

	/* Use LCD panel for debug output. */
//	gpanel_init (&display, &font_fixed6x8);
//	gpanel_clear (&display, 0);
//	printf (&display, "Testing CAN.\n");

//	debug_redirect (gpanel_putchar, &display);
//	debug_printf ("Testing CAN.\n");

	timer_init (&timer, KHZ, 10);

	can_init (&can, 1, 90, 1000);

	task_create (task_receive, 0, "rcv", 30,
		stack_receive, sizeof (stack_receive));
	task_create (task_transmit, 0, "send", 20,
		stack_transmit, sizeof (stack_transmit));
	task_create (task_console, 0, "console", 10,
		stack_console, sizeof (stack_console));
}
