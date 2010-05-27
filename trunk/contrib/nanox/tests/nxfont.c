/*
 * Demonstration program for freetype truetype font support
 * Martin Jolicoeur 2000 martinj@visuaide.com.
 */
#include <runtime/lib.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/nxcolors.h"

#define MAXW 800
#define MAXH 600

GR_GC_ID gid;
GR_FONT_ID fontid, fontid2;
GR_BOOL kerning = GR_FALSE;
GR_BOOL aa = GR_TRUE;
GR_BOOL underline = GR_FALSE;
int angle = 0;
int state = GR_TFBOTTOM;
unsigned char buffer[128] = "Hello, World!";
int n;

void render (GR_WINDOW_ID window)
{
	GrSetGCBackground (gid, GR_COLOR_WHITE);
	GrSetGCForeground (gid, GR_COLOR_WHITE);
	GrSetGCUseBackground (gid, GR_FALSE);
	GrFillRect (window, gid, 0, 0, MAXW, MAXH);
	GrSetGCForeground (gid, GR_COLOR_BLACK);

	/* Draw menu */
	GrSetGCFont (gid, fontid);
	GrSetFontAttr (fontid, GR_TFKERNING | GR_TFANTIALIAS, 0);
	GrText (window, gid, 5, 20, "+ Rotate string clockwise", 25, GR_TFASCII);
	GrText (window, gid, 5, 40, "- Rotate string counter-clockwise", 33, GR_TFASCII);
	GrText (window, gid, 5, 60, "a Toggle anti-aliasing", 22, GR_TFASCII);
	GrText (window, gid, 5, 80, "k Toggle kerning", 16, GR_TFASCII);
	GrText (window, gid, 5, 100, "u Toggle underline", 18, GR_TFASCII);
	GrText (window, gid, 5, 120, "l Toggle alignment bottom/baseline/top", 38, GR_TFASCII);
#if 0
{
char buf [40];
snprintf (buf, 40, "%04x", key);
GrText (window, gid, 5, 140, buf, strlen (buf), GR_TFASCII);
}
#endif
	/* Draw test string */
	GrSetGCFont (gid, fontid2);
	GrSetFontAttr (fontid2, (kerning ? GR_TFKERNING : 0) | (aa ? GR_TFANTIALIAS : 0) |
	    (underline ? GR_TFUNDERLINE : 0), -1);
	GrSetFontRotation (fontid2, angle);
	GrText (window, gid, MAXW / 2, MAXH / 2, buffer, n, state | GR_TFUTF8);

	/* Draw arrow */
	GrLine (window, gid, (MAXW / 2) - 10, MAXH / 2, (MAXW / 2) + 10, MAXH / 2);
	GrLine (window, gid, MAXW / 2, (MAXH / 2) - 10, MAXW / 2, (MAXH / 2) + 10);
}

void nxmain (void *arg)
{
	GR_EVENT event;
	GR_WINDOW_ID window;

	if (GrOpen () < 0) {
		debug_puts ("Cannot open graphics\n");
		uos_halt (0);
	}
	window = GrNewWindowEx (GR_WM_PROPS_APPWINDOW, (unsigned char*) "ftdemo",
	    GR_ROOT_WINDOW_ID, 0, 0, MAXW, MAXH, GR_COLOR_WHITE);
	GrMapWindow (window);

	gid = GrNewGC ();
	GrSelectEvents (window, GR_EVENT_MASK_KEY_DOWN |
	    GR_EVENT_MASK_CLOSE_REQ | GR_EVENT_MASK_EXPOSURE);

	n = strlen (buffer);

	fontid = GrCreateFont ((unsigned char*) GR_FONT_SYSTEM_FIXED, 0, 0);
	fontid2 = GrCreateFont ((unsigned char*) GR_FONT_SYSTEM_VAR, 0, 0);

	render (window);

	while (1) {
		GrGetNextEvent (&event);

		switch (event.type) {
		case GR_EVENT_TYPE_KEY_DOWN:
			switch (event.keystroke.ch) {
			case 171:	/* + */
			case '+':
			case '=':
				angle += 100;	/* Increase 10 degrees */
				angle %= 3600;
				break;
			case 173:	/* - */
			case '-':
			case '_':
				angle -= 100;	/* Decrease 10 degrees */
				angle %= 3600;
				break;
			case 'a':
				aa = (aa == GR_FALSE) ? GR_TRUE : GR_FALSE;
				break;
			case 'k':
				kerning = (kerning == GR_FALSE) ? GR_TRUE : GR_FALSE;
				break;
			case 'l':
				state = (state == GR_TFBOTTOM) ? GR_TFBASELINE : \
				    (state == GR_TFBASELINE) ? GR_TFTOP : GR_TFBOTTOM;
				break;
			case 'u':
				underline = underline ? GR_FALSE : GR_TRUE;
				break;
			default:
				continue;
				/* Unknown keystroke */
			}
			render (window);
			break;

		case GR_EVENT_TYPE_EXPOSURE:
			render (window);
			break;

		case GR_EVENT_TYPE_CLOSE_REQ:
			/* Ignore. */
			break;
		}
	}
}
