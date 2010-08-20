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
        const unsigned short *offset;	/* offsets into bitmap data*/
        const unsigned char *width;	/* character widths or 0 if fixed*/
        int             defaultchar;	/* default char (not glyph index)*/
        long            bits_size;	/* # words of bits*/
} gpanel_font_t;

typedef struct {
	struct _stream_interface_t *interface;	/* for printf */
	unsigned nrow;			/* number of rows */
	unsigned ncol;			/* number of columns */
	gpanel_font_t *font;		/* selected font */
	unsigned foreground;		/* color for putchar */
	unsigned background;		/* background for putchar */
	unsigned contrast;		/* current contrast */
	unsigned row;			/* current row */
	unsigned col;			/* current column */
	unsigned c1, c2;		/* utf8 decoder */
	volatile unsigned *volatile CMD;		/* command register */
	volatile unsigned *volatile DATA;	/* data register */
} gpanel_t;

/* 12-bit color definitions */
#define GPANEL_WHITE	0xFFF
#define GPANEL_BLACK	0x000
#define GPANEL_RED	0xF00
#define GPANEL_GREEN	0x0F0
#define GPANEL_BLUE	0x00F
#define GPANEL_CYAN	0x0FF
#define GPANEL_MAGENTA	0xF0F
#define GPANEL_YELLOW	0xFF0
#define GPANEL_BROWN	0xB22
#define GPANEL_ORANGE	0xFA0
#define GPANEL_PINK	0xF6A

void gpanel_init (gpanel_t *lcd, gpanel_font_t *font);
void gpanel_backlight (gpanel_t *lcd, int on);
void gpanel_contrast (gpanel_t *lcd, int contrast);
void gpanel_pixel (gpanel_t *lcd, int x, int y, int color);
void gpanel_line (gpanel_t *lcd, int x0, int y0, int x1, int y1, int color);
void gpanel_rect (gpanel_t *lcd, int x0, int y0, int x1, int y1, int color);
void gpanel_rect_filled (gpanel_t *lcd, int x0, int y0, int x1, int y1, int color);
void gpanel_circle (gpanel_t *lcd, int x0, int y0, int radius, int color);
void gpanel_image (gpanel_t *lcd, int x, int y, int width, int height,
	const unsigned short *data);
void gpanel_glyph (gpanel_t *gp, unsigned width,
	const unsigned short *data);
void gpanel_clear (gpanel_t *lcd, unsigned color);
void gpanel_move (gpanel_t *lcd, int x, int y);
void gpanel_color (gpanel_t *lcd, int fg, int bg);
int gpanel_text_width (gpanel_t *lcd, const unsigned char *text);
