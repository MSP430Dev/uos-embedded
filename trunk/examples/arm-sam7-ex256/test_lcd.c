/*
 * Testing Nokia 6610 LCD (Philips PCF8833) on
 * Olimex SAM7-EX256 evaluation board.
 * Based on article from James Lynch.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <random/rand15.h>
#include "sam7-ex256.h"

/*
 * Proportional/fixed font structure.
 */
typedef struct {
        const char *    name;		/* font name*/
        int             maxwidth;	/* max width in pixels*/
        unsigned int    height;		/* height in pixels*/
        int             ascent;		/* ascent (baseline) height*/
        int             firstchar;	/* first character in bitmap*/
        int             size;		/* font size in characters*/
        const unsigned short *bits;	/* 16-bit right-padded bitmap data*/
        const unsigned long *offset;	/* offsets into bitmap data*/
        const unsigned char *width;	/* character widths or 0 if fixed*/
        int             defaultchar;	/* default char (not glyph index)*/
        long            bits_size;	/* # words of bits*/
} lcd_font_t;

typedef struct {
	stream_interface_t *interface;	/* for printf */
	lcd_font_t *font;		/* selected font */
	unsigned foreground;		/* color for putchar */
	unsigned background;		/* background for putchar */
	small_uint_t contrast;		/* current contrast */
	small_uint_t row;		/* current row */
	small_uint_t col;		/* current column */
	small_uint_t c1, c2;		/* utf8 decoder */
} lcd_t;

/*
 * Internally, the controller has 132x132 pixels.
 * But visible area is only 130x130.
 */
#define NROW 	130
#define NCOL 	130

static void lcd_putchar (lcd_t *d, short c);

static stream_interface_t lcd_interface = {
        (void (*) (stream_t*, short)) lcd_putchar,
        0, 0,
};

#include <lcd6100/lucidasans11.c>

#include "image_frog.c"

ARRAY (task, 400);
lcd_t display;

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

/* 12-bit color definitions */
#define LCD_WHITE	0xFFF
#define LCD_BLACK	0x000
#define LCD_RED		0xF00
#define LCD_GREEN	0x0F0
#define LCD_BLUE	0x00F
#define LCD_CYAN	0x0FF
#define LCD_MAGENTA	0xF0F
#define LCD_YELLOW	0xFF0
#define LCD_BROWN	0xB22
#define LCD_ORANGE	0xFA0
#define LCD_PINK	0xF6A

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
 */
void lcd_init (lcd_t *lcd)
{
	lcd->interface = &lcd_interface;
	lcd->font = &font_lucidasans11;
	lcd->foreground = LCD_WHITE;
	lcd->background = LCD_BLACK;
	lcd->contrast = 0x38;
	lcd->row = 0;
	lcd->col = 0;
	lcd->c1 = 0;
	lcd->c2 = 0;

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
		(AT91C_SPI_SCBR & (12 << 8)) |	/* 48MHz/12 = 4 MHz */
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
	write_data (lcd->contrast);

	/* Booster voltage on. */
	write_command (PHILIPS_BSTRON);

	/* Display on. */
	write_command (PHILIPS_DISPON);
}

/*
 * Turn the backlight on and off.
 */
void lcd_backlight (lcd_t *lcd, int on)
{
	if (on)
		*AT91C_PIOB_SODR = AT91C_PIO_PB20;	/* Set to HIGH */
	else
		*AT91C_PIOB_CODR = AT91C_PIO_PB20;	/* Set to LOW */
}

/*
 * Valid contrast values are -64..63.
 */
void lcd_contrast (lcd_t *lcd, int contrast)
{
	lcd->contrast = contrast & 0x7f;
	write_command (PHILIPS_SETCON);
	write_data (lcd->contrast);
}

/*
 * Write an image to LCD screen.
 */
void lcd_image (lcd_t *lcd, int x, int y, int width, int height,
	const unsigned short *data)
{
	unsigned i, pixels = height * width;
	unsigned long rgbrgb;

	write_command (PHILIPS_DISPOFF);
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
	write_command (PHILIPS_DISPON);
}

/*
 * Fill the LCD screen with a given 12-bit color.
 * The color value should have a format rrrrggggbbbb.
 */
void lcd_clear (lcd_t *lcd, unsigned color)
{
	unsigned i;

	write_command (PHILIPS_DISPOFF);
	write_command (PHILIPS_CASET);
	write_data (0);
	write_data (NCOL + 1);
	write_command (PHILIPS_PASET);
	write_data (0);
	write_data (NROW + 1);
	write_command (PHILIPS_RAMWR);
	for (i=0; i<(NROW+2)*(NCOL+2)/2; i++) {
		write_data (color >> 4);
		write_data ((color << 4) |
			      ((color >> 8) & 0xF));
		write_data (color);
	}
	write_command (PHILIPS_DISPON);

	lcd->row = 0;
	lcd->col = 0;
}

/*
 * Lights a single pixel in the specified color
 * at the specified x and y addresses
 */
void lcd_pixel (lcd_t *lcd, int x, int y, int color)
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
 * Draw a line in the specified color from (x0,y0) to (x1,y1).
 */
void lcd_line (lcd_t *lcd, int x0, int y0, int x1, int y1, int color)
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
	dy <<= 1;				       // dy is now 2*dy
	dx <<= 1;				       // dx is now 2*dx
	lcd_pixel (lcd, x0, y0, color);
	if (dx > dy) {
		fraction = dy - (dx >> 1);	       // same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;	       // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;		       // same as fraction -= 2*dy
			lcd_pixel (lcd, x0, y0, color);
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
			lcd_pixel (lcd, x0, y0, color);
		}
	}
}

//
// Draw a rectangle in the specified color from (x1,y1) to (x2,y2).
//
void lcd_rect (lcd_t *lcd, int x0, int y0, int x1, int y1, int color)
{
	lcd_line (lcd, x0, y0, x1, y0, color);
	lcd_line (lcd, x0, y1, x1, y1, color);
	lcd_line (lcd, x0, y0, x0, y1, color);
	lcd_line (lcd, x1, y0, x1, y1, color);
}

//
// Draw a filled rectangle in the specified color from (x1,y1) to (x2,y2).
//
// The best way to fill a rectangle is to take advantage of the "wrap-around" featute
// built into the Philips PCF8833 controller. By defining a drawing box, the memory can
// be simply filled by successive memory writes until all pixels have been illuminated.
//
void lcd_rect_filled (lcd_t *lcd, int x0, int y0, int x1, int y1, int color)
{
	int xmin, xmax, ymin, ymax, i;

	// calculate the min and max for x and y directions
	xmin = (x0 <= x1) ? x0 : x1;
	xmax = (x0 > x1) ? x0 : x1;
	ymin = (y0 <= y1) ? y0 : y1;
	ymax = (y0 > y1) ? y0 : y1;

	// specify the controller drawing box according to those limits
	write_command (PHILIPS_PASET);
	write_data (ymin + 1);
	write_data (ymax + 1);
	write_command (PHILIPS_CASET);
	write_data (xmin + 1);
	write_data (xmax + 1);
	write_command (PHILIPS_RAMWR);

	// loop on total number of pixels / 2
	for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++) {
		// use the color value to output three data bytes covering two pixels
		write_data (color >> 4);
		write_data ((color << 4) | ((color >> 8) & 0xF));
		write_data (color);
	}
}

/*
 * Draw a circle in the specified color at center (x0,y0) with radius.
 */
void lcd_circle (lcd_t *lcd, int x0, int y0, int radius, int color)
{
	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	lcd_pixel (lcd, x0, y0 + radius, color);
	lcd_pixel (lcd, x0, y0 - radius, color);
	lcd_pixel (lcd, x0 + radius, y0, color);
	lcd_pixel (lcd, x0 - radius, y0, color);
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		lcd_pixel (lcd, x0 + x, y0 + y, color);
		lcd_pixel (lcd, x0 - x, y0 + y, color);
		lcd_pixel (lcd, x0 + x, y0 - y, color);
		lcd_pixel (lcd, x0 - x, y0 - y, color);
		lcd_pixel (lcd, x0 + y, y0 + x, color);
		lcd_pixel (lcd, x0 - y, y0 + x, color);
		lcd_pixel (lcd, x0 + y, y0 - x, color);
		lcd_pixel (lcd, x0 - y, y0 - x, color);
	}
}

/*
 * Move cursor to given position.
 */
void lcd_move (lcd_t *lcd, int x, int y)
{
        if (y < 0 || y >= NROW || x < 0 || x >= NCOL)
		return;
	lcd->row = y;
	lcd->col = x;
}

/*
 * Set foreground and background colors.
 */
void lcd_color (lcd_t *lcd, int fg, int bg)
{
	lcd->foreground = fg;
	lcd->background = bg;
}

#if 0
/*
 * Calculate a width of text output.
 * Handles both fixed and proportional fonts.  Passed UTF8string.
 */
int lcd_text_width (lcd_t *lcd, const unsigned char *text, int nchars)
{
	int width;
	unsigned c;

	/* TODO: UTF8 decoding. */
	if (! nchars)
		nchars = strlen (text);
	if (! lcd->font->width)
		return nchars * lcd->font->maxwidth;

	width = 0;
	while (--nchars >= 0) {
		c = *text++;
		if (c < lcd->font->firstchar || c >= lcd->font->firstchar + lcd->font->size)
			c = lcd->font->defaultchar;
		width += lcd->font->width [c - lcd->font->firstchar];
	}
	return width;
}
#endif

/*
 * Print one symbol. Decode from UTF8.
 * Some characters are handled specially.
 */
static void lcd_putchar (lcd_t *lcd, short c)
{
	unsigned i, j, cindex, width, glyph_row;
	unsigned long rgbrgb;
	const unsigned short *bits;

	switch (c) {
	case '\n':		/* ignore line feeds */
		lcd->row += lcd->font->height;
		return;
	case '\t':		/* tab replaced by space */
		c = ' ';
		break;
	case '\r':		/* carriage return - go to begin of line */
		lcd->col = 0;
		return;
	}

	/* Decode UTF-8. */
	if (! (c & 0x80)) {
		lcd->c1 = 0;
		lcd->c2 = 0;
	} else if (lcd->c1 == 0) {
		lcd->c1 = c;
		return;
	} else if (lcd->c2 == 0) {
		if (lcd->c1 & 0x20) {
			lcd->c2 = c;
			return;
		}
		c = (lcd->c1 & 0x1f) << 6 | (c & 0x3f);
		lcd->c1 = 0;
	} else {
		if (lcd->c1 == 0xEF && lcd->c2 == 0xBB && c == 0xBF) {
			/* Skip zero width no-break space. */
			lcd->c1 = 0;
			lcd->c2 = 0;
			return;
		}
		c = (lcd->c1 & 0x0f) << 12 |
			(lcd->c2 & 0x3f) << 6 | (c & 0x3f);
		lcd->c1 = 0;
		lcd->c2 = 0;
	}

	if (c < lcd->font->firstchar || c >= lcd->font->firstchar + lcd->font->size)
		c = lcd->font->defaultchar;
	cindex = c - lcd->font->firstchar;

	/* Get font bitmap depending on fixed pitch or not. */
	if (lcd->font->width) {
		/* Proportional font. */
		width = lcd->font->width [cindex];
		bits = lcd->font->bits + lcd->font->offset [cindex];
	} else {
		/* Fixed width font. */
		width = lcd->font->maxwidth;
		bits = lcd->font->bits + (lcd->font->height * cindex);
	}

	/* Scrolling. */
	if (lcd->col > NCOL - width) {
		lcd->col = 0;
		lcd->row += lcd->font->height;
		if (lcd->row > NROW - lcd->font->height)
			lcd->row = 0;
	}

	/* Draw a character. */
	write_command (PHILIPS_PASET);
	write_data (lcd->row + 1);
	write_data (lcd->row + lcd->font->height);
	write_command (PHILIPS_CASET);
	write_data (lcd->col + 1);
	write_data (lcd->col + 1 + ((width - 1) | 1));
	write_command (PHILIPS_RAMWR);

	/* Loop on each glyph row, backwards from bottom to top. */
	for (i=0; i<lcd->font->height; i++) {
		glyph_row = bits[i];

		/* Loop on every two pixels in the row (left to right). */
		for (j=0; j<width; j+=2) {
			/* Get rgb values for two successive pixels. */
			if (glyph_row & 0x8000)
				rgbrgb = (unsigned long) lcd->foreground << 12;
			else
				rgbrgb = (unsigned long) lcd->background << 12;
			if (glyph_row & 0x4000)
				rgbrgb |= lcd->foreground;
			else
				rgbrgb |= lcd->background;
			glyph_row <<= 2;

			/* Output three data bytes. */
			write_data (rgbrgb >> 16);
			write_data (rgbrgb >> 8);
			write_data (rgbrgb);
		}
	}
	write_command (PHILIPS_NOP);
	lcd->col += width;
}

void draw (unsigned page)
{
	lcd_clear (&display, LCD_BLACK);
	switch (page % 5) {
	case 0:
		/* Color test - show boxes of different colors. */
		lcd_rect_filled (&display, 0, 0, 129, 15, LCD_RED);
		lcd_move (&display, 0, 0);
		lcd_color (&display, LCD_WHITE, LCD_RED);
		puts (&display, "Красный");

		lcd_rect_filled (&display, 0, 16, 129, 16+15, LCD_GREEN);
		lcd_move (&display, 0, 16);
		lcd_color (&display, LCD_WHITE, LCD_GREEN);
		puts (&display, "Зелёный");

		lcd_rect_filled (&display, 0, 2*16, 129, 2*16+15, LCD_BLUE);
		lcd_move (&display, 0, 2*16);
		lcd_color (&display, LCD_WHITE, LCD_BLUE);
		puts (&display, "Синий");

		lcd_rect_filled (&display, 0, 3*16, 129, 3*16+15, LCD_MAGENTA);
		lcd_move (&display, 0, 3*16);
		lcd_color (&display, LCD_WHITE, LCD_MAGENTA);
		puts (&display, "Лиловый");

		lcd_rect_filled (&display, 0, 4*16, 129, 4*16+15, LCD_CYAN);
		lcd_move (&display, 0, 4*16);
		lcd_color (&display, LCD_BLACK, LCD_CYAN);
		puts (&display, "Бирюзовый");

		lcd_rect_filled (&display, 0, 5*16, 129, 5*16+15, LCD_YELLOW);
		lcd_move (&display, 0, 5*16);
		lcd_color (&display, LCD_BLACK, LCD_YELLOW);
		puts (&display, "Жёлтый");

		lcd_rect_filled (&display, 0, 6*16, 129, 6*16+15, LCD_WHITE);
		lcd_move (&display, 0, 6*16);
		lcd_color (&display, LCD_BLACK, LCD_WHITE);
		puts (&display, "Белый");

		lcd_move (&display, 0, 7*16);
		lcd_color (&display, LCD_WHITE, LCD_BLACK);
		puts (&display, "Чёрный");
		break;
	case 1:
		/* Boxes. */
		lcd_rect (&display,    0,    0, NCOL-1,      NROW-1,      LCD_CYAN);
		lcd_rect (&display,   11,   11, NCOL-1-11,   NROW-1-11,   LCD_GREEN);
		lcd_rect (&display, 2*11, 2*11, NCOL-1-2*11, NROW-1-2*11, LCD_YELLOW);
		lcd_rect (&display, 3*11, 3*11, NCOL-1-3*11, NROW-1-3*11, LCD_RED);
		lcd_rect (&display, 4*11, 4*11, NCOL-1-4*11, NROW-1-4*11, LCD_MAGENTA);
		lcd_rect (&display, 5*11, 5*11, NCOL-1-5*11, NROW-1-5*11, LCD_BLUE);
		break;
	case 4:
		/* Image. */
		lcd_image (&display, 0, 0, NROW, NCOL, image_frog);
		break;
	}
}

void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page % 5) {
	case 2:
		/* Rain. */
		if (radius == 0) {
			/* Generate next circle. */
			x0 = 10 + rand15() % (NCOL - 20);
			y0 = 10 + rand15() % (NROW - 20);
		} else {
			/* Clear previous circle. */
			lcd_circle (&display, x0, y0, radius, LCD_BLACK);
			lcd_circle (&display, x0, y0, radius-1, LCD_BLACK);
		}
		radius += 2;
		if (radius > 10)
			radius = 0;
		else {
			/* Draw next circle. */
			lcd_circle (&display, x0, y0, radius, LCD_WHITE);
			lcd_circle (&display, x0, y0, radius-1, LCD_WHITE);
		}
		break;
	case 3:
		/* Rectangles. */
		do {
			x0 = rand15() % NCOL;
			y0 = rand15() % NROW;
			x1 = rand15() % NCOL;
			y1 = rand15() % NROW;
		} while (abs (x0-x1) < 2 || abs (y0-y1) < 2);
		lcd_rect_filled (&display, x0, y0, x1, y1, rand15());
		break;
	}
}

/*
 * Task of polling buttons.
 */
void poll_buttons (void *arg)
{
	unsigned pagenum = 0;
	unsigned up_pressed = 0, left_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;

	draw (pagenum);
	for (;;) {
		mdelay (20);
		draw_next (pagenum);

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up: increase contrast. */
			lcd_contrast (&display, display.contrast + 1);
			lcd_move (&display, 10, 130-17);
			lcd_color (&display, LCD_WHITE, LCD_BLACK);
			printf (&display, "Contrast = %02x", display.contrast);
		}
		if (! joystick_left ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left button: show previous page of symbols. */
			draw (--pagenum);
		}
		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			draw (++pagenum);
		}
		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Up: decrease contrast. */
			lcd_contrast (&display, display.contrast - 1);
			lcd_move (&display, 10, 130-17);
			lcd_color (&display, LCD_WHITE, LCD_BLACK);
			printf (&display, "Contrast = %02x", display.contrast);
		}
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting LCD.\n");
	lcd_init (&display);
	task_create (poll_buttons, 0, "poll", 1, task, sizeof (task));
}
