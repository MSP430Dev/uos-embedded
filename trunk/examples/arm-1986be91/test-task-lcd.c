/*
 * Проверка формирования задачи пользователя.
 * Вывод на LCD-индикатор отладочной платы Миландр 1986BE91.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include "board-1986be91.h"

ARRAY (task, 1000);
gpanel_t display;

extern gpanel_font_t font_fixed6x8;

void hello (void *arg)
{
	unsigned counter = 0;

	for (;;) {
		printf (&display, "%s #%d\n", arg, ++counter);
		printf (&display, "Task space %d bytes\n",
			sizeof (task));
		printf (&display, "Free %d bytes\n",
			task_stack_avail ((task_t*) task));
		printf (&display, "(Press DOWN)\n");
//asm volatile (".inst.w 0xe81c0c20");

		/* Ждём отпускания кнопки DOWN. */
		while (joystick_down ())
			mdelay (20);

		/* Ждём нажатия кнопки DOWN. */
		while (! joystick_down ())
			mdelay (20);

		gpanel_clear (&display, 0);
		puts (&display, "Button pressed.\r\n\n");
	}
}

void uos_init (void)
{
	buttons_init ();
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	puts (&display, "Testing task.\r\n\n");

	task_create (hello, "Hello!", "hello", 1, task, sizeof (task));
}
