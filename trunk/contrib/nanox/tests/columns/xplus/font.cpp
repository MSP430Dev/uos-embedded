# include <X11/Xlib.h>
# include "window.h"
# include "font.h"
# include "extern.h"

xFont::xFont (xScreen *scr, char *name)
{
	screen = scr;
	info = XLoadQueryFont (screen->xdpy, name);
	if (! info)
		xFatal ("Cannot load font '%s'\n", name);
	width = info->max_bounds.width;
	height = info->ascent;
	depth = info->descent;
}

int xFont::Width (char *str, int len)
{
	if (! len)
		len = strlen (str);
	return (XTextWidth (info, str, len));
}

int xFont::Width (int sym)
{
	char str [1];
	str [0] = sym;
	return (XTextWidth (info, str, 1));
}

xFont::~xFont ()
{
	XFreeFont (screen->xdpy, info);
}
