# include <X11/Xlib.h>
# include "window.h"
# include "pen.h"
# include "extern.h"

xPixmap::xPixmap (xScreen *scr, int wid, int hgh)
{
	screen = scr;
	width = wid;
	height = hgh;
	xpixmap = XCreatePixmap (screen->xdpy, screen->xwin,
		width, height, screen->depth);
	if (! xpixmap)
		xFatal ("Cannot create pixmap\n");
}

xPixmap::~xPixmap ()
{
	XFreePixmap (screen->xdpy, xpixmap);
}
