enum xCursorShape {
	xCursorXCursor                = 0,
	xCursorArrow                  = 2,
	xCursorBasedArrowDown         = 4,
	xCursorBasedArrowUp           = 6,
	xCursorBoat                   = 8,
	xCursorBogosity               = 10,
	xCursorBottomLeftCorner       = 12,
	xCursorBottomRightCorner      = 14,
	xCursorBottomSide             = 16,
	xCursorBottomTee              = 18,
	xCursorBoxSpiral              = 20,
	xCursorCenterPtr              = 22,
	xCursorCircle                 = 24,
	xCursorClock                  = 26,
	xCursorCoffeeMug              = 28,
	xCursorCross                  = 30,
	xCursorCrossReverse           = 32,
	xCursorCrosshair              = 34,
	xCursorDiamondCross           = 36,
	xCursorDot                    = 38,
	xCursorDotbox                 = 40,
	xCursorDoubleArrow            = 42,
	xCursorDraftLarge             = 44,
	xCursorDraftSmall             = 46,
	xCursorDrapedBox              = 48,
	xCursorExchange               = 50,
	xCursorFleur                  = 52,
	xCursorGobbler                = 54,
	xCursorGumby                  = 56,
	xCursorHand1                  = 58,
	xCursorHand2                  = 60,
	xCursorHeart                  = 62,
	xCursorIcon                   = 64,
	xCursorIronCross              = 66,
	xCursorLeftPtr                = 68,
	xCursorLeftSide               = 70,
	xCursorLeftTee                = 72,
	xCursorLeftButton             = 74,
	xCursorLowerLeftAngle         = 76,
	xCursorLowerRightAngle        = 78,
	xCursorMan                    = 80,
	xCursorMiddlebutton           = 82,
	xCursorMouse                  = 84,
	xCursorPencil                 = 86,
	xCursorPirate                 = 88,
	xCursorPlus                   = 90,
	xCursorQuestionArrow          = 92,
	xCursorRightPtr               = 94,
	xCursorRightSide              = 96,
	xCursorRightTee               = 98,
	xCursorRightButton            = 100,
	xCursorRtlLogo                = 102,
	xCursorSailboat               = 104,
	xCursorSbDownArrow            = 106,
	xCursorSbHorDoubleArrow       = 108,
	xCursorSbLeftArrow            = 110,
	xCursorSbRightArrow           = 112,
	xCursorSbUpArrow              = 114,
	xCursorSbVertDoubleArrow      = 116,
	xCursorShuttle                = 118,
	xCursorSizing                 = 120,
	xCursorSpider                 = 122,
	xCursorSpraycan               = 124,
	xCursorStar                   = 126,
	xCursorTarget                 = 128,
	xCursorTcross                 = 130,
	xCursorTopLeftArrow           = 132,
	xCursorTopLeftCorner          = 134,
	xCursorTopRightCorner         = 136,
	xCursorTopSide                = 138,
	xCursorTopTee                 = 140,
	xCursorTrek                   = 142,
	xCursorUpperLeftAngle         = 144,
	xCursorUmbrella               = 146,
	xCursorUpperRightAngle        = 148,
	xCursorWatch                  = 150,
	xCursorXterm                  = 152,
};

class xBitmap;
class xFont;

class xCursor {
public:
	xScreen         *screen;

	xColor          fgcolor;
	xColor          bgcolor;

	xCursor (xScreen *, xCursorShape);      // create cursor from cursor font
	xCursor (xScreen *, xFont *, int, xFont * =0, int =0); // create cursor from glyphs
	xCursor (xScreen *, xBitmap *, xBitmap *, int, int); // create cursor from bitmaps
	xCursor (xScreen *, int, int, char *, char *, int, int); // create cursor from data
	xCursor (xScreen *, char *, char *);    // load cursor from XBM files
	~xCursor ();

	void Foreground (xColor fg)             // set foregroud color
		{ Recolor (fg, bgcolor); }
	void Background (xColor bg)             // set backgroud color
		{ Recolor (fgcolor, bg); }
	void Recolor (xColor, xColor);          // change cursor colors

private:
	long            xcursor;

	friend class xWindow;
};
