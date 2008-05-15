# include "window.h"
# include "event.h"
# include "sensor.h"
# include "pen.h"
# include "image.h"
# include "extern.h"

class Show : public xWindow {
public:
	xSensor *input;
	xPen    *pen;
	xImage  *image;

	Show (xWindow *, char *, char *);
	~Show ();

	virtual void Redraw (int, int, int, int);
	virtual void ComputeSize ();
	virtual void Initialize ();
	virtual void Handle (xEvent &);
};

main (int argc, char **argv)
{
	xScreen *screen = new xScreen ("Foo", &argc, argv);

	if (! argv [1])
		xFatal ("Usage: show filename\n");

	new Show (screen, "show", argv [1]);

	screen->Run ();
	return (0);
}

Show::Show (xWindow *r, char *nam, char *filenam) : (r, "Show", nam)
{
	input = 0;
	pen = 0;
	image = new xImage (screen, filenam);
}

Show::~Show ()
{
	delete image;
	if (input)
		delete input;
	if (pen)
		delete pen;
}

void Show::ComputeSize ()
{
	width = image->width;
	height = image->height;
}

void Show::Initialize ()
{
	xWindow::Initialize ();

	pen = new xPen (screen);
	input = new xSensor ();

	input->Catch (xEventKey);

	Listen (input);
}

void Show::Redraw (int x, int y, int w, int h)
{
	if (w > width-x)
		w = width - x;
	if (h > height-y)
		h = height - y;
	pen->PutImage (this, image, x, y, w, h, x, y);
}

void Show::Handle (xEvent &ev)
{
	switch (ev.type) {
	case xEventKey:
		if (ev.button == 'q')
			ev.Quit ();
		break;
	}
}
