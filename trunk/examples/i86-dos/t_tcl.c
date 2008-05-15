/*
 * tclTest.c --
 *
 *	Test driver for TCL.
 *
 * Copyright 1987-1991 Regents of the University of California
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Id: t_tcl.c,v 1.1 2003-08-22 17:36:26 vak Exp $
 */
#include <runtime/lib.h>
#include <mem/mem.h>
#include <stream/stream.h>
/*#include <uart/uart.h>*/

#include <tcl/tcl.h>

#define MEM_SIZE	15000

char task [0x300];
mem_pool_t pool;
/*char memory [MEM_SIZE];*/
char line [100];
/*uart_t uart;*/

/*
 * Tcl_LoopCmd --
 *	Implements the TCL loop command:
 *		loop var start end [increment] command
 * Results:
 *	Standard TCL results.
 */
static int
loop_cmd (void *dummy, Tcl_Interp *interp, int argc, char **argv)
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
		sprintf (itxt, "%d", i);
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

				sprintf (buf, "\n    (\"loop\" body line %d)",
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
	sprintf (itxt, "%d", i);
	if (! Tcl_SetVar (interp, argv [1], itxt, TCL_LEAVE_ERR_MSG))
		return TCL_ERROR;

	return result;
}

	/* ARGSUSED */
static int
echo_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	int i;

	for (i=1; ; i++) {
		if (! argv[i]) {
			if (i != argc)
echoError:			sprintf (interp->result,
					"argument list wasn't properly NULL-terminated in \"%s\" command",
					argv[0]);
			break;
		}
		if (i >= argc)
			goto echoError;

		if (i > 1)
			debug_putchar (0, ' ');
		debug_puts (argv[i]);
	}
	debug_putchar (0, '\n');
	return TCL_OK;
}

/*
 * Read a newline-terminated string from stream.
 */
char *
debug_gets (char *buf, int len)
{
	int c;
	char *s;

	s = buf;
        while (--len > 0) {
		c = debug_getchar ();
		*s++ = c;
		if (c == '\n')
			break;
	}
	*s = '\0';
	return buf;
}

void task_tcl (void *arg)
{
	char *cmd;
	unsigned char result, got_partial, quit_flag;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;
again:
	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, "loop", loop_cmd, 0, 0);
	Tcl_CreateCommand (interp, "echo", echo_cmd, 0, 0);

	buffer = Tcl_CreateCmdBuf (&pool);
#if 0
	result = Tcl_Eval (interp, "puts stdout \"\nuOS Tcl 6.8\n\";"
		"source tcl_sys/autoinit.tcl", 0, 0);
	if (result != TCL_OK) {
		debug_printf ("%s\n", interp->result);
		exit (1);
	}
#endif
	got_partial = 0;
	quit_flag = 0;
	while (! quit_flag) {
/*		clearerr (stdin);*/
		if (! got_partial) {
			debug_puts ("% ");
/*			fflush (&uart);*/
		}
		if (! debug_gets (line, sizeof (line))) {
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
			debug_puts ("Error");

			if (result != TCL_ERROR)
				debug_printf (" %d", result);

			if (*interp->result != 0)
				debug_printf (": %s", interp->result);

			debug_putchar (0, '\n');
			continue;
		}

		if (*interp->result != 0)
			debug_printf ("%s\n", interp->result);
	}

	Tcl_DeleteInterp (interp);
	Tcl_DeleteCmdBuf (buffer);
/*	exit (0);*/
	goto again;
}

void uos_init (void)
{
/*	uart_init (&uart, 0, 90, 10000, 9600);*/
/*	mem_init (&pool, (mem_size_t) memory, (mem_size_t) memory + MEM_SIZE);*/
	task_create (task_tcl, 0, "tcl", 1, task, sizeof (task), 0);
}
