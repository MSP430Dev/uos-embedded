#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "watchdog/watchdog.h"
#include "lcd/lcd.h"
#include "motor/motor.h"
#include "irman/irman.h"

uart_t uart;			/* Драйвер асинхронного порта */
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
	uart_init (&uart, 90, KHZ, 9600);

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

void power_print ()
{
	signed short left, right;

	left = motor_get_power (&motor, MOTOR_LEFT);
	right = motor_get_power (&motor, MOTOR_RIGHT);
	lcd_printf (&line2, CONST("\fPower = %d, %d"), left, right);
}

void power_less (unsigned char num)
{
	signed short power;

	power = motor_get_power (&motor, num) - 10;
	if (power < -128)
		power = -128;
	motor_set_power (&motor, num, power);
}

void power_more (unsigned char num)
{
	signed short power;

	power = motor_get_power (&motor, num) + 10;
	if (power > POWER_MAX)
		power = POWER_MAX;
	motor_set_power (&motor, num, power);
}

void power_reverse (unsigned char num)
{
	signed short power;

	power = - motor_get_power (&motor, num);
	if (power > POWER_MAX)
		power = POWER_MAX;
	motor_set_power (&motor, num, power);
}

void power_stop (unsigned char num)
{
	motor_set_power (&motor, num, 0);
}

/*
 * Вычисляем и печатаем изменение состояния
 * для каждой из кнопок.
 */
void do_keys (unsigned char key, unsigned char oldkey)
{
	unsigned char i;

	for (i=0; i<8; ++i) {
		unsigned char val, oldval;

		val = key >> i & 1;
		oldval = oldkey >> i & 1;

		if (oldval && ! val) {
			switch (i) {
			case 7: /* D7 - левый меньше */
				power_less (MOTOR_LEFT);
				power_print ();
				break;
			case 6: /* D6 - левый больше */
				power_more (MOTOR_LEFT);
				power_print ();
				break;
			case 5: /* D5 - левый инверсия */
				power_reverse (MOTOR_LEFT);
				power_print ();
				break;
			case 4: /* D4 - левый стоп */
				power_stop (MOTOR_LEFT);
				power_print ();
				break;
			case 3: /* D3 - правый меньше */
				power_less (MOTOR_RIGHT);
				power_print ();
				break;
			case 2: /* D2 - правый больше */
				power_more (MOTOR_RIGHT);
				power_print ();
				break;
			case 1: /* D1 - правый инверсия */
				power_reverse (MOTOR_RIGHT);
				power_print ();
				break;
			case 0: /* D0 - правый стоп */
				power_stop (MOTOR_RIGHT);
				power_print ();
				break;
			}
		}
	}
}

void do_infrared (unsigned char c)
{
	c = irman_command (c);
	switch (c) {
	case IRCMD_PLAY:
		/* левый, правый максимум */
		motor_set_power(&motor, MOTOR_LEFT, POWER_MAX);
		motor_set_power(&motor, MOTOR_RIGHT, POWER_MAX);
		power_print ();
		break;
	case IRCMD_STOP:
		/* левый, правый -макс */
		motor_set_power(&motor, MOTOR_LEFT, -POWER_MAX);
		motor_set_power(&motor, MOTOR_RIGHT, -POWER_MAX);
		power_print ();
		break;
	case IRCMD_PAUSE:
		/* левый, правый стоп */
		power_stop (MOTOR_LEFT);
		power_stop (MOTOR_RIGHT);
		power_print ();
		break;
	case IRCMD_REW:
		/* левый назад, а правый вперед макс (Налево) */
		motor_set_power(&motor, MOTOR_LEFT, -POWER_MAX);
		motor_set_power(&motor, MOTOR_RIGHT, POWER_MAX);
		power_print ();
		break;
	case IRCMD_FF:
		/* левый вперед, а правый назад макс (Направо) */
		motor_set_power(&motor, MOTOR_LEFT, POWER_MAX);
		motor_set_power(&motor, MOTOR_RIGHT, -POWER_MAX);
		power_print ();
		break;
	}
}

/*
 * Задача периодического опроса.
 * Параметр не используем.
 */
void main_poll (void *data)
{
	unsigned char key, oldkey;
	int c;

	/* Драйвер моторов. */
	motor_init (&motor);

	if (irman_init (&uart, &timer))
		lcd_printf (&line1, CONST("\fIrman OK."));
	else
		lcd_printf (&line1, CONST("\fIrman failed."));

	/* Запоминаем начальное состояние кнопочек. */
	oldkey = inb (PIND);

	for (;;) {
		/* Делаем паузу на 30 мсек. */
		timer_delay (&timer, 30);

		/* Опрашиваем новое состояние кнопочек. */
		key = inb (PIND);
		if (key != oldkey) {
			/* Вычисляем и печатаем изменение состояния
			 * для каждой из кнопок. */
			do_keys (key, oldkey);

			/* Запоминаем старое состояние кнопочек. */
			oldkey = key;
		}

		/* Принимаем команды от инфракрасного пульта. */
		c = uart_peekchar (&uart);
		if (c >= 0)
			do_infrared (uart_getchar (&uart));
	}
}
