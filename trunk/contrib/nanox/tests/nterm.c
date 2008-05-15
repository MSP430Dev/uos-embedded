/*
 * Nano-X terminal emulator
 *
 * Al Riddoch
 * Greg Haerr
 * Serge Vakulenko
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <mem/mem.h>
#include <stream/stream.h>
#include <stream/pipe.h>
#include <posix/stdlib.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/nxcolors.h"

#include "nterm.h"

#define MIN_WIDTH(u)	((u)->char_width * 20 + 1)
#define MIN_HEIGHT(u)	((u)->char_height * 10)

#define	_	((unsigned) 0)		/* off bits */
#define	X	((unsigned) 1)		/* on bits */
#define	MASK(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
	((((((((((((((((a)*2+b)*2+c)*2+d)*2+e)*2+f)*2+g)*2+h)*2+ \
			i)*2+j)*2+k)*2+l)*2+m)*2+n)*2+o)*2+p)

static void redraw_cursor (nterm_t *u)
{
	if (u->have_focus)
		GrFillRect (u->topwin, u->draw_cursor,
			u->xpos * u->char_width + 1,
			u->ypos * u->char_height + 1,
			u->char_width, u->char_height - 1);
	else
		GrRect (u->topwin, u->draw_cursor,
			u->xpos * u->char_width + 1,
			u->ypos * u->char_height + 1,
			u->char_width, u->char_height - 1);
}

static void show_cursor (nterm_t *u)
{
	if (! u->cursor_visible) {
		u->cursor_visible = 1;
		redraw_cursor (u);
	}
}

static void hide_cursor (nterm_t *u)
{
	if (u->cursor_visible) {
		u->cursor_visible = 0;
		redraw_cursor (u);
	}
	u->cursor_blink = 0;
}

void nterm_clear (nterm_t *u)
{
	memset (u->text, 0, u->rows * u->cols * sizeof (nterm_char_t));
}

void nterm_scroll (nterm_t *u)
{
	memmove (u->text, u->text + u->cols,
		(u->rows-1) * u->cols * sizeof (nterm_char_t));
	memset (u->text + (u->rows-1) * u->cols, 0,
		u->cols * sizeof (nterm_char_t));
}

void nterm_store (nterm_t *u, int sym, GR_COLOR fg, GR_COLOR bg)
{
	nterm_char_t *ch = &u->text [u->ypos * u->cols + u->xpos];

	ch->sym = sym;
	ch->fg = fg;
	ch->bg = bg;
}

void nterm_resize (nterm_t *u, int w, int h)
{
	nterm_char_t *oldtext = u->text;
	int oldrows = u->rows;
	int oldcols = u->cols;
	int r, c;

	u->window_height = h;
	u->window_width = w;
	u->rows = u->window_height / u->char_height;
	u->cols = (u->window_width - 1) / u->char_width;
	if (u->cols < 1)
		u->cols = 1;
	if (u->rows < 1)
		u->rows = 1;

	u->text = calloc (u->rows * u->cols, sizeof (nterm_char_t));
	nterm_clear (u);

	if (oldtext) {
		/* Preserve old contents. */
		for (r=0; r<oldrows; ++r) {
			if (r >= u->rows)
				break;
			for (c=0; c<oldcols; ++c) {
				if (c >= u->cols)
					break;
				u->text [r * u->cols + c] =
					oldtext [r * oldcols + c];
			}
		}
		free (oldtext);
	}
	if (u->xpos >= u->rows)
		u->xpos = u->rows - 1;
	if (u->ypos >= u->cols)
		u->ypos = u->cols - 1;
}

void nterm_redraw (nterm_t *u, int x, int y, int w, int h)
{
	nterm_char_t *ch;
	int r, c, r0, c0, maxr, maxc;

	/* Redraw all contents. */
	c0 = x / u->char_width;
	r0 = y / u->char_height;
	maxc = (x + w) / u->char_width;
	maxr = (y + h) / u->char_height;
	if (maxc >= u->cols)
		maxc = u->cols - 1;
	if (maxr >= u->rows)
		maxr = u->rows - 1;

	if (c0 <= u->xpos && u->xpos <= maxc &&
	    r0 <= u->ypos && u->ypos <=maxr)
		u->cursor_visible = 0;

	for (r=r0; r<=maxr; ++r) {
		for (c=c0; c<=maxc; ++c) {
			ch = &u->text [r * u->cols + c];
			if (ch->sym < ' ')
				ch->sym = ' ';
			GrText (u->topwin, u->draw_text, c * u->char_width + 1,
				r * u->char_height, &ch->sym, 1, GR_TFTOP);
		}
	}
}

static void nterm_put_unicode (nterm_t *u, unsigned short ch)
{
	switch (ch) {
	case '\r':
	case '\n':
		hide_cursor (u);
		u->xpos = 0;
		++u->ypos;
		while (u->ypos >= u->rows) {
			--u->ypos;
			hide_cursor (u);
			GrCopyArea (u->topwin, u->draw_text, 1, 0,
				u->window_width - 1, u->ypos * u->char_height,
				u->topwin, 1, u->char_height, MWROP_SRCCOPY);
			GrFillRect (u->topwin, u->clear_text, 1,
				u->ypos * u->char_height, u->window_width - 1,
				u->window_height - u->ypos * u->char_height);
			nterm_scroll (u);
		}
		return;

	case '\f':				/* clear screen */
		hide_cursor (u);
		u->xpos = 0;
		u->ypos = 0;
		GrClearWindow (u->topwin, GR_FALSE);
		nterm_clear (u);
		return;

	case '\t':
		nterm_put_unicode (u, ' ');
		while (u->xpos & 7)
			nterm_put_unicode (u, ' ');
		return;

	case '\b':
		if (u->xpos <= 0)
			return;
		hide_cursor (u);
		--u->xpos;
		GrFillRect (u->topwin, u->clear_text,
			u->xpos * u->char_width + 1, u->ypos * u->char_height,
			u->char_width, u->char_height);
		return;

	case ' ':
		u->cursor_visible = 0;
		GrFillRect (u->topwin, u->clear_text,
			u->xpos * u->char_width + 1, u->ypos * u->char_height,
			u->char_width, u->char_height);
		nterm_store (u, ' ', GR_COLOR_GRAY75, GR_COLOR_BLACK);
		break;

	default:
		if (ch < ' ')
			return;
		u->cursor_visible = 0;
		GrText (u->topwin, u->draw_text,
			u->xpos * u->char_width + 1, u->ypos * u->char_height,
			&ch, 1, GR_TFTOP);
		nterm_store (u, ch, GR_COLOR_GRAY75, GR_COLOR_BLACK);
		break;
	}
	++u->xpos;

	/* Roll over right margin. */
	if (u->xpos >= u->cols)
		nterm_put_unicode (u, '\n');
}

int nterm_run (nterm_t *u, int x, int y, int w, int h, pipe_t *pipe)
{
	GR_EVENT	event;		/* current event */
	GR_BITMAP	bitmap_fg [15];	/* mouse cursor */
	GR_BITMAP	bitmap_bg [15];

	if (GrOpen() < 0) {
		debug_printf ("cannot open graphics\n");
		return 0;
	}

	GrGetScreenInfo (&u->screen);

	u->topwin = GrNewWindowEx (GR_WM_PROPS_APPFRAME | GR_WM_PROPS_CAPTION |
		GR_WM_PROPS_CLOSEBOX, (unsigned char*) "nterm", GR_ROOT_WINDOW_ID,
		x, y, w, h, GR_COLOR_BLACK);

	bitmap_fg[ 0] = MASK(_,_,X,X,X,_,X,X,X,_,_,_,_,_,_,_);
	bitmap_fg[ 1] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 2] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 3] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 4] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 5] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 6] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 7] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 8] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[ 9] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[10] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[11] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[12] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[13] = MASK(_,_,_,_,_,X,_,_,_,_,_,_,_,_,_,_);
	bitmap_fg[14] = MASK(_,_,X,X,X,_,X,X,X,_,_,_,_,_,_,_);

	bitmap_bg[ 0] = MASK(_,X,X,X,X,X,X,X,X,X,_,_,_,_,_,_);
	bitmap_bg[ 1] = MASK(_,_,X,X,X,X,X,X,X,_,_,_,_,_,_,_);
	bitmap_bg[ 2] = MASK(_,_,_,X,X,X,X,X,_,_,_,_,_,_,_,_);
	bitmap_bg[ 3] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 4] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 5] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 6] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 7] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 8] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[ 9] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[10] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[11] = MASK(_,_,_,_,X,X,X,_,_,_,_,_,_,_,_,_);
	bitmap_bg[12] = MASK(_,_,_,X,X,X,X,X,_,_,_,_,_,_,_,_);
	bitmap_bg[13] = MASK(_,_,X,X,X,X,X,X,X,_,_,_,_,_,_,_);
	bitmap_bg[14] = MASK(_,X,X,X,X,X,X,X,X,X,_,_,_,_,_,_);

	GrSetCursor (u->topwin, 11, 15, 5, 7, GR_COLOR_BLACK, GR_COLOR_WHITE,
		bitmap_fg, bitmap_bg);

	u->draw_text = GrNewGC ();

	GrSetGCForeground (u->draw_text, GR_COLOR_GRAY75);
	GrSetGCBackground (u->draw_text, GR_COLOR_BLACK);
	GrSetGCFont (u->draw_text, GrCreateFont ((unsigned char*) GR_FONT_SYSTEM_FIXED, 0, 0));

	GrGetGCTextSize (u->draw_text, "A", 1, GR_TFASCII,
		&u->char_width, &u->char_height, &u->char_base);
	nterm_resize (u, w, h);

	u->clear_text = GrNewGC ();
	GrSetGCForeground (u->clear_text, GR_COLOR_BLACK);
	GrSetGCBackground (u->clear_text, GR_COLOR_GRAY75);

	u->draw_cursor = GrNewGC ();
	GrSetGCForeground (u->draw_cursor, GR_COLOR_WHITE);
	GrSetGCBackground (u->draw_cursor, GR_COLOR_BLACK);
	GrSetGCMode (u->draw_cursor, GR_MODE_XOR);

	u->timer = GrCreateTimer (u->topwin, 150);

	GrRegisterInput ((int) &pipe->lock);
	GrSelectEvents (u->topwin, GR_EVENT_MASK_BUTTON_DOWN |
		GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_EXPOSURE |
		GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT |
		GR_EVENT_MASK_CLOSE_REQ | GR_EVENT_MASK_UPDATE |
		GR_EVENT_MASK_TIMER | GR_EVENT_MASK_FDINPUT);
	GrMapWindow (u->topwin);

	while (peekchar (&pipe->master) >= 0)
		nterm_put_unicode (u, getchar (&pipe->master));

	for (;;) {
		if (! u->cursor_blink)
			show_cursor (u);
		GrGetNextEvent (&event);
		/* if (event.type != GR_EVENT_TYPE_TIMER)
			debug_printf ("Event %d\n", event.type); */

		switch (event.type) {
		case GR_EVENT_TYPE_CLOSE_REQ:
			GrClose ();
			fclose (&pipe->master);
			return 1;

		case GR_EVENT_TYPE_EXPOSURE:
			nterm_redraw (u, event.exposure.x, event.exposure.y,
				event.exposure.width, event.exposure.height);
			break;

		case GR_EVENT_TYPE_UPDATE:
			if (event.update.utype != GR_UPDATE_SIZE)
				break;

			/* Window resized. */
			hide_cursor (u);
			nterm_resize (u, event.update.width,
				event.update.height);

			/* Do not allow resizing too small. */
			if (u->window_width < MIN_WIDTH(u) ||
			    u->window_height < MIN_HEIGHT(u)) {
				int w = MIN_WIDTH(u);
				int h = MIN_HEIGHT(u);

				if (w < u->window_width)
					w = u->window_width;
				if (h < u->window_height)
					h = u->window_height;
				GrResizeWindow (u->topwin, w, h);
			}
			break;

		case GR_EVENT_TYPE_KEY_DOWN:
			/* Keyboard press. */
			putchar (&pipe->master, event.keystroke.ch);
			break;

		case GR_EVENT_TYPE_FDINPUT:
			if (peekchar (&pipe->master) < 0)
				break;

			/* Keyboard press. */
			do {
				nterm_put_unicode (u, getchar (&pipe->master));
			} while (peekchar (&pipe->master) >= 0);

			/* Restart a timer, to make cursor visible longer. */
			u->cursor_blink = 0;
			GrRestartTimer (u->timer);
			break;

		case GR_EVENT_TYPE_FOCUS_IN:
			/* We got a focus - set blue border. */
			GrSetBorderColor (u->topwin, GR_COLOR_CYAN);
			hide_cursor (u);
			u->have_focus = 1;
			break;

		case GR_EVENT_TYPE_FOCUS_OUT:
			/* We lost a focus - set gray border. */
			GrSetBorderColor (u->topwin, GR_COLOR_GRAY);
			hide_cursor (u);
			u->have_focus = 0;
			break;

		case GR_EVENT_TYPE_TIMER:
			if (u->have_focus) {
				if (u->cursor_blink) {
					u->cursor_blink = 0;
				} else {
					hide_cursor (u);
					u->cursor_blink = 1;
				}
			}
			break;
		}
	}
}
