# include <X11/Xlib.h>
# include "window.h"
# include "pen.h"
# include "font.h"
# include "image.h"
# include "extern.h"

xPen::xPen (xScreen *scr)
{
	screen = scr;
	xgc = XCreateGC (screen->xdpy, RootWindowOfScreen ((Screen*) screen->xscr),
		0L, (XGCValues *) 0);
	XCopyGC (screen->xdpy, DefaultGCOfScreen ((Screen*) screen->xscr),
		(2L << GCLastBit) - 1, xgc);
}

xPen::xPen (xPen *out)
{
	screen = out->screen;
	xgc = XCreateGC (screen->xdpy, RootWindowOfScreen ((Screen*) screen->xscr),
		0L, (XGCValues *) 0);
	XCopyGC (screen->xdpy, out->xgc, (2L << GCLastBit) - 1, xgc);
}

xPen::~xPen ()
{
	XFreeGC (screen->xdpy, xgc);
}

void xPen::Foreground (xPixel pixel)
{
	XSetForeground (screen->xdpy, xgc, pixel);
}

void xPen::Background (xPixel pixel)
{
	XSetBackground (screen->xdpy, xgc, pixel);
}

void xPen::Function (xPenFunction fun)
{
	XSetFunction (screen->xdpy, xgc, (int) fun);
}

void xPen::Clip (int x, int y, int w, int h)
{
	XRectangle rect;

	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;
	XSetClipRectangles (screen->xdpy, xgc, 0, 0, &rect, 1, Unsorted);
}

void xPen::Clip ()
{
	XSetClipMask (screen->xdpy, xgc, None);
}

void xPen::Tile ()
{
	XGCValues gcv;

	gcv.fill_style = FillSolid;
	XChangeGC (screen->xdpy, xgc, GCFillStyle, &gcv);
}

void xPen::Tile (int x, int y)
{
	XSetTSOrigin (screen->xdpy, xgc, x, y);
}

void xPen::Tile (xPixmap *pm)
{
	XGCValues gcv;

	gcv.tile = pm->xpixmap;
	gcv.fill_style = FillTiled;
	gcv.ts_x_origin = 0;
	gcv.ts_y_origin = 0;
	XChangeGC (screen->xdpy, xgc, GCTile | GCFillStyle |
		GCTileStipXOrigin | GCTileStipYOrigin, &gcv);
}

void xPen::Tile (xBitmap *bm, int nopq)
{
	XGCValues gcv;

	gcv.stipple = bm->xpixmap;
	gcv.fill_style = nopq ? FillStippled : FillOpaqueStippled;
	gcv.ts_x_origin = 0;
	gcv.ts_y_origin = 0;
	XChangeGC (screen->xdpy, xgc, GCStipple | GCFillStyle |
		GCTileStipXOrigin | GCTileStipYOrigin, &gcv);
}

void xPen::Copy (long xwin1, long xwin2, int x1, int y1,
	int w, int h, int x2, int y2)
{
	XCopyArea (screen->xdpy, xwin1, xwin2, xgc, x1, y1, w, h, x2, y2);
}

void xPen::BitCopy (long xwin1, long xwin2, int x1, int y1,
	int w, int h, int x2, int y2)
{
	XCopyPlane (screen->xdpy, xwin1, xwin2, xgc, x1, y1, w, h, x2, y2, 1);
}

void xPen::Point (long xwin, int x, int y)
{
	XDrawPoint (screen->xdpy, xwin, xgc, x, y);
}

void xPen::Line (long xwin, int x1, int y1, int x2, int y2)
{
	XDrawLine (screen->xdpy, xwin, xgc, x1, y1, x2, y2);
}

void xPen::Rectangle (long xwin, int x1, int y1, int wid, int hgh)
{
	XDrawRectangle (screen->xdpy, xwin, xgc, x1, y1, wid, hgh);
}

void xPen::Arc (long xwin, int x, int y, int w, int h, int a1, int a2)
{
	XDrawArc (screen->xdpy, xwin, xgc, x, y, w, h, a1, a2);
}

void xPen::RectangleFill (long xwin, int x1, int y1, int wid, int hgh)
{
	XFillRectangle (screen->xdpy, xwin, xgc, x1, y1, wid, hgh);
}

void xPen::ArcFill (long xwin, int x, int y, int w, int h, int a1, int a2,
	int segmflag)
{
	if (segmflag)           // set chord fill mode
		XSetArcMode (screen->xdpy, xgc, ArcChord);
	XFillArc (screen->xdpy, xwin, xgc, x, y, w, h, a1, a2);
	if (segmflag)
		XSetArcMode (screen->xdpy, xgc, ArcPieSlice);
}

void xPen::Polygon (long xwin, int count, int *array)
{
	XPoint points [count+1];

	points[count].x = *array;       // last point is equal
	points[count].y = *array;       // to the first one
	for (int i=0; i<count; ++i) {
		points[i].x = *array++;
		points[i].y = *array++;
	}
	XDrawLines (screen->xdpy, xwin, xgc, points, count+1, CoordModeOrigin);
}

void xPen::Points (long xwin, int count, int *array)
{
	XPoint points [count];

	for (int i=0; i<count; ++i) {
		points[i].x = *array++;
		points[i].y = *array++;
	}
	XDrawPoints (screen->xdpy, xwin, xgc, points, count, CoordModeOrigin);
}

void xPen::Polyline (long xwin, int count, int *array)
{
	XPoint points [count];

	for (int i=0; i<count; ++i) {
		points[i].x = *array++;
		points[i].y = *array++;
	}
	XDrawLines (screen->xdpy, xwin, xgc, points, count, CoordModeOrigin);
}

void xPen::Lines (long xwin, int count, int *array)
{
	XSegment segments [count];

	for (int i=0; i<count; ++i) {
		segments[i].x1 = *array++;
		segments[i].y1 = *array++;
		segments[i].x2 = *array++;
		segments[i].y2 = *array++;
	}
	XDrawSegments (screen->xdpy, xwin, xgc, segments, count);
}

void xPen::PolygonFill (long xwin, int count, int *array, int wflag)
{
	XPoint points [count];

	for (int i=0; i<count; ++i) {
		points[i].x = *array++;
		points[i].y = *array++;
	}
	if (wflag)              // set winding fill mode
		XSetFillRule (screen->xdpy, xgc, WindingRule);
	XFillPolygon (screen->xdpy, xwin, xgc, points, count,
		Complex, CoordModeOrigin);
	if (wflag)
		XSetFillRule (screen->xdpy, xgc, EvenOddRule);
}

void xPen::Text (long xwin, xFont *fn, int x, int y,
	char *str, int len, int imgflag)
{
	if (len <= 0)
		len = strlen (str);
	XSetFont (screen->xdpy, xgc, fn->info->fid);
	if (imgflag)
		XDrawImageString (screen->xdpy, xwin, xgc, x, y, str, len);
	else
		XDrawString (screen->xdpy, xwin, xgc, x, y, str, len);
}

#if 0
void xPen::PlaneMask (int x)
{
	XSetPlaneMask (screen->xdpy, xgc, (unsigned long) x);
}
#endif

void xPen::Dashes (char *dashlist, int offset)
{
	XSetDashes (screen->xdpy, xgc, offset, dashlist, strlen (dashlist));
}

void xPen::LineWidth (int x)
{
	XGCValues gcv;

	gcv.line_width = x;
	XChangeGC (screen->xdpy, xgc, GCLineWidth, &gcv);
}

void xPen::LineStyle (int x)
{
	XGCValues gcv;

	gcv.line_style = x;
	XChangeGC (screen->xdpy, xgc, GCLineStyle, &gcv);
}

void xPen::CapStyle (int x)
{
	XGCValues gcv;

	gcv.cap_style = x;
	XChangeGC (screen->xdpy, xgc, GCCapStyle, &gcv);
}

void xPen::JoinStyle (int x)
{
	XGCValues gcv;

	gcv.join_style = x;
	XChangeGC (screen->xdpy, xgc, GCJoinStyle, &gcv);
}
