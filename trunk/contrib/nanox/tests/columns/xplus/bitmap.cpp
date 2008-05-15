# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include "window.h"
# include "pen.h"
# include "extern.h"

static int contains (char *str, int symb)
{
	for (; *str; ++str)
		if (*str == symb)
			return (1);
	return (0);
}

xBitmap::xBitmap (xScreen *scr, char *name, int *photx, int *photy)
{
	char buf [100];
	unsigned int wid, hgh;
	int hotx, hoty;
	Pixmap pmap;

	screen = scr;
	if (! photx)
		photx = &hotx;
	if (! photy)
		photy = &hoty;
	*photx = *photy = 0;

	if (contains (name, '/') || access (name, 4) == 0)
		strcpy (buf, name);
	else {
		strcpy (buf, "/usr/include/X11/bitmaps/");
		strcat (buf, name);
	}
	int rez = XReadBitmapFile (screen->xdpy, screen->xwin,
	    buf, &wid, &hgh, &pmap, photx, photy);
	switch (rez) {
	case BitmapOpenFailed:
		xFatal ("Cannot open bitmap file\n");
	case BitmapFileInvalid:
		xFatal ("Bad bitmap file\n");
	case BitmapNoMemory:
		xFatal ("No memory for bitmap\n");
	default:
		xFatal ("Unknown error reading bitmap file\n");
	case BitmapSuccess:
		break;
	}
	xpixmap = pmap;
	width = wid;
	height = hgh;
}

xBitmap::xBitmap (xScreen *scr, int wid, int hgh, char *data)
{
	screen = scr;
	width = wid;
	height = hgh;
	xpixmap = XCreateBitmapFromData (screen->xdpy, screen->xwin,
		data, width, height);
	if (! xpixmap)
		xFatal ("Cannot create bitmap from data\n");
}

xBitmap::~xBitmap ()
{
	XFreePixmap (screen->xdpy, xpixmap);
}
