# include <X11/Xlib.h>
# include "window.h"
# include "pen.h"
# include "cursor.h"
# include "font.h"
# include "extern.h"

xCursor::xCursor (xScreen *scr, xCursorShape shape)
	: fgcolor (0), bgcolor (xColorMax)
{
	screen = scr;
	xcursor = XCreateFontCursor (screen->xdpy, (int) shape);
}

xCursor::xCursor (xScreen *scr, int w, int h, char *data, char *mask, int hotx, int hoty)
	: fgcolor (0), bgcolor (xColorMax)
{
	XColor fg, bg;
	Pixmap cpix, cmpix;

	screen = scr;
	cpix = XCreateBitmapFromData (screen->xdpy, screen->xwin, data, w, h);
	if (! cpix)
		xFatal ("Cannot create cursor bitmap\n");

	cmpix = XCreateBitmapFromData (screen->xdpy, screen->xwin, mask, w, h);
	if (! cmpix)
		xFatal ("Cannot create mask bitmap\n");

	fg.red = fg.green = fg.blue = 0;
	bg.red = bg.green = bg.blue = 0xffff;
	xcursor = XCreatePixmapCursor (screen->xdpy, cpix, cmpix, &fg, &bg, hotx, hoty);
	if (! xcursor)
		xFatal ("Cannot create cursor\n");

	XFreePixmap (screen->xdpy, cpix);
	XFreePixmap (screen->xdpy, cmpix);
}

xCursor::xCursor (xScreen *scr, char *datafile, char *maskfile)
	: fgcolor (0), bgcolor (xColorMax)
{
	XColor fg, bg;
	int hotx, hoty;

	screen = scr;
	xBitmap shape (screen, datafile, &hotx, &hoty); // load shape file
	xBitmap mask (screen, maskfile);                // load mask file
	fg.red = fg.green = fg.blue = 0;
	bg.red = bg.green = bg.blue = 0xffff;
	xcursor = XCreatePixmapCursor (screen->xdpy, shape.xpixmap,
		mask.xpixmap, &fg, &bg, hotx, hoty);
	if (! xcursor)
		xFatal ("Cannot create cursor\n");
}

xCursor::xCursor (xScreen *scr, xBitmap *shape, xBitmap *mask, int hotx, int hoty)
	: fgcolor (0), bgcolor (xColorMax)
{
	XColor fg, bg;

	screen = scr;
	fg.red = fg.green = fg.blue = 0;
	bg.red = bg.green = bg.blue = 0xffff;
	xcursor = XCreatePixmapCursor (screen->xdpy, shape->xpixmap,
		mask->xpixmap, &fg, &bg, hotx, hoty);
	if (! xcursor)
		xFatal ("Cannot create cursor\n");
}

xCursor::xCursor (xScreen *scr, xFont *gfnt, int gindx, xFont *mfnt, int mindx)
	: fgcolor (0), bgcolor (xColorMax)
{
	XColor fg, bg;

	if (! mfnt) {
		mfnt = gfnt;
		mindx = gindx + 1;
	}
	screen = scr;
	fg.red = fg.green = fg.blue = 0;
	bg.red = bg.green = bg.blue = 0xffff;
	xcursor = XCreateGlyphCursor (screen->xdpy, gfnt->info->fid,
		mfnt->info->fid, gindx, mindx, &fg, &bg);
	if (! xcursor)
		xFatal ("Cannot create glyph cursor\n");
}

xCursor::~xCursor ()
{
	XFreeCursor (screen->xdpy, xcursor);
}

void xCursor::Recolor (xColor f, xColor b)
{
	XColor fg, bg;

	fgcolor = f;
	bgcolor = b;
	fg.red = f.red;
	fg.green = f.green;
	fg.blue = f.blue;
	bg.red = b.red;
	bg.green = b.green;
	bg.blue = b.blue;
	XRecolorCursor (screen->xdpy, xcursor, &fg, &bg);
}
