/*#include "stdlib.h"*/
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "watchdog/watchdog.h"

uart_t uart;			/* Драйвер асинхронного порта */

timer_t timer;			/* Драйвер таймера */

char stack_poll [0x200];	/* Задача: опрос по таймеру */
void main_poll (void *data);

/*
 * Выполнение системы начинается с этой функции.
 * Инициализируем аппаратуру, драйверы, создаем задачи.
 */
void uos_init (void)
{
	/* Драйвер асинхронного порта.
	 * Задаем приоритет, частоту процессора, скорость
	 * передачи данных (бит/сек). */
	uart_init (&uart, 90, KHZ, 9600);

	/* Драйвер таймера.
	 * Задаем приоритет, частоту процессора,
	 * количество миллисекунд между тиками таймера. */
	timer_init (&timer, 100, KHZ, 10);

	/* Порт B переключаем на выход - светодиоды. */
	outb (0xff, DDRB);

	/* Включаем сторожевой таймер на 2 секунды. */
	watchdog_enable (7);

	/* Создаем задачу опроса. Задаем:
	 * имя стартовой функции, ее аргумент, название задачи, приоритет,
	 * массив данных задачи, его размер, семафор окончания. */
	task_create (main_poll, 0, "poll", 1,
		stack_poll, sizeof (stack_poll), 0);
}

/*
 * Задача периодического опроса.
 * Параметр не используем.
 */
void main_poll (void *data)
{
	unsigned char i, key, oldkey;

	uart_printf (&uart, "\nTesting keys and LEDs.\n\n");

	/* Запоминаем начальное состояние кнопочек. */
	key = inb (PIND);

	for (;;) {
		/* Делаем паузу на 30 мсек. */
		timer_delay (&timer, 30);

		/* Говорим сторожевому таймеру, что мы живы.
		 * Если этого не делать, через 2 секунды произойдет
		 * аппаратный сброс процессора (reset). */
		watchdog_alive ();

		/* Запоминаем старое состояние кнопочек. */
		oldkey = key;

		/* Опрашиваем новое состояние. */
		key = inb (PIND);

		/* Зажигаем лампочки в соответствии с кнопочками. */
		outb (key, PORTB);

		/* Вычисляем и печатаем изменение состояния
		 * для каждой из кнопок. */
		for (i=0; i<8; ++i) {
			unsigned char val, oldval;

			val = key >> i & 1;
			oldval = oldkey >> i & 1;

			if (val == oldval)
				continue;

			if (val == 0)
				uart_printf (&uart, "Key %d pressed\n", i);
			else
				uart_printf (&uart, "Key %d released\n", i);
		}
	}
}
