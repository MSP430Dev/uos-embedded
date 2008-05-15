# include "window.h"
# include "pen.h"
# include "event.h"
# include "sensor.h"
# include "extern.h"
# include <random/rand15.h>

class Glass : public xWindow {
public:
	Glass (xWindow *, char *);
	~Glass ();

	virtual void Redraw (int, int, int, int);
	virtual void Resize ();
	virtual void ComputeSize ();
	virtual void Initialize ();
	virtual void Handle (xEvent &);

	void Run ();

private:
	int             glass_width;            // width of glass in squares
	int             glass_height;           // height of glass in squares
	int             square_width;           // width of square in pixels
	int             square_height;          // height of square in pixels
	int             bar_length;             // length of bar
	int             squeeze_length;         // length of squeezed chains
	int             num_colors;             // number of different colors

	int             idleflag;               // game is paused
	int             mouseflag;              // mouse is in window

	xPen            *pen;                   // pen for drawing bars
	xPixmap         **pattern;              // patterns for drawing bars
	xPixmap         *squeezepattern;        // squeezed rectangle
	xPixel          *fgpixel;               // pixels for bars
	xPixel          *lightpixel;            // light pixels
	xPixel          *shadowpixel;           // shadow pixels
	xPixel          borderpixel;            // color of bar border
	xPixel          squeezepixel;           // color of bar border
	xPixel          bgpixel;                // color of glass background

	int             **glass;                // current contents of glass
	int             *bar;                   // current bar
	int             barx, bary;             // position of bottom of bar

	int             timeout;                // pause in milliseconds
	int             beepflag;               // enable beep

	void Left ();                           // move bar to the left
	void Right ();                          // move bar to the right
	void Rotate ();                         // rotate bar colors
	int Drop ();                            // drop bar
	int Step ();                            // move bar down
	void Reset ();                          // reset game
	void BuildPatterns ();                  // make pixmaps for drawing bars
	void GenerateBar ();                    // generate new bar
	void ClearBar ();                       // clear bar image
	void DrawGlass ();                      // draw glass with squares
	void DrawBar ();                        // draw bar
	void Squeeze (int, int);                // remove some squares
	void ExpandVert (int, int, int &, int &);
	void ExpandHor (int, int, int &, int &);
	void ExpandDiag1 (int, int, int &, int &);
	void ExpandDiag2 (int, int, int &, int &);
};

struct ListEntry {
	int             x;
	int             y;
	ListEntry       *next;
};

class List {
public:
		List ()         { list = ptr = 0; }
		~List ()        { Clear (); }

	void    Clear ();
	void    Rewind ()       { ptr = list; }
	int     Empty ()        { return (list == 0); }
	void    Put (int, int);
	int     Get (int &, int &);

private:
	ListEntry       *list;
	ListEntry       *ptr;
};

char *colornames [8] = {
	"red", "green", "blue", "cyan", "magenta", "yellow", "violet", "white",
};

int nxmain (void *arg)
{
	int argc = 1;
	char *argv[] = { "a.out", 0 };
	xScreen *screen = new xScreen ("XColumns", &argc, argv);

	if (argv [1])
		xFatal ("Usage: xcolumns\n");
// 	srand15 (time (0));

	Glass *glass = new Glass (screen, "glass");

	screen->Initialize ();
	glass->Run ();
	return (0);
}

Glass::Glass (xWindow *r, char *nam) : xWindow (r, "Glass", nam)
{
	mouseflag = 0;          // mouse is out of window
	idleflag = 1;           // game starts paused
	beepflag = 0;           // no beep by default

	pen = new xPen (screen);

	Attribute (glass_width, 6, 2, 32, "glassWidth");
	Attribute (glass_height, 18, 5, 64, "glassHeight");
	Attribute (square_width, 20, 8, 128, "squareWidth");
	Attribute (square_height, 20, 8, 128, "squareHeight");
	Attribute (bar_length, 3, 1, 5, "barLength");
	Attribute (squeeze_length, 3, 1, 5, "squeezeLength");
	Attribute (num_colors, 6, 1, 8, "colors");
	Attribute (timeout, 6, 1, 50, "timeout");
	square_width = square_width / 4 * 4;
	square_height = square_height / 4 * 4;

	Attribute (bgpixel, "dim gray", "background");
	Attribute (borderpixel, "black", "borderColor");
	Attribute (squeezepixel, "white", "squeezeColor");

	fgpixel = new xPixel [num_colors];
	lightpixel = new xPixel [num_colors];
	shadowpixel = new xPixel [num_colors];

	int i;
	for (i=0; i<num_colors; ++i) {

		// read color name attribute

		char cname [80];
		snprintf (cname, sizeof(cname), "color%d", i+1);
		char *p = Attribute (cname);
		if (! p)
			p = colornames [i];

		// create pixel for color

		lightpixel [i] = screen->Pixel (p);
		xColor lightcolor = screen->Color (lightpixel [i]);

		// create foreground and shadow pixels

		fgpixel [i] = screen->Pixel (lightcolor * 0.8);
		shadowpixel [i] = screen->Pixel (lightcolor / 2);
	}
	pattern = 0;
	BuildPatterns ();

	glass = new int * [glass_width];
	for (i=0; i<glass_width; ++i)
		glass[i] = new int [glass_height];
	bar = new int [bar_length];
	Reset ();
}

Glass::~Glass ()
{
	int i;

	for (i=0; i<glass_width; ++i)
		delete glass [i];
	delete glass;
	delete bar;
	for (i=0; i<num_colors; ++i)
		delete pattern [i];
	delete pattern;
	delete squeezepattern;
	delete fgpixel;
	delete lightpixel;
	delete shadowpixel;
	delete pen;
}

void Glass::ComputeSize ()
{
	width = square_width * glass_width;
	height = square_height * glass_height;

	min_width = 8 * glass_width;
	min_height = 8 * glass_height;
	max_width = 128 * glass_width;
	max_height = 128 * glass_height;

	width_inc = glass_width * 4;
	height_inc = glass_height * 4;
}

void Glass::Resize ()
{
	square_width = width / glass_width;
	square_height = width / glass_height;
	BuildPatterns ();
}

void Glass::Initialize ()
{
	xWindow::Initialize ();
	Background (bgpixel);

	xSensor *input = new xSensor ();
	input->Catch (xEventEnter);
	input->Catch (xEventLeave);
	input->Catch (xEventDown);
	input->Catch (xEventKey);
	Listen (input);
	delete input;
}

void Glass::Handle (xEvent &ev)
{
	switch (ev.type) {
	case xEventEnter:
		mouseflag = 1;
		break;
	case xEventLeave:
		mouseflag = 0;
		break;
	case xEventKey:
		switch (ev.button) {
		case 'q': case 'Q':
			ev.Quit ();
			return;
		}
		if (idleflag) {
			idleflag = 0;
			break;
		}
		if (! mouseflag)
			break;
		switch (ev.button) {
		case ',': case '4': case 'j': case 's': case 'J': case 'S':
			Left ();
			break;
		case '/': case '6': case 'l': case 'f': case 'L': case 'F':
			Right ();
			break;
		case '.': case '5': case 'k': case 'd': case 'K': case 'D':
			Rotate ();
			break;
		case ' ': case '2':
			if (! Drop ())
				ev.Quit ();
			break;
		case 'r': case 'R':
			Reset ();
			break;
		case 'b': case 'B':
			beepflag = ! beepflag;
			break;
		case 'p': case 'P':
			idleflag = 1;
			break;
		case '+': case '=':
			if (--timeout < 1)
				timeout = 1;
			break;
		case '-': case '_':
			++timeout;
			break;
		}
		break;
	case xEventDown:
		if (idleflag) {
			idleflag = 0;
			break;
		}
		if (! mouseflag)
			break;
		switch (ev.button) {
		case xButtonLeft:
			Left ();
			break;
		case xButtonRight:
			Right ();
			break;
		case xButtonMiddle:
			Rotate ();
			break;
		}
		break;
	default:
		break;
	}
}

void Glass::BuildPatterns ()
{
	int i;

	// delete old pixmaps

	if (pattern) {
		for (i=0; i<num_colors; ++i)
			delete pattern [i];
		delete squeezepattern;
	} else
		pattern = new xPixmap * [num_colors];

	// build new ones

	for (i=0; i<num_colors; ++i) {
		pattern[i] = new xPixmap (screen, square_width, square_height);
//              pen->Foreground (borderpixel);
//              pen->Rectangle (pattern [i], 0, 0, square_width-1,
//                      square_height-1);
		pen->Foreground (fgpixel[i]);
		pen->Box (pattern [i], 2, 2, square_width-2,
			square_height-2);
		pen->Foreground (lightpixel[i]);
		pen->Line (pattern [i], 0, 0, square_width-1, 0);
		pen->Line (pattern [i], 1, 1, square_width-2, 1);
		pen->Line (pattern [i], 0, 0, 0, square_height-1);
		pen->Line (pattern [i], 1, 1, 1, square_height-2);
		pen->Foreground (shadowpixel [i]);
		pen->Line (pattern [i], square_width-1, square_height-1,
			0, square_height-1);
		pen->Line (pattern [i], square_width-2, square_height-2,
			1, square_height-2);
		pen->Line (pattern [i], square_width-1, square_height-1,
			square_width-1, 0);
		pen->Line (pattern [i], square_width-2, square_height-2,
			square_width-2, 1);
	}
	squeezepattern = new xPixmap (screen, square_width, square_height);
	xBitmap *bm = new xBitmap (screen, 4, 4, "\3\11\14\6");
	pen->Foreground (squeezepixel);
	pen->Background (borderpixel);
	pen->Tile (bm);
	pen->Box (squeezepattern, 0, 0, square_width, square_height);
	pen->Tile ();
	delete bm;
}

void Glass::GenerateBar ()
{
	barx = glass_width/2;
	bary = 0;
	for (int i=0; i<bar_length; ++i)
		bar[i] = rand15 () % num_colors;
}

void Glass::Reset ()
{
	for (int x=0; x<glass_width; ++x)
		for (int y=0; y<glass_height; ++y)
			glass[x][y] = -1;
	GenerateBar ();
}

void Glass::Redraw (int, int, int, int)
{
	DrawGlass ();
	DrawBar ();
}

void Glass::DrawGlass ()
{
	for (int x=0; x<glass_width; ++x)
		for (int y=0; y<glass_height; ++y)
			if (glass[x][y] >= 0)
				pen->Copy (pattern[glass[x][y]], this,
					x * square_width,
					y * square_height);
}

void Glass::DrawBar ()
{
	for (int i=0; i<bar_length; ++i)
		if (bary - i >= 0)
			pen->Copy (pattern[bar[i]], this,
				barx * square_width,
				(bary - i) * square_height);
}

void Glass::ClearBar ()
{
	for (int i=0; i<bar_length; ++i)
		if (bary - i >= 0)
			Clear (barx * square_width,
				(bary - i) * square_height,
				square_width, square_height);
}

void Glass::Run ()
{
	xEvent event;
	int timecnt = 0;

	for (;;) {
		if (timecnt >= timeout) {
			if (! Step ())
				break;
			timecnt = 0;
		}
		if (! screen->Read (event, 1)) {
			usleep (100000L);
			if (! idleflag && mouseflag)
				++timecnt;
			continue;
		}
		xWindow *w = event.window;
		w->Handle (event);
		if (! event.window)
			break;
	}
	screen->Flush ();
}

void Glass::Left ()
{
	if (barx <= 0 || glass[barx-1][bary] >= 0)
		return;
	ClearBar ();
	--barx;
	DrawBar ();
}

void Glass::Right ()
{
	if (barx >= glass_width-1 || glass[barx+1][bary] >= 0)
		return;
	ClearBar ();
	++barx;
	DrawBar ();
}

void Glass::Rotate ()
{
	ClearBar ();
	int c = bar [0];
	for (int i=0; i<bar_length-1; ++i)
		bar[i] = bar[i+1];
	bar[bar_length-1] = c;
	DrawBar ();
}

int Glass::Drop ()
{
	ClearBar ();
	while (bary < glass_height-1 && glass[barx][bary+1] < 0)
		++bary;
	DrawBar ();
	if (bary - bar_length < 0)
		return (0);
	for (int i=0; i<bar_length; ++i)
		glass[barx][bary-i] = bar[i];
	Squeeze (barx, bary);
	GenerateBar ();
	DrawBar ();
	return (1);
}

int Glass::Step ()
{
	if (bary < glass_height-1 && glass[barx][bary+1] < 0) {
		ClearBar ();
		++bary;
		DrawBar ();
	}
	if (bary >= glass_height-1 || glass[barx][bary+1] >= 0) {
		if (bary - bar_length < 0)
			return (0);
		for (int i=0; i<bar_length; ++i)
			glass[barx][bary-i] = bar[i];
		Squeeze (barx, bary);
		GenerateBar ();
		DrawBar ();
	}
	return (1);
}

void Glass::Squeeze (int x, int y)
{
	List movelist, squeezelist;

	// create list of moved squares
	for (int i=0; i<bar_length; ++i)
		movelist.Put (x, y-i);

	for (;;) {
		// compute squeeze squares
		movelist.Rewind ();
		while (movelist.Get (x, y)) {
			int i, d1, d2;

			// try to expand vertically
			ExpandVert (x, y, d1, d2);
			if (d2 + d1 >= squeeze_length-1)
				for (i= -d1; i<=d2; ++i)
					squeezelist.Put (x, y+i);

			// try to expand horizontally
			ExpandHor (x, y, d1, d2);
			if (d2 + d1 >= squeeze_length-1)
				for (i= -d1; i<=d2; ++i)
					squeezelist.Put (x+i, y);

			// try to expand diagonally
			ExpandDiag1 (x, y, d1, d2);
			if (d2 + d1 >= squeeze_length-1)
				for (i= -d1; i<=d2; ++i)
					squeezelist.Put (x+i, y+i);
			ExpandDiag2 (x, y, d1, d2);
			if (d2 + d1 >= squeeze_length-1)
				for (i= -d1; i<=d2; ++i)
					squeezelist.Put (x-i, y+i);
		}
		movelist.Clear ();

		// if nothing to squeeze, just return
		if (squeezelist.Empty ())
			break;

		// clear squeezed squares
		squeezelist.Rewind ();
		for (;;) {
			if (! squeezelist.Get (x, y))
				break;
			glass[x][y] = -1;
			pen->Copy (squeezepattern, this,
				x * square_width,
				y * square_height);
		}
		squeezelist.Clear ();
		if (beepflag)
			screen->Bell ();
		screen->Flush ();
		usleep (400000L);

		// remove squeezed squares and compute
		// new move list
		for (x=0; x<glass_width; ++x) {
			// squeeze column

			// find first hole
			for (y=glass_height-1; y>=0 && glass[x][y]>=0; --y);
			if (y < 0)
				continue;
			int hole = y;

			// move all above the hole
			for (;;) {
				// find square above the hole
				for (; y>=0 && glass[x][y]<0; --y);
				if (y < 0)
					break;
				for (; y>=0 && glass[x][y]>=0; --y) {
					glass[x][hole] = glass [x][y];
					glass[x][y] = -1;
					movelist.Put (x, hole);
					--hole;
				}
			}

			for (y=0; y<glass_height && glass[x][y]<0; ++y);
			Clear (x * square_width, 0,
				square_width, y * square_height);
		}
		DrawGlass ();
	}
}

void Glass::ExpandHor (int x, int y, int &d1, int &d2)
{
	for (d1=1; x-d1>=0 && glass[x-d1][y] == glass[x][y]; ++d1);
	for (d2=1; x+d2<=glass_width-1 && glass[x+d2][y] == glass[x][y]; ++d2);
	--d1; --d2;
}

void Glass::ExpandVert (int x, int y, int &d1, int &d2)
{
	for (d1=1; y-d1>=0 && glass[x][y-d1] == glass[x][y]; ++d1);
	for (d2=1; y+d2<=glass_height-1 && glass[x][y+d2] == glass[x][y]; ++d2);
	--d1; --d2;
}

void Glass::ExpandDiag1 (int x, int y, int &d1, int &d2)
{
	for (d1=1; x-d1>=0 && y-d1>=0 &&
		glass[x-d1][y-d1] == glass[x][y]; ++d1);
	for (d2=1; x+d2<=glass_width-1 && y+d2<=glass_height-1 &&
		glass[x+d2][y+d2] == glass[x][y]; ++d2);
	--d1; --d2;
}

void Glass::ExpandDiag2 (int x, int y, int &d1, int &d2)
{
	for (d1=1; x+d1<=glass_width-1 && y-d1>=0 &&
		glass[x+d1][y-d1] == glass[x][y]; ++d1);
	for (d2=1; x-d2>=0 && y+d2<=glass_height-1 &&
		glass[x-d2][y+d2] == glass[x][y]; ++d2);
	--d1; --d2;
}

void List::Clear ()
{
	for (ptr=list; ptr; ptr=ptr->next)
		delete ptr;
	list = ptr = 0;
}

void List::Put (int x, int y)
{
	ListEntry *p = new ListEntry;
	p->x = x;
	p->y = y;
	p->next = list;
	list = p;
}

int List::Get (int &x, int &y)
{
	if (! ptr)
		return (0);
	x = ptr->x;
	y = ptr->y;
	ptr = ptr->next;
	return (1);
}
