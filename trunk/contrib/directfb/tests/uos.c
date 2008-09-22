#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <mem/mem.h>
#include <input/mouse.h>
#include <input/keyboard.h>
#include <i8042/mouse.h>
#include <i8042/keyboard.h>

mem_pool_t *uos_memory;
timer_t *uos_timer;
mouse_t *uos_mouse;
keyboard_t *uos_keyboard;

extern void directfb_main (void *arg);

int errno;

int fprintf (int *fd, const char *fmt, ...)
{
	va_list args;
	int err;

	va_start (args, fmt);
	err = debug_vprintf (fmt, args);
	va_end (args);
	return err;
}

void uos_init (void)
{
	static mem_pool_t pool;
	char *task;
	int tasksz = 0x10000;

	uos_memory = &pool;
	mem_init (uos_memory, (size_t) 0x200000, (size_t) 0x400000);
	debug_printf ("Free memory: %ld bytes\n", mem_available (uos_memory));

	task = mem_alloc (uos_memory, tasksz);
	uos_timer = (timer_t*) mem_alloc (uos_memory, sizeof (timer_t));
	uos_keyboard = (keyboard_t*) mem_alloc (uos_memory, sizeof (keyboard_ps2_t));
	uos_mouse = (mouse_t*) mem_alloc (uos_memory, sizeof (mouse_ps2_t));
	if (! task || ! uos_timer || ! uos_keyboard) {
		debug_printf ("No memory for task\n");
		abort();
	}

	timer_init (uos_timer, 100, 1193182, 10);
	debug_puts ("Timer initialized.\n");

	keyboard_ps2_init ((keyboard_ps2_t*) uos_keyboard, 80);
	debug_puts ("Keyboard initialized.\n");

	mouse_ps2_init ((mouse_ps2_t*) uos_mouse, 90);
	debug_puts ("Mouse initialized.\n");

	task_create (directfb_main, "task", "directfb", 20, task, tasksz);
}
