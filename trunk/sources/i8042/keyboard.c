/*
 * Driver for PS/2 keyboard.
 *
 * Copyright (C) 2005 Serge Vakulenko
 *
 * Specifications of PS/2 keyboard interface are available
 * at http://www.computer-engineering.org/ps2keyboard/
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "input/keyboard.h"
#include "i8042/keyboard.h"
#include "i8042/i8042.h"
#include <runtime/i386/int86.h>

#if I386
#   define RECEIVE_IRQ		1
#endif

#define SCAN_CTRL	0x1D	/* Control */
#define SCAN_LSHIFT	0x2A	/* Left shift */
#define SCAN_RSHIFT	0x36	/* Right shift */
#define SCAN_ALT	0x38	/* Right shift */
#define SCAN_DEL	0x53	/* Del */
#define SCAN_CAPS	0x3A	/* Caps lock */
#define SCAN_NUM	0x45	/* Num lock */
#define SCAN_LGUI	0x5B	/* Left Windows */
#define SCAN_RGUI	0x5C	/* Right Windows */

#define STATE_BASE	0
#define STATE_E0	1	/* got E0 */
#define STATE_E1	2	/* got E1 */
#define STATE_E11D	3	/* got E1-1D or E1-9D */

static const unsigned short scan_to_key [256] = {
/* 00 */	0,		KEY_ESCAPE,	'1',		'2',
/* 04 */	'3',		'4',		'5',		'6',
/* 08 */	'7',		'8',		'9',		'0',
/* 0C */	'-',		'=',		KEY_BACKSPACE,	KEY_TAB,
/* 10 */	'Q',		'W',		'E',		'R',
/* 14 */	'T',		'Y',		'U',		'I',
/* 18 */	'O',		'P',		'[',		']',
/* 1C */	KEY_ENTER,	KEY_LCTRL,	'A',		'S',
/* 20 */	'D',		'F',		'G',		'H',
/* 24 */	'J',		'K',		'L',		';',
/* 28 */	'\'',		'`',		KEY_LSHIFT,	'\\',
/* 2C */	'Z',		'X',		'C',		'V',
/* 30 */	'B',		'N',		'M',		',',
/* 34 */	'.',		'/',		KEY_RSHIFT,	KEY_KP_MULTIPLY,
/* 38 */	KEY_LALT,	' ',		KEY_CAPSLOCK,	KEY_F1,
/* 3C */	KEY_F2,		KEY_F3,		KEY_F4,		KEY_F5,
/* 40 */	KEY_F6,		KEY_F7,		KEY_F8,		KEY_F9,
/* 44 */	KEY_F10,	KEY_NUMLOCK,	KEY_SCROLLOCK,	KEY_KP7,
/* 48 */	KEY_KP8,	KEY_KP9,	KEY_KP_MINUS,	KEY_KP4,
/* 4C */	KEY_KP5,	KEY_KP6,	KEY_KP_PLUS,	KEY_KP1,
/* 50 */	KEY_KP2,	KEY_KP3,	KEY_KP0,	KEY_KP_PERIOD,
/* 54 */	0,		0,		0,		KEY_F11,
/* 58 */	KEY_F12,	0,		0,		0,
/* 5C */	0,		0,		0,		0,
/* 60 */	0,		0,		0,		0,
/* 64 */	0,		0,		0,		0,
/* 68 */	0,		0,		0,		0,
/* 6C */	0,		0,		0,		0,
/* 70 */	0,		0,		0,		0,
/* 74 */	0,		0,		0,		0,
/* 78 */	0,		0,		0,		0,
/* 7C */	0,		0,		0,		0,
/* E0,00 */	0,		0,		0,		0,
/* E0,04 */	0,		0,		0,		0,
/* E0,08 */	0,		0,		0,		0,
/* E0,0C */	0,		0,		0,		0,
/* E0,10 */	KEY_TRACK_PREV,	0,		0,		0,
/* E0,14 */	0,		0,		0,		0,
/* E0,18 */	0,		KEY_TRACK_NEXT,	0,		0,
/* E0,1C */	KEY_KP_ENTER,	KEY_RCTRL,	0,		0,
/* E0,20 */	KEY_MUTE,	/*Calc*/ 0,	KEY_PLAY,	0,
/* E0,24 */	KEY_STOP,	0,		0,		0,
/* E0,28 */	0,		0,		0,		0,
/* E0,2C */	0,		0,		KEY_VOLUME_DOWN, 0,
/* E0,30 */	KEY_VOLUME_UP,	0,		/*WWWHome*/ 0,	0,
/* E0,34 */	0,		KEY_KP_DIVIDE,	0,		KEY_PRINT,
/* E0,38 */	KEY_RALT,	0,		0,		0,
/* E0,3C */	0,		0,		0,		0,
/* E0,40 */	0,		0,		0,		0,
/* E0,44 */	0,		0,		0,		KEY_HOME,
/* E0,48 */	KEY_UP,		KEY_PAGEUP,	0,		KEY_LEFT,
/* E0,4C */	0,		KEY_RIGHT,	0,		KEY_END,
/* E0,50 */	KEY_DOWN,	KEY_PAGEDOWN,	KEY_INSERT,	KEY_DELETE,
/* E0,54 */	0,		0,		0,		0,
/* E0,58 */	0,		0,		0,		KEY_LMETA,
/* E0,5C */	KEY_RMETA,	KEY_MENU,	KEY_POWER,	/*Sleep*/ 0,
/* E0,60 */	0,		0,		0,		/* Wake */ 0,
/* E0,64 */	0,		/*WWWSearch*/0,	/*WWWFavor*/ 0,	/*WWWRefresh*/0,
/* E0,68 */	/*WWWStop*/ 0,	/*WWWForw*/ 0,	/*WWWBack*/ 0,	/*MyComp*/ 0,
/* E0,6C */	/*EMail*/ 0,	/*MediaSel*/ 0,	0,		0,
/* E0,70 */	0,		0,		0,		0,
/* E0,74 */	0,		0,		0,		0,
/* E0,78 */	0,		0,		0,		0,
/* E0,7C */	0,		0,		0,		0,
};

static int
set_rate_delay (int cps, int msec)
{
	unsigned char param;

	if      (cps < 3)  param = 0x1f;	/* 2 chars/sec */
	else if (cps < 4)  param = 0x1a;	/* 3 chars/sec */
	else if (cps < 5)  param = 0x17;	/* 4 chars/sec */
	else if (cps < 6)  param = 0x14;	/* 5 chars/sec */
	else if (cps < 7)  param = 0x12;	/* 6 chars/sec */
	else if (cps < 10) param = 0x0f;	/* 8 chars/sec */
	else if (cps < 11) param = 0x0c;	/* 10 chars/sec */
	else if (cps < 13) param = 0x0a;	/* 12 chars/sec */
	else if (cps < 16) param = 0x08;	/* 15 chars/sec */
	else if (cps < 20) param = 0x06;	/* 17 chars/sec */
	else if (cps < 23) param = 0x04;	/* 21 chars/sec */
	else if (cps < 26) param = 0x02;	/* 24 chars/sec */
	else if (cps < 29) param = 0x01;	/* 27 chars/sec */
	else               param = 0x00;	/* 30 chars/sec */

	if      (msec > 800) param |= 0x60;	/* 1.0 sec */
	else if (msec > 600) param |= 0x40;	/* 0.75 sec */
	else if (msec > 400) param |= 0x20;	/* 0.5 sec */

	return i8042_kbd_command (KBDK_TYPEMATIC, param);
}

static int
set_leds (int leds)
{
	unsigned char param;

	param = 0;
	if (leds & KEYLED_NUM)    param |= KBDK_SETLED_NUM_LOCK;
	if (leds & KEYLED_CAPS)   param |= KBDK_SETLED_CAPS_LOCK;
	if (leds & KEYLED_SCROLL) param |= KBDK_SETLED_SCROLL_LOCK;

	return i8042_kbd_command (KBDK_SETLED, param);
}

/*
 * Process a byte, received from keyboard.
 * Return 1 when new event record is generated.
 */
static int
make_event (keyboard_ps2_t *u, keyboard_event_t *m, unsigned char byte)
{
	switch (u->state) {
	case STATE_BASE:
		m->key = scan_to_key [byte & 0x7f];
		switch (byte) {
		case 0xE0:
			/* First part of two-byte sequence. */
			u->state = STATE_E0;
			return 0;
		case 0xE1:
			/* First byte of Pause sequence. */
			u->state = STATE_E1;
			return 0;
		case SCAN_CTRL:
			if (u->modifiers & KEYMOD_LCTRL)
				return 0;
			u->modifiers |= KEYMOD_LCTRL;
			break;
		case SCAN_CTRL | 0x80:
			if (! (u->modifiers & KEYMOD_LCTRL))
				return 0;
			u->modifiers &= ~KEYMOD_LCTRL;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_LSHIFT:
			if (u->modifiers & KEYMOD_LSHIFT)
				return 0;
			u->modifiers |= KEYMOD_LSHIFT;
			break;
		case SCAN_RSHIFT:
			if (u->modifiers & KEYMOD_RSHIFT)
				return 0;
			u->modifiers |= KEYMOD_RSHIFT;
			break;
		case SCAN_LSHIFT | 0x80:
			if (! (u->modifiers & KEYMOD_LSHIFT))
				return 0;
			u->modifiers &= ~KEYMOD_LSHIFT;
			break;
		case SCAN_RSHIFT | 0x80:
			if (! (u->modifiers & KEYMOD_RSHIFT))
				return 0;
			u->modifiers &= ~KEYMOD_RSHIFT;
			break;
		case SCAN_ALT:
			if (u->modifiers & KEYMOD_LALT)
				return 0;
			u->modifiers |= KEYMOD_LALT;
			break;
		case SCAN_ALT | 0x80:
			if (! (u->modifiers & KEYMOD_LALT))
				return 0;
			u->modifiers &= ~KEYMOD_LALT;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_CAPS:
			if (u->capslock)
				return 0;
			u->capslock = 1;
			if (u->modifiers & KEYMOD_CAPS) {
				u->modifiers &= ~KEYMOD_CAPS;
				u->leds &= ~KEYLED_CAPS;
			} else {
				u->modifiers |= KEYMOD_CAPS;
				u->leds |= KEYLED_CAPS;
			}
			set_leds (u->leds);
			break;
		case SCAN_CAPS | 0x80:
			/* Caps lock released - ignore. */
			u->capslock = 0;
			return 0;
		case SCAN_NUM:
			if (u->numlock)
				return 0;
			u->numlock = 1;
			if (u->modifiers & KEYMOD_NUM) {
				u->modifiers &= ~KEYMOD_NUM;
				u->leds &= ~KEYLED_NUM;
			} else {
				u->modifiers |= KEYMOD_NUM;
				u->leds |= KEYLED_NUM;
			}
			set_leds (u->leds);
			break;
		case SCAN_NUM | 0x80:
			/* Num lock released - ignore. */
			u->numlock = 0;
			return 0;
		case SCAN_DEL:
			if ((u->modifiers & KEYMOD_CTRL) &&
			    (u->modifiers & KEYMOD_ALT)) {
				/* Reboot on Ctrl-Alt-Del. */
				++u->ctrl_alt_del;
				if (u->ctrl_alt_del >= 3) {
					debug_printf ("Rebooting...\n");
					i386_reboot (0x1234);
				}
			}
			break;
		case SCAN_DEL | 0x80:
			u->ctrl_alt_del = 0;
			break;
		}
		break;

	case STATE_E0:
		/* Second part of two-byte sequence. */
		m->key = scan_to_key [byte | 0x80];
		u->state = STATE_BASE;
		switch (byte) {
		case SCAN_CTRL:
			if (u->modifiers & KEYMOD_RCTRL)
				return 0;
			u->modifiers |= KEYMOD_RCTRL;
			break;
		case SCAN_CTRL | 0x80:
			if (! (u->modifiers & KEYMOD_RCTRL))
				return 0;
			u->modifiers &= ~KEYMOD_RCTRL;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_ALT:
			if (u->modifiers & KEYMOD_RALT)
				return 0;
			u->modifiers |= KEYMOD_RALT;
			break;
		case SCAN_ALT | 0x80:
			if (! (u->modifiers & KEYMOD_RALT))
				return 0;
			u->modifiers &= ~KEYMOD_RALT;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_LSHIFT:
		case SCAN_LSHIFT | 0x80:
			/* Part of Print Screen sequence - ignore. */
			return 0;
		case SCAN_LGUI:
			if (u->modifiers & KEYMOD_LMETA)
				return 0;
			u->modifiers |= KEYMOD_LMETA;
			break;
		case SCAN_LGUI | 0x80:
			if (! (u->modifiers & KEYMOD_LMETA))
				return 0;
			u->modifiers &= ~KEYMOD_LMETA;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_RGUI:
			if (u->modifiers & KEYMOD_RMETA)
				return 0;
			u->modifiers |= KEYMOD_RMETA;
			break;
		case SCAN_RGUI | 0x80:
			if (! (u->modifiers & KEYMOD_RMETA))
				return 0;
			u->modifiers &= ~KEYMOD_RMETA;
			u->ctrl_alt_del = 0;
			break;
		case SCAN_DEL:
			if ((u->modifiers & KEYMOD_CTRL) &&
			    (u->modifiers & KEYMOD_ALT)) {
				/* Reboot on Ctrl-Alt-Del. */
				++u->ctrl_alt_del;
				if (u->ctrl_alt_del >= 3) {
					debug_printf ("Rebooting...\n");
					i386_reboot (0x1234);
				}
			}
			break;
		case SCAN_DEL | 0x80:
			u->ctrl_alt_del = 0;
			break;
		}
		break;

	case STATE_E1:
		if (byte == 0x1D || byte == 0x9D) {
			/* Second byte of Pause sequence. */
			u->state = STATE_E11D;
		} else
			u->state = STATE_BASE;
		return 0;

	case STATE_E11D:
		u->state = STATE_BASE;
		if (byte == 0x45 || byte == 0xC5) {
			/* Third byte of Pause sequence. */
			m->key = KEY_PAUSE;
			break;
		}
		return 0;
	}
	if (! m->key)
		return 0;
	m->release = (byte > 0x7f);
	return 1;
}

/*
 * Process a byte, received from keyboard.
 * Return 1 when new event record is generated.
 */
static int
receive_byte (keyboard_ps2_t *u, unsigned char byte)
{
	keyboard_event_t *newlast;

	if (byte == KBDR_TEST_OK && ! (u->modifiers & KEYMOD_LSHIFT) &&
	    u->state == STATE_BASE) {
		/* New device connected. */
		set_leds (u->leds);
		set_rate_delay (u->rate, u->delay);
		u->state = STATE_BASE;
		u->in_first = u->in_last = u->in_buf;
		return 0;
	}

	/* Advance queue pointer. */
	newlast = u->in_last + 1;
	if (newlast >= u->in_buf + KBD_INBUFSZ)
		newlast = u->in_buf;

	/* Ignore input on buffer overflow. */
	if (newlast == u->in_first)
		return 0;

	/* Make event record. */
	if (! make_event (u, u->in_last, byte))
		return 0;
	u->in_last->modifiers = u->modifiers;

	u->in_last = newlast;
	return 1;
}

/*
 * Process keyboard interrupt.
 * Get raw data and put to buffer. Return 1 when not enough data
 * for a event record.
 * Return 0 when a new event record is generated and
 * a signal to keyboard task is needed.
 */
static bool_t
keyboard_ps2_interrupt (keyboard_ps2_t *u)
{
	unsigned char c, sts, strobe;
	int event_generated = 0;

	/* Read the pending information. */
	for (;;) {
		sts = inb (KBDC_AT_CTL);
		if (! (sts & KBSTS_DATAVL))
			return (event_generated == 0);

		c = inb (KBD_DATA);
		if (sts & KBSTS_AUX_DATAVL) {
			/* This byte is for mouse.
			 * Just ignore it for now (TODO). */
			continue;
		}

		/* Strobe the keyboard to ack the char. */
		strobe = inb (KBDC_XT_CTL);
		outb (strobe | KBDC_XT_CLEAR, KBDC_XT_CTL);
		outb (strobe, KBDC_XT_CTL);

		/*debug_printf ("<%02x> ", c);*/
		if (receive_byte (u, c))
			event_generated = 1;
	}
}

/*
 * Mouse task.
 */
static void
keyboard_ps2_task (void *arg)
{
	keyboard_ps2_t *u = arg;

	lock_take_irq (&u->lock, RECEIVE_IRQ,
		(handler_t) keyboard_ps2_interrupt, u);

	i8042_kbd_enable ();
	if (i8042_kbd_probe ()) {
		set_leds (u->leds);
		set_rate_delay (u->rate, u->delay);
	}

	for (;;) {
		lock_wait (&u->lock);
		/* Nothing to do. */
	}
}

/*
 * Read mouse movement data. Return immediately, do not wait.
 * Return 1 if data available, 0 if no data.
 */
static int
keyboard_ps2_get_event (keyboard_ps2_t *u, keyboard_event_t *data)
{
	lock_take (&u->lock);
	if (u->in_first == u->in_last) {
		lock_release (&u->lock);
		return 0;
	} else {
		*data = *u->in_first++;
		if (u->in_first >= u->in_buf + KBD_INBUFSZ)
			u->in_first = u->in_buf;
		lock_release (&u->lock);
		return 1;
	}
}

/*
 * Wait for the mouse movement and return it.
 */
static void
keyboard_ps2_wait_event (keyboard_ps2_t *u, keyboard_event_t *data)
{
	lock_take (&u->lock);

	/* Wait until receive data available. */
	while (u->in_first == u->in_last)
		lock_wait (&u->lock);

	*data = *u->in_first++;
	if (u->in_first >= u->in_buf + KBD_INBUFSZ)
		u->in_first = u->in_buf;

	lock_release (&u->lock);
}

static int
keyboard_ps2_get_modifiers (keyboard_ps2_t *u)
{
	int modifiers;

	lock_take (&u->lock);
	modifiers = u->modifiers;
	lock_release (&u->lock);
	return modifiers;
}

static int
keyboard_ps2_get_leds (keyboard_ps2_t *u)
{
	int leds;

	lock_take (&u->lock);
	leds = u->leds;
	lock_release (&u->lock);
	return leds;
}

static void
keyboard_ps2_set_leds (keyboard_ps2_t *u, int leds)
{
	lock_take (&u->lock);

	u->leds = leds;
	set_leds (u->leds);

	lock_release (&u->lock);
}

static int
keyboard_ps2_get_rate (keyboard_ps2_t *u)
{
	int rate;

	lock_take (&u->lock);
	rate = u->rate;
	lock_release (&u->lock);
	return rate;
}

static void
keyboard_ps2_set_rate (keyboard_ps2_t *u, int cps)
{
	lock_take (&u->lock);

	if      (cps < 3)  u->rate = 2;
	else if (cps < 4)  u->rate = 3;
	else if (cps < 5)  u->rate = 4;
	else if (cps < 6)  u->rate = 5;
	else if (cps < 7)  u->rate = 6;
	else if (cps < 10) u->rate = 8;
	else if (cps < 11) u->rate = 10;
	else if (cps < 13) u->rate = 12;
	else if (cps < 16) u->rate = 15;
	else if (cps < 20) u->rate = 17;
	else if (cps < 23) u->rate = 21;
	else if (cps < 26) u->rate = 24;
	else if (cps < 29) u->rate = 27;
	else               u->rate = 30;

	set_rate_delay (u->rate, u->delay);

	lock_release (&u->lock);
}

static int
keyboard_ps2_get_delay (keyboard_ps2_t *u)
{
	int delay;

	lock_take (&u->lock);
	delay = u->delay;
	lock_release (&u->lock);
	return delay;
}

static void
keyboard_ps2_set_delay (keyboard_ps2_t *u, int msec)
{
	lock_take (&u->lock);

	if      (msec > 800) u->delay = 1000;
	else if (msec > 600) u->delay = 750;
	else if (msec > 400) u->delay = 500;
	else                 u->delay = 250;

	set_rate_delay (u->rate, u->delay);

	lock_release (&u->lock);
}

static keyboard_interface_t keyboard_ps2_interface = {
	(void (*) (keyboard_t*, keyboard_event_t*))	keyboard_ps2_wait_event,
	(int (*) (keyboard_t*, keyboard_event_t*))	keyboard_ps2_get_event,
	(int (*) (keyboard_t*))				keyboard_ps2_get_modifiers,
	(int (*) (keyboard_t*))				keyboard_ps2_get_leds,
	(void (*) (keyboard_t*, int))			keyboard_ps2_set_leds,
	(int (*) (keyboard_t*))				keyboard_ps2_get_rate,
	(void (*) (keyboard_t*, int))			keyboard_ps2_set_rate,
	(int (*) (keyboard_t*))				keyboard_ps2_get_delay,
	(void (*) (keyboard_t*, int))			keyboard_ps2_set_delay,
};

void
keyboard_ps2_init (keyboard_ps2_t *u, int prio)
{
        u->interface = &keyboard_ps2_interface;
	u->rate = 20;
	u->delay = 500;
	u->state = STATE_BASE;
	u->in_first = u->in_last = u->in_buf;

	/* Create keyboard receive task. */
	task_create (keyboard_ps2_task, u, "kbd", prio,
		u->stack, sizeof (u->stack));
}
