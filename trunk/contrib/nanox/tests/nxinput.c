/*
 * Testing Nano-X keyboard and mouse.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <mem/mem.h>
#include <input/mouse.h>
#include <input/keyboard.h>
#include <i8042/mouse.h>
#include <i8042/keyboard.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/device.h"

/*
 * Update mouse status and issue events on it if necessary.
 * This function doesn't block, but is normally only called when
 * there is known to be some data waiting to be read from the mouse.
 */
int myCheckMouseEvent (void)
{
	GR_COORD	rootx;		/* latest mouse x position */
	GR_COORD	rooty;		/* latest mouse y position */
	int		newbuttons;	/* latest buttons */
	int		mousestatus;	/* latest mouse status */

	/* Read the latest mouse status: */
	mousestatus = GdReadMouse (&rootx, &rooty, &newbuttons);
	if (mousestatus <= 0) {
		if (mousestatus < 0)
			debug_printf ("Mouse error\n");
		return 0;
	}
	debug_printf ("Mouse: (%d, %d) %d\n", rootx, rooty, newbuttons);
	return 1;
}

/*
 * Update keyboard status and issue events on it if necessary.
 * This function doesn't block, but is normally only called when
 * there is known to be some data waiting to be read from the keyboard.
 */
int myCheckKeyboardEvent (void)
{
	GR_KEY		ch;		/* latest character */
	GR_KEYMOD	modifiers;	/* latest modifiers */
	GR_SCANCODE	scancode;
	int		keystatus;	/* latest keyboard status */

	/* Read the latest keyboard status: */
	keystatus = GdReadKeyboard (&ch, &modifiers, &scancode);
	if (keystatus <= 0) {
		if (keystatus < 0)
			debug_printf ("Kbd error\n");
		return 0;
	}

	debug_printf (keystatus == 1 ? "Kbd: " : "Release: ");
	if (ch < ' ')
		debug_printf ("^%c", ch + '@');
	else if (ch <= '~')
		debug_printf ("'%c'", ch);
	else if (ch == KEY_UNKNOWN)
		debug_printf ("UNKNOWN");
	else
		debug_printf ("%02x", ch);

	if (modifiers) {
		debug_printf (" +");
		if (modifiers & KEYMOD_NUM)    debug_printf (" Num");
		if (modifiers & KEYMOD_CAPS)   debug_printf (" Caps");
		if (modifiers & KEYMOD_LALT)   debug_printf (" LAlt");
		if (modifiers & KEYMOD_RALT)   debug_printf (" RAlt");
		if (modifiers & KEYMOD_LMETA)  debug_printf (" LMeta");
		if (modifiers & KEYMOD_RMETA)  debug_printf (" RMeta");
		if (modifiers & KEYMOD_LCTRL)  debug_printf (" LCtrl");
		if (modifiers & KEYMOD_RCTRL)  debug_printf (" RCtrl");
		if (modifiers & KEYMOD_LSHIFT) debug_printf (" LShift");
		if (modifiers & KEYMOD_RSHIFT) debug_printf (" RShift");
	}
	debug_printf ("\n");
	return 1;
}

void mySelect (void)
{
	int have_events = 0;
	extern timer_t *uos_timer;

	/* If mouse data present, service it*/
	while (myCheckMouseEvent ())
		have_events = 1;

	/* If keyboard data present, service it*/
	while (myCheckKeyboardEvent ())
		have_events = 1;

	if (! have_events)
		timer_delay (uos_timer, 20);
}

void nxmain ()
{
	if (GdOpenKeyboard() < 0) {
		debug_printf ("Cannot initialise keyboard\n");
		uos_halt();
	}

	if (GdOpenMouse() < 0) {
		debug_printf ("Cannot initialise mouse\n");
		GdCloseKeyboard ();
		uos_halt();
	}

	for (;;)
		mySelect ();
}

mem_pool_t *uos_memory;
timer_t *uos_timer;
mouse_t *uos_mouse;
keyboard_t *uos_keyboard;

extern unsigned long i386_highmem_addr;
extern unsigned long i386_highmem_len;

void uos_init (void)
{
	static mem_pool_t pool;
	char *task, *wm;
	int tasksz = 0x10000;
	extern unsigned long _end;

	uos_memory = &pool;
	if (i386_highmem_len)
		mem_init (uos_memory, (mem_size_t) &_end,
			i386_highmem_addr + i386_highmem_len);

	debug_printf ("Free memory: %ld bytes\n", mem_available (uos_memory));

	task = mem_alloc (uos_memory, tasksz);
	wm = mem_alloc (uos_memory, tasksz);
	uos_timer = (timer_t*) mem_alloc (uos_memory, sizeof (timer_t));
	uos_keyboard = (keyboard_t*) mem_alloc (uos_memory, sizeof (keyboard_ps2_t));
	uos_mouse = (mouse_t*) mem_alloc (uos_memory, sizeof (mouse_ps2_t));
	if (! task || ! wm || ! uos_timer || ! uos_keyboard || ! uos_mouse) {
		debug_printf ("No memory for task\n");
		abort();
	}

	timer_init (uos_timer, 100, 1193182, 10);
	debug_puts ("Timer initialized.\n");

	keyboard_ps2_init ((keyboard_ps2_t*) uos_keyboard, 80);
	debug_puts ("Keyboard initialized.\n");

	mouse_ps2_init ((mouse_ps2_t*) uos_mouse, 90);
	debug_puts ("Mouse initialized.\n");

	task_create (nxmain, 0, "nanox", 10, task, tasksz);
}
