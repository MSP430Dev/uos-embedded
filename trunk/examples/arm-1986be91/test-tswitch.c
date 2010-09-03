/*
 * Измерение времени переключения задач.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>

ARRAY (task_console, 1000);
ARRAY (task_receiver, 1000);
gpanel_t display;
mutex_t mailbox;

volatile unsigned nmessages;
volatile unsigned latency_min = ~0;
volatile unsigned latency_max;

/*
 * Печать рационального числа a/b с двумя знаками после запятой.
 */
void print_rational (char *title, unsigned a, unsigned b)
{
	unsigned val = a * 100 / b;

	printf (&display, "%s%u.%02u usec\n", title, val/100, val%100);
}

/*
 * Задача выдачи статистики на консоль.
 */
void console (void *arg)
{
	unsigned t0;

	for (;;) {
		t0 = ARM_SYSTICK->VAL;
		mutex_signal (&mailbox, (void*) t0);

		gpanel_move (&display, 0, 0);
		printf (&display, "CPU: %u MHz\n\n", KHZ / 1000);

		printf (&display, "Task switches: %u\n\n", nmessages);

		if (nmessages > 100) {
			printf (&display, "Task switch time:\n");
			print_rational ("  min: ", latency_min * 1000, KHZ);
			print_rational ("  max: ", latency_max * 1000, KHZ);
		}
	}
}

/*
 * Задача приёма сообщений.
 */
void receiver (void *arg)
{
	unsigned t0, t1, latency;

	for (;;) {
		t0 = (unsigned) mutex_wait (&mailbox);
		t1 = ARM_SYSTICK->VAL;

		/* Вычисляем количество тактов, затраченных на вход в прерывание. */
		latency = t0 - t1;

		/*debug_printf ("<%u> ", latency);*/
		if (++nmessages > 10) {
			if (latency_min > latency)
				latency_min = latency;
			if (latency_max < latency)
				latency_max = latency;
		}
	}
}

void uos_init (void)
{
	extern gpanel_font_t font_fixed6x8;

	/* Стираем экран. */
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);

	/* Запускаем задачу выдачи статистики на консоль. */
	task_create (console, 0, "console", 1, task_console, sizeof (task_console));

	/* Запускаем задачу приёма сообщений. */
	task_create (receiver, 0, "rcv", 2, task_receiver, sizeof (task_receiver));
}
