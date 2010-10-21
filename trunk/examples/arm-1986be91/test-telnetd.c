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
#include <gpanel/gpanel.h>
#include <milandr/k5600bg1.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/telnet.h>

#define TASKSZ		1000		/* Task: telnet menu */
#define MAXSESS		4		/* Up to 4 telnet sessions */

ARRAY (stack_telnet, TASKSZ);
ARRAY (stack_console, TASKSZ);
ARRAY (stack_poll, TASKSZ);

task_t *tasktab [MAXSESS];
stream_t *streamtab [MAXSESS];

/*
 * Priorities for tasks.
 */
#define PRIO_POLL	1
#define PRIO_SHELL	10
#define PRIO_CONSOLE	20
#define PRIO_TELNET	30
#define PRIO_IP		40
#define PRIO_ETH	70
#define PRIO_UART	90

mem_pool_t pool;
timer_t timer;
uart_t uart;
k5600bg1_t eth;
arp_t *arp;
route_t route;
ip_t ip;

gpanel_t display;
extern gpanel_font_t font_fixed6x8;

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
	//task_print (stream, (task_t*) uart.rstack);

	putchar (stream, '\n');
}

static void
eth_cmd (stream_t *stream)
{
	long speed;
	int full_duplex;

	putchar (stream, '\n');
	if (k5600bg1_get_carrier (&eth)) {
		speed = k5600bg1_get_speed (&eth, &full_duplex);
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
	k5600bg1_debug (&eth, stream);

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

void poll_task (void *data)
{
	unsigned last_sec = 0;

	for (;;) {
		unsigned sec = timer_milliseconds (&timer) / 1000;
		if (sec == last_sec)
			continue;
		last_sec = sec;

		unsigned min = sec / 60;
		unsigned hour = min / 60;
		sec -= min*60;
		min -= hour*60;
		gpanel_clear (&display, 0);
		puts (&display, "--Работает 5600ВГ1У--\r\n");
		printf (&display, "Время теста:%3u:%02u:%02u\r\n", hour, min, sec);
		printf (&display, " TX пакетов:%9lu\r\n", eth.netif.out_packets);
		printf (&display, "     ошибок:%9lu\r\n", eth.netif.out_errors);
		printf (&display, " RX пакетов:%9lu\r\n", eth.netif.in_packets);
		printf (&display, "     ошибок:%9lu\r\n", eth.netif.in_errors);
		printf (&display, " Прерываний:%9lu\r\n", eth.intr);
		printf (&display, "Своб.байтов:%9u\r\n", mem_available (&pool));
		timer_delay (&timer, 500);
	}
}

void
console_task (void *data)
{
	for (;;)
		user_shell ((stream_t*) &uart);
}

void telnet_task (void *data)
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

void uos_init (void)
{
	/* Используем только внутреннюю память.
	 * Оставляем 256 байтов для задачи "idle". */
	extern unsigned __bss_end[], _estack[];
	mem_init (&pool, (unsigned) __bss_end, (unsigned) _estack - 256);

	timer_init (&timer, KHZ, 50);
	uart_init (&uart, 1, PRIO_UART, KHZ, 115200);
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	puts (&display, "Работает 5600ВГ1У.\r\n\n");

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
	k5600bg1_init (&eth, "eth0", PRIO_ETH, &pool, arp, mac_addr);

	static unsigned char ip_addr [4] = { 192, 168, 20, 222 };
	route_add_netif (&ip, &route, ip_addr, 24, &eth.netif);

	task_create (console_task, 0, "console", PRIO_CONSOLE,
		stack_console, sizeof (stack_console));
	task_create (poll_task, 0, "poll", PRIO_POLL,
		stack_poll, sizeof (stack_poll));
	task_create (telnet_task, 0, "telnet", PRIO_TELNET,
		stack_telnet, sizeof (stack_telnet));
}
