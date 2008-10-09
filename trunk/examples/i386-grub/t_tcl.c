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
 * $Id: t_tcl.c,v 1.9 2005-09-21 18:11:34 vak Exp $
 */
#include <runtime/lib.h>
#include <runtime/i386/int86.h>
#include <mem/mem.h>
#include <stream/stream.h>
#include <tcl/tcl.h>
#include <vesa/vesa.h>
#include <pci/pci.h>

#undef printf
#define printf debug_printf

ARRAY (task, 0xa00);
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

	/* ARGSUSED */
static int
reboot_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	debug_puts ("Rebooting...\n");
	i386_reboot (0x1234);
	return TCL_OK;
}

	/* ARGSUSED */
static int
drives_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
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
			printf ("fd%d: ", drive);
		else
			printf ("hd%d: ", drive-2);

		printf ("%d cylinders, %d heads, %d sectors per track\n",
			cylinders, heads, sectors);
	}
	return TCL_OK;
}

	/* ARGSUSED */
static int
vesa_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	vesa_info_t info;
	vesa_mode_info_t mi;
	unsigned short *mode;
	int bpp, n;

	if (! vesa_get_info (&info)) {
		printf ("No VESA found.\n");
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
				printf ("Direct color modes with %d bits per pixel:\n",
					bpp);
			printf ("%x - ", *mode);
			printf ("%4d x %-4d - %d bits/pixel %d:%d:%d",
				mi.width, mi.height, mi.bits_per_pixel,
				mi.red_mask_size, mi.green_mask_size,
				mi.blue_mask_size);
			if (mi.direct_color_info & 0x02)
				printf (":%d", mi.rsvd_mask_size);
			if (mi.phys_base_ptr)
				printf (", memory at 0x%lx", mi.phys_base_ptr);
			printf ("\n");
			++n;
		}
		return TCL_OK;
	}
	printf ("Signature = `%c%c%c%c'\n",
		info.signature[0], info.signature[1],
		info.signature[2], info.signature[3]);
	printf ("Version = %d.%d\n",
		info.version >> 8, info.version & 255);
	if (info.oem_string)
		printf ("OEM = %s\n", info.oem_string);
	if (info.vendor_name)
		printf ("Vendor = %s\n", info.vendor_name);
	if (info.product_name)
		printf ("Product Name = %s\n", info.product_name);
	if (info.product_rev)
		printf ("Product Revision = %s\n", info.product_rev);
	printf ("Revision = %d.%d\n",
		info.revision >> 8, info.revision & 255);
	printf ("Capabilities = 0x%lx\n", info.capabilities);
	printf ("Memory = %ldk\n", info.memory * 64L);
	return TCL_OK;
}

static void
display_print_mode (vesa_mode_t *m)
{
	if (! m->xres)
		return;
	printf ("      %d x %d at %d Hz\n",
		m->xres, m->yres, m->refresh);
}

static void
display_print_extmode (vesa_extmode_t *m)
{
	if (! m->xres)
		return;

	if (! m->khz) {
		display_print_mode ((vesa_mode_t*) m);
		return;
	}
	printf ("      %d x %d at %d Hz, size %d x %d\n",
		m->xres, m->yres, m->refresh, m->xsize, m->ysize);
	printf ("      %d MHz ", (int) (m->khz / 1000));
	printf ("%d %d %d %d ", m->xres, m->xres + m->right_margin,
		m->xres + m->right_margin + m->hsync_len,
		m->xres + m->xblank);
	printf ("%d %d %d %d ", m->yres, m->yres + m->lower_margin,
		m->yres + m->lower_margin + m->vsync_len,
		m->yres + m->yblank);
	printf ("%sHSync %sVSync\n",
		m->hsync_positive ? "+" : "-",
		m->vsync_positive ? "+" : "-");
}

	/* ARGSUSED */
static int
display_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	vesa_display_t display;
	int i;

	if (! vesa_get_display_info (&display)) {
		printf ("No display data available.\n");
		return TCL_OK;
	}
	if (argv[1]) {
		/* List supported display modes. */
		printf ("Supported VESA Modes\n");
		for (i=0; i<17; ++i)
			display_print_mode (display.vesa_modes + i);

		printf ("Standard Timings\n");
		for (i=0; i<8; ++i)
			display_print_mode (display.std_modes + i);

		printf ("Detailed Timings\n");
		for (i=0; i<24; ++i)
			display_print_extmode (display.detailed_modes + i);
		return TCL_OK;
	}
	printf ("EDID Version %d.%d\n", (int) display.version,
	       (int) display.revision);
	printf ("Manufacturer: %s\n", display.manufacturer);
	printf ("Model: %x\n", display.model);
	printf ("Serial#: %u\n", display.serial);
	printf ("Year: %u Week %u\n", display.year, display.week);

	if (display.monitor[0])
		printf ("Monitor Name: %s\n", display.monitor);
	if (display.serial_no[0])
		printf ("Serial Number: %s\n", display.serial_no);
	if (display.ascii[0])
		printf ("ASCII Block: %s\n", display.ascii);

	printf ("Display Characteristics:\n");
	if (display.vfmax != 0) {
		printf ("   Monitor Operating Limits: ");
		printf ("H: %d-%d KHz  V: %d-%d Hz  DCLK: %d MHz\n",
			(int) (display.hfmin / 1000),
			(int) (display.hfmax / 1000),
			display.vfmin, display.vfmax,
			(int) (display.dclkmax / 1000000));
	}
	if (display.input & VESA_DISP_DDI) {
		printf ("   Digital Display Input");
	} else {
		printf ("   Analog Display Input: Input Voltage - ");
		if (display.input & VESA_DISP_ANA_700_300)
			printf ("0.700V/0.300V");
		else if (display.input & VESA_DISP_ANA_714_286)
			printf ("0.714V/0.286V");
		else if (display.input & VESA_DISP_ANA_1000_400)
			printf ("1.000V/0.400V");
		else if (display.input & VESA_DISP_ANA_700_000)
			printf ("0.700V/0.000V");
		else
			printf ("???");
		printf ("\n");
	}

	printf ("   Sync: ");
	if (display.signal & VESA_SIGNAL_BLANK_BLANK)
		printf ("Blank to Blank ");
	if (display.signal & VESA_SIGNAL_SEPARATE)
		printf ("Separate ");
	if (display.signal & VESA_SIGNAL_COMPOSITE)
		printf ("Composite ");
	if (display.signal & VESA_SIGNAL_SYNC_ON_GREEN)
		printf ("Sync on Green ");
	if (display.signal & VESA_SIGNAL_SERRATION_ON)
		printf ("Serration on ");
	printf ("\n");

	if (display.max_x && display.max_y)
		printf ("   Max. Image Size: %d x %d centimeters\n",
			display.max_x, display.max_y);

	printf ("   Gamma: %d.%d\n", display.gamma/100, display.gamma % 100);

	printf ("   DPMS: Active %s, Suspend %s, Standby %s\n",
	       (display.dpms & VESA_DPMS_ACTIVE_OFF) ? "yes" : "no",
	       (display.dpms & VESA_DPMS_SUSPEND)    ? "yes" : "no",
	       (display.dpms & VESA_DPMS_STANDBY)    ? "yes" : "no");

	if (display.input & VESA_DISP_MONO)
		printf ("   Monochrome/Grayscale\n");
	else if (display.input & VESA_DISP_RGB)
		printf ("   RGB Color Display\n");
	else if (display.input & VESA_DISP_MULTI)
		printf ("   Non-RGB Multicolor Display\n");
	else if (display.input & VESA_DISP_UNKNOWN)
		printf ("   Unknown\n");

	printf ("   Chroma\n");
	printf ("      Red: 0.%03d x 0.%03d\n",
		display.chroma_redx, display.chroma_redy);
	printf ("      Green: 0.%03d x 0.%03d\n",
		display.chroma_greenx, display.chroma_greeny);
	printf ("      Blue: 0.%03d x 0.%03d\n",
		display.chroma_bluex, display.chroma_bluey);
	printf ("      White: 0.%03d x 0.%03d\n",
		display.chroma_whitex, display.chroma_whitey);

        if (display.misc & VESA_MISC_PRIM_COLOR)
                printf ("   Default color format is primary\n");
        if (display.misc & VESA_MISC_1ST_DETAIL)
                printf ("   First DETAILED Timing is preferred\n");
        if (display.gtf)
                printf ("   Display is GTF capable\n");
	return TCL_OK;
}

static void
draw_moire16 (int xres, int yres, long screen_ptr, long bytes_per_line)
{
	/* For 16 bits per pixel. */
	typedef unsigned short color_t;

	void put_pixel (int x, int y, color_t color)
	{
		((color_t*) (screen_ptr + y * bytes_per_line)) [x] = color;
	}

	void line (int x1, int y1, int x2, int y2, color_t color)
	{
		int d;
		int dx, dy;
		int e_incr, ne_incr;
		int yincr;
		int t;

		dx = x2 - x1;
		if (dx < 0)
			dx = -dx;

		dy = y2 - y1;
		if (dy < 0)
			dy = -dy;

		if (dy <= dx) {
			if (x2 < x1) {
				t = x2; x2 = x1; x1 = t;
				t = y2; y2 = y1; y1 = t;
			}
			if (y2 > y1)
				yincr = 1;
			else
				yincr = -1;
			d = 2 * dy - dx;
			e_incr = 2 * dy;
			ne_incr = 2 * (dy - dx);
			put_pixel (x1, y1, color);

			for (x1++; x1 <= x2; x1++) {
				if (d < 0)
					d += e_incr;
				else {
					d += ne_incr;
					y1 += yincr;
				}
				put_pixel (x1, y1, color);
			}
		} else {
			if (y2 < y1) {
				t = x2; x2 = x1; x1 = t;
				t = y2; y2 = y1; y1 = t;
			}
			if (x2 > x1)
				yincr = 1;
			else
				yincr = -1;
			d = 2 * dx - dy;
			e_incr = 2 * dx;
			ne_incr = 2 * (dx - dy);
			put_pixel (x1, y1, color);

			for (y1++; y1 <= y2; y1++) {
				if (d < 0)
					d += e_incr;
				else {
					d += ne_incr;
					x1 += yincr;
				}
				put_pixel (x1, y1, color);
			}
		}
	}
	int i;
	color_t step, color;

	step = ((color_t) -1) * 3UL / (xres + yres);
	color = 0;
	for (i = 0; i < xres; i += 3)
		line (xres / 2, yres / 2, i, 0, color += step);
	for (i = 0; i < yres; i += 3)
		line (xres / 2, yres / 2, xres, i, color += step);
	color += step;
	for (i = xres-1; i >= 0; i -= 3)
		line (xres / 2, yres / 2, i, yres, color -= step);
	for (i = yres-1; i >= 0; i -= 3)
		line (xres / 2, yres / 2, 0,    i, color -= step);

	line (0, 0, xres - 1, 0, (color_t) -1);
	line (0, 0, 0, yres - 1, (color_t) -1);
	line (xres - 1, 0, xres - 1, yres - 1, (color_t) -1);
	line (0, yres - 1, xres - 1, yres - 1, (color_t) -1);
}

static void
draw_moire32 (int xres, int yres, long screen_ptr, long bytes_per_line)
{
	/* For 32 bits per pixel. */
	typedef unsigned long color_t;

	void put_pixel (int x, int y, color_t color)
	{
		((color_t*) (screen_ptr + y * bytes_per_line)) [x] = color;
	}

	void line (int x1, int y1, int x2, int y2, color_t color)
	{
		int d;
		int dx, dy;
		int e_incr, ne_incr;
		int yincr;
		int t;

		dx = x2 - x1;
		if (dx < 0)
			dx = -dx;

		dy = y2 - y1;
		if (dy < 0)
			dy = -dy;

		if (dy <= dx) {
			if (x2 < x1) {
				t = x2; x2 = x1; x1 = t;
				t = y2; y2 = y1; y1 = t;
			}
			if (y2 > y1)
				yincr = 1;
			else
				yincr = -1;
			d = 2 * dy - dx;
			e_incr = 2 * dy;
			ne_incr = 2 * (dy - dx);
			put_pixel (x1, y1, color);

			for (x1++; x1 <= x2; x1++) {
				if (d < 0)
					d += e_incr;
				else {
					d += ne_incr;
					y1 += yincr;
				}
				put_pixel (x1, y1, color);
			}
		} else {
			if (y2 < y1) {
				t = x2; x2 = x1; x1 = t;
				t = y2; y2 = y1; y1 = t;
			}
			if (x2 > x1)
				yincr = 1;
			else
				yincr = -1;
			d = 2 * dx - dy;
			e_incr = 2 * dx;
			ne_incr = 2 * (dx - dy);
			put_pixel (x1, y1, color);

			for (y1++; y1 <= y2; y1++) {
				if (d < 0)
					d += e_incr;
				else {
					d += ne_incr;
					x1 += yincr;
				}
				put_pixel (x1, y1, color);
			}
		}
	}
	int i;
	color_t step, color;

	step = 0xffffff * 3UL / (xres + yres);
	color = 0;
	for (i = 0; i < xres; i += 3)
		line (xres / 2, yres / 2, i, 0, color += step);
	for (i = 0; i < yres; i += 3)
		line (xres / 2, yres / 2, xres, i, color += step);
	color += step;
	for (i = xres-1; i >= 0; i -= 3)
		line (xres / 2, yres / 2, i, yres, color -= step);
	for (i = yres-1; i >= 0; i -= 3)
		line (xres / 2, yres / 2, 0,    i, color -= step);

	line (0, 0, xres - 1, 0, (color_t) -1);
	line (0, 0, 0, yres - 1, (color_t) -1);
	line (xres - 1, 0, xres - 1, yres - 1, (color_t) -1);
	line (0, yres - 1, xres - 1, yres - 1, (color_t) -1);
}

static unsigned short
find_mode (int xres, int yres, int bpp, vesa_mode_info_t *mi)
{
	vesa_info_t info;
	unsigned short *mode;

	if (! vesa_get_info (&info))
		return 0;

	/* Find direct color graphics mode with linear frame buffer
	 * and given resolution and depth. */
	for (mode=info.modes; *mode!=0xffff; ++mode) {
		if (! vesa_get_mode_info (*mode, mi))
			continue;

		if (mi->memory_model == 6 &&	/* direct color */
		    (mi->mode_attr & 0x01) &&	/* supported */
		    (mi->mode_attr & 0x08) &&	/* color */
		    (mi->mode_attr & 0x10) &&	/* graphics */
		    (mi->mode_attr & 0x80) &&	/* linear */
		    mi->bits_per_pixel == bpp &&
		    mi->width == xres && mi->height == yres)
			return *mode;
	}
	return 0;
}

	/* ARGSUSED */
static int
moire_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	vesa_mode_info_t mi;
	vesa_display_t display;
	unsigned short mode, old_mode;
	int bpp, xres, yres;

	bpp = 16;
	if (argv[1])
		bpp = strtol (argv[1], 0, 0);
	if (bpp != 16)
		bpp = 32;

	if (vesa_get_display_info (&display) &&
	    (display.misc & VESA_MISC_1ST_DETAIL)) {
		/* Get preferred display resolution. */
		xres = display.detailed_modes[0].xres;
		yres = display.detailed_modes[0].yres;
		mode = find_mode (xres, yres, bpp, &mi);
		if (! mode)
			goto simple;
	} else {
simple:		xres = 800;
		yres = 600;
		mode = find_mode (xres, yres, bpp, &mi);
	}
	if (! mode) {
		printf ("No %d x %d x %d resolution found.\n",
			xres, yres, bpp);
		return TCL_OK;
	}
	old_mode = vesa_get_mode ();
	vesa_set_mode (mode | 0x4000); /* linear */
	if (bpp == 16)
		draw_moire16 (xres, yres, mi.phys_base_ptr,
			mi.bytes_per_scan_line);
	else
		draw_moire32 (xres, yres, mi.phys_base_ptr,
			mi.bytes_per_scan_line);
	debug_getchar ();
	vesa_set_mode (old_mode);

	printf ("Resolution %d x %d x %d.\n", xres, yres, bpp);
	return TCL_OK;
}

void print_device (pcibios_t *pb, unsigned char bus, unsigned char dev,
	unsigned char fn, unsigned short vendor, unsigned short devid)
{
	unsigned long addr;
	unsigned short class_code;
	unsigned char irq, htype;
	int i, need_tab;
	static struct {
		unsigned short code;
		char *name;
	} classtab [] = {
		#include "pci-classes.h"
		{ 0, 0 }
	};

	printf ("bus %d dev%2d.%d: vendor/device %04x/%04x",
		bus, dev, fn, vendor, devid);

	pcibios_read_short (pb, PCI_BUSDEVFN (bus, dev, fn),
		PCI_CLASS_DEVICE, &class_code);
	for (i=0; classtab[i].name; ++i) {
		if (classtab[i].code == class_code) {
			printf (" - %s", classtab[i].name);
			break;
		}
	}
	if (! classtab[i].name)
		printf (" - Class %x", class_code);

	pcibios_read_byte (pb, PCI_BUSDEVFN (bus, dev, fn),
		PCI_HEADER_TYPE, &htype);
	need_tab = 1;
	if ((htype & 0x7f) == PCI_HEADER_TYPE_NORMAL) {
		pcibios_read_byte (pb, PCI_BUSDEVFN (bus, dev, fn),
			PCI_INTERRUPT_LINE, &irq);
		if (irq && irq != 0xff) {
			if (need_tab) {
				printf (",\n               ");
				need_tab = 0;
			} else
				printf (", ");
			printf ("irq %d", irq);
		}
		for (i=0; i<6; ++i) {
			pcibios_read_long (pb, PCI_BUSDEVFN (bus, dev, fn),
				PCI_BASE_ADDRESS_0 + i*4, &addr);
			if (addr & PCI_BASE_ADDRESS_SPACE_IO) {
				if (need_tab) {
					printf (",\n               ");
					need_tab = 0;
				} else
					printf (", ");
				printf ("port %lx", addr & PCI_BASE_ADDRESS_IO_MASK);
			} else if (addr & PCI_BASE_ADDRESS_MEM_MASK) {
				if (need_tab) {
					printf (",\n               ");
					need_tab = 0;
				} else
					printf (", ");
				printf ("mem %lx", addr & PCI_BASE_ADDRESS_MEM_MASK);
			}
		}
	}
	printf ("\n");
}

	/* ARGSUSED */
static int
lspci_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	pcibios_t pb;
	unsigned short bus, dev, fn, vendor, devid0, devid;

	if (! pcibios_init (&pb)) {
		printf ("No PCI BIOS found.\n");
		return TCL_OK;
	}
	debug_printf ("PCI BIOS revision %x.%02x hw %02x entry at %lx, last bus=%d\n",
		pb.version_major, pb.version_minor, pb.hw_mechanism,
		pb.entry_address, pb.last_bus);

	for (bus=0; bus <= pb.last_bus; ++bus) {
		for (dev=0; dev<32; ++dev) {
			devid0 = 0xffff;
			if (! pcibios_read_short (&pb,
			    PCI_BUSDEVFN (bus, dev, 0),
			    PCI_DEVICE_ID, &devid0) || devid0 == 0xffff)
				continue;
			pcibios_read_short (&pb, PCI_BUSDEVFN (bus, dev, 0),
				PCI_VENDOR_ID, &vendor);
			print_device (&pb, bus, dev, 0, vendor, devid0);

			for (fn=1; fn<8; ++fn) {
				devid = 0xffff;
				if (! pcibios_read_short (&pb,
				    PCI_BUSDEVFN (bus, dev, fn),
				    PCI_DEVICE_ID, &devid) || devid == 0xffff)
					continue;
				if (devid == devid0)
					break;
				print_device (&pb, bus, dev, fn, vendor, devid);
			}
		}
	}
	return TCL_OK;
}

	/* ARGSUSED */
static int
help_cmd (void *clientData, Tcl_Interp *interp, int argc, char **argv)
{
	debug_puts ("Available commands:\n");
	debug_puts ("    loop var first limit [incr] command\n");
	debug_puts ("    echo [param...]\n");
	debug_puts ("    reboot\n");
	debug_puts ("    drives\n");
	debug_puts ("    vesa [15 | 16 | 24 | 32]\n");
	debug_puts ("    display [modes]\n");
	debug_puts ("    moire [16 | 32]\n");
	debug_puts ("    lspci\n");
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

void task_tcl (void *arg)
{
	char *cmd;
	unsigned char result, got_partial, quit_flag;
	Tcl_Interp *interp;
	Tcl_CmdBuf buffer;

	mem_init (&pool, (size_t) 0x200000, (size_t) 0x400000);
again:
	printf ("\nEmbedded TCL\n\n");
	printf ("Enter \"help\" for a list of commands\n\n");

	interp = Tcl_CreateInterp (&pool);
	Tcl_CreateCommand (interp, "loop", loop_cmd, 0, 0);
	Tcl_CreateCommand (interp, "echo", echo_cmd, 0, 0);
	Tcl_CreateCommand (interp, "help", help_cmd, 0, 0);
	Tcl_CreateCommand (interp, "reboot", reboot_cmd, 0, 0);
	Tcl_CreateCommand (interp, "drives", drives_cmd, 0, 0);
	Tcl_CreateCommand (interp, "vesa", vesa_cmd, 0, 0);
	Tcl_CreateCommand (interp, "display", display_cmd, 0, 0);
	Tcl_CreateCommand (interp, "moire", moire_cmd, 0, 0);
	Tcl_CreateCommand (interp, "lspci", lspci_cmd, 0, 0);

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
				printf (" %d", result);

			if (*interp->result != 0)
				printf (": %s", interp->result);

			debug_putchar (0, '\n');
			continue;
		}

		if (*interp->result != 0)
			printf ("%s\n", interp->result);
	}

	Tcl_DeleteInterp (interp);
	Tcl_DeleteCmdBuf (buffer);
	goto again;
}

void uos_init (void)
{
	task_create (task_tcl, 0, "tcl", 1, task, sizeof (task));
}
