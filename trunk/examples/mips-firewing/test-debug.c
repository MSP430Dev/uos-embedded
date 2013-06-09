/*
 * Testing debug output.
 */
#include <runtime/lib.h>

int main (void)
{
    /* Unlock CFGCON register. */
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCON &= (1 << 13);            // clear IOLOCK

    /* Remap peripheral pins. */
    RPB4R = 1;                      // U1TX to RB4 - D0/PTX
    U2RXR = 1;                      // U2RX to RB5 - D1/PRX

    debug_puts ("--- Test started ---\n");
    for (;;) {
        debug_puts ("Hello, World!\n");
        debug_getchar();
    }
}
