class xBitmap;
class xPixmap;
class xImage;

struct _XGC;

enum xPenFunction {
	xPenFunctionClear,              /* 0 */
	xPenFunctionAnd,                /* src AND dst */
	xPenFunctionAndReverse,         /* src AND NOT dst */
	xPenFunctionCopy,               /* src */
	xPenFunctionAndInverted,        /* NOT src AND dst */
	xPenFunctionNoop,               /* dst */
	xPenFunctionXor,                /* src XOR dst */
	xPenFunctionOr,                 /* src OR dst */
	xPenFunctionNor,                /* NOT src AND NOT dst */
	xPenFunctionEquiv,              /* NOT src XOR dst */
	xPenFunctionInvert,             /* NOT dst */
	xPenFunctionOrReverse,          /* src OR NOT dst */
	xPenFunctionCopyInverted,       /* NOT src */
	xPenFunctionOrInverted,         /* NOT src OR dst */
	xPenFunctionNand,               /* NOT src OR NOT dst */
	xPenFunctionSet,                /* 1 */
};

class xBitmap {
public:
	short           width;
	short           height;

	xBitmap (xScreen *, char *, int * =0, int * =0); // create bitmap by name
	xBitmap (xScreen *, int, int, char *);  // create bitmap from data
	~xBitmap ();                            // free bitmap

private:
	xScreen         *screen;                // screen of bitmap
	long            xpixmap;                // Xlib pixmap value

	friend class xPen;
	friend class xCursor;
};

class xPixmap {
public:
	short           width;
	short           height;

	xPixmap (xScreen *, int, int);          // create empty pixmap
	~xPixmap ();                            // free bitmap

private:
	xScreen         *screen;                // screen of bitmap
	long            xpixmap;                // Xlib pixmap value

	friend class xPen;
};

class xPen {
public:
	xScreen         *screen;                // screen of GC

	xPen (xScreen *);                       // get default GC of screen
	xPen (xPen *);                          // create copy of GC
	~xPen ();                               // destroy GC

	void Foreground (xPixel);               // set foreground color
	void Background (xPixel);               // set background color

	void Function (xPenFunction);           // set drawing function
	void PlaneMask (int);                   // set plane mask

	void LineWidth (int);                   // set line width >= 0
	void LineStyle (int);                   // set line style 0..2
	void CapStyle (int);                    // set cap style 0..3
	void JoinStyle (int);                   // set join style 0..2
	void Dashes (char *, int=0);            // set dash pattern

	void Clip (int, int, int, int);
	void Clip ();

	void Tile (xPixmap *);
	void Tile (xBitmap *, int=0);
	void Tile (int, int);
	void Tile ();

	void Copy (xWindow *w1, xWindow *w2, int x2=0, int y2=0)
		{ Copy (w1->xwin, w2->xwin, 0, 0, w1->width, w1->height, x2, y2); }
	void Copy (xWindow *w1, xPixmap *w2, int x2=0, int y2=0)
		{ Copy (w1->xwin, w2->xpixmap, 0, 0, w1->width, w1->height, x2, y2); }
	void Copy (xPixmap *w1, xWindow *w2, int x2=0, int y2=0)
		{ Copy (w1->xpixmap, w2->xwin, 0, 0, w1->width, w1->height, x2, y2); }
	void Copy (xPixmap *w1, xPixmap *w2, int x2=0, int y2=0)
		{ Copy (w1->xpixmap, w2->xpixmap, 0, 0, w1->width, w1->height, x2, y2); }
	void Copy (xBitmap *w1, xWindow *w2, int x2=0, int y2=0)
		{ BitCopy (w1->xpixmap, w2->xwin, 0, 0, w1->width, w1->height, x2, y2); }
	void Copy (xBitmap *w1, xPixmap *w2, int x2=0, int y2=0)
		{ BitCopy (w1->xpixmap, w2->xpixmap, 0, 0, w1->width, w1->height, x2, y2); }

	void Copy (xWindow *w1, xWindow *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ Copy (w1->xwin, w2->xwin, x1, y1, w, h, x2, y2); }
	void Copy (xWindow *w1, xPixmap *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ Copy (w1->xwin, w2->xpixmap, x1, y1, w, h, x2, y2); }
	void Copy (xPixmap *w1, xWindow *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ Copy (w1->xpixmap, w2->xwin, x1, y1, w, h, x2, y2); }
	void Copy (xPixmap *w1, xPixmap *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ Copy (w1->xpixmap, w2->xpixmap, x1, y1, w, h, x2, y2); }
	void Copy (xBitmap *w1, xWindow *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ BitCopy (w1->xpixmap, w2->xwin, x1, y1, w, h, x2, y2); }
	void Copy (xBitmap *w1, xPixmap *w2, int x1, int y1, int w, int h, int x2, int y2)
		{ BitCopy (w1->xpixmap, w2->xpixmap, x1, y1, w, h, x2, y2); }

	void Point (xWindow *w, int x, int y)
		{ Point (w->xwin, x, y); }
	void Point (xPixmap *w, int x, int y)
		{ Point (w->xpixmap, x, y); }

	void Points (xWindow *w, int n, int *p)
		{ Points (w->xwin, n, p); }
	void Points (xPixmap *w, int n, int *p)
		{ Points (w->xpixmap, n, p); }

	void Line (xWindow *w, int x1, int y1, int x2, int y2)
		{ Line (w->xwin, x1, y1, x2, y2); }
	void Line (xPixmap *w, int x1, int y1, int x2, int y2)
		{ Line (w->xpixmap, x1, y1, x2, y2); }

	void Lines (xWindow *w, int n, int *p)
		{ Lines (w->xwin, n, p); }
	void Lines (xPixmap *w, int n, int *p)
		{ Lines (w->xpixmap, n, p); }

	void Polyline (xWindow *w, int n, int *p)
		{ Polyline (w->xwin, n, p); }
	void Polyline (xPixmap *w, int n, int *p)
		{ Polyline (w->xpixmap, n, p); }

	void Rectangle (xWindow *w, int x, int y, int wid, int hgh)
		{ Rectangle (w->xwin, x, y, wid, hgh); }
	void Rectangle (xPixmap *w, int x, int y, int wid, int hgh)
		{ Rectangle (w->xpixmap, x, y, wid, hgh); }

	void Polygon (xWindow *w, int n, int *p)
		{ Polygon (w->xwin, n, p); }
	void Polygon (xPixmap *w, int n, int *p)
		{ Polygon (w->xpixmap, n, p); }

	void Circle (xWindow *w, int x, int y, int diam)
		{ Arc (w->xwin, x, y, diam, diam, 0, 360*64); }
	void Circle (xPixmap *w, int x, int y, int diam)
		{ Arc (w->xpixmap, x, y, diam, diam, 0, 360*64); }

	void Circle (xWindow *w, int x, int y, int hdiam, int vdiam)
		{ Arc (w->xwin, x, y, hdiam, vdiam, 0, 360*64); }
	void Circle (xPixmap *w, int x, int y, int hdiam, int vdiam)
		{ Arc (w->xpixmap, x, y, hdiam, vdiam, 0, 360*64); }

	void Arc (xWindow *w, int x, int y, int diam, int a1, int a2)
		{ Arc (w->xwin, x, y, diam, diam, a1, a2); }
	void Arc (xPixmap *w, int x, int y, int diam, int a1, int a2)
		{ Arc (w->xpixmap, x, y, diam, diam, a1, a2); }

	void Arc (xWindow *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ Arc (w->xwin, x, y, wid, hgh, a1, a2); }
	void Arc (xPixmap *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ Arc (w->xpixmap, x, y, wid, hgh, a1, a2); }

	void Box (xWindow *w, int x, int y, int wid, int hgh)
		{ RectangleFill (w->xwin, x, y, wid, hgh); }
	void Box (xPixmap *w, int x, int y, int wid, int hgh)
		{ RectangleFill (w->xpixmap, x, y, wid, hgh); }

	void Circumference (xWindow *w, int x, int y, int diam)
		{ ArcFill (w->xwin, x, y, diam, diam, 0, 64*360); }
	void Circumference (xPixmap *w, int x, int y, int diam)
		{ ArcFill (w->xpixmap, x, y, diam, diam, 0, 64*360); }

	void Circumference (xWindow *w, int x, int y, int hdiam, int vdiam)
		{ ArcFill (w->xwin, x, y, hdiam, vdiam, 0, 64*360); }
	void Circumference (xPixmap *w, int x, int y, int hdiam, int vdiam)
		{ ArcFill (w->xpixmap, x, y, hdiam, vdiam, 0, 64*360); }

	void PieSlice (xWindow *w, int x, int y, int diam, int a1, int a2)
		{ ArcFill (w->xwin, x, y, diam, diam, a1, a2); }
	void PieSlice (xPixmap *w, int x, int y, int diam, int a1, int a2)
		{ ArcFill (w->xpixmap, x, y, diam, diam, a1, a2); }

	void PieSlice (xWindow *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ ArcFill (w->xwin, x, y, wid, hgh, a1, a2); }
	void PieSlice (xPixmap *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ ArcFill (w->xpixmap, x, y, wid, hgh, a1, a2); }

	void Segment (xWindow *w, int x, int y, int diam, int a1, int a2)
		{ ArcFill (w->xwin, x, y, diam, diam, a1, a2, 1); }
	void Segment (xPixmap *w, int x, int y, int diam, int a1, int a2)
		{ ArcFill (w->xpixmap, x, y, diam, diam, a1, a2, 1); }

	void Segment (xWindow *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ ArcFill (w->xwin, x, y, wid, hgh, a1, a2, 1); }
	void Segment (xPixmap *w, int x, int y, int wid, int hgh, int a1, int a2)
		{ ArcFill (w->xpixmap, x, y, wid, hgh, a1, a2, 1); }

	void FillPolygon (xWindow *w, int n, int *p, int wflag=0)
		{ PolygonFill (w->xwin, n, p, wflag); }
	void FillPolygon (xPixmap *w, int n, int *p, int wflag=0)
		{ PolygonFill (w->xpixmap, n, p, wflag); }

	void Text (xWindow *w, xFont *f, int x, int y, int c, int im=0)
		{ char b=c; Text (w->xwin, f, x, y, &b, 1, im); }
	void Text (xPixmap *w, xFont *f, int x, int y, int c, int im=0)
		{ char b=c; Text (w->xpixmap, f, x, y, &b, 1, im); }

	void Text (xWindow *w, xFont *f, int x, int y, char *s, int im=0)
		{ Text (w->xwin, f, x, y, s, 0, im); }
	void Text (xPixmap *w, xFont *f, int x, int y, char *s, int im=0)
		{ Text (w->xpixmap, f, x, y, s, 0, im); }

private:
	_XGC            *xgc;                           // Xlib GC id

	void Copy (long, long, int, int, int, int, int, int);
	void BitCopy (long, long, int, int, int, int, int, int);

	void Point (long, int, int);
	void Points (long, int, int *);
	void Line (long, int, int, int, int);
	void Lines (long, int, int *);
	void Polyline (long, int, int *);
	void Rectangle (long, int, int, int, int);
	void Arc (long, int, int, int, int, int, int);
	void Polygon (long, int, int *);

	void RectangleFill (long, int, int, int, int);
	void ArcFill (long, int, int, int, int, int, int, int=0);
	void PolygonFill (long, int, int *, int wflag=0);

	void Text (long, xFont *, int, int, char *, int, int);
};
