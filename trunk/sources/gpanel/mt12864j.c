/*
 * Driver for MELT MT-12864J LCD (Samsung KS0108B).
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

/* Флаги состояния LCD */
enum {
	BUSY	= 7,
	ONOFF	= 5,
	RESET	= 4,
};

static void set_crystal (gpanel_t *gp, int num)
{
	if (num & 64) {
		/* Кристалл #2. */
		gp->DATA = (unsigned*) 0x18200000;
		gp->CMD  = (unsigned*) 0x10200000;
		ARM_GPIOE->DATA = 1 << 5;
	} else {
		/* Кристалл #1. */
		gp->DATA = (unsigned*) 0x18100000;
		gp->CMD  = (unsigned*) 0x10100000;
		ARM_GPIOE->DATA = 1 << 4;
	}
	ARM_GPIOE->OE = 0x30;
	udelay (8);
}

static void wait_status (gpanel_t *gp, int status)
{
	unsigned stat;

	for (;;) {
		udelay (8);
		stat = *gp->CMD;
		udelay (8);
		if (stat != (1 << status))
			return;
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
 * Set up hardware for communication to Nokia 6100 LCD Display.
 * Internally, the controller has 132x132 pixels.
 * But visible area is only 130x130.
 */
void gpanel_init (gpanel_t *gp, gpanel_font_t *font)
{
	extern stream_interface_t gpanel_interface;
	int x;

	gp->interface = &gpanel_interface;
	gp->nrow = 63;
	gp->ncol = 127;
	gp->font = font;
	gp->foreground = GPANEL_WHITE;
	gp->background = GPANEL_BLACK;
	gp->contrast = 0x38;
	gp->row = 0;
	gp->col = 0;
	gp->c1 = 0;
	gp->c2 = 0;

	ARM_RSTCLK->PER_CLOCK = 0xFFFFFFFF;

	/* Инициализация портов внешней шины и выводов для работы с экраном */
        ARM_GPIOA->FUNC = 0x00005555;   /* Main Function для DATA[7:0] */
        ARM_GPIOA->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOA->PWR = 0x00005555;    /* Fast */

        ARM_GPIOE->FUNC = 0x00400500;   /* Main Function для ADDR[20,21,27] */
        ARM_GPIOE->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOE->PWR = 0x00400500;    /* Fast */

        ARM_GPIOC->FUNC = 0x15504010;   /* Main Function для RESET WE & CLOCK & KEYS*/
        ARM_GPIOC->ANALOG = 0xFFFF;     /* Digital */
        ARM_GPIOC->PWR = 0x0008C010;    /* Fast */

	/* Инициализация внешней шины */
        ARM_EXTBUS->CONTROL = 0x0000F001;

	/* Программный сброс экрана. */
	ARM_GPIOC->DATA = 0x00000200;
	ARM_GPIOC->OE = 0x00000200;
	for (x=0; x<255; x++)
		ARM_GPIOC->DATA = 0;
	ARM_GPIOC->DATA = 0x00000200;

	/* Инициализация всех кристаллов. */
	for (x=0; x<gp->ncol; x+=64) {
		set_crystal (gp, x);
		wait_status (gp, BUSY);

		*gp->CMD = 0x3F;		// LCD on
		udelay (8);
		wait_status (gp, ONOFF);

		*gp->CMD = 0xC0;		// start line 0
		udelay (8);
	}
}

/*
 * Fill the LCD screen with a given color: black or white.
 */
void gpanel_clear (gpanel_t *gp, unsigned color)
{
	unsigned x, i, j;

	if (color)
		color = 0xFF;
	else
		color = 0;

	/* Очистка данных для всех кристаллов */
	for (x=0; x<gp->ncol; x+=64) {
		set_crystal (gp, x);
		wait_status (gp, BUSY);

		*gp->CMD = 0x3E;		// LCD off
		udelay (8);

		for (i=0; i<8; i++) {
			*gp->CMD = 0xB8 + i;	// set page
			udelay (8);

			*gp->CMD = 0x40;	// set address 0
			udelay (8);

			for (j=0; j<64; j++) {
				*gp->DATA = color;
				udelay (8);
			}
		}
		*gp->CMD = 0x3F;		// LCD on
		udelay (8);
		wait_status (gp, ONOFF);
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
	unsigned data;

	if (x >= gp->ncol || y >= gp->nrow)
		return;

	set_crystal (gp, x);
	x &= 63;

	*gp->CMD = 0xB8 + (y >> 3);	// set page
	udelay (8);

	*gp->CMD = 0x40 + x;		// set address
	udelay (8);

	/* Первое чтение - необходимо для получения корректных данных */
	*gp->DATA;
	udelay (8);
	data = *gp->DATA;
	udelay (8);

	if (color)
		data |= 1 << (y & 7);
	else
		data &= ~(1 << (y & 7));

	*gp->CMD = 0x40 + x;		// set address
	udelay (8);

	*gp->DATA = (unsigned char) data;
	udelay (8);
}

static void graw_glyph8 (gpanel_t *gp, unsigned width, unsigned height,
	const unsigned short *bits, unsigned ypage, unsigned yoffset)
{
	unsigned char data [16];
	unsigned i, k, x, crystal = -1;

	if (height > 8 - yoffset)
		height = 8 - yoffset;
	if (width > 16)
		width = 16;
	/*debug_printf ("glyph8 %ux%u at %u-%u offset %u-%u\n", width, height, gp->col, gp->row, ypage, yoffset);*/

	/* Read graphics memory. */
	if (yoffset != 0 || height != 8) {
		x = gp->col;
		for (i=0; i<width; i++, x++) {
			if (crystal != (x & 64)) {
				crystal = (x & 64);
				set_crystal (gp, crystal);

				*gp->CMD = 0xB8 + ypage;	// set page
				udelay (8);

				*gp->CMD = 0x40 + (x & 63);	// set address
				udelay (8);

				/* Первое чтение - необходимо для получения корректных данных */
				*gp->DATA;
				udelay (8);
			}
			data[i] = *gp->DATA;
			udelay (8);
		}

		/* Clear glyph background. */
		unsigned mask = ~(((1 << height) - 1) << yoffset);
		for (i=0; i<width; i++)
			data[i] &= mask;
	} else
		memset (data, 0, width);

	/* Place glyph image. */
	for (i=0; i<width; i++) {
		for (k=0; k<height; k++) {
			if (bits[k] & (0x8000 >> i)) {
				data[i] |= 1 << (k + yoffset);
			}
		}
	}

	/* Write graphics memory. */
	x = gp->col;
	for (i=0; i<width; i++, x++) {
		if (crystal != (x & 64)) {
			crystal = (x & 64);
			set_crystal (gp, crystal);

			*gp->CMD = 0xB8 + ypage;	// set page
			udelay (8);

			*gp->CMD = 0x40 + (x & 63);	// set address
			udelay (8);

		} else if (i == 0) {
			*gp->CMD = 0x40 + (x & 63);	// set address
			udelay (8);
		}

		*gp->DATA = data[i];
		udelay (8);
	}
}

/*
 * Print one symbol. Decode from UTF8.
 * Some characters are handled specially.
 */
void gpanel_glyph (gpanel_t *gp, unsigned width, const unsigned short *bits)
{
	unsigned ypage = gp->row >> 3;
	unsigned yoffset = gp->row & 7;
	int height = gp->font->height;

	/*debug_printf ("<glyph %d at %d-%d>", width, gp->col, gp->row);*/
	for (;;) {
		graw_glyph8 (gp, width, height, bits, ypage, yoffset);
		height -= 8 - yoffset;
		if (height <= 0)
			break;
		ypage++;
		if (ypage >= 8)
			break;
		bits += 8 - yoffset;
		yoffset = 0;
	}
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
