/*
 * Copyright (c) 2005 Serge Vakulenko <vak@cronyx.ru>
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X keyboard driver for uOS.
 */
#include <runtime/lib.h>
#include <input/keyboard.h>
#include "device.h"

extern keyboard_t *uos_keyboard;

/*
 * Open the keyboard.
 */
int
MWKbd_Open (KBDDEVICE *pkd)
{
	/* Do nothing. */
	return 1;
}

/*
 * Close the keyboard.
 */
void
MWKbd_Close (void)
{
	/* Do nothing. */
}

/*
 * Return the possible modifiers for the keyboard.
 */
void
MWKbd_GetModifierInfo (MWKEYMOD *modifiers, MWKEYMOD *curmodifiers)
{
        *modifiers = MWKMOD_CTRL | MWKMOD_SHIFT | MWKMOD_ALT |
		MWKMOD_META | MWKMOD_CAPS | MWKMOD_NUM;
	if (curmodifiers)
		*curmodifiers = keyboard_get_modifiers (uos_keyboard);

}

static int
scan_unicode_to_nanox (int key)
{
	switch (key) {
	case '0':		return 11;
	case '1':		return 2;
	case '2':		return 3;
	case '3':		return 4;
	case '4':		return 5;
	case '5':		return 6;
	case '6':		return 7;
	case '7':		return 8;
	case '8':		return 9;
	case '9':		return 10;
	case ' ':		return 57;
	case '-':		return 12;
	case '=':		return 13;
	case '[':		return 26;
	case ']':		return 27;
	case ';':		return 39;
	case '\'':		return 40;
	case '`':		return 41;
	case '\\':		return 43;
	case ',':		return 51;
	case '.':		return 52;
	case '/':		return 53;
	case 'A':		return 30;
	case 'B':		return 48;
	case 'C':		return 46;
	case 'D':		return 32;
	case 'E':		return 18;
	case 'F':		return 33;
	case 'G':		return 34;
	case 'H':		return 35;
	case 'I':		return 23;
	case 'J':		return 36;
	case 'K':		return 37;
	case 'L':		return 38;
	case 'M':		return 50;
	case 'N':		return 49;
	case 'O':		return 24;
	case 'P':		return 25;
	case 'Q':		return 16;
	case 'R':		return 19;
	case 'S':		return 31;
	case 'T':		return 20;
	case 'U':		return 22;
	case 'V':		return 47;
	case 'W':		return 17;
	case 'X':		return 45;
	case 'Y':		return 21;
	case 'Z':		return 44;
	case KEY_F1:		return 59;
	case KEY_F2:		return 60;
	case KEY_F3:		return 61;
	case KEY_F4:		return 62;
	case KEY_F5:		return 63;
	case KEY_F6:		return 64;
	case KEY_F7:		return 65;
	case KEY_F8:		return 66;
	case KEY_F9:		return 67;
	case KEY_F10:		return 68;
	case KEY_F11:		return 87;
	case KEY_F12:		return 88;
	case KEY_KP0:		return 82;
	case KEY_KP1:		return 79;
	case KEY_KP2:		return 80;
	case KEY_KP3:		return 81;
	case KEY_KP4:		return 75;
	case KEY_KP5:		return 76;
	case KEY_KP6:		return 77;
	case KEY_KP7:		return 71;
	case KEY_KP8:		return 72;
	case KEY_KP9:		return 73;
	case KEY_KP_DIVIDE:	return 98;
	case KEY_KP_ENTER:	return 96;
	case KEY_KP_MINUS:	return 74;
	case KEY_KP_MULTIPLY:	return 55;
	case KEY_KP_PERIOD:	return 83;
	case KEY_KP_PLUS:	return 78;
	case KEY_BACKSPACE:	return 14;
	case KEY_BREAK:		return 101;
	case KEY_CAPSLOCK:	return 58;
	case KEY_DELETE:	return 111;
	case KEY_DOWN:		return 108;
	case KEY_END:		return 107;
	case KEY_ENTER:		return 28;
	case KEY_ESCAPE:	return 1;
	case KEY_HOME:		return 102;
	case KEY_INSERT:	return 110;
	case KEY_LALT:		return 56;
	case KEY_LCTRL:		return 29;
	case KEY_LEFT:		return 105;
	case KEY_LMETA:		return 125;
	case KEY_LSHIFT:	return 42;
	case KEY_MENU:		return 127;
	case KEY_NUMLOCK:	return 69;
	case KEY_PAGEDOWN:	return 109;
	case KEY_PAGEUP:	return 104;
	case KEY_PAUSE:		return 119;
	case KEY_PRINT:		return 99;
	case KEY_RALT:		return 100;
	case KEY_RCTRL:		return 97;
	case KEY_RIGHT:		return 106;
	case KEY_RMETA:		return 126;
	case KEY_RSHIFT:	return 54;
	case KEY_SCROLLOCK:	return 70;
	case KEY_TAB:		return 15;
	case KEY_UP:		return 103;
	}
	return 0;
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the mode keys (ALT, SHIFT, CTRL).  Returns -1 on error, 0 if no data
 * is ready, and 1 if data was read.  This is a non-blocking call.
 */
int
MWKbd_Read (MWKEY *buf, MWKEYMOD *modifiers, MWSCANCODE *scancode)
{
	keyboard_event_t event;
again:
	if (! keyboard_get_event (uos_keyboard, &event))
		return 0;

	/* Compute a standard Nano-X scancode value.
	 * (See keymap_standard.h). */
	*scancode = scan_unicode_to_nanox (event.key);

	/* Process shift, ctrl, capslock, numlock. */
	keyboard_translate (&event);
	if (event.key == KEY_UNKNOWN)
		goto again;

        *buf = event.key;
	*modifiers = event.modifiers;
	return event.release ? 2 : 1;
}

KBDDEVICE kbddev = {
        MWKbd_Open,
	MWKbd_Close,
	MWKbd_GetModifierInfo,
	MWKbd_Read,
	0
};
