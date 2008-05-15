/*
 * Test for Telnet protocol, using TCL as a command interpreter.
 *
 * Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <mem/mem.h>
#include <buf/buf.h>
#include <stream/stream.h>
#include <tcl/tcl.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/telnet.h>
#include <timer/timer.h>
#include <tap/tap.h>

#define MEM_SIZE	100000
#define TASKSZ		10000
#define MAXSESS		4

task_t *tasktab [MAXSESS];
tcp_socket_t *socktab [MAXSESS];

char task [6000];
char memory [MEM_SIZE];
char group [sizeof(lock_group_t) + 4 * sizeof(lock_slot_t)];
mem_pool_t pool;
tap_t tap;
route_t route;
timer_t timer;
ip_t ip;

void tcl_main (void *arg);

/*
 * Tcl_LoopCmd --
 *	Implements the TCL loop command:
 *		loop var start end [increment] command
 * Results:
 *	Standard TCL results.
 */
static int
loop_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	int result = TCL_OK;
	int i, first, limit, incr = 1;
	char *command;
	char itxt [12];

	if ((argc < 5) || (argc > 6)) {
		Tcl_AppendResult (interp, "bad # args: ", argv [0],
			" var first limit [incr] command", (char*) 0);
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
				char buf [64];

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

	/* ARGSUSED */
static int
echo_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
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
		puts (stream, argv[i]);
	}
	putchar (stream, '\n');
	return TCL_OK;
}

	/* ARGSUSED */
static int
mem_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;

	printf (stream, "Free memory: %ld kbytes\n",
		mem_available (&pool) / 1024);
	return TCL_OK;
}

	/* ARGSUSED */
static int
help_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;

	puts (stream, "Available commands:\n");
	puts (stream, "    loop var first limit [incr] command\n");
	puts (stream, "    echo [param...]\n");
	puts (stream, "    mem\n");
	return TCL_OK;
}

/*
 * Read a newline-terminated string from stream.
 */
static char *
getline (stream_t *stream, char *buf, int len)
{
	int c;
	char *s;

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
	tcp_socket_t *sock;
	stream_t *stream;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;
	char line [200], *cmd;
	unsigned char result, got_partial, quit_flag, n;

	sock = (tcp_socket_t*) arg;
	stream = telnet_init (sock);
	if (! stream) {
		debug_printf ("Error initializing telnet\n");
		task_exit (0);
		return;
	}

	printf (stream, "Embedded TCL\n\n");
	printf (stream, "Enter \"help\" for a list of commands\n\n");

	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, "loop", loop_cmd, stream, 0);
	Tcl_CreateCommand (interp, "echo", echo_cmd, stream, 0);
	Tcl_CreateCommand (interp, "help", help_cmd, stream, 0);
	Tcl_CreateCommand (interp, "mem", mem_cmd, stream, 0);
	/* TODO: "tasks" command. */
	/* TODO: "netstat" command. */

	buffer = Tcl_CreateCmdBuf (&pool);
	got_partial = 0;
	quit_flag = 0;
	while (! quit_flag) {
/*		clearerr (stdin);*/
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

	fclose (stream);
	for (n=0; n<MAXSESS; ++n)
		if (socktab[n] == sock)
			socktab[n] = 0;
	task_exit (0);
}

void start_session (tcp_socket_t *sock)
{
	int n;
	char *t;

	for (n=0; n<MAXSESS; ++n)
		if (! socktab[n])
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
	t = mem_alloc (&pool, TASKSZ);
	if (! t) {
		debug_printf ("No memory for task\n");
		tcp_close (sock);
		return;
	}
	socktab[n] = sock;
	tasktab[n] = task_create (tcl_main, sock, "tcl", 11, t, TASKSZ);
}

void telnet_task (void *data)
{
	tcp_socket_t *lsock, *sock;
	unsigned short serv_port = 2222;

	debug_printf ("Server started on port %d\n", serv_port);
	lsock = tcp_listen (&ip, 0, serv_port);
	if (! lsock) {
		debug_printf ("Error on listen, aborted\n");
		abort();
	}
	for (;;) {
		sock = tcp_accept (lsock);
		if (! sock) {
			debug_printf ("Error on accept\n");
			break;
		}
		start_session (sock);
	}
	tcp_close (lsock);
	debug_printf ("Server finished\n");
	uos_halt();
}

void uos_init (void)
{
	lock_group_t *g;

	timer_init (&timer, 100, KHZ, 10);
	mem_init (&pool, (mem_size_t) memory, (mem_size_t) memory + MEM_SIZE);

	/*
	 * Create a group of two locks: timer and tap.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &tap.netif.lock);
	lock_group_add (g, &timer.decisec);
	ip_init (&ip, &pool, 70, &timer, 0, g);

	/*
	 * Create interface tap0 200.0.0.2 / 255.255.255.0
	 */
	tap_init (&tap, "tap0", 80, &pool, 0);
	route_add_netif (&ip, &route, "\310\0\0\2", 24, &tap.netif);

	task_create (telnet_task, 0, "telnetd", 1, task, sizeof (task));
}
