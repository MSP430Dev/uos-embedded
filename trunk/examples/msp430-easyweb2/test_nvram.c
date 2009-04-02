/*
 * Testing NVRAM.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>
#include <lcd2/lcd.h>
#include "msp430-easyweb2.h"

#define EEPROM_BYTES	(64*1024L)
lcd_t line1, line2;
ARRAY (task, 280);

/*
 * TODO: Implement software I2C interface to 24LC515.
 */

void wait_for_button ()
{
	puts (&line2, "\fPress button B4.");

	/* Wait until button 1 released. */
	while (button1_pressed ())
		continue;

	/* Wait until button 1 pressed. */
	while (! button1_pressed ())
		continue;
}

void test (void *data)
{
	unsigned errors;
	unsigned char c;
	unsigned long i;

	puts (&line1, "\fTesting NVRAM.");
	for (;;) {
		wait_for_button ();
		errors = 0;

		for (i=0; i<EEPROM_BYTES; ++i) {
			if ((i & 63) == 0)
				printf (&line2, "\fWriting %d...", i);
			nvram_write_byte (i, ~i);
		}
		for (i=0; i<EEPROM_BYTES; ++i) {
			if ((i & 1023) == 0)
				printf (&line2, "\fReading %d...", i);
			c = nvram_read_byte (i);
			if (c != (unsigned char) ~i) {
				++errors;
				printf (&line1, "\f%d: w %02X r %02X\n",
					i, (unsigned char) ~i, c);
				wait_for_button ();
				printf (&line2, "\fReading %d", i);
			}
		}
		if (errors)
			printf (&line1, "\fTotal %d errs.", errors);
		else
			printf (&line1, "\f%d bytes OK.", EEPROM_BYTES);
	}
}

void uos_init (void)
{
	lcd_init (&line1, &line2, 0);
	nvram_init ();
	task_create (test, 0, "lcd", 1, task, sizeof (task));
}
