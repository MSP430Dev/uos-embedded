class   xScreen;
class   xSensor;
class   xEvent;
class   xPen;
class   xCursor;

typedef unsigned long xPixel;

const int xColorMax = 255;              // maximum value of color component

class xColor {
public:
	unsigned char   red;
	unsigned char   green;
	unsigned char   blue;

	xColor (int r, int g, int b)    // create RGB color
		{ red = r; green = g; blue = b; }
	xColor (int gray)               // create gray color
		{ red = green = blue = gray; }
	xColor () {}                    // create undefined color

	xColor operator + (int);
	xColor operator + (xColor);
	xColor operator += (int);
	xColor operator += (xColor);
	xColor operator - (int);
	xColor operator - (xColor);
	xColor operator -= (int);
	xColor operator -= (xColor);
	xColor operator * (int);
	xColor operator * (double);
	xColor operator *= (int);
	xColor operator *= (double);
	xColor operator / (int);
	xColor operator / (double);
	xColor operator /= (int);
	xColor operator /= (double);
};

class xWindow {                         // window object
public:
	int             x;              // x coordinate relative to parent
	int             y;              // y coordinate relative to parent
	short           width;          // width of object
	short           height;         // height of object

	short           min_width;      // minimal width of object
	short           min_height;     // minimal height of object
	short           max_width;      // maximal width of object
	short           max_height;     // maximal height of object
	short           opt_width;      // optimal width of object
	short           opt_height;     // optimal height of object
	short           width_inc;      // width increment
	short           height_inc;     // height increment

	xScreen         *screen;        // screen of window
	xWindow         *parent;        // parent object

	xWindow (xWindow *, char *, char *); // create window with parent
	virtual ~xWindow ();		// destroy window

	void Listen (xSensor *);        // set input mask

	void Insert (xWindow *);        // add new child
	void Delete (xWindow *);        // remove child

	virtual void ComputeSize ();            // compute width, height
	virtual void Initialize ();             // create window
	virtual void Resize () {}               // window changed size
	virtual void Handle (xEvent &) {}       // window changed size
	virtual void Redraw (int, int, int, int) {}; // redraw part of window
	virtual void Redraw ()                  // redraw window
		{ Redraw (0, 0, width-1, height-1); }

	int QueryPointer (int *, int *);        // query relative pointer position

	void Move (int, int);                   // move window
	void Map ();                            // map window to screen
	void Map (int a, int b) { Move (a, b); Map (); }
	void Unmap ();                          // unmap window from screen

	void Background (xPixel);               // set window background

	void Clear ();                          // clear window
	void Clear (int, int, int, int, int=0); // clear area

	void Cursor (xCursor *);                // set window background
	void Cursor ();

	void AdjustWidth ();
	void AdjustHeight ();

	char *Attribute (char *, char * =0);    // get resource
	void Attribute (int &, int, char *, char * =0); // get boolean
	void Attribute (int &, int, int, int, char *, char * =0); // get integer
	void Attribute (xPixel &, xPixel, char *, char * =0); // get color
	void Attribute (xPixel &, char *, char *, char * =0);
	void Attribute (xColor &, xColor, char *, char * =0);
	void Attribute (xColor &, char *, char *, char * =0);

// readonly:
	long            xwin;           // Xlib Window id

protected:
	class SimpleList *childlist;    // childs of window
	class QuarkList *classq;        // class name of object
	QuarkList       *nameq;         // name of object

private:
	int             mapflag;        // 1 if window is/should be mapped

	xWindow () {};                  // create dummy window

	friend class xScreen;
	friend class xPen;
	friend class olInfo;
};

class   xFont;
struct  _XrmHashBucketRec;

class xScreen : public xWindow {        // root window of screen
public:
	int             depth;          // depth of screen
	xPixel          fgpixel;        // foreground (black) pixel
	xPixel          bgpixel;        // background (white) pixel
	xFont           *font;          // default font

	xScreen (char *, int *, char **); // open connection
	~xScreen ();                    // close connection

	virtual void Initialize ();     // create all windows
	int Read (xEvent&, int=0);      // read next event
	void Run ();                    // read and process events

	void WarpPointer (int, int);    // change position of pointer
	void Bell (int = 100);          // bell ring
	void Flush ();                  // flush output queue
	int Queue (int=2);              // number of events in input queue

	xPixel Pixel (char *);          // allocate color by name
	xPixel Pixel (int, int, int);   // allocate color by RGB value
	xPixel Pixel (int g)            // allocate gray color
		{ return (Pixel (g, g, g)); };
	xPixel Pixel (xColor c)         // convert color to pixel
		{ return (Pixel (c.red, c.green, c.blue)); }
	xColor Color (xPixel);          // convert pixel to color
	xColor Color (char *);          // convert name to color

// readonly:
	struct _XDisplay *xdpy;		// Xlib connection descriptor
	void		*xscr;		// Xlib screen structure
	long            xcmap;          // Xlib colormap
	void		*xvisual;       // Xlib visual
	_XrmHashBucketRec *database;    // resource database

private:
	class TargetTable *ttable;      // table of targets

	struct PixelTable *pixtable;    // table of pixels
	class NameTable *cnametable;    // table of color names
	class HashTable *cvaltable;     // pixel to index hash table
	int             nred;           // number of red shades
	int             ngreen;         // number of green shades
	int             nblue;          // number of blue shades
	int             ngray;          // number of gray shades

	friend class xWindow;
	friend class xPen;
	friend class xColor;
	friend class xCursor;
	friend class xBitmap;
	friend class xPixmap;
	friend class xImage;
	friend class xFont;
	friend class olInfo;

	void RegisterTarget (xWindow *);        // register window for input
	void UnregisterTarget (xWindow *);      // unregister window
	xWindow *FindTarget (long);             // find class pointer by id
	void CreateDatabase (char *);           // initialize resource database

	PixelTable *GetColorRGBIndex (int, int, int); // find index of RGB color
	void AllocateRGBColor (PixelTable *, int, int, int); // allocate pixel by RGB value
	int InitializeColor (char *);           // add new entry to pixel table
};
