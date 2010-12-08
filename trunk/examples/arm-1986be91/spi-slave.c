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

/*
 * Redirect debug output.
 */
void gpanel_putchar (void *arg, short c)
{
	putchar ((stream_t*) arg, c);
}

void uos_init (void)
{
	buttons_init ();

	/* Use LCD panel for debug output. */
	extern gpanel_font_t font_fixed6x8;
	gpanel_init (&display, &font_fixed6x8);
	gpanel_clear (&display, 0);
	debug_redirect (gpanel_putchar, &display);
	printf (&debug, "Testing SPI slave.\n");

	/* SPI2 slave, 16-bit words, at 1 Mbit/sec. */
	spi_init (&spi, 1, 0, 16, 1000);

	/* Poll buttons. */
	unsigned word = '?';
	for (;;) {
		spi_output (&spi, word);
		spi_input_wait (&spi, &word);
		printf (&debug, "'%c'\n", word);
	}
}
