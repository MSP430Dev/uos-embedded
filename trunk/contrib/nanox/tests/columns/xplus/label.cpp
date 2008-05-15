# include "window.h"
# include "pen.h"
# include "label.h"
# include "extern.h"

xLabel::xLabel (xWindow *r, char *nam, char *lab) : (r, "Label", nam)
{
	label = strdup (lab);
	pen = new xPen (screen);
}

void xLabel::ComputeSize ()
{
	min_width = pen->TextWidth (label);
	min_height = pen->TextHeight ();
	opt_width = min_width + min_height;
	opt_height = min_height * 3 / 2;

	width = opt_width;
	height = opt_height;
}

void xLabel::Redraw (int, int, int, int) // redraw part of window
{
	pen->DrawText (this, (width - min_width) / 2,
		pen->Ascent () + (height - min_height) / 2, label);
}
