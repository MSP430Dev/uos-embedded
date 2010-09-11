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

#define NSHAPES		7
#define NBLOCKS		5

#define FIN		999

typedef struct {
	int	x, y;
} coord_t;

typedef struct {
	int	dx, dy;
	coord_t p [NBLOCKS];
} shape_t;

const shape_t shape [NSHAPES] = {
/* OOOO */	{ 0, 3, { {0,0}, {0,1}, {0,2}, {0,3}, {FIN,FIN} } },

/* O   */	{ 1, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {FIN,FIN} } },
/* OOO */

/*  O  */	{ 1, 2, { {0,1}, {1,0}, {1,1}, {1,2}, {FIN,FIN} } },
/* OOO */

/*   O */	{ 1, 2, { {0,2}, {1,0}, {1,1}, {1,2}, {FIN,FIN} } },
/* OOO */

/*  OO */	{ 1, 2, { {0,0}, {0,1}, {1,1}, {1,2}, {FIN,FIN} } },
/* OO  */

/* OO  */	{ 1, 2, { {0,1}, {0,2}, {1,0}, {1,1}, {FIN,FIN} } },
/*  OO */

/* OO */	{ 1, 1, { {0,0}, {0,1}, {1,0}, {1,1}, {FIN,FIN} } },
/* OO */
};

int pit [PITDEPTH+1] [PITWIDTH];
int pitcnt [PITDEPTH];
coord_t old [NBLOCKS], new [NBLOCKS], chk [NBLOCKS];

gpanel_t display;

/*
 * Output piece coordinates given its center and angle
 */
void translate (const shape_t *t, coord_t c, int a, coord_t *res)
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
	for (i=0; t->p[i].x!=FIN; i++) {
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
	res[i].x = res[i].y = FIN;

	do {
		xw = 0;
		for (i=0; res[i+1].x!=FIN; i++) {
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
 * Draw the block
 */
void draw_block (int h, int w, int visible)
{
	h *= 5;
	w *= 5;
	if (visible) {
		gpanel_rect_filled (&display, h, w, h + 4, w + 4, 1);
	} else {
		gpanel_rect_filled (&display, h, w, h + 4, w + 4, 0);
		if (w == (PITWIDTH-1)*5)
			gpanel_pixel (&display, h + 2, w - 1, 1);
		else if (w % 15 == 0)
			gpanel_pixel (&display, h + 2, w, 1);
	}
}

/*
 * Move the piece
 */
void move (coord_t *old, coord_t *new)
{
	for (;;) {
		if (old->x == FIN) {
draw:			if (new->x == FIN)
				break;
			if (new->x >= 0)
				draw_block (new->x, new->y, 1);
			new++;
			continue;
		}
		if (new->x == FIN) {
clear:			if (old->x >= 0)
				draw_block (old->x, old->y, 0);
			old++;
			continue;
		}
		if (old->x > new->x)
			goto draw;
		if (old->x < new->x)
			goto clear;
		if (old->y > new->y)
			goto draw;
		if (old->y == new->y) {
			/* old & new at the same place */
			old++;
			new++;
		}
	}
}

/*
 * Draw the pit
 */
void clear ()
{
	int h, w;

	for (h=0; h<PITDEPTH; h++) {
		for (w=0; w<PITWIDTH; w++) {
			draw_block (&display, h, w, 0);
			pit[h][w] = 0;
		}
		pitcnt[h] = 0;
	}
	for (w=0; w<PITWIDTH; w++)
		pit[PITDEPTH][w] = 1;
}

/*
 * The piece reached the bottom
 */
void stopped (coord_t *c)
{
	int h, nfull, w, k;

	/* Count the full lines */
	nfull = 0;
	for (; c->x!=FIN; c++) {
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
	for (h=0; h<nfull; h++) {
		for (w=0; w<PITWIDTH; w++) {
			if (pit[h][w]) {
				draw_block (h, w, 0);
			}
		}
	}

	/* Move lines down */
	k = nfull;
	for (h=nfull; h<PITDEPTH && k>0; h++) {
		if (pitcnt[h-k] == PITWIDTH) {
			k--;
			h--;
			continue;
		}
		for (w=0; w<PITWIDTH; w++) {
			if (pit[h][w] != pit[h-k][w]) {
				draw_block (h, w, pit[h-k][w]);
			}
		}
	}

	/* Now fix the pit contents */
	for (h=PITDEPTH-1; h>0; h--) {
		if (pitcnt[h] != PITWIDTH)
			continue;
		memmove (pit[0]+PITWIDTH, pit[0], h * sizeof(pit[0]));
		memset (pit[0], 0, sizeof(pit[0]));
		memmove (pitcnt+1, pitcnt, h * sizeof(pitcnt[0]));
		pitcnt[0] = 0;
		h++;
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

	buttons_init ();
	gpanel_init (&display, 0);
	gpanel_clear (&display, 0);
	gpanel_line (&display, PITDEPTH*5+1, 0, PITDEPTH*5+1, PITWIDTH*5-1, 1);

	/* Draw the pit */
	clear();

newpiece:
	ptype = rand15() % NSHAPES;
	angle = rand15() % 3;

	c.y = PITWIDTH/2 - 1;
	for (c.x= -2; ; c.x++) {
		translate (&shape[ptype], c, angle, new);
		for (cp=new; cp->x!=FIN; cp++) {
			if (cp->x >= 0)
				goto ok;
		}
	}
ok:
	/* Draw new piece */
	for (cp=new; cp->x!=FIN; cp++) {
		if (cp->x >= 0) {
			draw_block (cp->x, cp->y, 1);
		}
	}
	memcpy (old, new, sizeof old);

	msec = 500;
	for (;;) {
		cnew = c;
		anew = angle;

		if (msec <= 0) {
			/* Timeout: move down */
			msec = 500;
			cnew.x++;
			translate (&shape[ptype], cnew, anew, chk);
			for (cp=chk; cp->x!=FIN; cp++) {
				if (cp->x >= 0 && pit[cp->x][cp->y]) {
					stopped (new);
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
			translate (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_up ())
			up_pressed = 0;
		else if (! up_pressed) {
			up_pressed = 1;

			/* Up: move left. */
			if (cnew.y <= 0)
				continue;
			cnew.y--;
			translate (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_down ())
			down_pressed = 0;
		else if (! down_pressed) {
			down_pressed = 1;

			/* Down: move right */
			if (cnew.y >= PITWIDTH-1)
				continue;
			cnew.y++;
			translate (&shape[ptype], cnew, anew, chk);
			goto check;
		}

		if (! joystick_right ())
			right_pressed = 0;
		else if (! right_pressed) {
			right_pressed = 1;

			/* Right: drop */
			for (;;) {
				cnew.x++;
				translate (&shape[ptype], cnew, anew, chk);
				for (cp=chk; cp->x!=FIN; cp++) {
					if (cp->x >= 0 && pit[cp->x][cp->y]) {
						cnew.x--;
						translate (&shape[ptype], cnew, anew, chk);
						move (new, chk);
						stopped (chk);
						goto newpiece;
					}
				}
			}
		}

		mdelay (10);
		msec -= 10;
		continue;

check:		for (cp=chk; cp->x!=FIN; cp++) {
			if (cp->y < 0 || cp->y >= PITWIDTH)
				goto done;
		}
		for (cp=chk; cp->x!=FIN; cp++) {
			if (cp->x >= 0 && pit[cp->x][cp->y])
				goto done;
		}
		c = cnew;
		angle = anew;
		memcpy (old, new, sizeof old);
		memcpy (new, chk, sizeof new);
		move (old, new);
done:		;
	}
}
