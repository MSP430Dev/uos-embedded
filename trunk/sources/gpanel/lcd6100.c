/*
 * Driver for Nokia 6610 LCD (Philips PCF8833).
 * Based on article from James Lynch.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

/*
 * Commands for Philips PCF8833 LCD controller.
 */
#define PHILIPS_NOP		0x00	/* nop */
#define PHILIPS_SWRESET		0x01	/* software reset */
#define PHILIPS_BSTROFF		0x02	/* booster voltage OFF */
#define PHILIPS_BSTRON		0x03	/* booster voltage ON */
#define PHILIPS_RDDIDIF		0x04	/* read display identification */
#define PHILIPS_RDDST		0x09	/* read display status */
#define PHILIPS_SLEEPIN		0x10	/* sleep in */
#define PHILIPS_SLEEPOUT	0x11	/* sleep out */
#define PHILIPS_PTLON		0x12	/* partial display mode */
#define PHILIPS_NORON		0x13	/* display normal mode */
#define PHILIPS_INVOFF		0x20	/* inversion OFF */
#define PHILIPS_INVON		0x21	/* inversion ON */
#define PHILIPS_DALO		0x22	/* all pixel OFF */
#define PHILIPS_DAL		0x23	/* all pixel ON */
#define PHILIPS_SETCON		0x25	/* write contrast */
#define PHILIPS_DISPOFF		0x28	/* display OFF */
#define PHILIPS_DISPON		0x29	/* display ON */
#define PHILIPS_CASET		0x2A	/* column address set */
#define PHILIPS_PASET		0x2B	/* page address set */
#define PHILIPS_RAMWR		0x2C	/* memory write */
#define PHILIPS_RGBSET		0x2D	/* colour set */
#define PHILIPS_PTLAR		0x30	/* partial area */
#define PHILIPS_VSCRDEF		0x33	/* vertical scrolling definition */
#define PHILIPS_TEOFF		0x34	/* test mode */
#define PHILIPS_TEON		0x35	/* test mode */
#define PHILIPS_MADCTL		0x36	/* memory access control */
#define PHILIPS_SEP		0x37	/* vertical scrolling start address */
#define PHILIPS_IDMOFF		0x38	/* idle	mode OFF */
#define PHILIPS_IDMON		0x39	/* idle mode ON */
#define PHILIPS_COLMOD		0x3A	/* interface pixel format */
#define PHILIPS_SETVOP		0xB0	/* set Vop */
#define PHILIPS_BRS		0xB4	/* bottom row swap */
#define PHILIPS_TRS		0xB6	/* top row swap */
#define PHILIPS_DISCTR		0xB9	/* display control */
#define PHILIPS_DOR		0xBA	/* data order */
#define PHILIPS_TCDFE		0xBD	/* enable/disable DF temperature compensation */
#define PHILIPS_TCVOPE		0xBF	/* enable/disable Vop temp comp */
#define PHILIPS_EC		0xC0	/* internal or external oscillator */
#define PHILIPS_SETMUL		0xC2	/* set multiplication factor */
#define PHILIPS_TCVOPAB		0xC3	/* set TCVOP slopes A and B */
#define PHILIPS_TCVOPCD		0xC4	/* set TCVOP slopes c and d */
#define PHILIPS_TCDF		0xC5	/* set divider frequency */
#define PHILIPS_DF8COLOR	0xC6	/* set divider frequency 8-color mode */
#define PHILIPS_SETBS		0xC7	/* set bias system */
#define PHILIPS_RDTEMP		0xC8	/* temperature read back */
#define PHILIPS_NLI		0xC9	/* n-line inversion */
#define PHILIPS_RDID1		0xDA	/* read ID1 */
#define PHILIPS_RDID2		0xDB	/* read ID2 */
#define PHILIPS_RDID3		0xDC	/* read ID3 */

/*
 * Write 9-bit command to LCD display via SPI interface.
 */
static void write_command (unsigned data)
{
	/* Wait for the transfer to complete. */
	while (! (*AT91C_SPI0_SR & AT91C_SPI_TXEMPTY))
		continue;

	*AT91C_SPI0_TDR = (unsigned short) data;
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
	gp->nrow = 130;
	gp->ncol = 130;
	gp->font = &font_lucidasans11;
	gp->foreground = GPANEL_WHITE;
	gp->background = GPANEL_BLACK;
	gp->contrast = 0x38;
	gp->row = 0;
	gp->col = 0;
	gp->c1 = 0;
	gp->c2 = 0;

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
}

/*
 * Turn the backlight on and off.
 */
void gpanel_backlight (gpanel_t *gp, int on)
{
	if (on)
		*AT91C_PIOB_SODR = AT91C_PIO_PB20;	/* Set to HIGH */
	else
		*AT91C_PIOB_CODR = AT91C_PIO_PB20;	/* Set to LOW */
}

/*
 * Valid contrast values are -64..63.
 */
void gpanel_contrast (gpanel_t *gp, int contrast)
{
	gp->contrast = contrast & 0x7f;
	write_command (PHILIPS_SETCON);
	write_data (gp->contrast);
}

/*
 * Write an image to LCD screen.
 */
void gpanel_image (gpanel_t *gp, int x, int y, int width, int height,
	const unsigned short *data)
{
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
}

/*
 * Fill the LCD screen with a given 12-bit color.
 * The color value should have a format rrrrggggbbbb.
 */
void gpanel_clear (gpanel_t *gp, unsigned color)
{
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

	gp->row = 0;
	gp->col = 0;
}

/*
 * Lights a single pixel in the specified color
 * at the specified x and y addresses
 */
void gpanel_pixel (gpanel_t *gp, int x, int y, int color)
{
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
}

/*
 * Print one symbol. Decode from UTF8.
 * Some characters are handled specially.
 */
void gpanel_glyph (gpanel_t *gp, unsigned width, const unsigned short *bits)
{
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
}
