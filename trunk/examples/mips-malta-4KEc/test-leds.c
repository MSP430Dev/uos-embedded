/*
 * Testing LEDs.
 */
#include <runtime/lib.h>

int main (void)
{
    int i;
    
    for (;;) {
        for (i = 0; i < 8; ++i) {
            MALTA_LEDBAR = (1 << i);
            mdelay(300);
        }
    }
}
