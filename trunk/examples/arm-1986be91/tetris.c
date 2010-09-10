/*
 * Tetris (C) Copyright 1995, Vadim Antonov
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <random/rand15.h>
#include "board-1986be91.h"

#define PITWIDTH	12
#define PITDEPTH	24

#define NTYPES		7
#define NPIECES		5

#define END		999

typedef struct {
	int     x, y;
} coord_t;

typedef struct {
	int	dx, dy;
	coord_t p [NPIECES];
} shape_t;

const shape_t shape [NTYPES] = {
	/* OOOO */
	{ 0, 3, { {0,0}, {0,1}, {0,2}, {0,3}, {END,END} } },

	/* O    O    O  OO OO */
	/* OOO OOO OOO OO   OO */
	{ 1, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ 1, 2, { {0,1}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ 1, 2, { {0,2}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ 1, 2, { {0,0}, {0,1}, {1,1}, {1,2}, {END,END} } },
	{ 1, 2, { {0,1}, {0,2}, {1,0}, {1,1}, {END,END} } },

	/* OO */
	/* OO */
	{ 1, 1, { {0,0}, {0,1}, {1,0}, {1,1}, {END,END} } },
};

int pit [PITDEPTH+1] [PITWIDTH];
int pitcnt [PITDEPTH];
coord_t old [NPIECES], new [NPIECES], chk [NPIECES];

gpanel_t display;

/*
 * Output piece coordinates given its center and angle
 */
void pdots (const shape_t *t, coord_t c, int a, coord_t *res)
{
	coord_t org, tmp;
	int yw, xw, i;

	if (a & 1) {		/* 90 deg */
		xw = t->dy;
		yw = t->dx;
	} else {
		xw = t->dx;
		yw = t->dy;
	}
	org = c;
	org.x -= (xw + 1) / 2;
	org.y -= yw / 2;
	if (org.y < 0)
		org.y = 0;
	if (org.y + yw >= PITWIDTH && c.y <= PITWIDTH)
		org.y = PITWIDTH-1 - yw;
	for (i=0; t->p[i].x!=END; i++) {
		switch (a) {
		case 0:
			res[i].x = t->p[i].x;
			res[i].y = t->p[i].y;
			break;
		case 3:
			res[i].x = xw - t->p[i].y;
			res[i].y = t->p[i].x;
			break;
		case 2:
			res[i].x = xw - t->p[i].x;
			res[i].y = yw - t->p[i].y;
			break;
		case 1:
			res[i].x = t->p[i].y;
			res[i].y = yw - t->p[i].x;
		}
		res[i].x += org.x;
		res[i].y += org.y;
	}
	res[i].x = res[i].y = END;

	do {
		xw = 0;
		for (i=0; res[i+1].x!=END; i++) {
			if (res[i].x < res[i+1].x)
				continue;
			if (res[i].x == res[i+1].x && res[i].y <= res[i+1].y)
				continue;
			xw++;
			tmp = res[i];
			res[i] = res[i+1];
			res[i+1] = tmp;
		}
	} while (xw);
}

/*
 * Draw the piece
 */
void draw (coord_t *p)
{
	for (; p->x!=END; p++) {
		if (p->x < 0)
			continue;
		gpanel_rect_filled (&display,
			p->x*5, p->y*5, p->x*5 + 4, p->y*5 + 4, 1);
	}
}

/*
 * Move the piece
 */
void move (coord_t *old, coord_t *new)
{
	for (;;) {
		if (old->x == END)
			goto draw;
		if (new->x == END)
			goto clear;
		if (old->x > new->x)
			goto draw;
		if (old->x < new->x)
			goto clear;
		if (old->y > new->y)
			goto draw;
		if (old->y == new->y) {
			old++;
			new++;
			continue;       /* old & new at the same place */
		}
clear:          if (old->x >= 0) {
			gpanel_rect_filled (&display,
				old->x*5, old->y*5, old->x*5 + 4, old->y*5 + 4, 0);
			gpanel_pixel (&display, old->x*5 + 2, old->y*5 + 2, 1);
		}
		old++;
		continue;

draw:           if (new->x == END)
			break;
		if (new->x >= 0) {
			gpanel_rect_filled (&display,
				new->x*5, new->y*5, new->x*5 + 4, new->y*5 + 4, 1);
		}
		new++;
	}
}

/*
 * Draw the pit
 */
void clear ()
{
	int y, x;

	gpanel_clear (&display, 0);
	for (y=0; y<PITDEPTH; y++) {
		for (x=0; x<PITWIDTH; x++) {
			gpanel_pixel (&display, y*5 + 2, x*5 + 2, 1);
			pit[y][x] = 0;
		}
		pitcnt[y] = 0;
	}
	for (x=0; x<PITWIDTH; x++)
		pit[PITDEPTH][x] = 1;
}

/*
 * The piece reached the bottom
 */
void scarp (coord_t *c)
{
	int y, nfull, x, k;

	/* Count the full lines */
	nfull = 0;
	for (; c->x!=END; c++) {
		if (c->x <= 0) {
			/* Game over. */
			clear();
			return;
		}
		pit[c->x][c->y] = 1;
		++pitcnt[c->x];
		if (pitcnt[c->x] == PITWIDTH)
			nfull++;
	}
	if (! nfull)
		return;

	/* Clear upper nfull lines */
	for (y=0; y<nfull; y++) {
		for (x=0; x<PITWIDTH; x++) {
			if (pit[y][x]) {
				gpanel_rect_filled (&display,
					x*5, y*5, x*5 + 4, y*5 + 4, 0);
				gpanel_pixel (&display, x*5 + 2, y*5 + 2, 1);
			}
		}
	}

	/* Move lines down */
	k = nfull;
	for (y=nfull; y<PITDEPTH && k>0; y++) {
		if (pitcnt[y-k] == PITWIDTH) {
			k--;
			y--;
			continue;
		}
		for (x=0; x<PITWIDTH; x++) {
			if (pit[y][x] != pit[y-k][x]) {
				if (pit[y-k][x]) {
					gpanel_rect_filled (&display,
						x*5, y*5, x*5 + 4, y*5 + 4, 1);
				} else {
					gpanel_rect_filled (&display,
						x*5, y*5, x*5 + 4, y*5 + 4, 0);
					gpanel_pixel (&display, x*5 + 2, y*5 + 2, 1);
				}
			}
		}
	}

	/* Now fix the pit contents */
	for (y=PITDEPTH-1; y>0; y--) {
		if (pitcnt[y] != PITWIDTH)
			continue;
		memmove (pit[0]+PITWIDTH, pit[0], y * sizeof(pit[0]));
		memset (pit[0], 0, sizeof(pit[0]));
		memmove (pitcnt+1, pitcnt, y * sizeof(pitcnt[0]));
		pitcnt[0] = 0;
		y++;
	}
}

int main ()
{
	int ptype;		/* Piece type */
	int angle, anew;	/* Angle */
	int msec;		/* Timeout */
	coord_t c, cnew, *cp;
	unsigned up_pressed = 0, left_pressed = 0;
	unsigned right_pressed = 0, down_pressed = 0;
	extern gpanel_font_t font_fixed6x8;

	buttons_init ();
	gpanel_init (&display, &font_fixed6x8);

	/* Draw the pit */
	clear();

newpiece:
	ptype = rand15() % NTYPES;
	angle = rand15() % 3;

	c.y = PITWIDTH/2 - 1;
	for (c.x= -2; ; c.x++) {
		pdots (&shape[ptype], c, angle, new);
		for (cp=new; cp->x!=END; cp++) {
			if (cp->x >= 0)
				goto ok;
		}
	}
ok:
	draw (new);
	memcpy (old, new, sizeof old);

	msec = 500;
	for (;;) {
		cnew = c;
		anew = angle;

		if (msec <= 0) {
			/* Timeout: move down */
			msec = 500;
			cnew.x++;
			pdots (&shape[ptype], cnew, anew, chk);
			for (cp=chk; cp->x!=END; cp++) {
				if (cp->x < 0)
					continue;
				if (pit[cp->x][cp->y]) {
					scarp (new);
					goto newpiece;
				}
			}
			goto check;
		}

		if (! joystick_left ())
			left_pressed = 0;
		else if (! left_pressed) {
			left_pressed = 1;

			/* Left: rotate */
			if (--anew < 0)
				anew = 3;
			pdots (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up: move left. */
			if (cnew.y <= 0)
				goto out;
			cnew.y--;
			pdots (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Down: move right */
			if (cnew.y >= PITWIDTH-1)
				goto out;
			cnew.y++;
			pdots (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right: drop */
			for (;;) {
				cnew.x++;
				pdots (&shape[ptype], cnew, anew, chk);
				for (cp=chk; cp->x!=END; cp++) {
					if (cp->x < 0)
						continue;
					if (pit[cp->x][cp->y]) {
						cnew.x--;
						pdots (&shape[ptype], cnew, anew, chk);
						move (new, chk);
						scarp (chk);
						goto newpiece;
					}
				}
			}
		}

		mdelay (10);
		msec -= 10;
		continue;

check:		for (cp=chk; cp->x!=END; cp++) {
			if (cp->y < 0 || cp->y >= PITWIDTH)
				goto out;
		}
		for (cp=chk; cp->x!=END; cp++) {
			if (cp->x < 0)
				continue;
			if (pit[cp->x][cp->y])
				goto out;
		}
		c = cnew;
		angle = anew;
		memcpy (old, new, sizeof old);
		memcpy (new, chk, sizeof new);
		move (old, new);
out:		;
	}
}
