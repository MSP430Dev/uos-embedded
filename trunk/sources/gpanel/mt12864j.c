/*
 * Driver for MELT MT-12864J LCD (Samsung KS0108B).
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

/*
 * Write 9-bit command to LCD display via SPI interface.
 */
static void write_command (unsigned data)
{
#if 0
	/* Wait for the transfer to complete. */
	while (! (*AT91C_SPI0_SR & AT91C_SPI_TXEMPTY))
		continue;

	*AT91C_SPI0_TDR = (unsigned short) data;
#endif
}

/*
 * Write data to LCD display.
 * Data differ from commands by 9 bit set to 1.
 */
static inline void write_data (unsigned data)
{
	write_command (data | 0x0100);
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
#if 0
	/* Backlight is controlled by pin PB20. */
	*AT91C_PIOB_SODR = AT91C_PIO_PB20;	/* Set high: enable backlight */
	*AT91C_PIOB_OER	 = AT91C_PIO_PB20;	/* Configure PB20 as output */

	/* LCD reset is connected to pin PA2. */
	*AT91C_PIOA_SODR = AT91C_PIO_PA2;	/* Set high: disable reset */
	*AT91C_PIOA_OER	 = AT91C_PIO_PA2;	/* Configure PA2 as output */

	/* Init SPI0:
	 * PA12 -> NPCS0
	 * PA16 -> MISO
	 * PA17 -> MOSI
	 * PA18 -> SPCK */
	*AT91C_PIOA_PDR = AT91C_PA12_SPI0_NPCS0 | AT91C_PA16_SPI0_MISO |
			  AT91C_PA17_SPI0_MOSI  | AT91C_PA18_SPI0_SPCK;
	*AT91C_PIOA_ASR = AT91C_PA12_SPI0_NPCS0 | AT91C_PA16_SPI0_MISO |
			  AT91C_PA17_SPI0_MOSI  | AT91C_PA18_SPI0_SPCK;
	*AT91C_PIOA_BSR = 0;
	*AT91C_PMC_PCER = 1 << AT91C_ID_SPI0;	/* Enable SPI clock. */
	*AT91C_SPI0_CR	= AT91C_SPI_SPIEN | AT91C_SPI_SWRST;
	*AT91C_SPI0_CR	= AT91C_SPI_SPIEN;	/* Fixed mode */

	*AT91C_SPI0_MR = AT91C_SPI_MSTR |	/* Master mode */
		AT91C_SPI_MODFDIS |		/* Fault detection disabled */
		(AT91C_SPI_PCS & (0xE << 16));	/* Chip select NPCS0 (PA12) */

	AT91C_SPI0_CSR[0] = AT91C_SPI_CPOL |	/* Clock inactive high */
		AT91C_SPI_BITS_9 |		/* 9 bits per transfer */
		(AT91C_SPI_SCBR & (8 << 8)) |	/* 48MHz/8 = 6 MHz */
		(AT91C_SPI_DLYBS & (1 << 16)) |	/* Delay Before SPCK */
		(AT91C_SPI_DLYBCT & (1 << 24));	/* Delay between transfers */

	/* Software Reset. */
	write_command (PHILIPS_NOP);
	write_command (PHILIPS_SWRESET);

	/* Normal display mode. */
	write_command (PHILIPS_NORON);

	/* Display data access modes: horizontal, mirror X. */
	write_command (PHILIPS_MADCTL);
	write_data (0x48);

	/* Sleep out. */
	write_command (PHILIPS_SLEEPOUT);

	/* Set contrast. */
	write_command (PHILIPS_SETCON);
	write_data (gp->contrast);

	/* Booster voltage on. */
	write_command (PHILIPS_BSTRON);

	/* Display on. */
	write_command (PHILIPS_DISPON);
#endif
}

/*
 * Turn the backlight on and off.
 */
void gpanel_backlight (gpanel_t *gp, int on)
{
#if 0
	if (on)
		*AT91C_PIOB_SODR = AT91C_PIO_PB20;	/* Set to HIGH */
	else
		*AT91C_PIOB_CODR = AT91C_PIO_PB20;	/* Set to LOW */
#endif
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
