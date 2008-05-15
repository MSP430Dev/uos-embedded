#include "nxlib.h"
#include <X11/Xutil.h>

XWMHints *
XAllocWMHints()
{
	XWMHints *wmhints;

	wmhints = (XWMHints*) calloc (1, sizeof (XWMHints));
	return wmhints;
}
