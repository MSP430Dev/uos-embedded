/*
 * Cronyx Bridge platform.
 *
 * Copyright (C) 2003-2009 Cronyx Engineering Ltd.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <uart/uart.h>
#include <s3c4530/eth.h>
#include <s3c4530/hdlc.h>
#include <s3c4530/gpio.h>
#include <nvram/nvram.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/telnet.h>
#include <tcl/tcl.h>

/*
 * Installed two chips 1M x 16 - total 4 megabytes of RAM.
 */
#define RAM_START	0x02000000	/* SDRAM start address */
#define RAM_SIZE	(4*1024*1024)	/* SDRAM size (bytes) */
#define REFRESH_USEC	8		/* SDRAM refresh period (usec) */
#define IO_START	0x03600000	/* address of i/o space */

#define ctl(c)         ((c) & 037)

#define POLL_STACKSZ	800		/* Task: poll watchdog */
array_t *stack_poll;

#define CONSOLE_STACKSZ	1500		/* Task: console menu */
array_t *stack_console;

#define TELNET_STACKSZ	800		/* Task: telnet daemon */
array_t *stack_telnet;

#define TASKSZ		1500		/* Task: telnet menu */
#define MAXSESS		4		/* Up to 4 telnet sessions */

task_t *tasktab [MAXSESS];
stream_t *streamtab [MAXSESS];

/*
 * General purpose signals.
 */
#define PIN_DCD		0		/* P0 (output)  - console DCD */
#define PIN_RTS		3		/* P3 (input)   - console RTS */
#define PIN_CTS		4		/* P4 (output)  - console CTS */
#define PIN_LINK	16		/* P16 (output) - led "LINK" */
#define PIN_DTR		19		/* P19 (input)  - console DTR */
#define PIN_DSR		21		/* P21 (output) - console DSR */

/*
 * Priorities for tasks.
 */
#define PRIO_POLL	10
#define PRIO_TCL	15
#define PRIO_CONSOLE	20
#define PRIO_TELNET	30
#define PRIO_IP		40
#define PRIO_ETH_TX	50
#define PRIO_HDLC_TX	60
#define PRIO_ETH_RX	70
#define PRIO_HDLC_RX	80
#define PRIO_UART	90

mem_pool_t pool;
timer_t timer;
uart_t uart;
hdlc_t *hdlc;
eth_t *eth;
arp_t *arp;
route_t route;
ip_t *ip;

ARRAY (group, sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t));
ARRAY (arp_data, sizeof(arp_t) + 10 * sizeof(arp_entry_t));

int reset_counter;			/* Device reset counter */

static inline void control_link_led (int on)
{
	if (! on) {
		/* Turn LINK LED off. */
		gpio_set (PIN_LINK, 1);
	} else {
		/* Turn LINK LED on. */
		gpio_set (PIN_LINK, 0);
	}
}

void configure_ram (unsigned long ram_start, unsigned long ram_end,
	int refresh_usec, unsigned long io_start)
{
	arm_memory_regs_t reg;

	arm_get_memory_regs (&reg);

	/* Disable sync DRAM mode. */
	ARM_SYSCFG &= ~ARM_SYSCFG_SDM;
	reg.dramcon0 = 0;

	/* Set memory configuration registers, all at once. */
	arm_set_memory_regs (&reg);

	/* DRAM bank 0: 32-bit bus width. */
	reg.extdbwth |= ARM_EXTDBWTH_32BIT << ARM_EXTDBWTH_DSD0_shift;

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

	/* Enable write buffer. */
	ARM_SYSCFG |= ARM_SYSCFG_WE;

	/* Enable cache. */
	ARM_SYSCFG |= ARM_SYSCFG_CE;
}

buf_t *
receive_hdlc (hdlc_t *c, buf_t *p)
{
	debug_printf ("serial: received %d bytes\n", p->tot_len);
	buf_free (p);
	return 0;
}

/*
 * Implement the TCL loop command:
 *	loop var start end [increment] command
 */
static int
loop_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	int result = TCL_OK;
	int i, first, limit, incr = 1;
	unsigned char *command;
	unsigned char itxt [12];

	if ((argc < 5) || (argc > 6)) {
		Tcl_AppendResult (interp, "bad # args: ", argv [0],
			" var first limit [incr] command", 0);
		return TCL_ERROR;
	}

	if (Tcl_GetInt (interp, argv[2], &first) != TCL_OK)
		return TCL_ERROR;

	if (Tcl_GetInt (interp, argv[3], &limit) != TCL_OK)
		return TCL_ERROR;

	if (argc == 5)
		command = argv[4];
	else {
		if (Tcl_GetInt (interp, argv[4], &incr) != TCL_OK)
			return TCL_ERROR;
		command = argv[5];
	}

	for (i = first;
	    (((i < limit) && (incr > 0)) || ((i > limit) && (incr < 0)));
	    i += incr) {
		snprintf (itxt, sizeof (itxt), "%d", i);
		if (! Tcl_SetVar (interp, argv [1], itxt, TCL_LEAVE_ERR_MSG))
			return TCL_ERROR;

		result = Tcl_Eval (interp, command, 0, 0);
		if (result != TCL_OK) {
			if (result == TCL_CONTINUE) {
				result = TCL_OK;
			} else if (result == TCL_BREAK) {
				result = TCL_OK;
				break;
			} else if (result == TCL_ERROR) {
				unsigned char buf [64];

				snprintf (buf, sizeof (buf),
					"\n    (\"loop\" body line %d)",
					interp->errorLine);
				Tcl_AddErrorInfo (interp, buf);
				break;
			} else {
				break;
			}
		}
	}

	/*
	 * Set variable to its final value.
	 */
	snprintf (itxt, sizeof (itxt), "%d", i);
	if (! Tcl_SetVar (interp, argv [1], itxt, TCL_LEAVE_ERR_MSG))
		return TCL_ERROR;

	return result;
}

/*
 * Implement the TCL echo command:
 *	echo arg ...
 */
static int
echo_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;
	int i;

	for (i=1; ; i++) {
		if (! argv[i]) {
			if (i != argc)
echoError:			snprintf (interp->result, TCL_RESULT_SIZE,
					"argument list wasn't properly NULL-terminated in \"%s\" command",
					argv[0]);
			break;
		}
		if (i >= argc)
			goto echoError;

		if (i > 1)
			putchar (stream, ' ');
		puts (stream, (char*) argv[i]);
	}
	putchar (stream, '\n');
	return TCL_OK;
}

/*
 * Reset the device.
 */
static int
reboot_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;
	int x;

	puts (stream, "Rebooting...\n\n");
	fflush (stream);
	timer_delay (&timer, 200);

	/* Light LINK led. */
	control_link_led (1);

	arm_intr_disable (&x);
	for (;;)
		continue;
	return 0;
}

static int
mem_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;
	int n;

	putchar (stream, '\n');
	printf (stream, "Free memory: %ld bytes\n", mem_available (&pool));

	putchar (stream, '\n');
	task_print (stream, 0);
	task_print (stream, (task_t*) stack_console);
	task_print (stream, (task_t*) stack_poll);
	task_print (stream, (task_t*) stack_telnet);
	for (n=0; n<MAXSESS; ++n)
		if (streamtab[n])
			task_print (stream, (task_t*) tasktab[n]);
	task_print (stream, (task_t*) ip->stack);
	task_print (stream, (task_t*) eth->rstack);
	task_print (stream, (task_t*) eth->tstack);
	task_print (stream, (task_t*) hdlc->rstack);
	task_print (stream, (task_t*) hdlc->tstack);
	task_print (stream, (task_t*) uart.rstack);

	putchar (stream, '\n');
	return TCL_OK;
}

static int
eth_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;
	int speed, full_duplex;

	putchar (stream, '\n');
	if (argv[1] && strcmp (argv[1], (unsigned char*) "hw") == 0) {
		/* Print Ethernet hardware registers. */
		puts (stream, "Ethernet hardware registers:\n");
		eth_debug (eth, stream);
	} else {
		if (eth_get_carrier (eth)) {
			speed = eth_get_speed (eth, &full_duplex);
			printf (stream, "Ethernet: %d Mbit/sec, %s Duplex\n",
				speed / 1000000, full_duplex ? "Full" : "Half");
		} else
			printf (stream, "Ethernet: No cable\n");

		putchar (stream, '\n');
		printf (stream, "   Rx packets: %n\n", eth->netif.in_packets);
		printf (stream, "   Tx packets: %n\n", eth->netif.out_packets);
		printf (stream, "    Rx errors: %n\n", eth->netif.in_errors);
		printf (stream, "    Tx errors: %n\n", eth->netif.out_errors);
		printf (stream, "  Rx discards: %n\n", eth->netif.in_discards);
		printf (stream, "Tx collisions: %n\n", eth->netif.out_collisions);

		putchar (stream, '\n');
		printf (stream, "Rx interrupts: %n\n", eth->rintr);
		printf (stream, "Tx interrupts: %n\n", eth->tintr);
		printf (stream, "    Underruns: %n\n", eth->underrun);
		printf (stream, "     Overruns: %n\n", eth->overrun);
		printf (stream, " Frame errors: %n\n", eth->frame);
		printf (stream, "   CRC errors: %n\n", eth->crc);
	}
	putchar (stream, '\n');
	return TCL_OK;
}

static int
serial_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;

	putchar (stream, '\n');
	if (argv[1] && strcmp (argv[1], (unsigned char*) "hw") == 0) {
		/* Print HDLC hardware registers. */
		puts (stream, "Serial hardware registers (HDLC):\n");
		printf (stream, "tn=%d, te=%d, data[tn]=%08x, st[tn]=%04x\n",
			hdlc->tn, hdlc->te,
			hdlc->tdesc[hdlc->tn].data,
			hdlc->tdesc[hdlc->tn].status);
		printf (stream, "rn=%d, data[rn]=%08x, st[rn]=%04x\n",
			hdlc->rn,
			hdlc->rdesc[hdlc->rn].data,
			hdlc->rdesc[hdlc->rn].status);
		printf (stream, "HSTAT=%b\n",
			ARM_HSTAT(0), ARM_HSTAT_BITS);
		printf (stream, "HCON=%b\n",
			ARM_HCON(0), ARM_HCON_BITS);

	} else if (argv[1] && argv[2] && strcmp (argv[1], (unsigned char*) "dtr") == 0) {
		if (strcmp (argv[2], (unsigned char*) "off") == 0) {
			hdlc_set_dtr (hdlc, 0);
			puts (stream, "Set DTR = OFF\n");
		} else {
			hdlc_set_dtr (hdlc, 1);
			puts (stream, "Set DTR = ON\n");
		}

	} else if (argv[1] && argv[2] && strcmp (argv[1], (unsigned char*) "rts") == 0) {
		if (strcmp (argv[2], (unsigned char*) "off") == 0) {
			hdlc_set_rts (hdlc, 0);
			puts (stream, "Set RTS = OFF\n");
		} else {
			hdlc_set_rts (hdlc, 1);
			puts (stream, "Set RTS = ON\n");
		}
	} else {
		printf (stream, "Serial: %sDTR, %sRTS, %sCTS, %sDCD\n",
			hdlc_get_dtr (hdlc) ? "" : "no ",
			hdlc_get_rts (hdlc) ? "" : "no ",
			hdlc_get_cts (hdlc) ? "" : "no ",
			hdlc_get_dcd (hdlc) ? "" : "no ");

		putchar (stream, '\n');
		printf (stream, "   Rx packets: %n\n", hdlc->netif.in_packets);
		printf (stream, "   Tx packets: %n\n", hdlc->netif.out_packets);
		printf (stream, "    Rx errors: %n\n", hdlc->netif.in_errors);
		printf (stream, "    Tx errors: %n\n", hdlc->netif.out_errors);
		printf (stream, "  Rx discards: %n\n", hdlc->netif.in_discards);

		putchar (stream, '\n');
		printf (stream, "Rx interrupts: %n\n", hdlc->rintr);
		printf (stream, "Tx interrupts: %n\n", hdlc->tintr);
		printf (stream, "    Underruns: %n\n", hdlc->underrun);
		printf (stream, "     Overruns: %n\n", hdlc->overrun);
		printf (stream, " Frame errors: %n\n", hdlc->frame);
		printf (stream, "   CRC errors: %n\n", hdlc->crc);
	}
	putchar (stream, '\n');
	return TCL_OK;
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

static int
net_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;

	putchar (stream, '\n');
	if (argv[1] && strcmp (argv[1], (unsigned char*) "arp") == 0) {
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
	} else if (argv[1] && strcmp (argv[1], (unsigned char*) "ip") == 0) {
		puts (stream, "IP statistics\n");

		puts (stream, "\nIP input:\n");
		printf (stream, "    Received packets: %ln\n", ip->in_receives);
		printf (stream, "       Header errors: %ln\n", ip->in_hdr_errors);
		printf (stream, "      Address errors: %ln\n", ip->in_addr_errors);
		printf (stream, "   Discarded packets: %ln\n", ip->in_discards);
		printf (stream, "      Unknown protos: %ln\n", ip->in_unknown_protos);
		printf (stream, "   Delivered packets: %ln\n", ip->in_delivers);

		puts (stream, "\nIP output:\n");
		printf (stream, "     Output requests: %ln\n", ip->out_requests);
		printf (stream, "   Discarded packets: %ln\n", ip->out_discards);
		printf (stream, "      Unknown routes: %ln\n", ip->out_no_routes);
		printf (stream, "   Forwarded packets: %ln\n", ip->forw_datagrams);

	} else if (argv[1] && strcmp (argv[1], (unsigned char*) "icmp") == 0) {
		puts (stream, "ICMP statistics\n");

		puts (stream, "\nICMP input:\n");
		printf (stream, "    Input packets: %ln\n", ip->icmp_in_msgs);
		printf (stream, "  Errored packets: %ln\n", ip->icmp_in_errors);
		printf (stream, "     Echo queries: %ln\n", ip->icmp_in_echos);

		puts (stream, "\nICMP output:\n");
		printf (stream, "   Output packets: %ln\n", ip->icmp_out_msgs);
		printf (stream, "    Output errors: %ln\n", ip->icmp_out_errors);
		printf (stream, "Dest.unreachables: %ln\n", ip->icmp_out_dest_unreachs);
		printf (stream, "     Time exceeds: %ln\n", ip->icmp_out_time_excds);
		printf (stream, "     Echo replies: %ln\n", ip->icmp_out_echo_reps);

	} else if (argv[1] && strcmp (argv[1], (unsigned char*) "tcp") == 0) {
		puts (stream, "TCP statistics\n\n");

		printf (stream, "    Input packets: %ln\n", ip->tcp_in_datagrams);
		printf (stream, "  Errored packets: %ln\n", ip->tcp_in_errors);
		printf (stream, "Discarded packets: %ln\n", ip->tcp_in_discards);
		printf (stream, "   Output packets: %ln\n", ip->tcp_out_datagrams);
		printf (stream, "    Output errors: %ln\n", ip->tcp_out_errors);

	} else {
		tcp_socket_t *s;

		puts (stream, "TCP sockets\n\n");
		puts (stream, "Local address   Port    Peer address    Port    State\n");
		for (s=ip->tcp_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
		for (s=ip->tcp_closing_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
		for (s=ip->tcp_listen_sockets; s; s=s->next)
			print_tcp_socket (stream, s);
	}
	putchar (stream, '\n');
	return TCL_OK;
}

static int
help_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;

	puts (stream, "Available commands:\n");
	puts (stream, "    loop var first limit [incr] command\n");
	puts (stream, "    echo [param...]\n");
	puts (stream, "    reboot\n");
	puts (stream, "    mem\n");
	puts (stream, "    net [ip | tcp | icmp | arp]\n");
	puts (stream, "    eth [hw]\n");
	puts (stream, "    serial [hw | dtr on | dtr off | rts on | rts off]\n");
	return TCL_OK;
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
		if (c == '\b') {
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

void tcl_main (void *arg)
{
	stream_t *stream = (stream_t*) arg;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;
	unsigned char line [200], *cmd;
	int result, got_partial, quit_flag, n;

	/* Give telnet some time to negotiate. */
	timer_delay (&timer, 500);
	fflush (stream);

	puts (stream, "\n\nEmbedded TCL\n");
	puts (stream, "~~~~~~~~~~~~\n");
	printf (stream, "Session: #%d\n", reset_counter);
	printf (stream, "Free memory: %ld bytes\n", mem_available (&pool));
	puts (stream, "\nEnter \"help\" for a list of commands\n\n");

	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, (unsigned char*) "loop", loop_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "echo", echo_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "help", help_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "reboot", reboot_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "mem", mem_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "eth", eth_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "serial", serial_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "net", net_cmd, stream, 0);

	buffer = Tcl_CreateCmdBuf (&pool);
	got_partial = 0;
	quit_flag = 0;
	while (! quit_flag) {
		/*clearerr (stream);*/
		if (! got_partial) {
			puts (stream, "% ");
		}
		if (! getline (stream, line, sizeof (line))) {
			if (! got_partial)
				break;

			line[0] = 0;
		}
		cmd = Tcl_AssembleCmd (buffer, line);
		if (! cmd) {
			got_partial = 1;
			continue;
		}

		got_partial = 0;
		result = Tcl_Eval (interp, cmd, 0, 0);

		if (result != TCL_OK) {
			puts (stream, "Error");

			if (result != TCL_ERROR)
				printf (stream, " %d", result);

			if (*interp->result != 0)
				printf (stream, ": %s", interp->result);

			putchar (stream, '\n');
			continue;
		}

		if (*interp->result != 0)
			printf (stream, "%s\n", interp->result);
	}

	Tcl_DeleteInterp (interp);
	Tcl_DeleteCmdBuf (buffer);

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
	tasktab[n] = task_create (tcl_main, stream, "tcl", PRIO_TCL, t, TASKSZ);
}

/*
 * Task of timer polling.
 */
static void
main_poll (void *arg)
{
	for (;;) {
		timer_delay (&timer, 200);
		watchdog_alive ();
	}
}

void
main_console (void *data)
{
	/* Read parameters from NVRAM. */
	nvram_init ();
	reset_counter = nvram_read16 (0);

	/* Increment reset counter. */
	++reset_counter;

	/* Write reset counter to NVRAM. */
	nvram_unprotect (&timer);
	nvram_write16 (0, reset_counter);
	nvram_protect (&timer);

	for (;;)
		tcl_main ((stream_t*) &uart);
}

void main_telnet (void *data)
{
	tcp_socket_t *lsock, *sock;
	unsigned short serv_port = 23;

	lsock = tcp_listen (ip, 0, serv_port);
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

/*
 * Check memory address.
 * Board-dependent function.
 */
bool_t uos_valid_memory_address (void *ptr)
{
        unsigned u = (unsigned) ptr;

        if (u >= ARM_SRAM_BASE && u < ARM_SRAM_BASE + 0x1000)
                return 1;
        if (u >= RAM_START && u < RAM_START + RAM_SIZE)
                return 1;
        return 0;
}

void uos_init (void)
{
	mutex_group_t *g;

	/* Baud 9600. */
	uart_init (&uart, 0, PRIO_UART, KHZ, 9600);
	timer_init (&timer, KHZ, 50);
	watchdog_alive ();

	/* Configure control pins. */
	control_link_led (0);
        gpio_config (PIN_DCD, 1);               /* P0 - output */
        gpio_config (PIN_CTS, 1);		/* P4 - output */
        gpio_config (PIN_LINK, 1);              /* P16 - output */
        gpio_config (PIN_DSR, 1);		/* P21 - output */

	configure_ram (RAM_START, RAM_START + RAM_SIZE, REFRESH_USEC, IO_START);
	mem_init (&pool, RAM_START, RAM_START + RAM_SIZE);
	watchdog_alive ();

	/* Configure Ethernet. */
	eth = mem_alloc (&pool, sizeof (eth_t));
	if (! eth) {
		debug_printf ("No memory for eth_t\n");
		uos_halt (1);
	}

	/*
	 * Create a group of two locks: timer and eth.
	 */
	g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &eth->netif.lock);
	mutex_group_add (g, &timer.decisec);
	ip = mem_alloc (&pool, sizeof (*ip));
	if (! ip) {
		debug_printf ("No memory for ip_t\n");
		uos_halt (1);
	}
	arp = arp_init (arp_data, sizeof(arp_data), ip);
	ip_init (ip, &pool, PRIO_IP, &timer, arp, g);

	/*
	 * Create interface eth0 144.206.181.188 / 255.255.255.0
	 */
	eth_init (eth, "eth0", PRIO_ETH_RX, PRIO_ETH_TX, &pool, arp);
	route_add_netif (ip, &route, (unsigned char*) "\220\316\265\274",
		24, &eth->netif);

	/* Configure HDLC. */
	hdlc = mem_alloc (&pool, sizeof (hdlc_t));
	if (! hdlc) {
		debug_printf ("No memory for hdlc_t\n");
		uos_halt (1);
	}
	hdlc_init (hdlc, 0, "hdlc0", PRIO_HDLC_RX, PRIO_HDLC_TX,
		&pool, KHZ * 1000);
	hdlc->callback_receive = receive_hdlc;

	stack_console = mem_alloc (&pool, CONSOLE_STACKSZ);
	assert (stack_console != 0);
	task_create (main_console, 0, "console", PRIO_CONSOLE,
		stack_console, CONSOLE_STACKSZ);

	stack_poll = mem_alloc (&pool, POLL_STACKSZ);
	assert (stack_poll != 0);
	task_create (main_poll, 0, "poll", PRIO_POLL,
		stack_poll, POLL_STACKSZ);

	stack_telnet = mem_alloc (&pool, TELNET_STACKSZ);
	assert (stack_telnet != 0);
	task_create (main_telnet, 0, "telnet", PRIO_TELNET,
		stack_telnet, TELNET_STACKSZ);
}
