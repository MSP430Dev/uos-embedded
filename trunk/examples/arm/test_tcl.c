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
 * $Id: test_tcl.c,v 1.5 2007-04-23 14:50:01 vak Exp $
 */
#include <runtime/lib.h>
#include <mem/mem.h>
#include <stream/stream.h>

#include <tcl/tcl.h>

#define RAM_START	0x02000000
#define RAM_SIZE	(2*1024*1024)
#define RAM_END		(RAM_START+RAM_SIZE)
#define REFRESH_USEC	8
#define IO_START	0x03600000

char task [0xa00];
char line [200];
mem_pool_t pool;

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
		snprintf (itxt, sizeof(itxt), "%d", i);
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

				snprintf (buf, sizeof(buf),
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
	snprintf (itxt, sizeof(itxt), "%d", i);
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
echoError:			snprintf (interp->result, TCL_RESULT_SIZE,
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
		if (c == '\b') {
			if (s > buf) {
				--s;
				debug_puts ("\b \b");
			}
			continue;
		}
		if (c == '\r')
			c = '\n';
		debug_putchar (0, c);
		*s++ = c;
		if (c == '\n')
			break;
	}
	*s = '\0';
	return buf;
}

void configure_ram (void)
{
	arm_memory_regs_t reg;

	arm_get_memory_regs (&reg);

	/* DRAM bank 0: 16-bit bus width. */
	reg.extdbwth |= ARM_EXTDBWTH_16BIT << ARM_EXTDBWTH_DSD0_shift;

	/* DRAM bank 0 address, size and timings. */
	reg.dramcon0 = ARM_DRAMCON_BASE (RAM_START) |
		ARM_DRAMCON_NEXT (RAM_END) |
		ARM_DRAMCON_CAN_8B |	/* column address = 8-bit */
		ARM_DRAMCON_TCS_1C |	/* CAS strobe time = 1 cycle */
					/* CAS pre-charge time = 1 cycle */
		ARM_DRAMCON_TRC_2C | 	/* RAS to CAS delay = 2 cycles */
		ARM_DRAMCON_TRP_4C;	/* RAS pre-charge time = 4 cycles */

	/* Setup DRAM refresh cycle = 8 usec. */
	reg.refextcon = ARM_REFEXTCON_BASE (IO_START) |
		ARM_REFEXTCON_VSF |	/* validity of special regs */
		ARM_REFEXTCON_REN |	/* refresh enable */
		ARM_REFEXTCON_TCHR_4C |	/* CAS hold time = 4 cycles */
					/* CAS setup time = 1 cycle */
		ARM_REFEXTCON_RFR (REFRESH_USEC, KHZ);

	/* Disable write buffer and cache. */
	ARM_SYSCFG &= ~(ARM_SYSCFG_WE | ARM_SYSCFG_CE);

	/* Sync DRAM mode. */
	ARM_SYSCFG |= ARM_SYSCFG_SDM;

	/* Set memory configuration registers, all at once. */
	arm_set_memory_regs (&reg);

	/* Enable write buffer and cache. */
	ARM_SYSCFG |= ARM_SYSCFG_WE | ARM_SYSCFG_CE;
}

void task_tcl (void *arg)
{
	char *cmd;
	unsigned char result, got_partial, quit_flag;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;

	configure_ram ();
	mem_init (&pool, (mem_size_t) RAM_START, (mem_size_t) RAM_END);
again:
	debug_printf ("\nEmbedded TCL\n\n");

	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, "loop", loop_cmd, 0, 0);
	Tcl_CreateCommand (interp, "echo", echo_cmd, 0, 0);

	buffer = Tcl_CreateCmdBuf (&pool);
	got_partial = 0;
	quit_flag = 0;
	while (! quit_flag) {
/*		clearerr (stdin);*/
		if (! got_partial) {
			debug_puts ("% ");
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
	goto again;
}

void uos_init (void)
{
	/* Baud 9600 at 50/2 MHz. */
	ARM_UCON(0) = ARM_UCON_WL_8 | ARM_UCON_TMODE_IRQ;
	ARM_UBRDIV(0) = ((KHZ * 500L / 9600 + 8) / 16 - 1) << 4;

	task_create (task_tcl, 0, "tcl", 1, task, sizeof (task));
}
