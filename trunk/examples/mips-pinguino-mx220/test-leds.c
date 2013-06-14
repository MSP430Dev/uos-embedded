/*
 * Testing LEDs.
 */
#include <runtime/lib.h>
#include "devcfg.h"

#define MASKB_LED1	(1 << 15)   /* RB15: green */
#define MASKA_LED2	(1 << 10)   /* RA10: red */

#define MASKC_BACKLIGHT (1 << 7)    /* signal D7, pin RC7 */

int main (void)
{
        /* Unlock CFGCON register. */
        SYSKEY = 0;
        SYSKEY = 0xAA996655;
        SYSKEY = 0x556699AA;
        CFGCON &= (1 << 13);            // clear IOLOCK

        /* Disable JTAG ports, to make more pins available. */
        CFGCON &= (1 << 3);             // clear JTAGEN

        /* Use all ports as digital. */
        ANSELA = 0;
        ANSELB = 0;
        ANSELC = 0;

	LATBCLR = MASKB_LED1;
	TRISBCLR = MASKB_LED1;

	LATACLR = MASKA_LED2;
	TRISACLR = MASKA_LED2;

	LATCCLR = MASKC_BACKLIGHT;
	TRISCCLR = MASKC_BACKLIGHT;
	for (;;) {
		LATBINV = MASKB_LED1; mdelay (100);
		LATAINV = MASKA_LED2; mdelay (100);
		LATCINV = MASKC_BACKLIGHT; mdelay (100);

		LATBINV = MASKB_LED1; mdelay (100);
		LATAINV = MASKA_LED2; mdelay (100);
		LATCINV = MASKC_BACKLIGHT; mdelay (100);
	}
}
