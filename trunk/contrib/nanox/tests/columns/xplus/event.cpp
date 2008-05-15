# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include "event.h"

void xEvent::Parse (XEvent& xev)
{
	KeySym ksym;

	switch (xev.type) {
	case ButtonPress:
		type = xEventDown;
		time = xev.xbutton.time;
		x = xev.xbutton.x;
		y = xev.xbutton.y;
		state = xev.xbutton.state;
		button = xev.xbutton.button;
		break;
	case ButtonRelease:
		type = xEventUp;
		time = xev.xbutton.time;
		x = xev.xbutton.x;
		y = xev.xbutton.y;
		state = xev.xbutton.state;
		button = xev.xbutton.button;
		break;
	case KeyPress:
		type = xEventKey;
		time = xev.xkey.time;
		x = xev.xkey.x;
		y = xev.xkey.y;
		state = xev.xkey.state;
		len = XLookupString (&xev.xkey, keystring, sizeof (keystring),
			&ksym, (XComposeStatus *) 0);
		button = ksym;
		break;
	case MotionNotify:
		type = xEventMotion;
		time = xev.xmotion.time;
		x = xev.xmotion.x;
		y = xev.xmotion.y;
		state = xev.xmotion.state;
		break;
	case EnterNotify:
		type = xEventEnter;
		time = xev.xcrossing.time;
		x = xev.xcrossing.x;
		y = xev.xcrossing.y;
		state = xev.xcrossing.state;
		break;
	case LeaveNotify:
		type = xEventLeave;
		time = xev.xcrossing.time;
		x = xev.xcrossing.x;
		y = xev.xcrossing.y;
		state = xev.xcrossing.state;
		break;
	}
}

void xEvent::Quit ()
{
	window = 0;
}
