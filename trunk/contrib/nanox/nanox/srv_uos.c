/*
 * Copyright (c) 2005 Serge Vakulenko <vak@cronyx.ru>
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X server routines for uOS.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <input/mouse.h>
#include <input/keyboard.h>
#include "serv.h"

extern timer_t *uos_timer;
extern mouse_t *uos_mouse;
extern keyboard_t *uos_keyboard;

void
GsSelect (GR_TIMEOUT timeout)
{
	char group [sizeof(mutex_group_t) + 4 * sizeof(mutex_slot_t)];
	mutex_group_t *g;
	mutex_t *source;
	GR_TIMEOUT waittime = 0;
again:
	/* If mouse data present, service it */
	while (GsCheckMouseEvent ())
		continue;

	/* If keyboard data present, service it */
	while (GsCheckKeyboardEvent ())
		continue;

	/* Did we process any input yet? */
	if (curclient->eventhead || (signed long) timeout < 0) {
		/* We have events -- return without sleeping */
		return;
	}

	/*
	 * Create a group of three event sources.
	 */
	memset (group, 0, sizeof(group));
	g = mutex_group_init (group, sizeof(group));
	mutex_group_add (g, &uos_timer->lock);
	mutex_group_add (g, &uos_mouse->lock);
	mutex_group_add (g, &uos_keyboard->lock);
	if (curclient->shm_cmds)
		mutex_group_add (g, (mutex_t*) curclient->shm_cmds);

	/* Sleep waiting for events. */
	mutex_group_listen (g);
	SERVER_UNLOCK();
	mutex_group_wait (g, &source, 0);
	SERVER_LOCK();
	mutex_group_unlisten (g);

	if (timeout > 0 && source == &uos_timer->lock)
		waittime += uos_timer->msec_per_tick;

	/* Have we timed out? */
	if ((timeout > 0 && waittime >= timeout) || GdTimeout() == TRUE) {
		/* Timeout has occured.
		 * Currently return a timeout event regardless of
		 * whether client has selected for it. */
		GR_EVENT_GENERAL *gp;

		gp = (GR_EVENT_GENERAL*) GsAllocEvent (curclient);
		if (gp)
			gp->type = GR_EVENT_TYPE_TIMEOUT;
	}

	/* Check for input on registered descriptor */
	if (source == (mutex_t*) curclient->shm_cmds) {
		GR_EVENT_FDINPUT *gp;

		gp = (GR_EVENT_FDINPUT*) GsAllocEvent (curclient);
		if (gp) {
			gp->type = GR_EVENT_TYPE_FDINPUT;
			gp->fd = (int) curclient->shm_cmds;
		}
	}
	goto again;
}

/*
 * Return # milliseconds elapsed since start of Microwindows
 */
GR_TIMEOUT
GsGetTickCount (void)
{
	return timer_milliseconds (uos_timer);
}

void
GrBell (void)
{
	SERVER_LOCK();
	/* TODO */
	SERVER_UNLOCK();
}

/*
 * Register specified lock as a source of FDINPUT events.
 * Only one input can be registered.
 */
void
GrRegisterInput (int fd)
{
	SERVER_LOCK();
	curclient->shm_cmds = (void*) fd;
	SERVER_UNLOCK();
}

void
GrUnregisterInput (int fd)
{
	SERVER_LOCK();
	curclient->shm_cmds = 0;
	SERVER_UNLOCK();
}
