/*
 * Testing LEDs.
 */
#include <runtime/lib.h>
#include "devcfg.h"

#define MASKB_LED	(1 << 14)   /* RB14: yellow */

int main (void)
{
    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;

    LATBCLR = MASKB_LED;
    TRISBCLR = MASKB_LED;

    for (;;) {
        LATBINV = MASKB_LED; mdelay (100);

        LATBINV = MASKB_LED; mdelay (100);
    }
}
