/*
 * Testing keyboard.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "input/keyboard.h"
#include "i8042/keyboard.h"

char task [0x400];
keyboard_ps2_t keyboard;

void hello (void *arg)
{
	keyboard_event_t event;

	debug_puts ("\nWaiting for keyboard events.\n");
	for (;;) {
		keyboard_wait_event (&keyboard, &event);
		keyboard_translate (&event);

		if (event.release)
			debug_printf ("\nRelease: ");
		else
			debug_printf ("\nKey: ");

		if (event.key < ' ')
			debug_printf ("^%c", event.key + '@');
		else if (event.key <= '~')
			debug_printf ("'%c'", event.key);
		else if (event.key == KEY_UNKNOWN)
			debug_printf ("UNKNOWN");
		else
			debug_printf ("%02x", event.key);

		if (! event.modifiers)
			continue;

		debug_printf (" +");

		if (event.modifiers & KEYMOD_NUM)    debug_printf (" Num");
		if (event.modifiers & KEYMOD_CAPS)   debug_printf (" Caps");
		if (event.modifiers & KEYMOD_LALT)   debug_printf (" LAlt");
		if (event.modifiers & KEYMOD_RALT)   debug_printf (" RAlt");
		if (event.modifiers & KEYMOD_LMETA)  debug_printf (" LMeta");
		if (event.modifiers & KEYMOD_RMETA)  debug_printf (" RMeta");
		if (event.modifiers & KEYMOD_LCTRL)  debug_printf (" LCtrl");
		if (event.modifiers & KEYMOD_RCTRL)  debug_printf (" RCtrl");
		if (event.modifiers & KEYMOD_LSHIFT) debug_printf (" LShift");
		if (event.modifiers & KEYMOD_RSHIFT) debug_printf (" RShift");
	}
}

void uos_init (void)
{
	debug_puts ("\nTesting keyboard.\n");
	keyboard_ps2_init (&keyboard, 90);
	task_create (hello, "task", "hello", 1, task, sizeof (task));
}
