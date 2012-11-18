/*
 * Проверка LCD-индикатора LCD4884 на плате SainSmart Graphic Shield.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "shield-lcd4884.h"

#define MASKB_LED1	(1 << 15)   /* RB15: green */
#define MASKA_LED2	(1 << 10)   /* RA10: red */

gpanel_t display;

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

static void draw (unsigned page)
{
        unsigned y;

	gpanel_clear (&display, 0);
	switch (page) {
	case 0:
		/* Show text. */
		gpanel_move (&display, 0, 0);
		puts (&display, "Hello World\r\n");
		break;
	case 1:
		/* Boxes. */
lcd_write (0x40, 0);
lcd_write (0x80, 0);
for (y=0; y<8; y++)
    lcd_write (0xff, 1);

//		for (y=0; y<display.ncol/4; y+=5)
//			gpanel_rect (&display, y+y, y, display.ncol-1-y-y, display.nrow-1-y, 1);
		break;
	}
}

static void draw_next (unsigned page)
{
	static int x0, y0, radius;
	int x1, y1;

	switch (page) {
	case 2:
		/* Rain. */
		if (radius == 0) {
			/* Generate next circle. */
			x0 = 10 + rand15() % (display.ncol - 20);
			y0 = 10 + rand15() % (display.nrow - 20);
		} else {
			/* Clear previous circle. */
			gpanel_circle (&display, x0, y0, radius, 0);
			gpanel_circle (&display, x0, y0, radius-1, 0);
		}
		radius += 2;
		if (radius > 10)
			radius = 0;
		else {
			/* Draw next circle. */
			gpanel_circle (&display, x0, y0, radius, 1);
			gpanel_circle (&display, x0, y0, radius-1, 1);
			mdelay (20);
		}
		break;
	case 3:
		/* Rectangles. */
		do {
			x0 = rand15() % display.ncol;
			y0 = rand15() % display.nrow;
			x1 = rand15() % display.ncol;
			y1 = rand15() % display.nrow;
		} while (abs (x0-x1) < 2 || abs (y0-y1) < 2);
		gpanel_rect (&display, x0, y0, x1, y1, 1);
		break;
	}
}

int main (void)
{
	unsigned pagenum = 1;
	unsigned left_pressed = 0;
	unsigned right_pressed = 0;
	extern gpanel_font_t font_fixed6x8;

        /* Disable JTAG and Trace ports, to make more pins available. */
        DDPCONCLR = 3 << 2;

        /* Use all ports as digital. */
        ANSELA = 0;
        ANSELB = 0;
        ANSELC = 0;

	LATBCLR = MASKB_LED1;
	TRISBCLR = MASKB_LED1;
	LATACLR = MASKA_LED2;
	TRISACLR = MASKA_LED2;

//	joystick_init ();
int i;
for (i=0; i<5; i++) { LATBINV = MASKB_LED1; udelay (100000); }
#if 1

    /* Set pins as outputs. */
    LATCSET = MASKC_LCD_CS | MASKC_LCD_RST;
    TRISCCLR = MASKC_LCD_SCK | MASKC_LCD_MOSI | MASKC_LCD_DC |
               MASKC_LCD_CS  | MASKC_LCD_RST  | MASKC_LCD_BL;

    /* Toggle chip select. */
    LATCCLR = MASKC_LCD_CS;
    udelay (1);

    /* Reset the display. */
    LATCCLR = MASKC_LCD_RST;
    udelay (1);
    LATCSET = MASKC_LCD_RST;
    udelay (1);

    lcd_write (0x21, 0);
    lcd_write (0xbf, 0);
    lcd_write (0x04, 0);
    lcd_write (0x14, 0);
    lcd_write (0x20, 0);
    lcd_write (0x0c, 0);

    /* Turn on backlight. */
    LATCSET = MASKC_LCD_BL;
    LATCSET = MASKC_LCD_CS;

    /* Clear data */
    lcd_write (0x40, 0);
    lcd_write (0x80, 0);
    for (i=0; i<504; i++) {
        lcd_write (0, 1);
    }
#endif
	gpanel_init (&display, &font_fixed6x8);
for (i=0; i<5; i++) { LATAINV = MASKA_LED2; udelay (100000); }

	draw (pagenum);
for (i=0; i<5; i++) { LATBINV = MASKB_LED1; udelay (100000); }

	/*
	 * Poll buttons.
	 */
	for (;;) {
		LATBINV = MASKB_LED1;
		mdelay (20);
		draw_next (pagenum);

                unsigned key = joystick_get();

		if (key != JOYSTICK_LEFT)
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left button: show previous page of symbols. */
			pagenum = (pagenum - 1) % 4;
			draw (pagenum);
		}

		if (key != JOYSTICK_RIGHT)
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right button: show next page of symbols. */
			pagenum = (pagenum + 1) % 4;
			draw (pagenum);
		}
	}
}
