/*
 * Embedded TCL on Elvees MC-24EM board.
 *
 * Copyright (C) 2009 Serge Vakulenko, <vak@cronyx.ru>
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <timer/timer.h>
#include <uart/uart.h>
#include <tcl/tcl.h>

/*
 * Priorities for tasks.
 */
#define PRIO_CONSOLE	20
#define PRIO_UART	90

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x00000000
#else
#   define SDRAM_START	0xA0000000
#endif
#define SDRAM_SIZE	(64*1024*1024)

uart_t uart;				/* Console driver */
timer_t timer;				/* Timer driver */
mem_pool_t pool;			/* Dynamic memory pool */
ARRAY (stack_console, 2000);		/* Task: console menu */

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

static int
mem_cmd (void *arg, Tcl_Interp *interp, int argc, unsigned char **argv)
{
	stream_t *stream = arg;

	putchar (stream, '\n');
	task_print (stream, 0);
	task_print (stream, (task_t*) stack_console);
	task_print (stream, (task_t*) uart.rstack);

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
	puts (stream, "    mem\n");
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
	int result, got_partial, quit_flag;

	/* Give telnet some time to negotiate. */
	timer_delay (&timer, 500);
	fflush (stream);

	puts (stream, "\n\nEmbedded TCL\n");
	puts (stream, "~~~~~~~~~~~~\n");
	printf (stream, "Free memory: %ld bytes\n", mem_available (&pool));
	puts (stream, "\nEnter \"help\" for a list of commands\n\n");

	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, (unsigned char*) "loop", loop_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "echo", echo_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "help", help_cmd, stream, 0);
	Tcl_CreateCommand (interp, (unsigned char*) "mem", mem_cmd, stream, 0);

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
}

void
main_console (void *data)
{
	for (;;)
		tcl_main ((stream_t*) &uart);
}

/*
 * Check memory address.
 * Board-dependent function.
 */
bool_t uos_valid_memory_address (void *ptr)
{
        unsigned address = (unsigned) ptr;
	extern unsigned __data_start[], _estack[];

	/* Internal SRAM. */
	if (address >= (unsigned) __data_start &&
	    address < (unsigned) _estack)
		return 1;

	/* SDRAM. */
	if (address >= SDRAM_START && address < SDRAM_START + SDRAM_SIZE)
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

	/* Baud 115200. */
	uart_init (&uart, 0, PRIO_UART, KHZ, 115200);
	timer_init (&timer, KHZ, 50);

	task_create (main_console, 0, "console", PRIO_CONSOLE,
		stack_console, sizeof(stack_console));
}
