/*
 * Demonstration program for Nano-X graphics.
 * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
 */
#include <runtime/lib.h>
#include <random/rand15.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <mem/mem.h>
#include <input/mouse.h>
#include <input/keyboard.h>
#include <i8042/mouse.h>
#include <i8042/keyboard.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/nxcolors.h"

/*
 * Definitions to make it easy to define cursors
 */
#define	_	((unsigned) 0)		/* off bits */
#define	X	((unsigned) 1)		/* on bits */
#define	MASK(a,b,c,d,e,f,g) \
	(((((((((((((a * 2) + b) * 2) + c) * 2) + d) * 2) \
	+ e) * 2) + f) * 2) + g) << 9)

#define	W2_WIDTH	70
#define	W2_HEIGHT	40

static	GR_WINDOW_ID	w1;		/* id for large window */
static	GR_WINDOW_ID	w2;		/* id for small window */
static	GR_WINDOW_ID	w3;		/* id for third window */
static	GR_WINDOW_ID	w4;		/* id for grabbable window */
static	GR_WINDOW_ID	w5;		/* id for testing enter/exit window */
static	GR_GC_ID	gc1;		/* graphics context for text */
static	GR_GC_ID	gc2;		/* graphics context for rectangle */
static	GR_GC_ID	gc3;		/* graphics context for circles */
static	GR_GC_ID	gc4;		/* graphics context for lines */
static	GR_COORD	begxpos;	/* beginning x position */
static	GR_COORD	xpos;		/* x position for text drawing */
static	GR_COORD	ypos;		/* y position for text drawing */
static	GR_COORD	linexpos;	/* x position for line drawing */
static	GR_COORD	lineypos;	/* y position for line drawing */
static	GR_COORD	xorxpos;	/* x position for xor line */
static	GR_COORD	xorypos;	/* y position for xor line */
static	GR_BOOL		lineok;		/* ok to draw line */
static	GR_SIZE		COLS, ROWS;
static	GR_SCREEN_INFO	si;		/* information about screen */
static	GR_TIMER_ID	timer_id;

void do_buttondown(GR_EVENT_BUTTON	*bp);
void do_buttonup(GR_EVENT_BUTTON	*bp);
void do_motion(GR_EVENT_MOUSE		*mp);
void do_keystroke(GR_EVENT_KEYSTROKE	*kp);
void do_exposure(GR_EVENT_EXPOSURE	*ep);
void do_focusin(GR_EVENT_GENERAL	*gp);
void do_focusout(GR_EVENT_GENERAL	*gp);
void do_enter(GR_EVENT_GENERAL		*gp);
void do_exit(GR_EVENT_GENERAL		*gp);
void do_idle(void);
/* routine to handle errors */
void errorcatcher(GR_EVENT *ep);

void
nxmain (void *arg)
{
	GR_EVENT	event;		/* current event */
	GR_BITMAP	bitmap1fg[7];	/* bitmaps for first cursor */
	GR_BITMAP	bitmap1bg[7];
	GR_BITMAP	bitmap2fg[7];	/* bitmaps for second cursor */
	GR_BITMAP	bitmap2bg[7];

	if (GrOpen() < 0) {
		debug_puts ("Cannot open graphics\n");
		abort ();
	}

	GrReqShmCmds(655360);

	GrGetScreenInfo(&si);
	COLS = si.cols - 40;
	ROWS = si.rows - 80;

	/* print error, but don't exit*/
	GrSetErrorHandler(errorcatcher);

	w1 = GrNewWindow(GR_ROOT_WINDOW_ID, 100, 50, COLS - 120,
		ROWS - 60, 1, GR_COLOR_BROWN, GR_COLOR_WHITE);
	w2 = GrNewWindow(GR_ROOT_WINDOW_ID, 6, 6, W2_WIDTH, W2_HEIGHT, 2,
		GR_COLOR_DARKGREEN, GR_COLOR_WHITE);
	w3 = GrNewWindow(GR_ROOT_WINDOW_ID, 250, 30, 80, 100, 1,
		 GR_COLOR_GREY75, GR_COLOR_DARKGREEN);
	w4 = GrNewWindow(GR_ROOT_WINDOW_ID, 350, 20, 200, 150, 5,
		 GR_COLOR_BLACK, GR_COLOR_WHITE);
	w5 = GrNewWindow(GR_ROOT_WINDOW_ID, 11, 143, 209, 100, 1,
		 GR_COLOR_NAVYBLUE, GR_COLOR_DARKGREEN);

	GrSelectEvents(w1, GR_EVENT_MASK_BUTTON_DOWN |
		GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_EXPOSURE |
		GR_EVENT_MASK_FOCUS_IN | GR_EVENT_MASK_FOCUS_OUT |
		GR_EVENT_MASK_CLOSE_REQ | GR_EVENT_MASK_TIMER);
	/* must select down and up for w2 to get implicit grab when
	 * running window manager, otherwise the wm-created parent
	 * window will get the grab, and we won't get the button up...
	 */
	GrSelectEvents(w2, GR_EVENT_MASK_BUTTON_DOWN |
			GR_EVENT_MASK_BUTTON_UP | GR_EVENT_MASK_CLOSE_REQ);
	GrSelectEvents(w3, GR_EVENT_MASK_BUTTON_DOWN |
		GR_EVENT_MASK_MOUSE_MOTION | GR_EVENT_MASK_CLOSE_REQ);
	GrSelectEvents(w4, GR_EVENT_MASK_BUTTON_DOWN |
		GR_EVENT_MASK_BUTTON_UP | GR_EVENT_MASK_MOUSE_POSITION |
		GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_CLOSE_REQ);
	GrSelectEvents(w5, GR_EVENT_MASK_MOUSE_ENTER |
		GR_EVENT_MASK_MOUSE_EXIT | GR_EVENT_MASK_CLOSE_REQ);
	GrSelectEvents(GR_ROOT_WINDOW_ID, GR_EVENT_MASK_BUTTON_DOWN |
			GR_EVENT_MASK_CLOSE_REQ);

	GrMapWindow(w1);
	GrMapWindow(w2);
	GrMapWindow(w3);
	GrMapWindow(w4);
	GrMapWindow(w5);

	gc1 = GrNewGC();
	gc2 = GrNewGC();
	gc3 = GrNewGC();
	gc4 = GrNewGC();

	GrSetGCForeground(gc1, GR_COLOR_WHITE);
	GrSetGCBackground(gc1, GR_COLOR_BROWN);
	GrSetGCForeground(gc2, GR_COLOR_MAGENTA);
	GrSetGCMode(gc4, GR_MODE_XOR);

	bitmap1fg[0] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[1] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[2] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[3] = MASK(X,X,X,X,X,X,X);
	bitmap1fg[4] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[5] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[6] = MASK(_,_,_,X,_,_,_);

	bitmap1bg[0] = MASK(_,_,X,X,X,_,_);
	bitmap1bg[1] = MASK(_,_,X,X,X,_,_);
	bitmap1bg[2] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[3] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[4] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[5] = MASK(_,_,X,X,X,_,_);
	bitmap1bg[6] = MASK(_,_,X,X,X,_,_);

	bitmap2fg[0] = MASK(_,_,X,X,X,_,_);
	bitmap2fg[1] = MASK(_,X,_,_,_,X,_);
	bitmap2fg[2] = MASK(X,_,_,_,_,_,X);
	bitmap2fg[3] = MASK(X,_,_,_,_,_,X);
	bitmap2fg[4] = MASK(_,X,_,_,_,X,_);
	bitmap2fg[5] = MASK(_,_,X,X,X,_,_);

	bitmap2bg[0] = MASK(_,_,X,X,X,_,_);
	bitmap2bg[1] = MASK(_,X,X,X,X,X,_);
	bitmap2bg[2] = MASK(X,X,X,X,X,X,X);
	bitmap2bg[3] = MASK(X,X,X,X,X,X,X);
	bitmap2bg[4] = MASK(_,X,X,X,X,X,_);
	bitmap2bg[5] = MASK(_,_,X,X,X,_,_);

	GrSetCursor(w1, 7, 7, 3, 3, GR_COLOR_WHITE, GR_COLOR_BLACK,
		bitmap1fg, bitmap1bg);
	GrSetCursor(w2, 7, 7, 3, 3, GR_COLOR_WHITE, GR_COLOR_BLACK,
		bitmap2fg, bitmap2bg);

	timer_id = GrCreateTimer(w1, 100);
	while (1) {
		GrGetNextEvent (&event);
		/*debug_printf ("Event %d\n", event.type);*/

		switch (event.type) {
		case GR_EVENT_TYPE_BUTTON_DOWN:
			do_buttondown(&event.button);
			break;

		case GR_EVENT_TYPE_BUTTON_UP:
			do_buttonup(&event.button);
			break;

		case GR_EVENT_TYPE_MOUSE_POSITION:
		case GR_EVENT_TYPE_MOUSE_MOTION:
			do_motion(&event.mouse);
			break;

		case GR_EVENT_TYPE_KEY_DOWN:
			do_keystroke(&event.keystroke);
			break;

		case GR_EVENT_TYPE_EXPOSURE:
			do_exposure(&event.exposure);
			break;

		case GR_EVENT_TYPE_FOCUS_IN:
			do_focusin(&event.general);
			break;

		case GR_EVENT_TYPE_FOCUS_OUT:
			do_focusout(&event.general);
			break;

		case GR_EVENT_TYPE_MOUSE_ENTER:
			do_enter(&event.general);
			break;

		case GR_EVENT_TYPE_MOUSE_EXIT:
			do_exit(&event.general);
			break;

		case GR_EVENT_TYPE_CLOSE_REQ:
			/* Ignore. */
			break;

		case GR_EVENT_TYPE_TIMER:
		case GR_EVENT_TYPE_TIMEOUT:
		case GR_EVENT_TYPE_NONE:
			do_idle();
			break;
		}
	}
}

/*
 * Here when a button is pressed.
 */
void
do_buttondown (GR_EVENT_BUTTON *bp)
{
	static GR_PIXELVAL intable [W2_WIDTH * W2_HEIGHT];
	static GR_PIXELVAL outtable [W2_WIDTH * W2_HEIGHT * 6];
	GR_PIXELVAL	*inp;
	GR_PIXELVAL	*outp;
	GR_PIXELVAL	*oldinp;
	GR_COORD	row;
	GR_COORD	col;

	/*static int xx = 100;
	static int yy = 50;*/

	if (bp->wid == w3) {
		GrRaiseWindow(w3);
		GrReadArea(w2, 0, 0, W2_WIDTH, W2_HEIGHT, intable);
		inp = intable;
		outp = outtable;
		for (row = 0; row < W2_HEIGHT; row++) {
			oldinp = inp;
			for (col = 0; col < W2_WIDTH; col++) {
				*outp++ = *inp;
				*outp++ = *inp++;
			}
			inp = oldinp;
			for (col = 0; col < W2_WIDTH; col++) {
				*outp++ = *inp;
				*outp++ = *inp++;
			}
			inp = oldinp;
			for (col = 0; col < W2_WIDTH; col++) {
				*outp++ = *inp;
				*outp++ = *inp++;
			}
		}
		GrArea(w1, gc1, 0, 0, W2_WIDTH * 2, W2_HEIGHT * 3, outtable,
			MWPF_PIXELVAL);
		return;
	}

	if (bp->wid == w4) {
		GrRaiseWindow(w4);
		linexpos = bp->x;
		lineypos = bp->y;
		xorxpos = bp->x;
		xorypos = bp->y;
		GrLine(w4, gc4, xorxpos, xorypos, linexpos, lineypos);
		lineok = GR_TRUE;
		return;
	}

	if (bp->wid != w1) {
		/*
		 * Cause a fatal error for testing if more than one
		 * button is pressed.
		 */
		if ((bp->buttons & -((int) bp->buttons)) != bp->buttons)
			GrClearWindow(-1, 0);
		return;
	}

	GrRaiseWindow(w1);
	/*GrMoveWindow(w1, ++xx, yy);*/

	if (bp->buttons & GR_BUTTON_L) {
		GrClearWindow(w1, GR_TRUE);
		return;
	}

	begxpos = bp->x;
	xpos = bp->x;
	ypos = bp->y;
}

/*
 * Here when a button is released.
 */
void
do_buttonup (GR_EVENT_BUTTON *bp)
{
	if (bp->wid == w4) {
		if (lineok) {
			GrLine(w4, gc4, xorxpos, xorypos, linexpos, lineypos);
			GrLine(w4, gc3, bp->x, bp->y, linexpos, lineypos);
		}
		lineok = GR_FALSE;
		return;
	}
}


/*
 * Here when the mouse has a motion event.
 */
void
do_motion (GR_EVENT_MOUSE *mp)
{
	if (mp->wid == w4) {
		if (lineok) {
			GrLine(w4, gc4, xorxpos, xorypos, linexpos, lineypos);
			xorxpos = mp->x;
			xorypos = mp->y;
			GrLine(w4, gc4, xorxpos, xorypos, linexpos, lineypos);
		}
		return;
	}

	if (mp->wid == w3) {
		GrPoint(w3, gc3, mp->x, mp->y);
		return;
	}
}

/*
 * Here when a keyboard press occurs.
 */
void
do_keystroke (GR_EVENT_KEYSTROKE *kp)
{
	GR_SIZE		width;		/* width of character */
	GR_SIZE		height;		/* height of character */
	GR_SIZE		base;		/* height of baseline */

	if (kp->wid == w4) {
		if (lineok) {
			GrLine(w4, gc4, xorxpos, xorypos, linexpos, lineypos);
			lineok = GR_FALSE;
		}
		return;
	}

	GrGetGCTextSize(gc1, &kp->ch, 1, GR_TFASCII, &width, &height, &base);
	if ((kp->ch == '\r') || (kp->ch == '\n')) {
		xpos = begxpos;
		ypos += height;
		return;
	}
	if (kp->ch == '\b') {		/* assumes fixed width font!! */
		if (xpos <= begxpos)
			return;
		xpos -= width;
		GrSetGCForeground(gc3, GR_COLOR_BROWN);
		GrFillRect(w1, gc3, xpos, ypos - height + base + 1,
			width, height);
		return;
	}
	if (kp->ch < ' ' || kp->ch > '~')
		return;

	GrText(w1, gc1, xpos, ypos + base, &kp->ch, 1, 0);
	xpos += width;
}

/*
 * Here when an exposure event occurs.
 */
void
do_exposure (GR_EVENT_EXPOSURE *ep)
{
	GR_POINT	points[4];

	if (ep->wid != w1)
		return;
	points[0].x = 311;
	points[0].y = 119;
	points[1].x = 350;
	points[1].y = 270;
	points[2].x = 247;
	points[2].y = 147;
	points[3].x = 311;
	points[3].y = 119;

	GrFillRect(w1, gc2, 50, 50, 150, 200);
	GrFillPoly(w1, gc2, 4, points);
}

/*
 * Here when a focus in event occurs.
 */
void
do_focusin (GR_EVENT_GENERAL *gp)
{
	if (gp->wid != w1)
		return;
	GrSetBorderColor(w1, GR_COLOR_WHITE);
}

/*
 * Here when a focus out event occurs.
 */
void
do_focusout (GR_EVENT_GENERAL *gp)
{
	if (gp->wid != w1)
		return;
	GrSetBorderColor(w1, GR_COLOR_GRAY);
}

/*
 * Here when a enter window event occurs.
 */
void
do_enter (GR_EVENT_GENERAL *gp)
{
	if (gp->wid != w5)
		return;
	GrSetBorderColor(w5, GR_COLOR_WHITE);
	GrRaiseWindow(w5);
}

/*
 * Here when a exit window event occurs.
 */
void
do_exit (GR_EVENT_GENERAL *gp)
{
	if (gp->wid != w5)
		return;
	GrSetBorderColor(w5, GR_COLOR_DARKGREEN);
	GrLowerWindow(w5);
}

/*
 * Here to do an idle task when nothing else is happening.
 * Just draw a randomly colored filled circle in the small window.
 */
void
do_idle (void)
{
	GR_COORD	x;
	GR_COORD	y;
	GR_SIZE		rx;
	GR_SIZE		ry;
	GR_PIXELVAL	pixelval;

	x = rand15() % 70;
	y = rand15() % 40;
	rx = (rand15() % 10) + 5;
	ry = (rx * si.ydpcm) / si.xdpcm;	/* make it appear circular */

	pixelval = rand15() % si.ncolors;

	GrSetGCForegroundPixelVal(gc3, pixelval);
	GrFillEllipse(w2, gc3, x, y, rx, ry);
}

/*
 * Here on a server error.  Print the std message but don't exit.
 */
void
errorcatcher (GR_EVENT *ep)
{
	debug_printf ("nxclient: Error (%s) ", ep->error.name);
	debug_printf (nxErrorStrings[ep->error.code], ep->error.id);
}

mem_pool_t *uos_memory;
timer_t *uos_timer;
mouse_t *uos_mouse;
keyboard_t *uos_keyboard;

extern unsigned long i386_highmem_addr;
extern unsigned long i386_highmem_len;

void uos_init (void)
{
	static mem_pool_t pool;
	char *task, *wm;
	int tasksz = 0x10000;
	extern unsigned long _end;

	uos_memory = &pool;
	if (i386_highmem_len)
		mem_init (uos_memory, (size_t) &_end,
			i386_highmem_addr + i386_highmem_len);

	debug_printf ("Free memory: %ld bytes\n", mem_available (uos_memory));

	task = mem_alloc (uos_memory, tasksz);
	wm = mem_alloc (uos_memory, tasksz);
	uos_timer = (timer_t*) mem_alloc (uos_memory, sizeof (timer_t));
	uos_keyboard = (keyboard_t*) mem_alloc (uos_memory, sizeof (keyboard_ps2_t));
	uos_mouse = (mouse_t*) mem_alloc (uos_memory, sizeof (mouse_ps2_t));
	if (! task || ! wm || ! uos_timer || ! uos_keyboard || ! uos_mouse) {
		debug_printf ("No memory for task\n");
		abort();
	}

	timer_init (uos_timer, 100, 1193182, 10);
	debug_puts ("Timer initialized.\n");

	keyboard_ps2_init ((keyboard_ps2_t*) uos_keyboard, 80);
	debug_puts ("Keyboard initialized.\n");

	mouse_ps2_init ((mouse_ps2_t*) uos_mouse, 90);
	debug_puts ("Mouse initialized.\n");

	task_create (nxmain, 0, "nanox", 10, task, tasksz);
}
