/*
 * Проверка драйвера таймера.
 * Вывод на LCD-индикатор отладочной платы Миландр 1986BE91.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>
#include "board-1986be91.h"

ARRAY (task, 1000);
gpanel_t display;
timer_t timer;

extern gpanel_font_t font_bigdigits;

void hello ()
{
	unsigned prev = -1;

	for (;;) {
		/* Ждём очередного значения счётчика секунд. */
		unsigned sec = timer_milliseconds (&timer) / 1000;
		if (sec == prev) {
			mutex_wait (&timer.decisec);
			continue;
		}
		prev = sec;

		/* Печатаем значение времени. */
		unsigned min = sec / 60;
		sec -= min*60;
		min %= 100;
		gpanel_clear (&display, 0);
		gpanel_move (&display, 0, (display.nrow - display.font->height) / 2);
		printf (&display, "%02d:%02d", min, sec);
	}
}

void uos_init (void)
{
	buttons_init ();
	gpanel_init (&display, &font_bigdigits);
	gpanel_clear (&display, 0);

	timer_init (&timer, KHZ, 10);

	task_create (hello, 0, "hello", 1, task, sizeof (task));
}
