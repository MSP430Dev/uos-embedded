class   xWindow;

enum xEventType {
	xEventDown,                     // button pressed
	xEventUp,                       // button released
	xEventKey,                      // key pressed, intepreted as ascii
	xEventEnter,                    // mouse enters canvas
	xEventLeave,                    // mouse leaves canvas
	xEventMotion,                   // mouse moved
	xEventButtonMotion,             // mouse moved while button pressed
	xEventButton1Motion,            // mouse moved while button 1 pressed
	xEventButton2Motion,            // mouse moved while button 2 pressed
	xEventButton3Motion,            // mouse moved while button 3 pressed
};

enum xButtonType {
	xButtonLeft     = 1,            // left button
	xButtonMiddle   = 2,            // middle button
	xButtonRight    = 3,            // right button
};

static const int xMaskDown          = 1 << xEventDown;
static const int xMaskUp            = 1 << xEventUp;
static const int xMaskKey           = 1 << xEventKey;
static const int xMaskEnter         = 1 << xEventEnter;
static const int xMaskLeave         = 1 << xEventLeave;
static const int xMaskMotion        = 1 << xEventMotion;
static const int xMaskButtonMotion  = 1 << xEventButtonMotion;
static const int xMaskButton1Motion = 1 << xEventButton1Motion;
static const int xMaskButton2Motion = 1 << xEventButton2Motion;
static const int xMaskButton3Motion = 1 << xEventButton3Motion;

static const int xModShift          = 1 << 0;       // must be the same
static const int xModLock           = 1 << 1;       // as in <X11/X.h> !
static const int xModControl        = 1 << 2;
static const int xModMeta           = 1 << 3;
static const int xModLeftMouse      = 1 << 8;
static const int xModMiddleMouse    = 1 << 9;
static const int xModRightMouse     = 1 << 10;

class xEvent {
public:
	xWindow* window;                // target window
	xEventType type;                // type of event
	unsigned long time;             // time of event
	int x, y;                       // mouse position relative to target
	int state;                      // modifiers state
	int button;                     // button pressed or released, if any
	int len;                        // length of ASCII string
	char keystring [8];             // ASCII interpretation of event, if any

	void Parse (union _XEvent &);	// fill event structure
	void Quit ();                   // quit from run loop
};
