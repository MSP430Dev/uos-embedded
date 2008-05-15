# include <stdio.h>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include "window.h"
# include "image.h"

extern "C" {
# include <X11/ppm.h>
};

static int contains (char *str, int symb)
{
	for (; *str; ++str)
		if (*str == symb)
			return (1);
	return (0);
}

inline xPixel xImage::GetPixel (int x, int y)
{
	return (XGetPixel (ximage, x, y));
}

inline void xImage::PutPixel (int x, int y, xPixel pix)
{
	XPutPixel (ximage, x, y, pix);
}

xImage::xImage (xScreen *scr, char *name)
{
	char buf [100];
	int wid, hgh, format;
	pixval maxval;

	// Build file name with pixmap.

	int pipeflag = 0;

	// Remove suffix .Z from file name.
	char *p = name;
	while (*p) ++p;
	if (*--p == 'Z' && *--p == '.')
		*p = 0;

	if (access (name, 4) == 0)
		strcpy (buf, name);
	else {
		strcpy (buf, name);
		strcat (buf, ".Z");
		if (access (buf, 4) == 0)
			pipeflag = 1;
		else if (! contains (name, '/')) {
			strcpy (buf, "/usr/include/X11/pixmaps/");
			strcat (buf, name);
			if (access (buf, 4) != 0) {
				strcat (buf, ".Z");
				if (access (buf, 4) == 0)
					pipeflag = 1;
			}
		}
	}

	// Open file, read PPM header, create array for row of pixels

	FILE *fd;
	if (pipeflag) {
		char cmd [80];
		strcpy (cmd, "/usr/bin/X11/uncompress -c ");
		strcat (cmd, buf);
		fd = popen (cmd, "r");
	} else
		fd = fopen (buf, "r");
	if (! fd)
		xFatal ("Cannot open image file\n");
	ppm_readppminit (fd, &wid, &hgh, &maxval, &format);
	pixel *pixels = new pixel [wid];

	// Create Xlib image structure.

	screen = scr;
	width = wid;
	height = hgh;
	ximage = XCreateImage (screen->xdpy, screen->xvisual, screen->depth,
		ZPixmap, 0, (char *) 0, width, height, 8, 0);
	if (! ximage)
		xFatal ("Cannot allocate XImage\n");
	ximage->data = new char [ximage->bytes_per_line * height];
	if (! ximage->data)
		xFatal ("Cannot allocate image data\n");

	// Read in pixels.

	for (int y=0; y<height; ++y) {
		ppm_readppmrow (fd, pixels, wid, maxval, format);
		pixel *p = pixels;
		for (int x=0; x<width; ++x, ++p) {
			int r = PPM_GETR (*p) * xColorMax / maxval;
			int g = PPM_GETG (*p) * xColorMax / maxval;
			int b = PPM_GETB (*p) * xColorMax / maxval;
			xPixel xpix = screen->Pixel (r, g, b);
			PutPixel (x, y, xpix);
		}
	}
	delete pixels;
	if (pipeflag)
		pclose (fd);
	else
		fclose (fd);
}

xImage::~xImage ()
{
	if (ximage->data) {
		delete ximage->data;
		ximage->data = 0;
	}
	XDestroyImage (ximage);
}
