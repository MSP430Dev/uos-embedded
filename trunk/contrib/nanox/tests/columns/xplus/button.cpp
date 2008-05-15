# include "window.h"
# include "pen.h"
# include "button.h"
# include "sensor.h"
# include "event.h"
# include "extern.h"

xButton::xButton (xWindow *r, char *nam) : (r, "Button", nam)
{
	pen = new xPen (screen);

	background = screen->Pixel (xMaxColor * 4 / 5);
	light = screen->Pixel (xMaxColor);
	shadow = screen->Pixel (xMaxColor * 2 / 5);

	active = 0;
	callback = 0;
}

void xButton::SetCallback (void *cb)
{
	callback = cb;
}

void xButton::Initialize ()
{
	xWindow::Initialize ();

	SetBackground (background);

	input = new xSensor ();

	input->Catch (xEventDown);
	input->Catch (xEventUp);

	Listen (input);
}

void xButton::ComputeSize ()
{
	min_width = 16;
	min_height = 16;
	max_width = 32;
	max_height = 32;
	opt_width = 24;
	opt_height = 24;

	width = opt_width;
	height = opt_height;
}

void xButton::Redraw (int, int, int, int)
{
	pen->SetForeground (background);
	pen->FillRectangle (this, 3, 3, width-4, height-4);

	pen->SetForeground (active ? shadow : light);
	pen->FillPolygon (this, 6, 0, 0, width-1, 0, width-4, 3,
	  3, 3, 3, height-4, 0, height-1);

	pen->SetForeground (active ? light : shadow);
	pen->FillPolygon (this, 6, width-1, height-1, 0, height-1, 3, height-4,
		width-4, height-4, width-4, 3, width-1, 0);
}

void xButton::Handle (xEvent &ev)
{
	switch (ev.type) {
	case xEventUp:
		if (active) {
			if (callback)
				(*callback) ();
			active = 0;
			Redraw ();
		}
		break;
	case xEventDown:
		if (! active) {
			active = 1;
			Redraw ();
		}
		break;
	}
}
