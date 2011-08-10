/*
 * Testing SPI slave.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <milandr/spi.h>
#include "board-1986be91.h"

gpanel_t display;
spi_t spi;

ARRAY (stack_console, 1000);

/*
 * Отображение состояния теста на LCD.
 */
void display_refresh (unsigned sent, unsigned received)
{
	gpanel_clear (&display, 0);
	printf (&display, " SPI slave: 5600ВГ1У\r\n");
	printf (&display, " Процессор: %d.%d МГц\r\n", KHZ/1000, KHZ/100%10);
	printf (&display, "  Кбит/сек: %d\r\n", spi.kbps);
	printf (&display, "  Передано: %lu\r\n", spi.out_packets);
	printf (&display, "   Принято: %lu\r\n", spi.in_packets);
	printf (&display, "Прерываний: %lu\r\n", spi.interrupts);
	if (sent != ~0) {
/*
		printf (&display, "Отправлено: '%c'\r\n", sent);
		if (received != ~0)
			printf (&display, "   Обратно: '%c'\r\n", received);
		else
			printf (&display, "   Обратно: ---\r\n");
*/
		printf (&display, "Отправлено: %04x\r\n", sent);
		if (received != ~0)
			printf (&display, "   Обратно: %04x\r\n", received);
		else
			printf (&display, "   Обратно: ---\r\n");
	}
}

/*
 * Задача принимает слово по SPI и отправляет его обратно
 * в следующей транзакции.
 */
void task_console (void *data)
{
	unsigned sent = '?';
	unsigned received;
	for (;;) {
		spi_output (&spi, sent);
		spi_input_wait (&spi, &received);
		display_refresh (sent, received);
		sent = received;
	}
}

void uos_init (void)
{
	buttons_init ();

	/* Use LCD panel for display. */
	extern gpanel_font_t font_fixed6x8;
	gpanel_init (&display, &font_fixed6x8);

	/* SPI2, 16-bit words, slave. */
	spi_init (&spi, 1, 16, 0, 0);
	display_refresh (~0, ~0);

	task_create (task_console, 0, "console", 10,
		stack_console, sizeof (stack_console));
}
