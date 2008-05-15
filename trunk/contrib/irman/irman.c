#include "runtime/lib.h"
#include "kernel/uos.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "irman/irman.h"

unsigned char irman_lastbyte;

unsigned char irman_init (uart_t *uart, timer_t *timer)
{
	int c;
	unsigned char count, try;

	/* Делаем несколько попыток. */
	for (try=10; try>0; --try) {
		/* Выдаем устройству команду 'IR',
		 * с учетом требуемых пауз.
		 * Устройство должно ответить 'OK'. */
		timer_delay (timer, 60);
		uart_putchar (uart, 'I');
		timer_delay (timer, 10);
		uart_putchar (uart, 'R');

		/* Ждем символ 'O'.
		 * Ограничиваем время ожидания. */
		for (count=100; count>0; --count) {
			c = uart_peekchar (uart);
			if (c < 0)
				timer_delay (timer, 10);
			else if (uart_getchar (uart) == 'O')
				break;
		}

		/* Не дождались - делаем новую попытку. */
		if (count == 0)
			continue;

		/* Ждем символ 'K'.
		 * Ограничиваем время ожидания. */
		for (count=100; count>0; --count) {
			c = uart_peekchar (uart);
			if (c < 0)
				timer_delay (timer, 10);
			else if (uart_getchar (uart) == 'K')
				break;
		}

		/* Не дождались - делаем новую попытку. */
		if (count == 0)
			continue;

		/* Получилось. */
		return 1;
	}
	/* Не получилось. */
	return 0;
}

unsigned char irman_command (unsigned char byte)
{

	if (irman_lastbyte == 0xc2)
		switch (byte) {
		case 0x14: case 0x18: case 0x1c: case 0x24:
		case 0x30: case 0x33: case 0x44: case 0x60:
		case 0x64: case 0x6c: case 0x7c: case 0x84:
		case 0x94: case 0x98: case 0xa4: case 0xb0:
		case 0xc0: case 0xc4: case 0xc8: case 0xcc:
		case 0xd0: case 0xe0: case 0xe4:
			irman_lastbyte = 0;
			return byte;
		}
	else if (irman_lastbyte == 0xd6)
		switch (byte) {
		case 0x12: case 0x15: case 0x22: case 0x25:
		case 0x2a: case 0x2d: case 0x35: case 0x45:
		case 0x4d: case 0x52: case 0x6d: case 0x75:
		case 0x88: case 0x8a: case 0x92: case 0xb2:
		case 0xba: case 0xbd: case 0xd2: case 0xe2:
		case 0xea: case 0xed: case 0xfd:
			irman_lastbyte = 0;
			return byte;
		}
	irman_lastbyte = byte;
	return 0;
}
