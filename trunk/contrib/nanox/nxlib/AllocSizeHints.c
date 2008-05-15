#include "nxlib.h"
#include <X11/Xutil.h>

XSizeHints *
XAllocSizeHints()
{
	XSizeHints *sizehints;

	sizehints = (XSizeHints*) calloc (1, sizeof (XSizeHints));
	return sizehints;
}
