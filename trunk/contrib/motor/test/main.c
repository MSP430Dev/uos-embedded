#include "runtime/lib.h"
#include "kernel/uos.h"
/*#include "uart/uart.h"*/
#include "timer/timer.h"
#include "watchdog/watchdog.h"
#include "lcd/lcd.h"
#include "motor/motor.h"

/*uart_t uart;			/ * Драйвер асинхронного порта*/
timer_t timer;			/* Драйвер таймера */
lcd_t line1, line2;		/* Драйвер индикатора */
motor_t motor;			/* Драйвер моторов */

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
/*	uart_init (&uart, 90, KHZ, 9600);*/

	/* Драйвер таймера.
	 * Задаем приоритет, частоту процессора,
	 * количество миллисекунд между тиками таймера. */
	timer_init (&timer, 100, KHZ, 10);

	/* Драйвер LCD-индикатора. */
	lcd_init (&line1, &line2);

	/* Создаем задачу опроса. Задаем:
	 * имя стартовой функции, ее аргумент, название задачи, приоритет,
	 * массив данных задачи, его размер, семафор окончания. */
	task_create (main_poll, 0, "poll", 1,
		stack_poll, sizeof (stack_poll), 0);
}

void power_less (unsigned char num)
{
	signed short power;

	power = motor_get_power (&motor, num) - 10;
	if (power < -128)
		power = -128;
	motor_set_power (&motor, num, power);
	lcd_printf (&line2, CONST("\f%S = %d"),
		num == MOTOR_LEFT ? CONST("Left") : CONST("Right"), power);
}

void power_more (unsigned char num)
{
	signed short power;

	power = motor_get_power (&motor, num) + 10;
	if (power > 127)
		power = 127;
	motor_set_power (&motor, num, power);
	lcd_printf (&line2, CONST("\f%S = %d"),
		num == MOTOR_LEFT ? CONST("Left") : CONST("Right"), power);
}

void power_reverse (unsigned char num)
{
	signed short power;

	power = - motor_get_power (&motor, num);
	if (power > 127)
		power = 127;
	motor_set_power (&motor, num, power);
	lcd_printf (&line2, CONST("\f%S = %d"),
		num == MOTOR_LEFT ? CONST("Left") : CONST("Right"), power);
}

void power_stop (unsigned char num)
{
	motor_set_power (&motor, num, 0);
	lcd_printf (&line2, CONST("\f%S = stop"),
		num == MOTOR_LEFT ? CONST("Left") : CONST("Right"));
}

/*
 * Задача периодического опроса.
 * Параметр не используем.
 */
void main_poll (void *data)
{
	unsigned char i, key, oldkey;

	lcd_printf (&line1, CONST("\fTesting motors."));

	/* Драйвер моторов. */
	motor_init (&motor);

	/* Запоминаем начальное состояние кнопочек. */
	key = inb (PIND);

	for (;;) {
		/* Делаем паузу на 30 мсек. */
		timer_delay (&timer, 30);

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
				switch (i) {
				case 7: /* D7 - левый меньше */
					power_less (MOTOR_LEFT);
					break;
				case 6: /* D6 - левый больше */
					power_more (MOTOR_LEFT);
					break;
				case 5: /* D5 - левый инверсия */
					power_reverse (MOTOR_LEFT);
					break;
				case 4: /* D4 - левый стоп */
					power_stop (MOTOR_LEFT);
					break;
				case 3: /* D3 - правый меньше */
					power_less (MOTOR_RIGHT);
					break;
				case 2: /* D2 - правый больше */
					power_more (MOTOR_RIGHT);
					break;
				case 1: /* D1 - правый инверсия */
					power_reverse (MOTOR_RIGHT);
					break;
				case 0: /* D0 - правый стоп */
					power_stop (MOTOR_RIGHT);
					break;
				}
			}
		}
	}
}
