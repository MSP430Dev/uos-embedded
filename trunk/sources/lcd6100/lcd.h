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
	struct _stream_interface_t *interface;	/* for printf */
	lcd_font_t *font;		/* selected font */
	unsigned foreground;		/* color for putchar */
	unsigned background;		/* background for putchar */
	unsigned contrast;		/* current contrast */
	unsigned row;			/* current row */
	unsigned col;			/* current column */
	unsigned c1, c2;		/* utf8 decoder */
} lcd_t;

/*
 * Internally, the controller has 132x132 pixels.
 * But visible area is only 130x130.
 */
#define LCD_NROW 	130
#define LCD_NCOL 	130

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

void lcd_init (lcd_t *lcd);
void lcd_backlight (lcd_t *lcd, int on);
void lcd_contrast (lcd_t *lcd, int contrast);
void lcd_pixel (lcd_t *lcd, int x, int y, int color);
void lcd_line (lcd_t *lcd, int x0, int y0, int x1, int y1, int color);
void lcd_rect (lcd_t *lcd, int x0, int y0, int x1, int y1, int color);
void lcd_rect_filled (lcd_t *lcd, int x0, int y0, int x1, int y1, int color);
void lcd_circle (lcd_t *lcd, int x0, int y0, int radius, int color);
void lcd_image (lcd_t *lcd, int x, int y, int width, int height,
	const unsigned short *data);
void lcd_clear (lcd_t *lcd, unsigned color);
void lcd_move (lcd_t *lcd, int x, int y);
void lcd_color (lcd_t *lcd, int fg, int bg);
int lcd_text_width (lcd_t *lcd, const unsigned char *text);

extern lcd_font_t font_lucidasans11;
