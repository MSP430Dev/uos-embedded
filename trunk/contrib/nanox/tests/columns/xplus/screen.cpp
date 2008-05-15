# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xresource.h>
# include <X11/Xatom.h>
# include "window.h"
# include "font.h"
# include "event.h"
# include "extern.h"

# include "slist.h"
# include "qlist.h"
# include "ttable.h"
# include "ntable.h"
# include "htable.h"

struct PixelTable {
	xPixel          pixel;
	unsigned char   red;
	unsigned char   green;
	unsigned char   blue;
	char            allocated;
};

static XrmOptionDescRec options [] = {
    { "+rv",          "*reverseVideo", XrmoptionNoArg,  "off" },
    { "+synchronous", "*synchronous",  XrmoptionNoArg,  "off" },
    { "-background",  "*background",   XrmoptionSepArg, 0     },
    { "-bd",          "*borderColor",  XrmoptionSepArg, 0     },
    { "-bg",          "*background",   XrmoptionSepArg, 0     },
    { "-bordercolor", "*borderColor",  XrmoptionSepArg, 0     },
    { "-borderwidth", ".borderWidth",  XrmoptionSepArg, 0     },
    { "-bw",          ".borderWidth",  XrmoptionSepArg, 0     },
    { "-display",     ".display",      XrmoptionSepArg, 0     },
    { "-fg",          "*foreground",   XrmoptionSepArg, 0     },
    { "-fn",          "*font",         XrmoptionSepArg, 0     },
    { "-font",        "*font",         XrmoptionSepArg, 0     },
    { "-foreground",  "*foreground",   XrmoptionSepArg, 0     },
    { "-geometry",    ".geometry",     XrmoptionSepArg, 0     },
    { "-iconic",      ".iconic",       XrmoptionNoArg,  "on"  },
    { "-name",        ".name",         XrmoptionSepArg, 0     },
    { "-reverse",     "*reverseVideo", XrmoptionNoArg,  "on"  },
    { "-rv",          "*reverseVideo", XrmoptionNoArg,  "on"  },
    { "-synchronous", "*synchronous",  XrmoptionNoArg,  "on"  },
    { "-title",       ".title",        XrmoptionSepArg, 0     },
    { "-xrm",         0,               XrmoptionResArg, 0     },
};

static const int num_options = sizeof (options) / sizeof (options [0]);

static char *appname (char *str)
{
	char *buf;
	char *p;

	// Make application name from full path name.
	// Remove directory names, and '.' and '*' characters.
	p = strrchr (str, '/');
	if (p)
		str = p+1;

	buf = (char*) malloc (strlen (str) + 1);
	if (! buf)
		return ("NONAME");

	for (p=buf; *str; ++str)
		if (*str != '*' && *str != '.')
			*p++ = *str;
	*p = 0;
	if (! *buf)
		return ("NONAME");
	return (buf);
}

static int get1hex (int c)
{
	switch (c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (c - '0');
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (c - 'a' + 10);
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (c - 'A' + 10);
	}
	return (0);
}

static int gethex (char *str, int len)
{
	int n = 0;

	while (--len >= 0)
		n = n << 4 | get1hex (*str++);
	return (n);
}

xScreen::xScreen (char *cnam, int *ac, char **av)  // open connection
{
	int scrnum = -1;
	char *application_name = appname (av [0]);

	screen = this;
	parent = this;

	// Initialize resource manager functions.

	XrmInitialize ();

	// Create class and name quark lists.

	classq = new QuarkList (cnam);
	nameq = new QuarkList (application_name);

	// Parse command line arguments.

	XrmParseCommand (&database, options, num_options, application_name, ac, av);

	// Get name of display.

	char *dispname = Attribute ("Display");

	// Open display.

	xdpy = XOpenDisplay (dispname);
	if (! xdpy)
		xFatal ("Cannot open display\n");

	if (scrnum < 0)
		scrnum = XDefaultScreen (xdpy);

	xscr = ScreenOfDisplay (xdpy, scrnum);
	xwin = RootWindowOfScreen ((Screen*) xscr);
	xvisual = DefaultVisual (xdpy, scrnum);
	xcmap = DefaultColormapOfScreen ((Screen*) xscr);
	width = WidthOfScreen ((Screen*) xscr);
	height = HeightOfScreen ((Screen*) xscr);
	depth = DefaultDepthOfScreen ((Screen*) xscr);

	// Initialize color table

	switch (((Visual*)xvisual)->c_class) {
		long m;
	case PseudoColor:               // read/write, limited colors
		if (((Visual*)xvisual)->map_entries > 500000) {
			nred = ngreen = nblue = 32;
			ngray = 4096;
		} else if (((Visual*)xvisual)->map_entries > 4000) {
			nred = ngreen = nblue = 13;
			ngray = 512;
		} else if (((Visual*)xvisual)->map_entries > 249) {
			nred = ngreen = nblue = 5;
			ngray = 17;
		} else if (((Visual*)xvisual)->map_entries > 15) {
			nred = ngreen = nblue = 2;
			ngray = 4;
		} else {
			nred = ngreen = nblue = 1;
			ngray = ((Visual*)xvisual)->map_entries * 3 / 4;
			if (ngray < 2)
				ngray = ((Visual*)xvisual)->map_entries;
		}
		break;
	case StaticColor:               // read-only, limited colors
		ngray = 2;
		while (ngray * ngray * ngray < ((Visual*)xvisual)->map_entries)
			++ngray;
		nred = ngreen = nblue = ngray;
		break;
	case DirectColor:               // read/write, many colors
		m = ((Visual*)xvisual)->red_mask;
		while (! (m & 01))
			m >>= 1;
		nred = m>=8 ? m/2 : m-1;
		if (nred > 32)
			nred = 32;

		m = ((Visual*)xvisual)->green_mask;
		while (! (m & 01))
			m >>= 1;
		ngreen = m>=8 ? m/2 : m-1;
		if (ngreen > 32)
			ngreen = 32;

		m = ((Visual*)xvisual)->blue_mask;
		while (! (m & 01))
			m >>= 1;
		nblue = m>=8 ? m/2 : m-1;
		if (nblue > 32)
			nblue = 32;

		ngray = ((Visual*)xvisual)->map_entries - nred * ngreen * nblue;
		if (ngray > 100000)
			ngray = 4096;
		else if (ngray > 1000)
			ngray = 512;
		else if (ngray > 100)
			ngray = 32;
		break;
	case TrueColor:                 // read-only, many colors
		m = ((Visual*)xvisual)->red_mask;
		while (! (m & 01))
			m >>= 1;
		nred = m + 1;
		if (nred > 32)
			nred = 32;

		m = ((Visual*)xvisual)->green_mask;
		while (! (m & 01))
			m >>= 1;
		ngreen = m + 1;
		if (ngreen > 32)
			ngreen = 32;

		m = ((Visual*)xvisual)->blue_mask;
		while (! (m & 01))
			m >>= 1;
		nblue = m + 1;
		if (nblue > 32)
			nblue = 32;

		ngray = nred;
		if (ngreen < ngray)
			ngray = ngreen;
		if (nblue < ngray)
			ngray = nblue;
		break;
	case GrayScale:                 // read/write, monocrome shades
		nred = ngreen = nblue = 0;
		ngray = ((Visual*)xvisual)->map_entries * 3 / 4;
		if (ngray < 2)
			ngray = ((Visual*)xvisual)->map_entries;
		break;
	case StaticGray:                // read-only, monocrome shades
		nred = ngreen = nblue = 0;
		ngray = ((Visual*)xvisual)->map_entries;
		break;
	default:
		xFatal ("bad visual class???\n");
	}
	pixtable = new PixelTable [ngray + nred * ngreen * nblue];
	for (int i = ngray + nred * ngreen * nblue - 1; i>=0; --i)
		pixtable[i].allocated = 0;
	cvaltable = new HashTable ();
	cnametable = new NameTable ();
	fgpixel = Pixel (0);
	bgpixel = Pixel (xColorMax);

	// Get application and user databases.

	CreateDatabase (cnam);

	char *font_name = Attribute ("Font");
	if (! font_name)
		font_name = "fixed";
	font = new xFont (this, font_name);

	ttable = new TargetTable ();    // create Window => xWindow* map
	childlist = new SimpleList ();

	max_width = opt_width = min_width = width;
	max_height = opt_height = min_height = height;
	x = y = width_inc = height_inc = 0;
}

xScreen::~xScreen ()
{
	delete pixtable;
	delete ttable;
	delete childlist;
	XCloseDisplay (xdpy);
}

void xScreen::WarpPointer (int dx, int dy)
{
	XWarpPointer (xdpy, None, None, 0, 0, 0, 0, dx, dy);
}

void xScreen::Flush ()
{
	XFlush (xdpy);
}

int xScreen::Read (xEvent& event, int asyncflag)
{
	XEvent xev;
	xWindow *win;
int qlen;
loop:
	if (asyncflag) {
		Flush ();
		if (! (qlen = Queue ()))
			return (0);
	}
	XNextEvent (xdpy, &xev);
	switch (xev.type) {
	default:
		goto loop;
	case ButtonPress:
	case ButtonRelease:
	case KeyPress:
	case MotionNotify:
	case EnterNotify:
	case LeaveNotify:
	case Expose:
	case ConfigureNotify:
		break;
	}
	win = FindTarget (xev.xany.window);
	if (! win)
		xFatal ("Cannot find event target\n");

	switch (xev.type) {
	case Expose:
		win->Redraw (xev.xexpose.x, xev.xexpose.y,
			xev.xexpose.width, xev.xexpose.height);
		goto loop;
	case ConfigureNotify:
		win->x = xev.xconfigure.x;
		win->y = xev.xconfigure.y;
		if (win->width != xev.xconfigure.width ||
		    win->height != xev.xconfigure.height) {
			win->width = xev.xconfigure.width;
			win->height = xev.xconfigure.height;
			win->Resize ();
		}
		goto loop;
	}
	event.Parse (xev);
	event.window = win;
	return (1);
}

void xScreen::Initialize ()
{
	// realize all windows
	xWindow *w;

	w = (xWindow*) childlist->First ();
	for (; w; w = (xWindow*) childlist->Next ()) {
		w->ComputeSize ();
		w->Initialize ();
	}
}

void xScreen::Run ()
{
	// realize all windows

	Initialize ();

	// read and process all input events

	while (! childlist->Empty ()) {
		xEvent event;

		Read (event);
		xWindow *w = event.window;
		w->Handle (event);
		if (! event.window) {
			Delete (w);
			delete w;
		}
	}
}

void xScreen::RegisterTarget (xWindow *win)
{
	ttable->Insert ((long) win->xwin, (void *) win);
}

void xScreen::UnregisterTarget (xWindow *win)
{
	ttable->Remove ((long) win->xwin);
}

xWindow *xScreen::FindTarget (long id)
{
	return ((xWindow *) ttable->Find (id));
}

void xScreen::CreateDatabase (char *cnam)
{
	XrmDatabase rdb, appdb;
	char filename [100];

	// Create empty database.

	appdb = XrmGetStringDatabase ("");

	// Read application defaults file.

	strcpy (filename, "/usr/lib/X11/app-defaults/");
	strcat (filename, cnam);
	rdb = XrmGetFileDatabase (filename);
	if (rdb)
		XrmMergeDatabases (rdb, &appdb);

	// Add server resource database.
	// If any, read ~/.Xdefaults.

	char *rdbstring;

	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long leftover;

	if (Success != XGetWindowProperty (xdpy, xwin, XA_RESOURCE_MANAGER,
	    0L, 100000000L, False, XA_STRING, &actual_type, &actual_format,
	    &nitems, &leftover, (unsigned char **) &rdbstring))
		rdbstring = 0;

	if (rdbstring)
		rdb = XrmGetStringDatabase (rdbstring);
	else {
		char *home = getenv ("HOME");
		if (! home)
			home = "/";
		strcpy (filename, home);
		strcat (filename, ".Xdefaults");
		rdb = XrmGetFileDatabase (filename);
	}
	if (rdb)
		XrmMergeDatabases (rdb, &appdb);

	// Add file specified in XENVIRONMENT global variable.

	char *xenv = getenv ("XENVIRONMENT");
	if (xenv) {
		rdb = XrmGetFileDatabase (xenv);
		if (rdb)
			XrmMergeDatabases (rdb, &appdb);
	}

	// Append command line arguments database.

	XrmMergeDatabases (database, &appdb);
	database = appdb;
}

xPixel xScreen::Pixel (char *cname)
{
	// Check if color name is of form #XXX
	if (*cname == '#') {
		int r = 0, g = 0, b = 0;

		++cname;
		if (strlen (cname) == 3) {
			r = gethex (cname, 1) << 4;
			g = gethex (++cname, 1) << 4;
			b = gethex (++cname, 1) << 4;
		} else if (strlen (cname) == 6) {
			r = gethex (cname, 2);
			g = gethex (cname += 2, 2);
			b = gethex (cname += 2, 2);
		} else if (strlen (cname) == 9) {
			r = gethex (cname, 3) >> 4;
			g = gethex (cname += 3, 3) >> 4;
			b = gethex (cname += 3, 3) >> 4;
		} else if (strlen (cname) == 12) {
			r = gethex (cname, 4) >> 8;
			g = gethex (cname += 4, 4) >> 8;
			b = gethex (cname += 4, 4) >> 8;
		} else
			xFatal ("Bad color name '%s'\n", --cname);
		return (Pixel (r, g, b));
	}

	// First, search for color in name hash.
	int i = cnametable->Find (cname);

	if (i < 0)
		// Unknown name; send query to server.
		i = InitializeColor (cname);

	return (pixtable[i].pixel);
}

int xScreen::InitializeColor (char *cname)
{
	// Unknown name; send query to server.
	XColor xc, xexact;

	if (! XLookupColor (screen->xdpy, xcmap, cname, &xexact, &xc)) {
		xError ("Unknown color, using default\n");
		return (0);
	}

	// Compute index in pixel table.
	int i = GetColorRGBIndex (xc.red >> 8, xc.green >> 8, xc.blue >> 8) -
		pixtable;

	// Add new color name to hash table.
	cnametable->Insert (cname, i);
	return (i);
}

xPixel xScreen::Pixel (int r, int g, int b)
{
	return (GetColorRGBIndex (r, g, b)->pixel);
}

void xScreen::AllocateRGBColor (PixelTable *p, int r, int g, int b)
{
	XColor xc;

	xc.red = r << 8;;
	xc.green = g << 8;
	xc.blue = b << 8;
	xc.flags = DoRed | DoGreen | DoBlue;
	XAllocColor (screen->xdpy, xcmap, &xc);
	p->pixel = xc.pixel;
	p->red = xc.red >> 8;
	p->green = xc.green >> 8;
	p->blue = xc.blue >> 8;
	p->allocated = 1;
	if (cvaltable->Find (p->pixel) < 0)
		cvaltable->Insert (p->pixel, p - pixtable);
}

PixelTable *xScreen::GetColorRGBIndex (int r, int g, int b)
{
	// Compute index of RGB color in pixel table.
	// Table consists of:
	// 1) ngray entries for gray shades
	// 2) nred*ngreen*nblue entries for colors
	// If nred==0 then there is no color entries,
	// and all colors must be gray.

	if (nred < 2 || r==g && g==b) {
		// Find gray color.
findgray:
		//int intens = (r * 30 + g * 59 + b * 11) / 100;
		PixelTable *p = pixtable + (r * (ngray - 1) + xColorMax/2) / xColorMax;
		if (! p->allocated)
			AllocateRGBColor (p, g, g, g);
		return (p);
	}
	int ri = (r * (nred - 1) + xColorMax/2) / xColorMax;
	int gi = (g * (nred - 1) + xColorMax/2) / xColorMax;
	int bi = (b * (nred - 1) + xColorMax/2) / xColorMax;
	if (ri==gi && gi==bi)
		goto findgray;
	PixelTable *p = pixtable + ngray + ri + nred * (gi + ngreen * bi);
	if (! p->allocated)
		AllocateRGBColor (p, ri * xColorMax / nred,
			gi * xColorMax / ngreen, bi * xColorMax / nblue);
	return (p);
}

xColor xScreen::Color (xPixel px)
{
	int i = cvaltable->Find (px);
	if (i < 0)
		return (xColor (0));
	PixelTable *p = pixtable + i;
	return (xColor (p->red, p->green, p->blue));
}

xColor xScreen::Color (char *cname)
{
	// First, search for color in name hash.
	int i = cnametable->Find (cname);

	if (i < 0)
		// Unknown name; send query to server.
		i = InitializeColor (cname);

	PixelTable *p = pixtable + i;
	return (xColor (p->red, p->green, p->blue));
}

int xScreen::Queue (int mode)
{
	return (XEventsQueued (xdpy, mode));
}

void xScreen::Bell (int volume)
{
	XBell (xdpy, volume);
}
