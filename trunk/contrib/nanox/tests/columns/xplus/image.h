class xImage {
public:
	short           width;
	short           height;

	xImage (xScreen *, char *);             // create image by file name
	xImage (xScreen *, char **);            // create image from data
	~xImage ();                             // free image

	xPixel GetPixel (int, int);
	void PutPixel (int, int, xPixel);

private:
	xScreen         *screen;                // screen of image
	XImage          *ximage;                // Xlib image structure

	friend class xPen;
};
