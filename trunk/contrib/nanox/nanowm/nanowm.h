/*
 * NanoWM- the NanoGUI window manager.
 * Copyright (C) 2000 Alex Holden <alex@linuxhacker.org>
 */

#ifndef __NANOWM_H
#define __NANOWM_H

#if 0
#define Dprintf debug_printf
#else
#define Dprintf(ignore...)
#endif

/* Where to place the first window on the screen */
#define FIRST_WINDOW_LOCATION 20

/* The distance to leave between windows when deciding where to place */
#define WINDOW_STEP 20

/* The different window types which can be used in windowlist->type */
enum {
	WINDOW_TYPE_ROOT,
	WINDOW_TYPE_CONTAINER,
	WINDOW_TYPE_CLIENT
	/* *WINDOW_TYPE_TOPBAR,
	WINDOW_TYPE_LEFTBAR,
	WINDOW_TYPE_RIGHTBAR,
	WINDOW_TYPE_BOTTOMBAR,
	WINDOW_TYPE_LEFTRESIZE,
	WINDOW_TYPE_RIGHTRESIZE,
	WINDOW_TYPE_CLOSEBUTTON,
	WINDOW_TYPE_MAXIMISEBUTTON,
	WINDOW_TYPE_RESTOREBUTTON,
	WINDOW_TYPE_ICONISEBUTTON,
	WINDOW_TYPE_ICON,
	WINDOW_TYPE_UTILITYBUTTON,
	WINDOW_TYPE_UTILITYMENU,
	WINDOW_TYPE_UTILITYMENUENTRY,
	WINDOW_TYPE_ROOTMENU,
	WINDOW_TYPE_ROOTMENUENTRY**/
};

/*
 * Used to keep a list of all the windows we know about so we can quickly
 * find out whether a window is "one of ours", and if it is, what kind of
 * window it is (title bar, side bar, button, icon, root menu, etc.), who
 * it's a child of, and any special data associated with it (the title
 * used in the title, the text of a root menu entry, the pixmap of an
 * icon, etc.).
 */
struct windowlist {
	GR_WINDOW_ID wid;	/* The ID of this window */
	GR_WINDOW_ID pid;	/* The ID of this window's parent */
	GR_WINDOW_ID clientid;	/* clientid for container window*/
	int type;		/* What kind of window this is */
	int sizing;		/* True if in the middle of a sizing request */
	int active;		/* Whether this window is active or not */
	void *data;		/* Data associated with this window */
	struct windowlist *next; /* The next window in the list */
};
typedef struct windowlist win;

/*
 * Used to record the offset position when performing a move.
 */
struct position {
	GR_COORD x;
	GR_COORD y;
};

/*
 * Used to record the original position, original size, and offset position
 * when performing a resize.
 */
struct pos_size {
	GR_COORD xoff;
	GR_COORD yoff;
	GR_COORD xorig;
	GR_COORD yorig;
	GR_SIZE width;
	GR_SIZE height;
};

#if 0000
/*
 * Used to record some general information about the client.
 */
struct clientinfo {
	GR_WINDOW_ID cid;
};
#endif

/* Function prototypes */
win *Wm_find_window(GR_WINDOW_ID wid);
int Wm_add_window(win *window);
int Wm_remove_window(win *window);
int Wm_remove_window_and_children(win *window);
int Wm_new_client_window(GR_WINDOW_ID wid);
void Wm_client_window_resize(win *window);
void Wm_client_window_destroy(win *window);

void Wm_client_window_remap(win *window);
void Wm_client_window_unmap(win *window);

void Wm_redraw_ncarea(win *window);
void Wm_do_exposure(GR_EVENT_EXPOSURE *event);
void Wm_do_button_down(GR_EVENT_BUTTON *event);
void Wm_do_button_up(GR_EVENT_BUTTON *event);
void Wm_do_mouse_enter(GR_EVENT_GENERAL *event);
void Wm_do_mouse_exit(GR_EVENT_GENERAL *event);
void Wm_do_mouse_moved(GR_EVENT_MOUSE *event);
void Wm_do_focus_in(GR_EVENT_GENERAL *event);
void Wm_do_key_down(GR_EVENT_KEYSTROKE *event);
void Wm_do_key_up(GR_EVENT_KEYSTROKE *event);
void Wm_do_focus_in(GR_EVENT_GENERAL *event);
void Wm_do_focus_out(GR_EVENT_GENERAL *event);
void Wm_do_update(GR_EVENT_UPDATE *event);
void Wm_do_chld_update(GR_EVENT_UPDATE *event);
void Wm_rootwindow_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_container_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_topbar_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_closebutton_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_maximisebutton_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_restorebutton_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_iconisebutton_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_utilitybutton_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_utilitymenu_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_utilitymenuentry_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_rootmenu_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_rootmenuentry_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_icon_exposure(win *window, GR_EVENT_EXPOSURE *event);
void Wm_rootwindow_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_container_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_topbar_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_resizebar_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_closebutton_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_maximisebutton_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_restorebutton_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_iconisebutton_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_utilitybutton_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_icon_buttondown(win *window, GR_EVENT_BUTTON *event);
void Wm_rootwindow_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_container_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_topbar_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_resizebar_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_closebutton_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_maximisebutton_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_restorebutton_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_iconisebutton_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_utilitybutton_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_icon_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_utilitymenuentry_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_rootmenuentry_buttonup(win *window, GR_EVENT_BUTTON *event);
void Wm_closebutton_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_maximisebutton_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_restorebutton_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_iconisebutton_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_utilitybutton_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_utilitymenu_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_utilitymenuentry_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_rootmenu_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_rootmenuentry_mouseexit(win *window, GR_EVENT_GENERAL *event);
void Wm_container_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_topbar_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_leftbar_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_leftresize_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_bottombar_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_rightresize_mousemoved(win *window, GR_EVENT_MOUSE *event);
void Wm_rightbar_mousemoved(win *window, GR_EVENT_MOUSE *event);

extern GR_SCREEN_INFO Wm_si;
extern win *Wm_windows;
extern GR_BITMAP Wm_utilitybutton_notpressed[];
extern GR_BITMAP Wm_utilitybutton_pressed[];
extern GR_BITMAP Wm_maximisebutton_notpressed[];
extern GR_BITMAP Wm_maximisebutton_pressed[];
extern GR_BITMAP Wm_iconisebutton_notpressed[];
extern GR_BITMAP Wm_iconisebutton_pressed[];
extern GR_BITMAP Wm_closebutton_notpressed[];
extern GR_BITMAP Wm_closebutton_pressed[];
extern GR_BITMAP Wm_restorebutton_notpressed[];
extern GR_BITMAP Wm_restorebutton_pressed[];
extern GR_BITMAP Wm_horizontal_resize_fg[];
extern GR_BITMAP Wm_horizontal_resize_bg[];
extern GR_BITMAP Wm_vertical_resize_fg[];
extern GR_BITMAP Wm_vertical_resize_bg[];
extern GR_BITMAP Wm_righthand_resize_fg[];
extern GR_BITMAP Wm_righthand_resize_bg[];
extern GR_BITMAP Wm_lefthand_resize_fg[];
extern GR_BITMAP Wm_lefthand_resize_bg[];
extern int Wm_horizontal_resize_columns, Wm_horizontal_resize_rows;
extern int Wm_horizontal_resize_hotx, Wm_horizontal_resize_hoty;
extern int Wm_vertical_resize_columns, Wm_vertical_resize_rows;
extern int Wm_vertical_resize_hotx, Wm_vertical_resize_hoty;
extern int Wm_lefthand_resize_columns, Wm_lefthand_resize_rows;
extern int Wm_lefthand_resize_hotx, Wm_lefthand_resize_hoty;
extern int Wm_righthand_resize_columns, Wm_righthand_resize_rows;
extern int Wm_righthand_resize_hotx, Wm_righthand_resize_hoty;

#endif
