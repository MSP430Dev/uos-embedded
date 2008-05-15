# include "window.h"
# include "event.h"
# include "sensor.h"
# include "pen.h"
# include "extern.h"

class GraphEditor : public xWindow {
public:
	xSensor *input;
	xPen *pen;

	GraphEditor (xScreen *);
	~GraphEditor ();

	virtual void ComputeSize ();
	virtual void Initialize ();
	virtual void Handle (xEvent &);

private:
	int lastx;
	int lasty;
};

main (int argc, char **argv)
{
	xScreen *screen = new xScreen ("Foo", &argc, argv);

	new GraphEditor (screen);

	screen->Run ();
	return (0);
}

GraphEditor::GraphEditor (xScreen *r, char *nam) : (r, "Gedit", nam)
{
	name = strdup ("gedit");
	lastx = lasty = -1;
	input = 0;
	pen = 0;
}

GraphEditor::~GraphEditor ()
{
	if (input)
		delete input;
	if (pen)
		delete pen;
}

void GraphEditor::ComputeSize ()
{
	width = 100;
	height = 100;
}

void GraphEditor::Initialize ()
{
	xWindow::Initialize ();

	pen = new xPen (screen);
	input = new xSensor ();

	input->Catch (xEventDown);
	input->Catch (xEventKey);

	Listen (input);
}

void GraphEditor::Handle (xEvent &ev)
{
	switch (ev.type) {
	case xEventKey:
		if (ev.button == 'q')
			ev.Quit ();
		break;
	case xEventDown:
		switch (ev.button) {
		case xButtonLeft:
			if (lastx >= 0)
				pen->DrawLine (this, lastx, lasty, ev.x, ev.y);
			lastx = ev.x;
			lasty = ev.y;
			break;
		case xButtonMiddle:
			if (lastx >= 0)
				pen->DrawRectangle (this, lastx, lasty,
					ev.x, ev.y);
			lastx = ev.x;
			lasty = ev.y;
			break;
		case xButtonRight:
			pen->Clear (this);
			lastx = lasty = -1;
			break;
		}
		break;
	}
}
