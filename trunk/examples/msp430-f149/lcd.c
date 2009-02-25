#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include "lcd.h"

/*
 * LCD indicator is connected to port C.
 */
#define RS	0b00000001	/* 0 - command, 1 - data */
#define RW	0b00000010	/* 0 - write, 1 - read */
#define E	0b00000100	/* strobe */
#define D4	0b00010000	/* data */
#define D5	0b00100000
#define D6	0b01000000
#define D7	0b10000000

/*
 * LCD commands.
 */
#define DISP_CLR	0x01
#define DISP_ON		0x0C
#define DISP_OFF	0x08
#define CUR_HOME        0x02
#define CUR_OFF 	0x0C
#define CUR_ON_UNDER    0x0E
#define CUR_ON_BLINK    0x0F
#define CUR_LEFT        0x10
#define CUR_RIGHT       0x14
#define CUR_UP  	0x80
#define CUR_DOWN	0xC0
#define DD_RAM_ADDR	0x80
#define DD_RAM_ADDR2	0xC0

static stream_interface_t lcd_interface = {
	(void (*) (stream_t*, short)) lcd_putchar,
	0, 0,
};

/*
 * Some symbols are missing.
 * Prepare glyphs for download.
 */
static const char backslash [8]  = {
	0b00000,
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00000,
	0b00000,
};
static const char leftbrace [8]  = {
	0b00010,
	0b00100,
	0b00100,
	0b01000,
	0b00100,
	0b00100,
	0b00010,
	0b00000,
};
static const char rightbrace [8]  = {
	0b01000,
	0b00100,
	0b00100,
	0b00010,
	0b00100,
	0b00100,
	0b01000,
	0b00000,
};
static const char softsign [8]  = {
	0b10000,
	0b10000,
	0b10000,
	0b11100,
	0b10010,
	0b10010,
	0b11100,
	0b00000,
};

/*
 * Convert a symbol from Unicode-16 to local encoding.
 * Olimex LED indicator K2-1602K-FSY-YBW-R has a non-standard charset,
 * which includes ASCII and cyrillic letters.
 */
static unsigned char
unicode_to_local (unsigned short val)
{
	static const unsigned char tab0 [128] = {
/* 00 - 07 */	0,    0,    0,    0,    0,    0,    0,    0,
/* 08 - 0f */	0,    0,    0,    0,    0,    0,    0,    0,
/* 10 - 17 */	0,    0,    0,    0,    0,    0,    0,    0,
/* 18 - 1f */	0,    0,    0,    0,    0,    0,    0,    0,
/*  !"#$%&' */	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
/* ()*+,-./ */	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
/* 01234567 */	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
/* 89:;<=>? */	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
/* @ABCDEFG */	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
/* HIJKLMNO */	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
/* PQRSTUVW */	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
/* XYZ[\]^_ */	0x58, 0x59, 0x5a, 0x5b, 0x04, 0x5d, 0x5e, 0x5f,
/* `abcdefg */	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
/* hijklmno */	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
/* pqrstuvw */	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
/* xyz{|}~  */	0x78, 0x79, 0x7a, 0x05, 0xd1, 0x06, 0xe9, 0xff,
	};
	static const unsigned char tab4 [128] = {
/* 400 - 407 */	0,    0xa2, 0,    0,    0,    0,    0,    0,
/* 408 - 40f */	0,    0,    0,    0,    0,    0,    0,    0,
/* 410 - 417 */	0x41, 0xa0, 0x42, 0xa1, 0xe0, 0x45, 0xa3, 0xa4,
/* 418 - 41f */	0xa5, 0xa6, 0x4b, 0xa7, 0x4d, 0x48, 0x4f, 0xa8,
/* 420 - 427 */	0x50, 0x43, 0x54, 0xa9, 0xaa, 0x58, 0xe1, 0xab,
/* 428 - 42f */	0xac, 0xe2, 0xad, 0xae, 0x07, 0xaf, 0xb0, 0xb1,
/* 430 - 437 */	0x61, 0xb2, 0xb3, 0xb4, 0xe3, 0x65, 0xb6, 0xb7,
/* 438 - 43f */	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0x6f, 0xbe,
/* 440 - 447 */	0x70, 0x63, 0xbf, 0x79, 0xe4, 0x78, 0xe5, 0xc0,
/* 448 - 44f */	0xc1, 0xe6, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
/* 450 - 457 */	0,    0xb5, 0,    0,    0,    0,    0,    0,
/* 458 - 45f */	0,    0,    0,    0,    0,    0,    0,    0,
/* 460 - 467 */	0,    0,    0,    0,    0,    0,    0,    0,
/* 468 - 46f */	0,    0,    0,    0,    0,    0,    0,    0,
/* 470 - 477 */	0,    0,    0,    0,    0,    0,    0,    0,
/* 478 - 47f */	0,    0,    0,    0,    0,    0,    0,    0,
	};
	switch (val & ~0x7f) {
	case 0x0000:
		/* ASCII */
		return readb (tab0 + val);
	case 0x0400:
		/* Cyrillic */
		return readb (tab4 + (val & 0x7f));
	}
	return 0;
}

static void lcd_pulse ()
{
	PORTC |= E;
	udelay (5);
	PORTC &= ~E;
}

/*
 * Write command.
 */
static void lcd_write_ctl (unsigned char val)
{
	/* Set RS port to 0 */
	unsigned char lowbits = PORTC & ~(D7 | D6 | D5 | D4 | RS);

	/* High nibble. */
	PORTC = (val & 0b11110000) | lowbits;
	lcd_pulse ();

	/* Low nibble. */
	PORTC = (val << 4) | lowbits;
	lcd_pulse ();

	mdelay (2);
}

/*
 * Write a byte of data.
 */
static void lcd_write_data (unsigned char val)
{
	/* Set RS port to 1 */
	unsigned char lowbits = (PORTC & ~(D7 | D6 | D5 | D4)) | RS;

	/* High nibble. */
	PORTC = (val & 0b11110000) | lowbits;
	lcd_pulse ();

	/* Low nibble. */
	PORTC = (val << 4) | lowbits;
	lcd_pulse ();

	mdelay (2);
}

/*
 * Initialize LCD controller. The screen contains two lines.
 * Every line could be printed independently.
 * When timer is give, then wide messages are slowly scrolled.
 */
void lcd_init (lcd_t *line1, lcd_t *line2, timer_t *timer)
{
	unsigned char i;

	PORTC &= ~(RS | RW | E | D4 | D5 | D6 | D7);
	DDRC |= RS | RW | E | D4 | D5 | D6 | D7;
	mdelay (110);

	/* Initialize 4-bit bus. */
	PORTC |= D5 | D4;
	lcd_pulse ();
	mdelay (10);

	lcd_pulse ();
	mdelay (10);

	lcd_pulse ();
	mdelay (10);

	PORTC &= ~D4;
	lcd_pulse ();
	mdelay (10);

	/* Clear screen */
	lcd_write_ctl (DISP_CLR);

	/* Enable display */
	lcd_write_ctl (DISP_ON);

	line1->interface = &lcd_interface;
	line1->timer = timer;
	line1->base = DD_RAM_ADDR;
	line1->col = 0;
	line1->c1 = 0;
	line1->c2 = 0;

	line2->interface = &lcd_interface;
	line2->timer = timer;
	line2->base = DD_RAM_ADDR2;
	line2->col = 0;
	line2->c1 = 0;
	line2->c2 = 0;

	for (i=0; i<NCOL; ++i) {
		line1->data[i] = ' ';
		line2->data[i] = ' ';
	}

	/* Load missing glyphs: 4 - \, 5 - {, 6 - }, 7 - Ь */
	lcd_load_glyph (4, backslash);
	lcd_load_glyph (5, leftbrace);
	lcd_load_glyph (6, rightbrace);
	lcd_load_glyph (7, softsign);
}

/*
 * Load a symbol glyph.
 * Eight symbols with codes 0-7 are loadable.
 * Glyph data must be placed in flash memory.
 */
void lcd_load_glyph (char n, const char *data)
{
	unsigned char i;

	lcd_write_ctl (0x40 + n * 8);		/* установка адреса */
	for (i=0; i<8; ++i)
		lcd_write_data (readb (data++));
}

/*
 * Clear a single line.
 */
void lcd_clear (lcd_t *line)
{
	unsigned char i;

	lcd_write_ctl (line->base);		/* установка адреса */
	for (i=0; i<NCOL; ++i) {
		lcd_write_data (' ');
		line->data[i] = ' ';
	}
	line->col = 0;
	line->c1 = 0;
	line->c2 = 0;
}

/*
 * Clear all screen.
 */
void lcd_clear_all (lcd_t *line1, lcd_t *line2)
{
	unsigned char i;

	/* Стираем экран */
	lcd_write_ctl (DISP_CLR);

	for (i=0; i<NCOL; ++i) {
		line1->data[i] = ' ';
		line2->data[i] = ' ';
	}
	line1->col = 0;
	line1->c1 = 0;
	line1->c2 = 0;
	line2->col = 0;
	line2->c1 = 0;
	line2->c2 = 0;
}

/*
 * Scroll a line by one character to the left.
 */
static void lcd_scroll (lcd_t *line)
{
	unsigned char i, c;

	if (line->col <= 0)
		return;

	lcd_write_ctl (line->base);		/* setup address */
	for (i=1; i<line->col; ++i) {
		c = line->data[i];
		lcd_write_data (c);
		line->data[i-1] = c;
	}
	lcd_write_data (' ');
	--line->col;
}

/*
 * Move cursor to given position.
 */
void lcd_move (lcd_t *line, int col)
{
	if (col < 0 || col >= NCOL)
		return;
	line->col = col;
}

/*
 * Print one symbol. Decode from UTF8 to local encoding (if not raw mode).
 * Some characters are handled specially.
 */
void lcd_putchar (lcd_t *line, short c)
{
	switch (c) {
	case '\n':		/* ignore line feeds */
		return;
	case '\t':		/* tab replaced by space */
		c = ' ';
		break;
	case '\f':		/* page feed - clear line */
		lcd_clear (line);
		return;
	case '\r':		/* carriage return - go to begin of line */
		line->col = 0;
		return;
	}

	if (! line->raw) {
		/* Decode UTF-8. */
		if (! (c & 0x80)) {
			line->c1 = 0;
			line->c2 = 0;
		} else if (line->c1 == 0) {
			line->c1 = c;
			return;
		} else if (line->c2 == 0) {
			if (line->c1 & 0x20) {
				line->c2 = c;
				return;
			}
			c = (line->c1 & 0x1f) << 6 | (c & 0x3f);
			line->c1 = 0;
		} else {
			if (line->c1 == 0xEF && line->c2 == 0xBB && c == 0xBF) {
				/* Skip zero width no-break space. */
				line->c1 = 0;
				line->c2 = 0;
				return;
			}
			c = (line->c1 & 0x0f) << 12 |
				(line->c2 & 0x3f) << 6 | (c & 0x3f);
			line->c1 = 0;
			line->c2 = 0;
		}
		/* Convert to LCD encoding. */
		c = unicode_to_local (c);
		if (c == 0)
			c = 0315;
	}
	if (c < 0 || c >= 256)
		c = 0315;

	if (line->col >= NCOL) {
		/* Scrolling. */
		if (line->timer)
			timer_delay (line->timer, 150);
		lcd_scroll (line);
	}
	lcd_write_ctl (line->base | line->col);	/* setup address */
	lcd_write_data (c);
	line->data [line->col++] = c;
}
