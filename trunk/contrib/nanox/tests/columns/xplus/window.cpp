# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xresource.h>
# include "window.h"
# include "event.h"
# include "sensor.h"
# include "cursor.h"
# include "extern.h"

# include "slist.h"
# include "qlist.h"

xWindow::xWindow (xWindow *win, char *cnam, char *nam)
{
	screen = win->screen;
	parent = win;
	xwin = 0;
	mapflag = 1;

	// Create class and name quark lists.
	// They are used for retrieving attributes (resources).
	// Append our name to parent's list.

	classq = new QuarkList (win->classq, cnam);
	nameq = new QuarkList (win->nameq, nam);

	x = y = 0;
	min_width = min_height = 1;
	width = height = opt_width = opt_height = 10;
	max_width = max_height = 10000;
	width_inc = height_inc = 1;

	childlist = new SimpleList ();

	parent->Insert (this);
}

xWindow::~xWindow ()
{
	parent->Delete (this);
	if (xwin) {
		screen->UnregisterTarget (this);
		XDestroyWindow (screen->xdpy, xwin);
	}
	delete nameq;
	delete classq;
	delete childlist;
}

void xWindow::ComputeSize ()
{
	xWindow *w;

	w = (xWindow*) childlist->First ();
	for (; w; w = (xWindow*) childlist->Next ())
		w->ComputeSize ();
}

void xWindow::Listen (xSensor *inp)
{
	long eventmask;

	eventmask = ExposureMask | StructureNotifyMask;
	if (inp->mask & xMaskDown)
		eventmask |= ButtonPressMask;
	if (inp->mask & xMaskUp)
		eventmask |= ButtonReleaseMask;
	if (inp->mask & xMaskKey)
		eventmask |= KeyPressMask;
	if (inp->mask & xMaskEnter)
		eventmask |= EnterWindowMask;
	if (inp->mask & xMaskLeave)
		eventmask |= LeaveWindowMask;
	if (inp->mask & xMaskMotion)
		eventmask |= PointerMotionMask;
	if (inp->mask & xMaskButtonMotion)
		eventmask |= ButtonMotionMask;
	if (inp->mask & xMaskButton1Motion)
		eventmask |= Button1MotionMask;
	if (inp->mask & xMaskButton2Motion)
		eventmask |= Button2MotionMask;
	if (inp->mask & xMaskButton2Motion)
		eventmask |= Button2MotionMask;
	XSelectInput (screen->xdpy, xwin, eventmask);
}

void xWindow::Initialize ()
{
	if (xwin)               // if window is already realized
		return;         // do nothing

	xwin = XCreateSimpleWindow (screen->xdpy, parent->xwin, x, y,
		width, height, 0, BlackPixelOfScreen ((Screen*) screen->xscr),
		WhitePixelOfScreen ((Screen*) screen->xscr));
	if (! xwin)
		xFatal ("Cannot create window\n");

	screen->RegisterTarget (this);

	XSelectInput (screen->xdpy, xwin, ExposureMask | StructureNotifyMask);

	if (mapflag)
		Map ();

	xWindow *w = (xWindow*) childlist->First ();
	for (; w; w = (xWindow*) childlist->Next ())
		w->Initialize ();

	if (screen == parent) {
		// If we are the top level window, then
		// we should set hints for window manager.

		XStoreName (screen->xdpy, xwin, nameq->LastString ());

		XSizeHints size_hints;

		size_hints.flags = PSize | PMinSize | PMaxSize | PResizeInc;
		size_hints.width = width;
		size_hints.height = height;
		size_hints.min_width = min_width;
		size_hints.min_height = min_height;
		size_hints.max_width = max_width;
		size_hints.max_height = max_height;
		size_hints.width_inc = width_inc;
		size_hints.height_inc = height_inc;

		XSetNormalHints (screen->xdpy, xwin, &size_hints);
	}
}

void xWindow::Move (int newx, int newy)
{
	x = newx;
	y = newy;
	if (xwin)
		XMoveWindow (screen->xdpy, xwin, x, y);
}

void xWindow::Map ()
{
	if (xwin)
		XMapWindow (screen->xdpy, xwin);
	mapflag = 1;
}

void xWindow::Unmap ()
{
	if (xwin)
		XUnmapWindow (screen->xdpy, xwin);
	mapflag = 0;
}

void xWindow::Cursor (xCursor *curs)
{
	XDefineCursor (screen->xdpy, xwin, curs->xcursor);
}

void xWindow::Cursor ()
{
	XUndefineCursor (screen->xdpy, xwin);
}

void xWindow::Background (xPixel pixval)
{
	XSetWindowBackground (screen->xdpy, xwin, pixval);
}

int xWindow::QueryPointer (int *px, int *py)
{
	Window root_win, child_win;
	int rx, ry;
	unsigned int mask;

	return (XQueryPointer (screen->xdpy, xwin, &root_win, &child_win,
		&rx, &ry, px, py, &mask));
}

void xWindow::Clear ()
{
	XClearWindow (screen->xdpy, xwin);
}

void xWindow::Clear (int x, int y, int wid, int hgh, int exposures)
{
	XClearArea (screen->xdpy, xwin, x, y, wid, hgh, exposures);
}

void xWindow::Insert (xWindow *win)
{
	childlist->Insert ((void *) win);
}

void xWindow::Delete (xWindow *win)
{
	childlist->Delete ((void *) win);
}

void xWindow::AdjustWidth ()
{
	if (width > max_width)
		width = max_width;
	else if (width < min_width)
		width = min_width;
	else
		width = min_width + (width - min_width) /
			width_inc * width_inc;
}

void xWindow::AdjustHeight ()
{
	if (height > max_height)
		height = max_height;
	else if (height < min_height)
		height = min_height;
	else
		height = min_height + (height - min_height) /
			height_inc * height_inc;
}

char *xWindow::Attribute (char *cnam, char *nam)
{
	char nambuf [100];
	XrmRepresentation atype;
	XrmValue avalue;

	if (! nam) {
		// Make name by converting the first letter
		// of class name to lowercase.

		strcpy (nambuf, cnam);
		nam = nambuf;
		if (*nam >= 'A' && *nam <= 'Z')
			*nam += 'a' - 'A';
	}

	// Append attribute name to quark lists.

	nameq->Append (nam);
	classq->Append (cnam);

	int rez = XrmQGetResource (screen->database, nameq->list, classq->list,
		&atype, &avalue);

	if (! rez)
		return (0);

	// You should not modify string, returned from GetAttribute.

	return (avalue.addr);
}

void xWindow::Attribute (int &val, int dflt, char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	if (! p)
		val = dflt;
	else if (!strcmp (p, "1") || !strcmp (p, "true") ||
	    !strcmp (p, "True"))
		val = 1;
	else
		val = 0;
}

void xWindow::Attribute (int &val, int dflt, int min, int max,
	char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	val = p ? atoi (p) : dflt;
	if (val < min)
		val = min;
	else if (max > min && val > max)
		val = max;
}

void xWindow::Attribute (xPixel &val, char *dflt, char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	val = screen->Pixel (p ? p : dflt);
}

void xWindow::Attribute (xPixel &val, xPixel dflt, char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	val = p ? screen->Pixel (p) : dflt;
}

void xWindow::Attribute (xColor &val, char *dflt, char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	val = screen->Color (p ? p : dflt);
}

void xWindow::Attribute (xColor &val, xColor dflt, char *cnam, char *nam)
{
	char *p = Attribute (cnam, nam);
	val = p ? screen->Color (p) : dflt;
}
