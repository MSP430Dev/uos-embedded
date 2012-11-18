/*
 * Driver for Nokia 5110 display.
 * See Philips PCD8544 datasheet.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>

/*
 * Pinout for SainSmart Graphic LCD4884 Shield.
 */
#define MASKC_LCD_SCK   (1 << 2)    /* signal D2, pin RC2 */
#define MASKC_LCD_MOSI  (1 << 3)    /* signal D3, pin RC3 */
#define MASKC_LCD_DC    (1 << 4)    /* signal D4, pin RC4 */
#define MASKC_LCD_CS    (1 << 5)    /* signal D5, pin RC5 */
#define MASKC_LCD_RST   (1 << 6)    /* signal D6, pin RC6 */
#define MASKC_LCD_BL    (1 << 7)    /* signal D7, pin RC7 */

#define MAXROW  48
#define MAXCOL  84

static unsigned char gpanel_screen [MAXROW*MAXCOL/8];

static void lcd_write (unsigned byte, unsigned data_flag)
{
    unsigned i;

    LATCCLR = MASKC_LCD_CS;
    if (data_flag)
        LATCSET = MASKC_LCD_DC;
    else
        LATCCLR = MASKC_LCD_DC;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

    for (i=0; i<8; i++, byte<<=1) {
        if (byte & 0x80) {
            LATCSET = MASKC_LCD_MOSI;  /* SDIN = 1 */
        } else {
            LATCCLR = MASKC_LCD_MOSI;  /* SDIN = 0 */
        }
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        LATCCLR = MASKC_LCD_SCK;       /* SCLK = 0 */
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        LATCSET = MASKC_LCD_SCK;       /* SCLK = 1 */
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }
    LATCSET = MASKC_LCD_CS;
}

/*
 * Fill the LCD screen with a given color: black or white.
 */
void gpanel_clear (gpanel_t *gp, unsigned color)
{
    unsigned i;

    if (color)
        color = 0xFF;
    else
        color = 0;

    /* Clear data */
    lcd_write (0x40, 0);
    lcd_write (0x80, 0);
    for (i=0; i<MAXROW*MAXCOL/8; i++) {
        gpanel_screen[i] = color;
        lcd_write (color, 1);
    }
    gp->row = 0;
    gp->col = 0;
}

/*
 * Set up hardware for communication to Nokia 6100 LCD Display.
 * Internally, the controller has 132x132 pixels.
 * But visible area is only 130x130.
 */
void gpanel_init (gpanel_t *gp, gpanel_font_t *font)
{
    extern stream_interface_t gpanel_interface;

    gp->interface = &gpanel_interface;
    gp->nrow = MAXROW;
    gp->ncol = MAXCOL;
    gp->font = font;
    gp->foreground = GPANEL_WHITE;
    gp->background = GPANEL_BLACK;
    gp->contrast = 0x38;
    gp->row = 0;
    gp->col = 0;
    gp->c1 = 0;
    gp->c2 = 0;
#if 0
    /* Set pins as outputs. */
    LATCSET = MASKC_LCD_CS | MASKC_LCD_RST;
    TRISCCLR = MASKC_LCD_SCK | MASKC_LCD_MOSI | MASKC_LCD_DC |
               MASKC_LCD_CS  | MASKC_LCD_RST  | MASKC_LCD_BL;
#if 1
    /* Toggle chip select. */
    LATCCLR = MASKC_LCD_CS;
    udelay (1);

    /* Reset the display. */
    LATCCLR = MASKC_LCD_RST;
    udelay (1);
    LATCSET = MASKC_LCD_RST;
    udelay (1);
#endif
#if 1
    lcd_write (0x21, 0);
    lcd_write (0xbf, 0);
    lcd_write (0x04, 0);
    lcd_write (0x14, 0);
    lcd_write (0x20, 0);
//    gpanel_clear (gp, 0);
    lcd_write (0x0c, 0);
#endif
    /* Turn on backlight. */
    LATCSET = MASKC_LCD_BL;
    LATCSET = MASKC_LCD_CS;
#endif
}

/*
 * Lights a single pixel in the specified color
 * at the specified x and y addresses
 */
void gpanel_pixel (gpanel_t *gp, int x, int y, int color)
{
    unsigned char *data;

    if (x >= gp->ncol || y >= gp->nrow)
        return;
    data = &gpanel_screen [(y >> 3) * MAXCOL + x];

    if (color)
        *data |= 1 << (y & 7);
    else
        *data &= ~(1 << (y & 7));

    lcd_write (0x40 | x, 0);
    lcd_write (0x80 | (y >> 3), 0);
    lcd_write (*data, 1);
}

/*
 * Draw a part of glyph, up to 8 pixels in height.
 */
static void graw_glyph8 (gpanel_t *gp, unsigned width, unsigned height,
    const unsigned short *bits, unsigned ypage, unsigned yoffset)
{
    unsigned char *data;
    unsigned x, y;

    if (height > 8 - yoffset)
        height = 8 - yoffset;
    if (width > 32)
        width = 32;
    data = &gpanel_screen [ypage * MAXCOL + gp->col % MAXCOL];
    /*debug_printf ("glyph8 %ux%u at %u-%u offset %u-%u\n", width, height, gp->col, gp->row, ypage, yoffset);*/

    /* Clear glyph background. */
    if (yoffset == 0 && height == 8) {
        for (x=0; x<width; x++)
            data[x] = 0;
    } else {
        unsigned mask = ~(((1 << height) - 1) << yoffset);
        for (x=0; x<width; x++)
            data[x] &= mask;
    }

    /* Place glyph image. */
    unsigned words_per_row = (width + 15) / 16;
    for (x=0; x<width; x++) {
        for (y=0; y<height; y++) {
            if (bits [y*words_per_row + x/16] & (0x8000 >> (x & 15))) {
                data[x] |= 1 << (y + yoffset);
            }
        }
    }

    /* Write graphics memory. */
    lcd_write (0x40 | gp->col, 0);
    lcd_write (0x80 | ypage, 0);
    for (x=0; x<width; x++) {
        lcd_write (data[x], 1);
    }
}

/*
 * Draw a glyph of one symbol.
 */
void gpanel_glyph (gpanel_t *gp, unsigned width, const unsigned short *bits)
{
    unsigned ypage = gp->row >> 3;
    unsigned yoffset = gp->row & 7;
    unsigned words_per_row = (width + 15) / 16;
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
        bits += (8 - yoffset) * words_per_row;
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
    /* Temporary solution */
    int xmin, xmax, ymin, ymax, x, y;

    /* calculate the min and max for x and y directions */
    if (x0 <= x1) {
        xmin = x0;
        xmax = x1;
    } else {
        xmin = x1;
        xmax = x0;
    }
    if (y0 <= y1) {
        ymin = y0;
        ymax = y1;
    } else {
        ymin = y1;
        ymax = y0;
    }
    for (y=ymin; y<=ymax; y++)
        for (x=xmin; x<=xmax; x++)
            gpanel_pixel (gp, x, y, color);
}

/*
 * No contrast control.
 */
void gpanel_contrast (gpanel_t *gp, int contrast)
{
}

/*
 * Backlight control.
 */
void gpanel_backlight (gpanel_t *gp, int on)
{
    // TODO
}

/*
 * Write an image to LCD screen.
 */
void gpanel_image (gpanel_t *gp, int x, int y, int width, int height,
    const unsigned short *data)
{
    // TODO
}
