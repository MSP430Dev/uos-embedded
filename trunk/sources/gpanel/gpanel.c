/*
 * Generic graphics panel routines.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

static void gpanel_putchar (gpanel_t *d, short c);

stream_interface_t gpanel_interface = {
        (void (*) (stream_t*, short)) gpanel_putchar,
        0, 0,
};

/*
 * Draw a line in the specified color from (x0,y0) to (x1,y1).
 */
void gpanel_line (gpanel_t *gp, int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, stepx, stepy, fraction;

	dy = y1 - y0;
	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	} else {
		stepy = 1;
	}
	dx = x1 - x0;
	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	} else {
		stepx = 1;
	}
	dy <<= 1;				       /* dy is now 2*dy */
	dx <<= 1;				       /* dx is now 2*dx */
	gpanel_pixel (gp, x0, y0, color);
	if (dx > dy) {
		fraction = dy - (dx >> 1);	       /* same as 2*dy - dx */
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;	       /* same as fraction -= 2*dx */
			}
			x0 += stepx;
			fraction += dy;		       /* same as fraction -= 2*dy */
			gpanel_pixel (gp, x0, y0, color);
		}
	} else {
		fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			gpanel_pixel (gp, x0, y0, color);
		}
	}
}

/*
 * Draw a rectangle in the specified color from (x1,y1) to (x2,y2).
 */
void gpanel_rect (gpanel_t *gp, int x0, int y0, int x1, int y1, int color)
{
	gpanel_line (gp, x0, y0, x1, y0, color);
	gpanel_line (gp, x0, y1, x1, y1, color);
	gpanel_line (gp, x0, y0, x0, y1, color);
	gpanel_line (gp, x1, y0, x1, y1, color);
}

/*
 * Draw a circle in the specified color at center (x0,y0) with radius.
 */
void gpanel_circle (gpanel_t *gp, int x0, int y0, int radius, int color)
{
	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	gpanel_pixel (gp, x0, y0 + radius, color);
	gpanel_pixel (gp, x0, y0 - radius, color);
	gpanel_pixel (gp, x0 + radius, y0, color);
	gpanel_pixel (gp, x0 - radius, y0, color);
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		gpanel_pixel (gp, x0 + x, y0 + y, color);
		gpanel_pixel (gp, x0 - x, y0 + y, color);
		gpanel_pixel (gp, x0 + x, y0 - y, color);
		gpanel_pixel (gp, x0 - x, y0 - y, color);
		gpanel_pixel (gp, x0 + y, y0 + x, color);
		gpanel_pixel (gp, x0 - y, y0 + x, color);
		gpanel_pixel (gp, x0 + y, y0 - x, color);
		gpanel_pixel (gp, x0 - y, y0 - x, color);
	}
}

/*
 * Move cursor to given position.
 */
void gpanel_move (gpanel_t *gp, int x, int y)
{
        if (y < 0 || y >= gp->nrow || x < 0 || x >= gp->ncol)
		return;
	gp->row = y;
	gp->col = x;
}

/*
 * Set foreground and background colors.
 */
void gpanel_color (gpanel_t *gp, int fg, int bg)
{
	gp->foreground = fg;
	gp->background = bg;
}

#if 0
/*
 * Calculate a width of text output.
 * Handles both fixed and proportional fonts.  Passed UTF8string.
 */
int gpanel_text_width (gpanel_t *gp, const unsigned char *text)
{
	int width;
	unsigned c;

	/* TODO: UTF8 decoding. */
	if (! nchars)
		nchars = strlen (text);
	if (! gp->font->width)
		return nchars * gp->font->maxwidth;

	width = 0;
	while (--nchars >= 0) {
		c = *text++;
		if (c < gp->font->firstchar || c >= gp->font->firstchar + gp->font->size)
			c = gp->font->defaultchar;
		width += gp->font->width [c - gp->font->firstchar];
	}
	return width;
}
#endif

/*
 * Print one symbol. Decode from UTF8.
 * Some characters are handled specially.
 */
static void gpanel_putchar (gpanel_t *gp, short c)
{
	unsigned cindex, width;
	const unsigned short *bits;

	switch (c) {
	case '\n':		/* goto next line */
		gp->row += gp->font->height;
		gp->col = 0;
		return;
	case '\t':		/* tab replaced by space */
		c = ' ';
		break;
	case '\r':		/* carriage return - go to begin of line */
		gp->col = 0;
		return;
	}

	/* Decode UTF-8. */
	if (! (c & 0x80)) {
		gp->c1 = 0;
		gp->c2 = 0;
	} else if (gp->c1 == 0) {
		gp->c1 = c;
		return;
	} else if (gp->c2 == 0) {
		if (gp->c1 & 0x20) {
			gp->c2 = c;
			return;
		}
		c = (gp->c1 & 0x1f) << 6 | (c & 0x3f);
		gp->c1 = 0;
	} else {
		if (gp->c1 == 0xEF && gp->c2 == 0xBB && c == 0xBF) {
			/* Skip zero width no-break space. */
			gp->c1 = 0;
			gp->c2 = 0;
			return;
		}
		c = (gp->c1 & 0x0f) << 12 |
			(gp->c2 & 0x3f) << 6 | (c & 0x3f);
		gp->c1 = 0;
		gp->c2 = 0;
	}

	if (c < gp->font->firstchar || c >= gp->font->firstchar + gp->font->size)
		c = gp->font->defaultchar;
	cindex = c - gp->font->firstchar;

	/* Get font bitmap depending on fixed pitch or not. */
	if (gp->font->width) {
		/* Proportional font. */
		width = gp->font->width [cindex];
	} else {
		/* Fixed width font. */
		width = gp->font->maxwidth;
	}
	if (gp->font->offset) {
		bits = gp->font->bits + gp->font->offset [cindex];
	} else {
		bits = gp->font->bits + cindex * gp->font->height;
	}

	/* Scrolling. */
	if (gp->col > gp->ncol - width) {
		gp->col = 0;
		gp->row += gp->font->height;
		if (gp->row > gp->nrow - gp->font->height)
			gp->row = 0;
	}

	/* Draw a character. */
	gpanel_glyph (gp, width, bits);
	gp->col += width;
}
