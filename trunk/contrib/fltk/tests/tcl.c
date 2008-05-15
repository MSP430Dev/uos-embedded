/*
 * Test driver for TCL.
 *
 * Copyright (C) 2000-2005 Serge Vakulenko, <vak@cronyx.ru>
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
#include <runtime/i386/int86.h>
#include <mem/mem.h>
#include <stream/stream.h>
#include <stream/pipe.h>
#include <tcl/tcl.h>
#include <vesa/vesa.h>

extern mem_pool_t *uos_memory;

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
reboot_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;

	puts (stream, "Rebooting...\n");
	fflush (stream);
	i386_reboot (0x1234);
	return TCL_OK;
}

	/* ARGSUSED */
static int
drives_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;
	int drive, heads, sectors, cylinders;
	int86_regs_t reg;
	int drivenum [6] = {0, 1, 0x80, 0x81, 0x82, 0x83 };

	/* Get the geometry of the drives */
	for (drive=0; drive<6; drive++) {
		/* Get drive parameters. */
		memset (&reg, 0, sizeof (reg));
		reg.x.ax = 0x0800;
		reg.x.dx = drivenum [drive];
		int86 (0x13, &reg, &reg);

		if ((reg.x.flags & 0x0001) || reg.x.cx == 0)
			continue;

		heads = (reg.x.dx >> 8) + 1;
		sectors = reg.x.cx & 0x3F;
		cylinders = ((reg.x.cx >> 8) | ((reg.x.cx & 0xC0) << 2)) + 1;

		if (drive < 2)
			printf (stream, "fd%d: ", drive);
		else
			printf (stream, "hd%d: ", drive-2);

		printf (stream, "%d cylinders, %d heads, %d sectors per track\n",
			cylinders, heads, sectors);
	}
	return TCL_OK;
}

	/* ARGSUSED */
static int
vesa_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;
	vesa_info_t info;
	vesa_mode_info_t mi;
	unsigned short *mode;
	int bpp, n;

	if (! vesa_get_info (&info)) {
		printf (stream, "No VESA found.\n");
		return TCL_OK;
	}
	if (argv[1]) {
		/* List direct color graphics modes
		 * with linear frame buffer and given bpp. */
		bpp = strtol (argv[1], 0, 0);
		n = 0;
		for (mode=info.modes; *mode!=0xffff; ++mode) {
			if (! vesa_get_mode_info (*mode, &mi))
				continue;

			if (mi.memory_model != 6 ||	/* direct color */
			    ! (mi.mode_attr & 0x01) ||	/* supported */
			    ! (mi.mode_attr & 0x08) ||	/* color */
			    ! (mi.mode_attr & 0x10) ||	/* graphics */
			    ! (mi.mode_attr & 0x80) ||	/* linear */
			    mi.bits_per_pixel != bpp)
				continue;
			if (n == 0)
				printf (stream, "Direct color modes with %d bits per pixel:\n",
					bpp);
			printf (stream, "%x - ", *mode);
			printf (stream, "%4d x %-4d - %d bits/pixel %d:%d:%d",
				mi.width, mi.height, mi.bits_per_pixel,
				mi.red_mask_size, mi.green_mask_size,
				mi.blue_mask_size);
			if (mi.direct_color_info & 0x02)
				printf (stream, ":%d", mi.rsvd_mask_size);
			if (mi.phys_base_ptr)
				printf (stream, ", memory at 0x%lx", mi.phys_base_ptr);
			printf (stream, "\n");
			++n;
		}
		return TCL_OK;
	}
	printf (stream, "Signature = `%c%c%c%c'\n",
		info.signature[0], info.signature[1],
		info.signature[2], info.signature[3]);
	printf (stream, "Version = %d.%d\n",
		info.version >> 8, info.version & 255);
	if (info.oem_string)
		printf (stream, "OEM = %s\n", info.oem_string);
	if (info.vendor_name)
		printf (stream, "Vendor = %s\n", info.vendor_name);
	if (info.product_name)
		printf (stream, "Product Name = %s\n", info.product_name);
	if (info.product_rev)
		printf (stream, "Product Revision = %s\n", info.product_rev);
	printf (stream, "Revision = %d.%d\n",
		info.revision >> 8, info.revision & 255);
	printf (stream, "Capabilities = 0x%lx\n", info.capabilities);
	printf (stream, "Memory = %ldk\n", info.memory * 64L);
	return TCL_OK;
}

static void
display_print_mode (stream_t *stream, vesa_mode_t *m)
{
	if (! m->xres)
		return;
	printf (stream, "      %d x %d at %d Hz\n",
		m->xres, m->yres, m->refresh);
}

static void
display_print_extmode (stream_t *stream, vesa_extmode_t *m)
{
	if (! m->xres)
		return;

	if (! m->khz) {
		display_print_mode (stream, (vesa_mode_t*) m);
		return;
	}
	printf (stream, "      %d x %d at %d Hz, size %d x %d\n",
		m->xres, m->yres, m->refresh, m->xsize, m->ysize);
	printf (stream, "      %d MHz ", (int) (m->khz / 1000));
	printf (stream, "%d %d %d %d ", m->xres, m->xres + m->right_margin,
		m->xres + m->right_margin + m->hsync_len,
		m->xres + m->xblank);
	printf (stream, "%d %d %d %d ", m->yres, m->yres + m->lower_margin,
		m->yres + m->lower_margin + m->vsync_len,
		m->yres + m->yblank);
	printf (stream, "%sHSync %sVSync\n",
		m->hsync_positive ? "+" : "-",
		m->vsync_positive ? "+" : "-");
}

	/* ARGSUSED */
static int
display_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;
	vesa_display_t display;
	int i;

	if (! vesa_get_display_info (&display)) {
		printf (stream, "No display data available.\n");
		return TCL_OK;
	}
	if (argv[1]) {
		/* List supported display modes. */
		printf (stream, "Supported VESA Modes\n");
		for (i=0; i<17; ++i)
			display_print_mode (stream, display.vesa_modes + i);

		printf (stream, "Standard Timings\n");
		for (i=0; i<8; ++i)
			display_print_mode (stream, display.std_modes + i);

		printf (stream, "Detailed Timings\n");
		for (i=0; i<24; ++i)
			display_print_extmode (stream,
				display.detailed_modes + i);
		return TCL_OK;
	}
	printf (stream, "EDID Version %d.%d\n", (int) display.version,
	       (int) display.revision);
	printf (stream, "Manufacturer: %s\n", display.manufacturer);
	printf (stream, "Model: %x\n", display.model);
	printf (stream, "Serial#: %u\n", display.serial);
	printf (stream, "Year: %u Week %u\n", display.year, display.week);

	if (display.monitor[0])
		printf (stream, "Monitor Name: %s\n", display.monitor);
	if (display.serial_no[0])
		printf (stream, "Serial Number: %s\n", display.serial_no);
	if (display.ascii[0])
		printf (stream, "ASCII Block: %s\n", display.ascii);

	printf (stream, "Display Characteristics:\n");
	if (display.vfmax != 0) {
		printf (stream, "   Monitor Operating Limits: ");
		printf (stream, "H: %d-%d KHz  V: %d-%d Hz  DCLK: %d MHz\n",
			(int) (display.hfmin / 1000),
			(int) (display.hfmax / 1000),
			display.vfmin, display.vfmax,
			(int) (display.dclkmax / 1000000));
	}
	if (display.input & VESA_DISP_DDI) {
		printf (stream, "   Digital Display Input");
	} else {
		printf (stream, "   Analog Display Input: Input Voltage - ");
		if (display.input & VESA_DISP_ANA_700_300)
			printf (stream, "0.700V/0.300V");
		else if (display.input & VESA_DISP_ANA_714_286)
			printf (stream, "0.714V/0.286V");
		else if (display.input & VESA_DISP_ANA_1000_400)
			printf (stream, "1.000V/0.400V");
		else if (display.input & VESA_DISP_ANA_700_000)
			printf (stream, "0.700V/0.000V");
		else
			printf (stream, "???");
		printf (stream, "\n");
	}

	printf (stream, "   Sync: ");
	if (display.signal & VESA_SIGNAL_BLANK_BLANK)
		printf (stream, "Blank to Blank ");
	if (display.signal & VESA_SIGNAL_SEPARATE)
		printf (stream, "Separate ");
	if (display.signal & VESA_SIGNAL_COMPOSITE)
		printf (stream, "Composite ");
	if (display.signal & VESA_SIGNAL_SYNC_ON_GREEN)
		printf (stream, "Sync on Green ");
	if (display.signal & VESA_SIGNAL_SERRATION_ON)
		printf (stream, "Serration on ");
	printf (stream, "\n");

	if (display.max_x && display.max_y)
		printf (stream, "   Max. Image Size: %d x %d centimeters\n",
			display.max_x, display.max_y);

	printf (stream, "   Gamma: %d.%d\n", display.gamma/100, display.gamma % 100);

	printf (stream, "   DPMS: Active %s, Suspend %s, Standby %s\n",
	       (display.dpms & VESA_DPMS_ACTIVE_OFF) ? "yes" : "no",
	       (display.dpms & VESA_DPMS_SUSPEND)    ? "yes" : "no",
	       (display.dpms & VESA_DPMS_STANDBY)    ? "yes" : "no");

	if (display.input & VESA_DISP_MONO)
		printf (stream, "   Monochrome/Grayscale\n");
	else if (display.input & VESA_DISP_RGB)
		printf (stream, "   RGB Color Display\n");
	else if (display.input & VESA_DISP_MULTI)
		printf (stream, "   Non-RGB Multicolor Display\n");
	else if (display.input & VESA_DISP_UNKNOWN)
		printf (stream, "   Unknown\n");

	printf (stream, "   Chroma\n");
	printf (stream, "      Red: 0.%03d x 0.%03d\n",
		display.chroma_redx, display.chroma_redy);
	printf (stream, "      Green: 0.%03d x 0.%03d\n",
		display.chroma_greenx, display.chroma_greeny);
	printf (stream, "      Blue: 0.%03d x 0.%03d\n",
		display.chroma_bluex, display.chroma_bluey);
	printf (stream, "      White: 0.%03d x 0.%03d\n",
		display.chroma_whitex, display.chroma_whitey);

        if (display.misc & VESA_MISC_PRIM_COLOR)
                printf (stream, "   Default color format is primary\n");
        if (display.misc & VESA_MISC_1ST_DETAIL)
                printf (stream, "   First DETAILED Timing is preferred\n");
        if (display.gtf)
                printf (stream, "   Display is GTF capable\n");
	return TCL_OK;
}

	/* ARGSUSED */
static int
mem_cmd (void *arg, Tcl_Interp *interp, int argc, char **argv)
{
	stream_t *stream = arg;

	printf (stream, "Free memory: %ld kbytes\n",
		mem_available (uos_memory) / 1024);
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
	puts (stream, "    reboot\n");
	puts (stream, "    drives\n");
	puts (stream, "    vesa [15 | 16 | 24 | 32]\n");
	puts (stream, "    display [modes]\n");
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
				putchar (stream, c);
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

void nxmain (void *arg)
{
	char pipe_buf [sizeof(pipe_t) + 400];
	pipe_t *pipe;
	task_t *output;
	char *task;
	int tasksz = 0x10000;
	stream_t *master, *stream;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;
	char line [200], *cmd;
	unsigned char result, got_partial, quit_flag;
	extern void output_main (void *arg);

	task = mem_alloc (uos_memory, tasksz);
	if (! task) {
		debug_printf ("tcl: no memory for task\n");
		task_exit (0);
	}
	pipe = pipe_init (pipe_buf, sizeof (pipe_buf), &master, &stream);
	output = task_create (output_main, pipe, "output", 9, task, tasksz);

	printf (stream, "Embedded TCL\n\n");
	printf (stream, "Enter \"help\" for a list of commands\n\n");

	interp = Tcl_CreateInterp (uos_memory);
	Tcl_CreateCommand (interp, "loop", loop_cmd, stream, 0);
	Tcl_CreateCommand (interp, "echo", echo_cmd, stream, 0);
	Tcl_CreateCommand (interp, "help", help_cmd, stream, 0);
	Tcl_CreateCommand (interp, "reboot", reboot_cmd, stream, 0);
	Tcl_CreateCommand (interp, "drives", drives_cmd, stream, 0);
	Tcl_CreateCommand (interp, "vesa", vesa_cmd, stream, 0);
	Tcl_CreateCommand (interp, "display", display_cmd, stream, 0);
	Tcl_CreateCommand (interp, "mem", mem_cmd, stream, 0);

	buffer = Tcl_CreateCmdBuf (uos_memory);
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

	task_wait (output);
	mem_free (task);
	task_exit (0);
}
