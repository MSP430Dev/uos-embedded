# include "window.h"
# include "panel.h"
# include "slist.h"

xPanel::xPanel (xWindow *pnt, char *nam, int vertflag = 0, int bd = 2, int g = 2)
	: (pnt, "Panel", nam)
{
	vertical = vertflag != 0;
	border = bd;
	gap = g;
}

void xPanel::ComputeSize ()
{
	xWindow *w;

	nchilds = 0;
	for (w=childlist->First(); w; w=childlist->Next()) {
		w->ComputeSize ();
		++nchilds;
	}

	if (! childlist->Empty ()) {
		min_width = min_height = 0;
		opt_width = opt_height = 0;

		for (w=childlist->First(); w; w=childlist->Next()) {
			if (vertical) {
				w->x = border;
				w->y = border + opt_height;

				if (w->min_width > min_width)
					min_width = w->min_width;
				if (w->opt_width > opt_width)
					opt_width = w->opt_width;

				min_height += w->min_height + gap;
				opt_height += w->opt_height + gap;
			} else {
				w->x = border + opt_width;
				w->y = border;

				if (w->min_height > min_height)
					min_height = w->min_height;
				if (w->opt_height > opt_height)
					opt_height = w->opt_height;

				min_width += w->min_width + gap;
				opt_width += w->opt_width + gap;
			}
		}
	}

	// align size of childs

	for (w=childlist->First(); w; w=childlist->Next())
		if (vertical) {
			w->width = opt_width < w->max_width ?
				opt_width : w->max_width;
			w->width = w->min_width + (w->width - w->min_width) /
				w->width_inc * w->width_inc;
		} else {
			w->height = opt_height < w->max_height ?
				opt_height : w->max_height;
			w->height = w->min_height + (w->height - w->min_height) /
				w->height_inc * w->height_inc;
		}
	if (vertical) {
		min_width += border + border;
		opt_width += border + border;
		min_height += border + border - gap;
		opt_height += border + border - gap;
	} else {
		min_height += border + border;
		opt_height += border + border;
		min_width += border + border - gap;
		opt_width += border + border - gap;
	}
	width = opt_width;
	height = opt_height;
	max_width = max_height = 10000;
	width_inc = height_inc = 1;
}

void xPanel::Resize ()
{
	xWindow *w;

	for (w=childlist->First(); w; w=childlist->Next()) {

	}
}
