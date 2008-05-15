#ifndef KBD_STACKSZ
#   if I386
#      define KBD_STACKSZ	0x400
#   endif
#endif
#define KBD_INBUFSZ	20

typedef struct _keyboard_ps2_t {
	keyboard_interface_t *interface;
	lock_t lock;
	char stack [KBD_STACKSZ];		/* task receive stack */
	keyboard_event_t in_buf [KBD_INBUFSZ];	/* keyboard event queue */
	keyboard_event_t *in_first, *in_last;	/* queue pointers */
	int rate;				/* chars per second */
	int delay;				/* milliseconds */
	int leds;				/* num, caps, scroll locks */
	int state;				/* state of scan decoder */
	int modifiers;				/* state of modifiers */
	int ctrl_alt_del;			/* reboot flag */
	int capslock;				/* caps lock pressed */
	int numlock;				/* num lock pressed */
} keyboard_ps2_t;

void keyboard_ps2_init (keyboard_ps2_t *u, int prio);
