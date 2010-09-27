/*
 * Example of telnet server.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <uart/uart.h>
#include <elvees/eth.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/telnet.h>

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x00000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

ARRAY (stack_telnet, 1500);
ARRAY (stack_console, 1500);

#define TASKSZ		1500		/* Task: telnet menu */
#define MAXSESS		4		/* Up to 4 telnet sessions */

task_t *tasktab [MAXSESS];
stream_t *streamtab [MAXSESS];

/*
 * Priorities for tasks.
 */
#define PRIO_SHELL	10
#define PRIO_CONSOLE	20
#define PRIO_TELNET	30
#define PRIO_IP		40
#define PRIO_ETH	70
#define PRIO_UART	90

mem_pool_t pool;
timer_t timer;
uart_t uart;
eth_t eth;
arp_t *arp;
route_t route;
ip_t ip;

ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));

static void
mem_cmd (stream_t *stream)
{
	int n;

	putchar (stream, '\n');
	printf (stream, "Free memory: %u bytes\n", mem_available (&pool));

	putchar (stream, '\n');
	task_print (stream, 0);
	task_print (stream, (task_t*) stack_console);
	task_print (stream, (task_t*) stack_telnet);
	for (n=0; n<MAXSESS; ++n)
		if (streamtab[n])
			task_print (stream, (task_t*) tasktab[n]);
	task_print (stream, (task_t*) ip.stack);
	task_print (stream, (task_t*) eth.stack);
	task_print (stream, (task_t*) eth.tstack);
	task_print (stream, (task_t*) uart.rstack);

	putchar (stream, '\n');
}

static void
eth_cmd (stream_t *stream)
{
	long speed;
	int full_duplex;

	putchar (stream, '\n');
	if (eth_get_carrier (&eth)) {
		speed = eth_get_speed (&eth, &full_duplex);
		printf (stream, "Ethernet: %ld Mbit/sec, %s Duplex\n",
			speed / 1000000, full_duplex ? "Full" : "Half");
	} else
		printf (stream, "Ethernet: No cable\n");

	putchar (stream, '\n');
	printf (stream, "   Rx packets: %ln\n", eth.netif.in_packets);
	printf (stream, "   Tx packets: %ln\n", eth.netif.out_packets);
	printf (stream, "    Rx errors: %ln\n", eth.netif.in_errors);
	printf (stream, "    Tx errors: %ln\n", eth.netif.out_errors);
	printf (stream, "  Rx discards: %ln\n", eth.netif.in_discards);
	printf (stream, "Tx collisions: %ln\n", eth.netif.out_collisions);
	printf (stream, "   Interrupts: %ln\n", eth.intr);

	/* Print Ethernet hardware registers. */
	puts (stream, "Ethernet hardware registers:\n");
	eth_debug (&eth, stream);

	putchar (stream, '\n');
}

static const char *
state_name (tcp_state_t state)
{
	switch (state) {
	case CLOSED:	  return "CLOSED";	break;
	case LISTEN:	  return "LISTEN";	break;
	case SYN_SENT:	  return "SYN_SENT";	break;
	case SYN_RCVD:	  return "SYN_RCVD";	break;
	case ESTABLISHED: return "ESTABLISHED";	break;
	case FIN_WAIT_1:  return "FIN_WAIT_1";	break;
	case FIN_WAIT_2:  return "FIN_WAIT_2";	break;
	case CLOSE_WAIT:  return "CLOSE_WAIT";	break;
	case CLOSING:	  return "CLOSING";	break;
	case LAST_ACK:	  return "LAST_ACK";	break;
	case TIME_WAIT:	  return "TIME_WAIT";	break;
	}
	return "???";
}

static void print_tcp_socket (stream_t *stream, tcp_socket_t *s)
{
	if (s->local_ip[0])
		printf (stream, "%d.%d.%d.%d\t", s->local_ip[0],
			s->local_ip[1], s->local_ip[2], s->local_ip[3]);
	else
		puts (stream, "*\t\t");

	printf (stream, "%u\t", s->local_port);

	if (s->remote_ip[0])
		printf (stream, "%d.%d.%d.%d\t", s->remote_ip[0],
			s->remote_ip[1], s->remote_ip[2], s->remote_ip[3]);
	else
		puts (stream, "*\t\t");

	if (s->remote_port)
		printf (stream, "%u\t", s->remote_port);
	else
		puts (stream, "*\t");

	puts (stream, state_name (s->state));
	putchar (stream, '\n');
}

static void
net_cmd (stream_t *stream, int arg)
{
	putchar (stream, '\n');
	if (arg == 'a') {
		arp_entry_t *e;

		puts (stream, "ARP table\n\n");
		for (e=arp->table; e<arp->table+arp->size; ++e) {
			if (! e->netif)
				continue;
			printf (stream, "%d.%d.%d.%d at %02x:%02x:%02x:%02x:%02x:%02x age %d on %s\n",
				e->ipaddr[0], e->ipaddr[1], e->ipaddr[2], e->ipaddr[3],
				e->ethaddr[0], e->ethaddr[1], e->ethaddr[2],
				e->ethaddr[3], e->ethaddr[4], e->ethaddr[5],
				e->age, e->netif->name);
		}
	} else if (arg == 'i') {
		puts (stream, "IP statistics\n");

		puts (stream, "\nIP input:\n");
		printf (stream, "    Received packets: %ln\n", ip.in_receives);
		printf (stream, "       Header errors: %ln\n", ip.in_hdr_errors);
		printf (stream, "      Address errors: %ln\n", ip.in_addr_errors);
		printf (stream, "   Discarded packets: %ln\n", ip.in_discards);
		printf (stream, "      Unknown protos: %ln\n", ip.in_unknown_protos);
		printf (stream, "   Delivered packets: %ln\n", ip.in_delivers);

		puts (stream, "\nIP output:\n");
		printf (stream, "     Output requests: %ln\n", ip.out_requests);
		printf (stream, "   Discarded packets: %ln\n", ip.out_discards);
		printf (stream, "      Unknown routes: %ln\n", ip.out_no_routes);
		printf (stream, "   Forwarded packets: %ln\n", ip.forw_datagrams);

	} else if (arg == 'c') {
		puts (stream, "ICMP statistics\n");

		puts (stream, "\nICMP input:\n");
		printf (stream, "    Input packets: %ln\n", ip.icmp_in_msgs);
		printf (stream, "  Errored packets: %ln\n", ip.icmp_in_errors);
		printf (stream, "     Echo queries: %ln\n", ip.icmp_in_echos);

		puts (stream, "\nICMP output:\n");
		printf (stream, "   Output packets: %ln\n", ip.icmp_out_msgs);
		printf (stream, "    Output errors: %ln\n", ip.icmp_out_errors);
		printf (stream, "Dest.unreachables: %ln\n", ip.icmp_out_dest_unreachs);
		printf (stream, "     Time exceeds: %ln\n", ip.icmp_out_time_excds);
		printf (stream, "     Echo replies: %ln\n", ip.icmp_out_echo_reps);

	} else if (arg == 't') {
		puts (stream, "TCP statistics\n\n");

		printf (stream, "    Input packets: %ln\n", ip.tcp_in_datagrams);
		printf (stream, "  Errored packets: %ln\n", ip.tcp_in_errors);
		printf (stream, "Discarded packets: %ln\n", ip.tcp_in_discards);
		printf (stream, "   Output packets: %ln\n", ip.tcp_out_datagrams);
		printf (stream, "    Output errors: %ln\n", ip.tcp_out_errors);

	} else {
		tcp_socket_t *s;

		puts (stream, "TCP sockets\n\n");
		puts (stream, "Local address   Port    Peer address    Port    State\n");
		for (s=ip.tcp_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
		for (s=ip.tcp_closing_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
		for (s=ip.tcp_listen_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
	}
	putchar (stream, '\n');
}

static void
help_cmd (stream_t *stream)
{
	puts (stream, "Available commands:\n");
	puts (stream, "    mem\n");
	puts (stream, "    eth\n");
	puts (stream, "    ip\n");
	puts (stream, "    arp\n");
	puts (stream, "    tcp\n");
	puts (stream, "    icmp\n");
	puts (stream, "    sock\n");
}

/*
 * Read a newline-terminated string from stream.
 */
static unsigned char *
getline (stream_t *stream, unsigned char *buf, int len)
{
	int c;
	unsigned char *s;

	s = buf;
        while (--len > 0) {
		c = getchar (stream);
		if (feof (stream))
			return 0;
		if (c == '\b' || c == 0177) {
			if (s > buf) {
				--s;
				puts (stream, "\b \b");
			}
			continue;
		}
		if (c == '\r')
			c = '\n';
		putchar (stream, c);
		*s++ = c;
		if (c == '\n')
			break;
	}
	*s = '\0';
	return buf;
}

void user_shell (void *arg)
{
	stream_t *stream = (stream_t*) arg;
	unsigned char line [200];
	int n;

	/* Give telnet some time to negotiate. */
	timer_delay (&timer, 500);
	fflush (stream);

	puts (stream, "\n\nTesting Telnet\n");
	puts (stream, "~~~~~~~~~~~~\n");
	printf (stream, "Free memory: %u bytes\n", mem_available (&pool));
	puts (stream, "\nEnter \"help\" for a list of commands\n\n");

	for (;;) {
		puts (stream, "% ");
		if (! getline (stream, line, sizeof (line)))
			break;
		if (line[0] == '\n')
			continue;

		if (strncmp ((unsigned char*) "help", line, 4) == 0) {
			help_cmd (stream);
		} else if (strncmp ((unsigned char*) "quit", line, 4) == 0) {
			break;
		} else if (strncmp ((unsigned char*) "mem", line, 3) == 0) {
			mem_cmd (stream);
		} else if (strncmp ((unsigned char*) "eth", line, 3) == 0) {
			eth_cmd (stream);
		} else if (strncmp ((unsigned char*) "ip", line, 2) == 0) {
			net_cmd (stream, 'i');
		} else if (strncmp ((unsigned char*) "arp", line, 3) == 0) {
			net_cmd (stream, 'a');
		} else if (strncmp ((unsigned char*) "tcp", line, 3) == 0) {
			net_cmd (stream, 't');
		} else if (strncmp ((unsigned char*) "icmp", line, 4) == 0) {
			net_cmd (stream, 'c');
		} else if (strncmp ((unsigned char*) "sock", line, 4) == 0) {
			net_cmd (stream, 0);
		} else
			printf (stream, "Unknown command: %s\n", line);
	}

	for (n=0; n<MAXSESS; ++n)
		if (streamtab[n] == stream) {
			fclose (stream);
			streamtab[n] = 0;
			task_exit (0);
		}
}

void start_session (tcp_socket_t *sock)
{
	int n;
	array_t *t;
	stream_t *stream;

	for (n=0; n<MAXSESS; ++n)
		if (! streamtab[n])
			break;
	if (n >= MAXSESS) {
		debug_printf ("Too many sessions\n");
		tcp_close (sock);
		return;
	}
	if (tasktab[n] != 0) {
		/* Free the memory of the previous session. */
		mem_free (tasktab[n]);
		tasktab[n] = 0;
	}
	t = (array_t*) mem_alloc (&pool, TASKSZ);
	if (! t) {
		debug_printf ("No memory for task\n");
		tcp_close (sock);
		return;
	}
	stream = telnet_init (sock);
	if (! stream) {
		debug_printf ("Error initializing telnet\n");
		tcp_close (sock);
		return;
	}
	streamtab[n] = stream;
	tasktab[n] = task_create (user_shell, stream, "shell", PRIO_SHELL, t, TASKSZ);
}

void
main_console (void *data)
{
	for (;;)
		user_shell ((stream_t*) &uart);
}

void main_telnet (void *data)
{
	tcp_socket_t *lsock, *sock;
	unsigned short serv_port = 23;

	lsock = tcp_listen (&ip, 0, serv_port);
	if (! lsock) {
		debug_printf ("Error on listen, aborted\n");
		uos_halt (1);
	}
	for (;;) {
		sock = tcp_accept (lsock);
		if (! sock) {
			debug_printf ("Error on accept\n");
			continue;
		}
		start_session (sock);
	}
}

bool_t __attribute__((weak))
uos_valid_memory_address (void *ptr)
{
	unsigned address = (unsigned) ptr;
	extern unsigned __data_start, _estack[];

	/* Internal SRAM. */
	if (address >= (unsigned) &__data_start &&
	    address < (unsigned) _estack)
		return 1;

	if (address >= SDRAM_START &&
	    address < SDRAM_START + SDRAM_SIZE)
		return 1;

	return 0;
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
	uart_init (&uart, 1, PRIO_UART, KHZ, 115200);

	/*
	 * Create a group of two locks: timer and eth.
	 */
	mutex_group_t *g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &eth.netif.lock);
	mutex_group_add (g, &timer.decisec);

	arp = arp_init (arp_data, sizeof(arp_data), &ip);
	ip_init (&ip, &pool, PRIO_IP, &timer, arp, g);

	/*
	 * Create interface eth0
	 */
	const unsigned char mac_addr [6] = { 0, 9, 0x94, 0xf1, 0xf2, 0xf3 };
	eth_init (&eth, "eth0", PRIO_ETH, &pool, arp, mac_addr);

	static unsigned char ip_addr [4] = { 192, 168, 20, 222 };
	route_add_netif (&ip, &route, ip_addr, 24, &eth.netif);

	task_create (main_console, 0, "console", PRIO_CONSOLE,
		stack_console, sizeof (stack_console));
	task_create (main_telnet, 0, "telnet", PRIO_TELNET,
		stack_telnet, sizeof (stack_telnet));
}
