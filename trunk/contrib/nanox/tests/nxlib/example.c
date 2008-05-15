#include <nanox/nxlib/nxlib.h>

#include <stdio.h>
#include <stdlib.h>

Display *display;
int screen;

/* values for window_size in main */
#define SMALL	1
#define OK	0

void load_font (font_info)
	XFontStruct **font_info;
{
	char *fontname = "10x20";

	if ((*font_info = XLoadQueryFont (display, fontname)) == NULL) {
		debug_printf ("cannot open 10x20 font\n");
		exit (-1);
	}
}

void get_GC (win, gc, font_info)
	Window win;
	GC *gc;
	XFontStruct *font_info;
{
	unsigned long valuemask = 0;
	XGCValues values;
	unsigned int line_width = 1;
	int line_style = LineSolid;	/* LineOnOffDash; */
	int cap_style = CapButt;
	int join_style = JoinMiter;
	int dash_offset = 0;
	static char dash_list[] = {20, 40};
	int list_length = sizeof (dash_list);

	*gc = XCreateGC (display, win, valuemask, &values);

	XSetFont (display, *gc, font_info->fid);

	XSetForeground (display, *gc, BlackPixel (display, screen));

	XSetLineAttributes (display, *gc, line_width, line_style, cap_style, join_style);

	XSetDashes (display, *gc, dash_offset, dash_list, list_length);
}

void TooSmall (win, gc, font_info)
	Window win;
	GC gc;
	XFontStruct *font_info;
{
	char *string1 = "Too Small";
	int x, y;

	y = font_info->max_bounds.ascent + 2;
	x = 2;

	XDrawString (display, win, gc, x, y, string1, strlen (string1));
}

void draw_text (win, gc, font_info, win_width, win_height)
	Window win;
	GC gc;
	XFontStruct *font_info;
	unsigned int win_width, win_height;
{
	int y = 20;
	char *string1 = "Hi! I'm a window, who are you?";
	char *string2 = "To terminate program; Press Q key";
	char *string3 = "while in this window.";
	char *string4 = "Screen Dimensions:";
	int len1, len2, len3, len4;
	int width1, width2, width3;
	char cd_height[50], cd_width[50], cd_depth[50];
	int font_height;
	int y_offset, x_offset;

	len1 = strlen (string1);
	len2 = strlen (string2);
	len3 = strlen (string3);

	width1 = XTextWidth (font_info, string1, len1);
	width2 = XTextWidth (font_info, string2, len2);
	width3 = XTextWidth (font_info, string3, len3);

	XDrawString (display, win, gc, (win_width - width1) / 2, y, string1, len1);
	XDrawString (display, win, gc, (win_width - width2) / 2, (int)(win_height - 35), string2, len2);
	XDrawString (display, win, gc, (win_width - width3) / 2, (int)(win_height - 15), string3, len3);

	snprintf (cd_height, sizeof(cd_height), "Height - %d pixels", DisplayHeight (display, screen));
	snprintf (cd_width,  sizeof(cd_width),  "Width - %d pixels", DisplayWidth (display, screen));
	snprintf (cd_depth,  sizeof(cd_depth),  "Depth - %d plane(s)", DefaultDepth (display, screen));

	len4 = strlen (string4);
	len1 = strlen (cd_height);
	len2 = strlen (cd_width);
	len3 = strlen (cd_depth);

	font_height = font_info->max_bounds.ascent +
	    font_info->max_bounds.descent;

	y_offset = win_height / 2 - font_height - font_info->max_bounds.descent;
	x_offset = (int)win_width / 4;

	XDrawString (display, win, gc, x_offset, y_offset, string4, len4);
	y_offset += font_height;

	XDrawString (display, win, gc, x_offset, y_offset, cd_height, len1);
	y_offset += font_height;

	XDrawString (display, win, gc, x_offset, y_offset, cd_width, len2);
	y_offset += font_height;

	XDrawString (display, win, gc, x_offset, y_offset, cd_depth, len3);
}

void draw_graphics (win, gc, window_width, window_height)
	Window win;
	GC gc;
	unsigned int window_width, window_height;
{
	int x, y;
	unsigned int width, height;

	height = window_height / 2;
	width = 3 * window_width / 4;

	x = window_width / 2 - width / 2;
	y = window_height / 2 - height / 2;

	XDrawRectangle (display, win, gc, x, y, width, height);
}

int nxmain (void *arg)
{
	Window win;
	unsigned int width, height, display_width, display_height;
	int x = 0, y = 0;
	unsigned int border_width = 4;
	XEvent report;
	GC gc;
	XFontStruct *font_info;
	char *display_name = NULL;
	int window_size = 0;

	if ((display = XOpenDisplay (display_name)) == NULL) {
		debug_printf ("basicwin: cannot connect to X server %s\n",
		    XDisplayName (display_name));
		exit (-1);
	}
	screen = DefaultScreen (display);

	display_width = DisplayWidth (display, screen);
	display_height = DisplayHeight (display, screen);

	width = display_width / 3;
	if (width < 350)
		width = 350;
	height = display_height / 4;
	if (height < 250)
		height = 250;

	win = XCreateSimpleWindow (display, RootWindow (display, screen),
	    x, y, width, height, border_width,
	    BlackPixel (display, screen),
	    WhitePixel (display, screen));
#if 0
	Pixmap icon_pixmap;
	XSizeHints size_hints;
	char *window_name = "Basic Window Program";
	char *icon_name = "basicwin";
#include "bitmaps/icon_bitmap"

	icon_pixmap = XCreateBitmapFromData (display, win, icon_bitmap_bits,
	    icon_bitmap_width, icon_bitmap_height);

	size_hints.flags = PPosition | PSize | PMinSize;
	size_hints.x = x;
	size_hints.y = y;
	size_hints.width = width;
	size_hints.height = height;
	size_hints.min_width = 350;
	size_hints.min_height = 250;

	/* XSetStandardProperties() is not implemented in nxlib yet. */
	XSetStandardProperties (display, win, window_name, icon_name, icon_pixmap,
	    0, 0, &size_hints);
#endif
	XSelectInput (display, win, ExposureMask |
	    KeyPressMask |
	    ButtonPressMask |
	    StructureNotifyMask);

	load_font (&font_info);

	get_GC (win, &gc, font_info);

	XMapWindow (display, win);

	while (1) {
		XNextEvent (display, &report);
		switch (report.type) {
		case Expose:
			while (XCheckTypedEvent (display, Expose, &report))
				continue;
			if (window_size == SMALL)
				TooSmall (win, gc, font_info);
			else {
				draw_text (win, gc, font_info, width, height);
				draw_graphics (win, gc, width, height);
			}
			break;

		case ConfigureNotify:
			width = report.xconfigure.width;
			height = report.xconfigure.height;
			if (width < 350 || height < 250)
				window_size = SMALL;
			else
				window_size = OK;
			break;

		case ButtonPress:
			break;

		case KeyPress:
			if (report.xkey.keycode != 'q' &&
			    report.xkey.keycode != 'Q')
				break;
			XUnloadFont (display, font_info->fid);
			XFreeGC (display, gc);
			XCloseDisplay (display);
			task_exit (0);
			break;

		default:
			break;
		}
	}
}
