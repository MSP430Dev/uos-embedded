#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "watchdog/watchdog.h"
#include "lcd/lcd.h"

uart_t uart;			/* Драйвер асинхронного порта */
timer_t timer;			/* Драйвер таймера */
lcd_t line1, line2;		/* Драйвер индикатора */

char stack_poll [0x200];	/* Задача: опрос по таймеру */
void main_poll (void *data);

const char message[] = "\fКто ходит в гости по утрам, тот поступает мудро! Та-рам-па-рам, па-рам-та-рам, на то оно и утро.";

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

	/* Создаем задачу опроса. Задаем:
	 * имя стартовой функции, ее аргумент, название задачи, приоритет,
	 * массив данных задачи, его размер, семафор окончания. */
	task_create (main_poll, 0, "poll", 1,
		stack_poll, sizeof (stack_poll), 0);
}

void page (unsigned char n)
{
	unsigned char i;

	lcd_clear_all (&line1, &line2);
	n <<= 5;
	for (i=0; i<8; ++i)
		lcd_putchar (&line1, n + i);
	if (n)
		for (i=8; i<16; ++i)
			lcd_putchar (&line1, n + i);
	for (i=16; i<32; ++i)
		lcd_putchar (&line2, n + i);
}

/*
 * Задача периодического опроса.
 * Параметр не используем.
 */
void main_poll (void *data)
{
	unsigned char i, key, oldkey;
	short sym;

	uart_printf (&uart, CONST("\nTesting LCD.\n\n"));

	/* Драйвер LCD-индикатора. */
	lcd_init (&line1, &line2);

	/* Запоминаем начальное состояние кнопочек. */
	key = inb (PIND);

	for (;;) {
		/* Делаем паузу на 30 мсек. */
		timer_delay (&timer, 30);

		/* Опрашиваем новое состояние. */
		if (uart_peekchar (&uart) >= 0) {
			sym = uart_getchar (&uart);
			switch (sym) {
			case ' ':
				uart_printf (&uart, CONST("Clear\n"));
				lcd_clear (&line1);
				break;
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8':
				uart_printf (&uart, CONST("Page %c\n"), sym);
				page (sym - '1');
				break;
			case '9':
				lcd_rprintf (&line1, &timer, message);
				break;
			default:
				uart_printf (&uart, CONST("Putchar `%c'\n"), sym);
				lcd_putchar (&line1, sym);
				break;
			}
		}

		/* Запоминаем старое состояние кнопочек. */
		oldkey = key;

		/* Опрашиваем новое состояние. */
		key = inb (PIND);

		/* Вычисляем и печатаем изменение состояния
		 * для каждой из кнопок. */
		for (i=0; i<8; ++i) {
			unsigned char val, oldval;

			val = key >> i & 1;
			oldval = oldkey >> i & 1;

			if (oldval && ! val) {
				/* По кнопке 0 выдаем фразу,
				 * по остальным - таблицу символов. */
				uart_printf (&uart, CONST("Key %d pressed\n"), i);
				if (i == 0)
					lcd_rprintf (&line1, &timer, message);
				else
					page (i);
			}
		}
	}
}
