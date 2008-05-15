class xScreen;

class xFont {
public:
	xScreen         *screen;
	int             width;                  // maximum width of character
	int             height;                 // font ascent
	int             depth;                  // font descent

	xFont (xScreen *scr, char *name);       // Load named font
	~xFont ();

	int Width (char *str, int len = 0);     // compute width of string
	int Width (int sym);                    // compute width of character

// readonly:
	XFontStruct     *info;
};
