#include "runtime/lib.h"
#include "kernel/uos.h"
#include "stream/stream.h"
#include "timer/timer.h"
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
 * Olimex LED indicator has a non-standard encoding table.
 * Here is a mapping table for KOI8-R output.
 */
static const unsigned char encoding [256]  = {
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
	0040, 0041, 0042, 0043, 0044, 0045, 0046, 0047, /*  !"#$%&' */
	0050, 0051, 0052, 0053, 0054, 0055, 0056, 0057,	/* ()*+,-./ */
	0060, 0061, 0062, 0063, 0064, 0065, 0066, 0067,	/* 01234567 */
	0070, 0071, 0072, 0073, 0074, 0075, 0076, 0077,	/* 89:;<=>? */

	0100, 0101, 0102, 0103, 0104, 0105, 0106, 0107, /* @ABCDEFG */
	0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117, /* HIJKLMNO */
	0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127, /* PQRSTUVW */
	0130, 0131, 0132, 0133, 0004, 0135, 0136, 0137, /* XYZ[\]^_ */
	0140, 0141, 0142, 0143, 0144, 0145, 0146, 0147, /* `abcdefg */
	0150, 0151, 0152, 0153, 0154, 0155, 0156, 0157, /* hijklmno */
	0160, 0161, 0162, 0163, 0164, 0165, 0166, 0167, /* pqrstuvw */
	0170, 0171, 0172, 0005, 0321, 0006, 0351, 0377, /* xyz{|}~\177 */

	0134, 0173, 0174, 0175, 0176, 0177, 0347, 0376,	/* ........ */
	0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317,	/* ........ */
	0320, 0321, 0322, 0323, 0324, 0325, 0326, 0327,	/* ........ */
	0330, 0331, 0332, 0333, 0334, 0335, 0336, 0337,	/* ........ */
	0350, 0355, 0352, 0265, 0353, 0354, 0355, 0356, /* ...ё.... */
	0357, 0360, 0361, 0362, 0363, 0364, 0365, 0366,	/* ........ */
	0367, 0370, 0371, 0242, 0372, 0373, 0374, 0375,	/* ...Ё.... */
	0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,	/* ........ */

	0306, 0141, 0262, 0345, 0343, 0145, 0344, 0264, /* юабцдефг */
	0170, 0270, 0271, 0272, 0273, 0274, 0275, 0157, /* хийклмно */
	0276, 0307, 0160, 0143, 0277, 0171, 0266, 0263, /* пярстужв */
	0304, 0303, 0267, 0301, 0305, 0346, 0300, 0302, /* ьызшэщчъ */
	0260, 0101, 0240, 0341, 0340, 0105, 0252, 0241, /* ЮАБЦДЕФГ */
	0130, 0245, 0246, 0113, 0247, 0115, 0110, 0117, /* ХИЙКЛМНО */
	0250, 0261, 0120, 0103, 0124, 0251, 0243, 0102, /* ПЯРСТУЖВ */
	0007, 0256, 0244, 0254, 0257, 0342, 0253, 0255, /* ЬЫЗШЭЩЧЪ */
};

/*
 * Some symbols are missing.
 * Prepare glyphs for download.
 */
#define ROW(a,b,c,d,e) (a<<4 | b<<3 | c<<2 | d<<1 | e)

static const char backslash [8]  = {
	ROW( 0,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,0,0,1 ),
	ROW( 0,0,0,0,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char leftbrace [8]  = {
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char rightbrace [8]  = {
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,0,1,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,0,1,0,0 ),
	ROW( 0,1,0,0,0 ),
	ROW( 0,0,0,0,0 ),
};
static const char softsign [8]  = {
	ROW( 1,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 1,0,0,0,0 ),
	ROW( 1,1,1,0,0 ),
	ROW( 1,0,0,1,0 ),
	ROW( 1,0,0,1,0 ),
	ROW( 1,1,1,0,0 ),
	ROW( 0,0,0,0,0 ),
};

/*
 * Microsecond delay.
 */
static void usleep (small_uint_t usec)
{
	do {
		/* Two ticks for decrement and branch */
#if KHZ > 2000
		asm volatile ("nop");
#endif
#if KHZ > 3000
		asm volatile ("nop");
#endif
#if KHZ > 4000
		asm volatile ("nop");
#endif
#if KHZ > 5000
		asm volatile ("nop");
#endif
#if KHZ > 6000
		asm volatile ("nop");
#endif
#if KHZ > 7000
		asm volatile ("nop");
#endif
#if KHZ > 8000
		asm volatile ("nop");
#endif
#if KHZ > 9000
		asm volatile ("nop");
#endif
#if KHZ > 10000
		asm volatile ("nop");
#endif
#if KHZ > 11000
		asm volatile ("nop");
#endif
#if KHZ > 12000
		asm volatile ("nop");
#endif
#if KHZ > 13000
		asm volatile ("nop");
#endif
#if KHZ > 14000
		asm volatile ("nop");
#endif
#if KHZ > 15000
		asm volatile ("nop");
#endif
#if KHZ > 16000
		asm volatile ("nop");
#endif
	} while (--usec);
}

/*
 * Millisecond delay.
 */
static void msleep (small_uint_t usec)
{
	while (usec-- > 0) {
		usleep (250);
		usleep (250);
		usleep (250);
		usleep (250);
	}
}

/*
 * Convert a symbol from Unicode-16 to KOI8-R.
 */
static unsigned char
unicode_to_koi8 (unsigned short val)
{
	static const unsigned char tab0 [128] = {
/* 00 - 07 */ 	0,     0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07,
/* 08 - 0f */ 	0x08,  0x09,  0x0a,  0x0b,  0x0c,  0x0d,  0x0e,  0x0f,
/* 10 - 17 */ 	0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,
/* 18 - 1f */ 	0x18,  0x19,  0x1a,  0x1b,  0x1c,  0x1d,  0x1e,  0x1f,
/*  !"#$%&' */	0x20,  0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,
/* ()*+,-./ */	0x28,  0x29,  0x2a,  0x2b,  0x2c,  0x2d,  0x2e,  0x2f,
/* 01234567 */	0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,
/* 89:;<=>? */	0x38,  0x39,  0x3a,  0x3b,  0x3c,  0x3d,  0x3e,  0x3f,
/* @ABCDEFG */	0x40,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,
/* HIJKLMNO */	0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d,  0x4e,  0x4f,
/* PQRSTUVW */	0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,
/* XYZ[\]^_ */	0x58,  0x59,  0x5a,  0x5b,  0x5c,  0x5d,  0x5e,  0x5f,
/* `abcdefg */	0x60,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,
/* hijklmno */	0x68,  0x69,  0x6a,  0x6b,  0x6c,  0x6d,  0x6e,  0x6f,
/* pqrstuvw */	0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,
/* xyz{|}~  */	0x78,  0x79,  0x7a,  0x7b,  0x7c,  0x7d,  0x7e,  0x7f,
	};
	static const unsigned char tab4 [128] = {
/* 00 - 07 */ 	0,     0xb3,  0,     0,     0,     0,     0,     0,
/* 08 - 0f */ 	0,     0,     0,     0,     0,     0,     0,     0,
/* 10 - 17 */	0xe1,  0xe2,  0xf7,  0xe7,  0xe4,  0xe5,  0xf6,  0xfa,
/* 18 - 1f */ 	0xe9,  0xea,  0xeb,  0xec,  0xed,  0xee,  0xef,  0xf0,
/* 20 - 27 */	0xf2,  0xf3,  0xf4,  0xf5,  0xe6,  0xe8,  0xe3,  0xfe,
/* 28 - 2f */	0xfb,  0xfd,  0xff,  0xf9,  0xf8,  0xfc,  0xe0,  0xf1,
/* 30 - 37 */	0xc1,  0xc2,  0xd7,  0xc7,  0xc4,  0xc5,  0xd6,  0xda,
/* 38 - 3f */	0xc9,  0xca,  0xcb,  0xcc,  0xcd,  0xce,  0xcf,  0xd0,
/* 40 - 47 */	0xd2,  0xd3,  0xd4,  0xd5,  0xc6,  0xc8,  0xc3,  0xde,
/* 48 - 4f */	0xdb,  0xdd,  0xdf,  0xd9,  0xd8,  0xdc,  0xc0,  0xd1,
/* 50 - 57 */	0,     0xa3,  0,     0,     0,     0,     0,     0,
/* 58 - 5f */ 	0,     0,     0,     0,     0,     0,     0,     0,
/* 60 - 67 */ 	0,     0,     0,     0,     0,     0,     0,     0,
/* 68 - 6f */ 	0,     0,     0,     0,     0,     0,     0,     0,
/* 70 - 77 */ 	0,     0,     0,     0,     0,     0,     0,     0,
/* 78 - 7f */ 	0,     0,     0,     0,     0,     0,     0,     0,
	};
	switch (val & ~0x7f) {
	case 0x0000:
		return readb (tab0 + val);
	case 0x0400:
		return readb (tab4 + (val & 0x7f));
	}
	return 0;
}

static void lcd_pulse ()
{
	PORTC |= E;
	usleep (5);
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

	msleep (2);
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

	msleep (2);
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
	msleep (110);

	/* Initialize 4-bit bus. */
	PORTC |= D5 | D4;
	lcd_pulse ();
	msleep (10);

	lcd_pulse ();
	msleep (10);

	lcd_pulse ();
	msleep (10);

	PORTC &= ~D4;
	lcd_pulse ();
	msleep (10);

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
		c = unicode_to_koi8 (c);
		if (c == 0)
			c = 0215;

		/* Convert to LCD encoding. */
		c = readb (encoding + c);
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
