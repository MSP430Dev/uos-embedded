/*
 * Driver for MELT MT-12864J LCD (Samsung KS0108B).
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

static void set_crystal (gpanel_t *gp, int num)
{
	ARM_GPIOE->DATA = ((num + 1) << 4);
	ARM_GPIOE->OE = 0x30;
	udelay (8);
	if (num) {
		/* Кристалл #2. */
		gp->DATA = (unsigned*) 0x18200000;
		gp->CMD  = (unsigned*) 0x10200000;
	} else {
		/* Кристалл #1. */
		gp->DATA = (unsigned*) 0x18100000;
		gp->CMD  = (unsigned*) 0x10100000;
	}
}

/*
 * Set up hardware for communication to Nokia 6100 LCD Display.
 * Internally, the controller has 132x132 pixels.
 * But visible area is only 130x130.
 */
void gpanel_init (gpanel_t *gp)
{
	extern stream_interface_t gpanel_interface;

	gp->interface = &gpanel_interface;
	gp->nrow = 127;
	gp->ncol = 63;
	gp->font = &font_lucidasans7;
	gp->foreground = GPANEL_WHITE;
	gp->background = GPANEL_BLACK;
	gp->contrast = 0x38;
	gp->row = 0;
	gp->col = 0;
	gp->c1 = 0;
	gp->c2 = 0;

	ARM_RSTCLK->PER_CLOCK = 0xFFFFFFFF;

	/* Программный сброс экрана. */
	ARM_GPIOC->DATA = 0x00000200;
	ARM_GPIOC->OE = 0x00000200;
	for (i=0; i<255; i++)
		ARM_GPIOC->DATA = 0;
	ARM_GPIOC->DATA = 0x00000200;

	/* Инициализация всех кристаллов. */
	for (crystal=0; crystal<2; crystal++) {
		set_crystal (gp, crystal);
		wait_status (gp, BUSY);
		write_cmd (gp, 0x3F);		// LCD on
		wait_status (gp, ONOFF);
		write_cmd (gp, 0xC0);		// start line 0
	}
}

/*
 * No backlight control.
 */
void gpanel_backlight (gpanel_t *gp, int on)
{
}

/*
 * No contrast control.
 */
void gpanel_contrast (gpanel_t *gp, int contrast)
{
}

/*
 * Write an image to LCD screen.
 */
void gpanel_image (gpanel_t *gp, int x, int y, int width, int height,
	const unsigned short *data)
{
#if 0
	unsigned i, pixels = height * width;
	unsigned long rgbrgb;

	write_command (PHILIPS_PASET);
	write_data (y + 1);
	write_data (y + height);
	write_command (PHILIPS_CASET);
	write_data (x + 1);
	write_data (x + width);
	write_command (PHILIPS_RAMWR);

	for (i=0; i<pixels; i+=2) {
		rgbrgb = (unsigned long) data [i] << 12;
		rgbrgb |= data [i+1];

		write_data (rgbrgb >> 16);
		write_data (rgbrgb >> 8);
		write_data (rgbrgb);
	}
#endif
}

/*
 * Fill the LCD screen with a given 12-bit color.
 * The color value should have a format rrrrggggbbbb.
 */
void gpanel_clear (gpanel_t *gp, unsigned color)
{
#if 0
	unsigned i;

	write_command (PHILIPS_CASET);
	write_data (0);
	write_data (gp->ncol + 1);
	write_command (PHILIPS_PASET);
	write_data (0);
	write_data (gp->nrow + 1);
	write_command (PHILIPS_RAMWR);
	for (i=0; i<(gp->nrow+2)*(gp->ncol+2)/2; i++) {
		write_data (color >> 4);
		write_data ((color << 4) |
			      ((color >> 8) & 0xF));
		write_data (color);
	}
#endif
	gp->row = 0;
	gp->col = 0;
}

/*
 * Lights a single pixel in the specified color
 * at the specified x and y addresses
 */
void gpanel_pixel (gpanel_t *gp, int x, int y, int color)
{
#if 0
	write_command (PHILIPS_PASET);
	write_data (y + 1);
	write_data (y + 1);
	write_command (PHILIPS_CASET);
	write_data (x + 1);
	write_data (x + 1);
	write_command (PHILIPS_RAMWR);
	write_data (color >> 4);
	write_data (color << 4);
	write_command (PHILIPS_NOP);
#endif
}

/*
 * Draw a filled rectangle in the specified color from (x1,y1) to (x2,y2).
 *
 * The best way to fill a rectangle is to take advantage of the "wrap-around" featute
 * built into the Philips PCF8833 controller. By defining a drawing box, the memory can
 * be simply filled by successive memory writes until all pixels have been illuminated.
 */
void gpanel_rect_filled (gpanel_t *gp, int x0, int y0, int x1, int y1, int color)
{
#if 0
	int xmin, xmax, ymin, ymax, i;

	/* calculate the min and max for x and y directions */
	xmin = (x0 <= x1) ? x0 : x1;
	xmax = (x0 > x1) ? x0 : x1;
	ymin = (y0 <= y1) ? y0 : y1;
	ymax = (y0 > y1) ? y0 : y1;

	/* specify the controller drawing box according to those limits */
	write_command (PHILIPS_PASET);
	write_data (ymin + 1);
	write_data (ymax + 1);
	write_command (PHILIPS_CASET);
	write_data (xmin + 1);
	write_data (xmax + 1);
	write_command (PHILIPS_RAMWR);

	/* loop on total number of pixels/2 */
	for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++) {
		/* use the color value to output three data bytes covering two pixels */
		write_data (color >> 4);
		write_data ((color << 4) | ((color >> 8) & 0xF));
		write_data (color);
	}
#endif
}

/*
 * Print one symbol. Decode from UTF8.
 * Some characters are handled specially.
 */
void gpanel_glyph (gpanel_t *gp, unsigned width, const unsigned short *bits)
{
#if 0
	unsigned i, j, glyph_row, rgbrgb;

	/* Draw a character. */
	write_command (PHILIPS_PASET);
	write_data (gp->row + 1);
	write_data (gp->row + gp->font->height);
	write_command (PHILIPS_CASET);
	write_data (gp->col + 1);
	write_data (gp->col + 1 + ((width - 1) | 1));
	write_command (PHILIPS_RAMWR);

	/* Loop on each glyph row, backwards from bottom to top. */
	for (i=0; i<gp->font->height; i++) {
		glyph_row = bits[i];

		/* Loop on every two pixels in the row (left to right). */
		for (j=0; j<width; j+=2) {
			/* Get rgb values for two successive pixels. */
			if (glyph_row & 0x8000)
				rgbrgb = gp->foreground << 12;
			else
				rgbrgb = gp->background << 12;
			if (glyph_row & 0x4000)
				rgbrgb |= gp->foreground;
			else
				rgbrgb |= gp->background;
			glyph_row <<= 2;

			/* Output three data bytes. */
			write_data (rgbrgb >> 16);
			write_data (rgbrgb >> 8);
			write_data (rgbrgb);
		}
	}
	write_command (PHILIPS_NOP);
#endif
}
