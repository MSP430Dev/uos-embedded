#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <mem/mem.h>

extern "C" {

#include <input/mouse.h>
#include <input/keyboard.h>
#include <i8042/mouse.h>
#include <i8042/keyboard.h>

mem_pool_t *uos_memory;
timer_t *uos_timer;
mouse_t *uos_mouse;
keyboard_t *uos_keyboard;
int errno;

extern void Wm_run (void *arg);
extern unsigned long i386_highmem_addr;
extern unsigned long i386_highmem_len;

extern void nxmain (void *arg);

void uos_init (void)
{
	static mem_pool_t pool;
	char *task, *wm;
	int tasksz = 0x10000;
	extern unsigned long _end;

	uos_memory = &pool;
	if (i386_highmem_len)
		mem_init (uos_memory, (size_t) &_end,
			i386_highmem_addr + i386_highmem_len);

	debug_printf ("Free memory: %ld bytes\n", mem_available (uos_memory));
	uos_call_global_initializers ();

	task = (char*) mem_alloc (uos_memory, tasksz);
	wm = (char*) mem_alloc (uos_memory, tasksz);
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

	task_create (Wm_run, 0, "nanowm", 50, wm, tasksz);
}

void __cxa_pure_virtual (void)
{
	debug_printf ("pure virtual method called\n");
	abort ();
}

} /* "C" */

void operator delete (void *ptr)
{
	mem_free (ptr);
}

void operator delete[] (void *ptr)
{
	mem_free (ptr);
}

void *operator new (unsigned size)
{
	return mem_alloc (uos_memory, size);
}

void *operator new[] (unsigned size)
{
	return mem_alloc (uos_memory, size);
}
