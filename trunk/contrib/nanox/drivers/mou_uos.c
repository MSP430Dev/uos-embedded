/*
 * Copyright (c) 2005 Serge Vakulenko <vak@cronyx.ru>
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X mouse driver for uOS.
 */
#include <runtime/lib.h>
#include <input/mouse.h>
#include "device.h"

extern mouse_t *uos_mouse;

/*
 * Open up the mouse device.
 */
static int
MWMou_Open (MOUSEDEVICE *pmd)
{
	/* Do nothing. */
	return 2;
}

/*
 * Close the mouse device.
 */
static void
MWMou_Close (void)
{
	/* Do nothing. */
}

/*
 * Get mouse buttons supported
 */
static int
MWMou_GetButtonInfo (void)
{
        return MOUSE_BTN_LEFT | MOUSE_BTN_RIGHT | MOUSE_BTN_MIDDLE;
}

/*
 * Get default mouse acceleration settings
 */
static void
MWMou_GetDefaultAccel (int *pscale,int *pthresh)
{
        *pscale = 3;		/* default scaling factor for acceleration */
	*pthresh = 5;		/* default threshhold for acceleration */
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
static int
MWMou_Read (MWCOORD *dx, MWCOORD *dy, MWCOORD *dz, int *bp)
{
	mouse_move_t move;

        if (! mouse_get_move (uos_mouse, &move))
		return 0;

	*bp = 0;
	if (move.buttons & MOUSE_BTN_LEFT)
		*bp |= MWBUTTON_L;
	if (move.buttons & MOUSE_BTN_RIGHT)
		*bp |= MWBUTTON_R;
	if (move.buttons & MOUSE_BTN_MIDDLE)
		*bp |= MWBUTTON_M;

	*dx = move.dx;
	*dy = move.dy;
	*dz = move.dz;

	return 1;
}

MOUSEDEVICE mousedev = {
        MWMou_Open,
	MWMou_Close,
	MWMou_GetButtonInfo,
	MWMou_GetDefaultAccel,
	MWMou_Read,
	0,
	MOUSE_NORMAL
};
