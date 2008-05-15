/*
 * Copyright (c) 2005 Serge Vakulenko <vak@cronyx.ru>
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X screen driver for uOS.
 * Uses Vesa 2.0 BIOS interface.
 */
#define _GNU_SOURCE 1
#include "device.h"
#include "genfont.h"
#include "genmem.h"
#include "fb.h"

#include <runtime/lib.h>
#include <vesa/vesa.h>
#include <timer/timer.h>

#if 1
#define DEFAULT_WIDTH	800
#define DEFAULT_HEIGHT	600
#else
#define DEFAULT_WIDTH	1024
#define DEFAULT_HEIGHT	768
#endif

static unsigned short old_mode;

/*
 * Debug output in graphics mode.
 * Draw ascii text using system FIXED font.
 */
static MWCOORD debug_x, debug_y;

static int display_x_centimeters, display_y_centimeters;

static void
debug_putchar_graphics (void *arg, short ch)
{
	PSD		psd = (PSD) arg;
	MWCOORD		width;			/* width of text area */
	MWCOORD 	height;			/* height of text area */
	MWCOORD		base;			/* baseline of text */
	const MWIMAGEBITS *bitmap;		/* bitmap for characters */
	MWPIXELVAL	saved_foreground;
	MWPIXELVAL	saved_background;
	MWBOOL		saved_usebg;
	int		saved_mode;
	extern MWPIXELVAL gr_foreground;
	extern MWBOOL	gr_usebg;

	/* Use default system FIXED font. */
	gen_gettextbits ((PMWFONT) &gen_fonts[1], ch,
		&bitmap, &width, &height, &base);

	/* Put character, move cursor. */
	switch (ch) {
	case '\n':
		debug_x = 0;
		debug_y += height;
		break;
	case '\r':
		debug_x = 0;
		break;
	case '\t':
		/* Tab stops at every 4 char heights. */
		debug_x = ((debug_x / height) + 4) / 4 * 4 * height;
		break;
	case '\b':
		if (debug_x >= width)
			debug_x -= width;
		break;
	default:
		/* Draw in COPY mode with black background. */
		saved_foreground = gr_foreground;
		saved_background = gr_background;
		saved_usebg = gr_usebg;
		saved_mode = gr_mode;
		gr_foreground = ~0L;
		gr_background = 0;
		gr_usebg = TRUE;
		gr_mode = MWMODE_COPY;
		drawbitmap (psd, debug_x, debug_y, width, height, bitmap);
		gr_foreground = saved_foreground;
		gr_background = saved_background;
		gr_usebg = saved_usebg;
		gr_mode = saved_mode;

		GdFixCursor (psd);

		debug_x += width;
		break;
	}

	if (debug_x + width > psd->xvirtres) {
		/* Roll over right margin. */
		debug_x = 0;
		debug_y += height;
	}

	if (debug_y + height > psd->yvirtres) {
		/* Roll over the screen. */
		debug_y = 0;
	}
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

/* init framebuffer */
static PSD
fb_open (PSD psd)
{
	vesa_mode_info_t mi;
	vesa_display_t display;
	PSUBDRIVER subdriver;
	unsigned short mode;

#if MWPIXEL_FORMAT == MWPF_TRUECOLOR565
	psd->bpp = 16;
#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR888
	psd->bpp = 24;
#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR0888
	psd->bpp = 32;
#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR8888
	psd->bpp = 32;
#else
#error Incorrect value of MWPIXEL_FORMAT!
#endif
	mode = 0;
	if (vesa_get_display_info (&display)) {
		display_x_centimeters = display.max_x;
		display_y_centimeters = display.max_y;
		if (display.misc & VESA_MISC_1ST_DETAIL) {
			/* Get preferred display resolution. */
			psd->xres = display.detailed_modes[0].xres;
			psd->yres = display.detailed_modes[0].yres;
			mode = find_mode (psd->xres, psd->yres, psd->bpp, &mi);
		}
	}
	if (! mode) {
		psd->xres = DEFAULT_WIDTH;
		psd->yres = DEFAULT_HEIGHT;
		mode = find_mode (psd->xres, psd->yres, psd->bpp, &mi);
	}
	if (! mode) {
		debug_printf ("No %d x %d x %d resolution found.\n",
			psd->xres, psd->yres, psd->bpp);
		return 0;
	}

	psd->xvirtres = psd->xres;
	psd->yvirtres = psd->yres;
	psd->planes = 1;
	psd->ncolors = (psd->bpp >= 24) ? (1 << 24) : (1 << psd->bpp);
	psd->linelen = mi.bytes_per_scan_line;
	psd->addr = (void*) mi.phys_base_ptr;

	psd->flags = PSF_SCREEN | PSF_HAVEBLIT;
	switch (psd->bpp) {
	case 16:
		psd->flags |= PSF_HAVEOP_COPY;
		psd->pixtype = MWPF_TRUECOLOR565;
		break;
	case 24:
		psd->pixtype = MWPF_TRUECOLOR888;
		break;
	case 32:
#if MWPIXEL_FORMAT == MWPF_TRUECOLOR8888
		psd->pixtype = MWPF_TRUECOLOR8888;
#else
		psd->pixtype = MWPF_TRUECOLOR0888;
#endif
		break;
	default:
		debug_printf ("Unsupported %d color (%d bpp) truecolor framebuffer\n",
			psd->ncolors, psd->bpp);
		return 0;
	}

	/* select a framebuffer subdriver based on planes and bpp */
	subdriver = select_fb_subdriver (psd);
	if (! subdriver) {
		debug_printf ("No driver for screen bpp %d\n", psd->bpp);
		return 0;
	}

	old_mode = vesa_get_mode ();
	vesa_set_mode (mode | 0x4000); /* linear */

	/*
	 * set and initialize subdriver into screen driver
	 * psd->size is calculated by subdriver init
	 */
	if (! set_subdriver (psd, subdriver, TRUE)) {
		vesa_set_mode (old_mode);
		debug_printf ("Driver initialize failed bpp %d\n", psd->bpp);
		return 0;
	}

	/* Redirect debug_printf() output. */
	debug_redirect (debug_putchar_graphics, (void*) psd);
	return psd;
}

/* close framebuffer*/
static void
fb_close (PSD psd)
{
	if (old_mode)
		vesa_set_mode (old_mode);

	debug_redirect (0, 0);
}

static int fade = 100;

/* convert Microwindows palette to framebuffer format and set it */
static void
fb_setpalette (PSD psd, int first, int count, MWPALENTRY *palette)
{
	MWPALENTRY faded [256];
	int i;

	if (count > 256)
		count = 256;
	for (i=0; i < count; i++) {
		faded[i].r = (palette[i].r * fade / 100) << 8;
		faded[i].g = (palette[i].g * fade / 100) << 8;
		faded[i].b = (palette[i].b * fade / 100) << 8;
	}
	vesa_set_palette (first, count, faded);
}

/* experimental palette animation */
void
setfadelevel (PSD psd, int f)
{
	extern MWPALENTRY gr_palette[256];

	if (psd->pixtype != MWPF_PALETTE)
		return;

	fade = f;
	if (fade > 100)
		fade = 100;
	fb_setpalette (psd, 0, 256, gr_palette);
}

static void
getscreeninfo (PSD psd, PMWSCREENINFO psi)
{
	psi->rows = psd->yvirtres;
	psi->cols = psd->xvirtres;
	psi->planes = psd->planes;
	psi->bpp = psd->bpp;
	psi->ncolors = psd->ncolors;
	psi->pixtype = psd->pixtype;
	psi->fonts = NUMBER_FONTS;

	if (display_x_centimeters > 0)
		psi->xdpcm = psd->xvirtres / display_x_centimeters;
	else
		psi->xdpcm = 33;

	if (display_y_centimeters > 0)
		psi->ydpcm = psd->yvirtres / display_y_centimeters;
	else
		psi->ydpcm = 33;

	psi->portrait = 0;
	psi->fbdriver = TRUE;
	switch (psd->pixtype) {
	case MWPF_TRUECOLOR0888:
	case MWPF_TRUECOLOR8888:
	case MWPF_TRUECOLOR888:
		psi->rmask = 0xff0000;
		psi->gmask = 0x00ff00;
		psi->bmask = 0x0000ff;
		break;
	case MWPF_TRUECOLOR565:
		psi->rmask = 0xf800;
		psi->gmask = 0x07e0;
		psi->bmask = 0x001f;
		break;
	case MWPF_TRUECOLOR555:
		psi->rmask = 0x7c00;
		psi->gmask = 0x03e0;
		psi->bmask = 0x001f;
		break;
	case MWPF_TRUECOLOR332:
		psi->rmask = 0xe0;
		psi->gmask = 0x1c;
		psi->bmask = 0x03;
		break;
	case MWPF_PALETTE:
	default:
		psi->rmask = 0xff;
		psi->gmask = 0xff;
		psi->bmask = 0xff;
		break;
	}
}

SCREENDEVICE scrdev = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	fb_open,
	fb_close,
	getscreeninfo,
	fb_setpalette,
	0,			/* DrawPixel subdriver*/
	0,			/* ReadPixel subdriver*/
	0,			/* DrawHorzLine subdriver*/
	0,			/* DrawVertLine subdriver*/
	0,			/* FillRect subdriver*/
	gen_fonts,
	0,			/* Blit subdriver*/
	0,			/* PreSelect*/
	0,			/* DrawArea subdriver*/
	0,			/* SetIOPermissions*/
	gen_allocatememgc,
	fb_mapmemgc,
	gen_freememgc
};

int
GdError (const char *fmt, ...)
{
	va_list args;
	int err;

	va_start (args, fmt);
	err = debug_vprintf (fmt, args);
	va_end (args);
	return err;

}

int
GdErrorNull (const char *fmt, ...)
{
	return 0;
}

/*
 * Microwindows Proportional Fonts & Routines (proportional font format)
 *
 * This file contains the generalized low-level font/text
 * drawing routines.  Both fixed and proportional fonts are
 * supported, with fixed pitch structure allowing much smaller
 * font files.
 */
extern MWCFONT font_lucidasans11;
extern MWCFONT font_10x20;
extern MWCFONT font_cursor;

/* handling routines for MWCOREFONT */
static MWFONTPROCS fontprocs = {
	MWTF_ASCII,		/* routines expect ascii*/
	gen_getfontinfo,
	gen_gettextsize,
	gen_gettextbits,
	gen_unloadfont,
	corefont_drawtext,
	NULL,			/* setfontsize*/
	NULL,			/* setfontrotation*/
	NULL,			/* setfontattr*/
};

/* first font is default font */
MWCOREFONT gen_fonts[NUMBER_FONTS] = {
	{&fontprocs, 0, 0, 0, MWFONT_SYSTEM_VAR,   &font_lucidasans11},
	{&fontprocs, 0, 0, 0, MWFONT_SYSTEM_FIXED, &font_10x20},
	/* Standard X11 fonts for nxlib */
	{&fontprocs, 0, 0, 0, "fixed",		   &font_10x20},
	{&fontprocs, 0, 0, 0, "variable",          &font_lucidasans11},
/*	{&fontprocs, 0, 0, 0, "cursor",            &font_cursor},*/
};

/*
 * Generalized low level get font info routine.  This
 * routine works with fixed and proportional fonts.
 */
MWBOOL
gen_getfontinfo(PMWFONT pfont, PMWFONTINFO pfontinfo)
{
	PMWCFONT	pf = ((PMWCOREFONT)pfont)->cfont;
	int		i;

	pfontinfo->maxwidth = pf->maxwidth;
	pfontinfo->height = pf->height;
	pfontinfo->baseline = pf->ascent;
	pfontinfo->firstchar = pf->firstchar;
	pfontinfo->lastchar = pf->firstchar + pf->size - 1;
	pfontinfo->fixed = pf->width == NULL? TRUE: FALSE;
	for(i=0; i<256; ++i) {
		if(pf->width == NULL)
			pfontinfo->widths[i] = pf->maxwidth;
		else {
			if(i<pf->firstchar || i >= pf->firstchar+pf->size)
				pfontinfo->widths[i] = 0;
			else pfontinfo->widths[i] = pf->width[i-pf->firstchar];
		}
	}
	return TRUE;
}

/*
 * Generalized low level routine to calc bounding box for text output.
 * Handles both fixed and proportional fonts.  Passed ascii string.
 */
void
gen_gettextsize(PMWFONT pfont, const void *text, int cc, MWTEXTFLAGS flags,
	MWCOORD *pwidth, MWCOORD *pheight, MWCOORD *pbase)
{
	PMWCFONT		pf = ((PMWCOREFONT)pfont)->cfont;
	const unsigned char *	str = text;
	unsigned int		c;
	int			width;

	if(pf->width == NULL)
		width = cc * pf->maxwidth;
	else {
		width = 0;
		while(--cc >= 0) {
			c = *str++;
			if(c >= pf->firstchar && c < pf->firstchar+pf->size)
				width += pf->width[c - pf->firstchar];
		}
	}
	*pwidth = width;
	*pheight = pf->height;
	*pbase = pf->ascent;
}

#if HAVE_FNT_SUPPORT | HAVE_PCF_SUPPORT
/*
 * Routine to calc bounding box for text output.
 * Handles both fixed and proportional fonts.  Passed MWTF_UC16 string.
 */
void
gen16_gettextsize(PMWFONT pfont, const void *text, int cc, MWTEXTFLAGS flags,
	MWCOORD *pwidth, MWCOORD *pheight, MWCOORD *pbase)
{
	PMWCFONT		pf = ((PMWCOREFONT) pfont)->cfont;
	const unsigned short *	str = text;
	unsigned		int c;
	int			width;

	if (pf->width == NULL)
		width = cc * pf->maxwidth;
	else {
		width = 0;
		while (--cc >= 0) {
			c = *str++;
			if (c >= pf->firstchar && c < pf->firstchar + pf->size)
				width += pf->width[c - pf->firstchar];
		}
	}
	*pwidth = width;
	*pheight = pf->height;
	*pbase = pf->ascent;
}
#endif /* HAVE_FNT_SUPPORT | HAVE_PCF_SUPPORT*/

/*
 * Generalized low level routine to get the bitmap associated
 * with a character.  Handles fixed and proportional fonts.
 */
void
gen_gettextbits(PMWFONT pfont, int ch, const MWIMAGEBITS **retmap,
	MWCOORD *pwidth, MWCOORD *pheight, MWCOORD *pbase)
{
	PMWCFONT		pf = ((PMWCOREFONT)pfont)->cfont;
	int 			count, width;
	const MWIMAGEBITS *	bits;

	/* if char not in font, map to first character by default*/
	if(ch < pf->firstchar || ch >= pf->firstchar+pf->size)
		ch = pf->firstchar;

	ch -= pf->firstchar;

	/* get font bitmap depending on fixed pitch or not*/
	bits = pf->bits + (pf->offset? pf->offset[ch]: (pf->height * ch));
 	width = pf->width ? pf->width[ch] : pf->maxwidth;
	count = MWIMAGE_WORDS(width) * pf->height;

	*retmap = bits;

	/* return width depending on fixed pitch or not*/
	*pwidth = width;
	*pheight = pf->height;
	*pbase = pf->ascent;
}

void
gen_unloadfont(PMWFONT pfont)
{
	/* builtins can't be unloaded*/
}
