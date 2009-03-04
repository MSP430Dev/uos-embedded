/*
 * Generic two-line LCD display.
 */
#include <stream/stream.h>

#define NCOL	16			/* Line width */

typedef struct {
	stream_interface_t *interface;	/* for printf */
	struct _timer_t *timer;		/* for scrolling */
	unsigned char base;		/* base address of this line */
	unsigned char col;		/* current column */
	unsigned char raw;		/* do not use Unicode mapping */
	unsigned char c1, c2;		/* utf8 decoder */
	unsigned char data [NCOL];	/* symbols */
} lcd_t;

/*
 * Initialization of LCD controller.
 * When timer is present, the printed line will be slowly scrolled.
 */
void lcd_init (lcd_t *line1, lcd_t *line2, struct _timer_t *timer);

/*
 * Load character image.
 * Eight symbols with codes 0-7 are loadable.
 */
void lcd_load_glyph (char n, const unsigned char *data);

/*
 * Clear the whole screen.
 */
void lcd_clear_all (lcd_t *line1, lcd_t *line2);

/*
 * Clear single line.
 */
void lcd_clear (lcd_t *line);

/*
 * Move cursor to given position.
 */
void lcd_move (lcd_t *line, int col);
