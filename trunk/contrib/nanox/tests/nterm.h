typedef struct _nterm_char_t {
	unsigned short	sym;
	GR_COLOR	fg;
	GR_COLOR	bg;
} nterm_char_t;

typedef struct _nterm_t {
	GR_SCREEN_INFO	screen;		/* screen info */
	GR_WINDOW_ID	topwin;		/* id for window */
	GR_GC_ID	draw_cursor;	/* graphics context */
	GR_GC_ID	draw_text;	/* graphics context */
	GR_GC_ID	clear_text;	/* graphics context */
	int		cols;		/* text columns */
	int		rows;		/* text rows */
	int		xpos;		/* x coord for text */
	int		ypos;		/* y coord for text */
	GR_SIZE		window_width;	/* width of window */
	GR_SIZE		window_height;	/* height of window */
	GR_SIZE		char_width;	/* width of character */
	GR_SIZE		char_height;	/* height of character */
	GR_SIZE		char_base;	/* height of baseline */
	GR_TIMER_ID	timer;		/* cursor blink timer */
	int		cursor_visible;	/* is cursor visible */
	int		cursor_blink;	/* cursor blinked */
	int		have_focus;	/* do we have focus */
	nterm_char_t	*text;
} nterm_t;

int nterm_run (nterm_t *u, int x, int y, int w, int h, pipe_t *pipe);
