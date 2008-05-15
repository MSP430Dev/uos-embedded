/*
 * nxclock - Nano-X clock program
 *
 * Copyright (C) 2000 by Greg Haerr <greg@censoft.com>
 * Copyright (C) 1999 Alistair Riddoch <ajr@ecs.soton.ac.uk>
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <runtime/i386/int86.h>

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

static	GR_WINDOW_ID	w1;		/* id for window */
static	GR_GC_ID	gc1;		/* graphics context for text */
static	GR_GC_ID	gc2;		/* graphics context for rectangle */
static	GR_TIMER_ID	timer_id;
static 	GR_COORD	clock_width, clock_height;
static int lh = -1, lm = -1, ls = -1;
static unsigned long then;

static short trig[91] = {
	0, 71, 143, 214, 286, 357, 428, 499, 570, 641, 711, 782, 852, 921,
	991, 1060, 1129, 1198, 1266, 1334, 1401, 1468, 1534, 1600, 1666,
	1731, 1796, 1860, 1923, 1986, 2048, 2110, 2171, 2231, 2290, 2349,
	2408, 2465, 2522, 2578, 2633, 2687, 2741, 2793, 2845, 2896, 2946,
	2996, 3044, 3091, 3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435,
	3474, 3511, 3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798,
	3824, 3849, 3873, 3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021,
	4034, 4046, 4056, 4065, 4074, 4080, 4086, 4090, 4094, 4095, 4096,
};

static void do_exposure(GR_EVENT_EXPOSURE *ep);
static void do_clock(void);

void
nxmain_nxclock (void *arg)
{
	GR_SCREEN_INFO	si;		/* screen information */
	GR_EVENT	event;		/* current event */
	GR_BITMAP	bitmap1fg[7];	/* bitmaps for first cursor */
	GR_BITMAP	bitmap1bg[7];

	if (GrOpen() < 0) {
		debug_puts ("Cannot open graphics\n");
		abort ();
	}
	GrGetScreenInfo (&si);

	/* create window*/
	clock_height = si.rows / 4;
	clock_width = clock_height;
	w1 = GrNewWindowEx (GR_WM_PROPS_APPFRAME | GR_WM_PROPS_CAPTION |
		GR_WM_PROPS_CLOSEBOX, (unsigned char*) "nxclock", GR_ROOT_WINDOW_ID,
		(si.cols - clock_width) / 2, (si.rows - clock_height)/4,
		clock_width, clock_height, GR_COLOR_NAVYBLUE);

	GrSelectEvents (w1, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_CLOSE_REQ |
		GR_EVENT_MASK_TIMER);

	gc1 = GrNewGC ();
	gc2 = GrNewGC ();

	GrSetGCForeground (gc1, GR_COLOR_NAVYBLUE);
	GrSetGCBackground (gc1, GR_COLOR_WHITE);
	GrSetGCForeground (gc2, GR_COLOR_WHITE);
	GrSetGCBackground (gc2, GR_COLOR_NAVYBLUE);

	bitmap1bg[0] = MASK(_,_,X,X,X,_,_);
	bitmap1bg[1] = MASK(_,X,X,X,X,X,_);
	bitmap1bg[2] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[3] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[4] = MASK(X,X,X,X,X,X,X);
	bitmap1bg[5] = MASK(_,X,X,X,X,X,_);
	bitmap1bg[6] = MASK(_,_,X,X,X,_,_);

	bitmap1fg[0] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[1] = MASK(_,X,_,X,_,X,_);
	bitmap1fg[2] = MASK(_,_,_,X,_,_,_);
	bitmap1fg[3] = MASK(X,_,_,X,X,_,X);
	bitmap1fg[4] = MASK(_,_,_,_,_,_,_);
	bitmap1fg[5] = MASK(_,X,_,_,_,X,_);
	bitmap1fg[6] = MASK(_,_,_,X,_,_,_);

	GrSetCursor (w1, 7, 7, 3, 3, GR_COLOR_WHITE, GR_COLOR_BLACK,
		bitmap1fg, bitmap1bg);
	GrMapWindow (w1);

	timer_id = GrCreateTimer(w1, 100);
	while (1) {
		GrGetNextEvent (&event);
		/*debug_printf ("Event %d\n", event.type);*/

		switch (event.type) {
		case GR_EVENT_TYPE_EXPOSURE:
			do_exposure (&event.exposure);
			break;

		case GR_EVENT_TYPE_CLOSE_REQ:
			GrClose();
			task_exit(0);
			break;

		case GR_EVENT_TYPE_TIMER:
		case GR_EVENT_TYPE_TIMEOUT:
			do_clock ();
			break;
		}
	}
}

static int bsin (int angle)
{
	if (angle < 91) {
		return trig[angle];
	} else if (angle < 181) {
		return trig[180 - angle];
	} else if (angle < 271) {
		return -trig[angle - 180];
	} else if (angle < 361) {
		return -trig[360 - angle];
	} else {
		return 0;
	}
}

static int bcos (int angle)
{
	if (angle < 91) {
		return trig[90 - angle];
	} else if (angle < 181) {
		return -trig[angle - 90];
	} else if (angle < 271) {
		return -trig[270 - angle];
	} else if (angle < 361) {
		return trig[angle - 270];
	} else {
		return 0;
	}
}

/*
 * Here when an exposure event occurs.
 */
void
do_exposure (GR_EVENT_EXPOSURE *ep)
{
	GR_COORD	midx = clock_width / 2;
	GR_COORD	midy = clock_height / 2;
	int i, l;

/*	GrFillRect (w1, gc2, 0, 0, clock_width, clock_height);*/
/*	GrFillEllipse (w1, gc1, midx, midy, midx, midy); */
	GrEllipse (w1, gc2, midx, midy, midx - 1, midy - 1);
	for(i = 0; i < 60; i++) {
		if (i % 5 == 0) {
			l = 5;
		} else {
			l = 0;
		}
		GrLine (w1, gc2,
			midx + (((midx - 3) * bsin(i * 6)) >> 12),
			midy + (((midy - 3) * bcos(i * 6)) >> 12),
			midx + (((midx - 3 - l) * bsin(i * 6)) >> 12),
			midy + (((midy - 3 - l) * bcos(i * 6)) >> 12));
	}

	lh = -1; lm = -1; ls = -1;
	then = 0;
	do_clock();
}

static void draw_time (int hour, int minutes, int sec)
{
	GR_COORD	midx = clock_width / 2;
	GR_COORD	midy = clock_height / 2;

	if (ls >= 0)
	    GrLine (w1, gc1,
		midx + (((midx - 8 - midx / 10) * bsin(ls)) >> 12),
		midy - (((midy - 8 - midy / 10) * bcos(ls)) >> 12),
		midx + (((midx - 8 - midx / 4) * bsin(ls)) >> 12),
		midy - (((midy - 8 - midx / 4) * bcos(ls)) >> 12));
	GrLine (w1, gc2,
		midx + (((midx - 8 - midx / 10) * bsin(sec)) >> 12),
		midy - (((midy - 8 - midy / 10) * bcos(sec)) >> 12),
		midx + (((midx - 8 - midx / 4) * bsin(sec)) >> 12),
		midy - (((midy - 8 - midx / 4) * bcos(sec)) >> 12));
	if ((lm != minutes) || (ls == minutes)) {
		if (lm >= 0)
		    GrLine (w1, gc1,
			midx + (((midx - 8 - midx / 10) * bsin(lm)) >> 12),
			midy - (((midy - 8 - midy / 10) * bcos(lm)) >> 12),
			midx + (((midx / 5) * bsin(lm)) >> 12),
			midy - (((midy / 5) * bcos(lm)) >> 12));
		GrLine (w1, gc2,
			midx + (((midx - 8 - midx / 10) * bsin(minutes)) >> 12),
			midy - (((midy - 8 - midy / 10) * bcos(minutes)) >> 12),
			midx + (((midx / 5) * bsin(minutes)) >> 12),
			midy - (((midy / 5) * bcos(minutes)) >> 12));
		if (lh >= 0)
		    GrLine (w1, gc1,
			midx + (((midx - 8 - midx / 2) * bsin(lh)) >> 12),
			midy - (((midy - 8 - midy / 2) * bcos(lh)) >> 12),
			midx + (((midx / 5) * bsin(lh)) >> 12),
			midy - (((midy / 5) * bcos(lh)) >> 12));
		GrLine (w1, gc2,
			midx + (((midx - 8 - midx / 2) * bsin(hour)) >> 12),
			midy - (((midy - 8 - midy / 2) * bcos(hour)) >> 12),
			midx + (((midx / 5) * bsin(hour)) >> 12),
			midy - (((midy / 5) * bcos(hour)) >> 12));
	}
	lh = hour;
	lm = minutes;
	ls = sec;
}

/*
 * Update the clock if the seconds have changed.
 */
void
do_clock(void)
{
	int minutes, hour, sec;
	static int old_sec;
	int86_regs_t reg;

	/* Read time from real time clock. */
	reg.x.ax = 0x0200;
	int86 (0x1a, &reg, &reg);

	sec = (reg.h.dh >> 4) * 10 + (reg.h.dh & 15);
	if (sec == old_sec)
		return;
	old_sec = sec;

	hour = (reg.h.ch >> 4) * 10 + (reg.h.ch & 15);
	minutes = (reg.h.cl >> 4) * 10 + (reg.h.cl & 15);
	if (hour > 12)
		hour -= 12;

	draw_time (hour*30 + minutes/2, minutes * 6, sec * 6);
}
