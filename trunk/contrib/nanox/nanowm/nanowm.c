/*
 * NanoWM - Window Manager for Nano-X
 *
 * Copyright (C) 2000 Greg Haerr <greg@censoft.com>
 * Copyright (C) 2000 Alex Holden <alex@linuxhacker.org>
 * Parts based on npanel.c Copyright (C) 1999 Alistair Riddoch.
 */
#include <runtime/lib.h>

#include "nanox/include/nano-X.h"

/* Uncomment this if you want debugging output from this file */
/*#define DEBUG*/

#include "nanowm.h"

GR_SCREEN_INFO Wm_si;

void Wm_run (void *arg)
{
	GR_EVENT event;
	GR_WM_PROPERTIES props;
	win window;

	if (GrOpen() < 0) {
		debug_printf ("Couldn't connect to Nano-X server!\n");
		abort();
	}

	/* pass errors through main loop, don't exit*/
	GrSetErrorHandler(0);

	GrGetScreenInfo(&Wm_si);

	/* add root window*/
	window.wid = GR_ROOT_WINDOW_ID;
	window.pid = GR_ROOT_WINDOW_ID;
	window.type = WINDOW_TYPE_ROOT;
	window.clientid = 1;
	window.sizing = GR_FALSE;
	window.active = 0;
	window.data = 0;
	Wm_add_window(&window);

	GrSelectEvents(GR_ROOT_WINDOW_ID, GR_EVENT_MASK_CHLD_UPDATE);

	/* Set new root window background color*/
	props.flags = GR_WM_FLAGS_BACKGROUND;
	props.background = GrGetSysColor(GR_COLOR_DESKTOP);
	GrSetWMProperties(GR_ROOT_WINDOW_ID, &props);

	while(1) {
		GrGetNextEvent(&event);

		switch(event.type) {
		case GR_EVENT_TYPE_ERROR:
			debug_printf ("nanowm: error %d\n", event.error.code);
			break;
		case GR_EVENT_TYPE_EXPOSURE:
			Wm_do_exposure(&event.exposure);
			break;
		case GR_EVENT_TYPE_BUTTON_DOWN:
			Wm_do_button_down(&event.button);
			break;
		case GR_EVENT_TYPE_BUTTON_UP:
			Wm_do_button_up(&event.button);
			break;
		case GR_EVENT_TYPE_MOUSE_ENTER:
			Wm_do_mouse_enter(&event.general);
			break;
		case GR_EVENT_TYPE_MOUSE_EXIT:
			Wm_do_mouse_exit(&event.general);
			break;
		case GR_EVENT_TYPE_MOUSE_POSITION:
			Wm_do_mouse_moved(&event.mouse);
			break;
		case GR_EVENT_TYPE_KEY_DOWN:
			Wm_do_key_down(&event.keystroke);
			break;
		case GR_EVENT_TYPE_KEY_UP:
			Wm_do_key_up(&event.keystroke);
			break;
		case GR_EVENT_TYPE_FOCUS_IN:
			Wm_do_focus_in(&event.general);
			break;
		case GR_EVENT_TYPE_CHLD_UPDATE:
			Wm_do_update(&event.update);
			break;
		default:
			debug_printf ("Got unexpected event %d\n", event.type);
			break;
		}
	}
}
