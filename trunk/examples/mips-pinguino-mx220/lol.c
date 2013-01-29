/*
 * Basic LoL Shield Test.
 *
 * Writen for the LoL Shield, designed by Jimmie Rodgers:
 * http://jimmieprodgers.com/kits/lolshield/
 *
 * Copyright (C) 2013 Serge Vakulenko, <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#include <runtime/lib.h>
#include "devcfg.h"

/*
 * Display a picture on LoL shield.
 * Duration in milliseconds is specified.
 */
void lol (msec, data)
    unsigned msec;
    const short *data;
{
    /* Number of control pins for LoL Shield. */
    #define LOL_NPINS   12

    /* Number of rows on LoL Shield. */
    #define LOL_NROW    9

    /* Number of columns on LoL Shield. */
    #define LOL_NCOL    14

    /* Sequence of pins to set high during refresh cycle. */
    static const unsigned high [LOL_NPINS] = {
        1 << 15,        /* RB15 - labeled D13 on board */
        1 << 13,        /* RB13 - D12 */
        1 << 5,         /* RB5  - D11 */
        1 << 1,         /* RA1  - D10 */
        1 << 10,        /* RA10 - D9 */
        1 << 7,         /* RB7  - D8 */
        0x10000 << 7,   /* RC7  - D7 */
        0x10000 << 6,   /* RC6  - D6 */
        0x10000 << 5,   /* RC5  - D5 */
        0x10000 << 4,   /* RC4  - D4 */
        0x10000 << 3,   /* RC3  - D3 */
        0x10000 << 2,   /* RC2  - D2 */
    };

    /* Remap pixels to pin indexes. */
    static const unsigned char lol_map [LOL_NROW*LOL_NCOL*2] =
    {
        0,8,0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,9,9,0,0,10,10,0,0,11,11,0,
        1,8,1,7,1,6,1,5,1,4,1,3,1,2,1,0,1,9,9,1,1,10,10,1,1,11,11,1,
        2,8,2,7,2,6,2,5,2,4,2,3,2,1,2,0,2,9,9,2,2,10,10,2,2,11,11,2,
        3,8,3,7,3,6,3,5,3,4,3,2,3,1,3,0,3,9,9,3,3,10,10,3,3,11,11,3,
        4,8,4,7,4,6,4,5,4,3,4,2,4,1,4,0,4,9,9,4,4,10,10,4,4,11,11,4,
        5,8,5,7,5,6,5,4,5,3,5,2,5,1,5,0,5,9,9,5,5,10,10,5,5,11,11,5,
        6,8,6,7,6,5,6,4,6,3,6,2,6,1,6,0,6,9,9,6,6,10,10,6,6,11,11,6,
        7,8,7,6,7,5,7,4,7,3,7,2,7,1,7,0,7,9,9,7,7,10,10,7,7,11,11,7,
        8,7,8,6,8,5,8,4,8,3,8,2,8,1,8,0,8,9,9,8,8,10,10,8,8,11,11,8,
    };

    unsigned row, mask, amask, bmask, cmask;
    const unsigned char *map;
    unsigned low [LOL_NPINS];

    /* Clear pin masks. */
    for (row = 0; row < LOL_NPINS; row++)
        low [row] = 0;

    /* Convert image to array of pin masks. */
    for (row = 0; row < LOL_NROW; row++) {
        mask = *data++ & ((1 << LOL_NCOL) - 1);
        map = &lol_map [row * LOL_NCOL * 2];
        while (mask != 0) {
            if (mask & 1) {
                low [map[0]] |= high [map[1]];
            }
            map += 2;
            mask >>= 1;
        }
    }
    amask = high[3] | high[4];
    bmask = high[0] | high[1] | high[2] | high[5];
    cmask = (high[6] | high[7] | high[8] | high[9] | high[10] |
             high[11]) >> 16;

    /* Display the image. */
    if (msec < 1)
        msec = 20;
    while (msec-- > 0) {
        for (row = 0; row < LOL_NPINS; row++) {
            /* Set all pins to tristate. */
            TRISASET = amask;
            TRISBSET = bmask;
            TRISCSET = cmask;

            /* Set one pin to high. */
            mask = high [row];
            if (row >= 6) {
                mask >>= 16;
                TRISCCLR = mask;
                LATCSET = mask;
            } else if (mask & amask) {
                TRISACLR = mask;
                LATASET = mask;
            } else {
                TRISBCLR = mask;
                LATBSET = mask;
            }

            /* Set other pins to low. */
            mask = low [row];
            TRISACLR = mask & amask;
            LATACLR = mask & amask;
            TRISBCLR = mask & bmask;
            LATBCLR = mask & bmask;
            mask >>= 16;
            TRISCCLR = mask;
            LATCCLR = mask;

            /* Pause to make it visible. */
            udelay (1000 / LOL_NPINS);
        }
    }

    /* Turn display off. */
    TRISASET = amask;
    TRISBSET = bmask;
    TRISCSET = cmask;
}

/*
 * Demo 1:
 * 1) vertical lines;
 * 2) horizontal lines;
 * 3) all LEDs on.
 */
void demo1()
{
    static unsigned short picture[9];
    int y, frame;

    for (;;) {
        for (frame = 0; frame<14; frame++) {
            memset (picture, 0, sizeof(picture));

            for (y=0; y<9; y++)
                picture[y] |= 1 << frame;

            /* Display a frame. */
            lol (100, picture);
        }
        for (frame = 0; frame<9; frame++) {
            memset (picture, 0, sizeof(picture));

            picture[frame] = (1 << 14) - 1;

            /* Display a frame. */
            lol (100, picture);
        }
        memset (picture, 0xFF, sizeof(picture));

        /* Display a frame. */
        lol (250, picture);
        lol (250, picture);
    }
}

/*
 * Demo 1: bouncing ball.
 */
void demo2()
{
    static unsigned short picture[9];
    int x, y, dx, dy;

    memset (picture, 0, sizeof(picture));
    x = 0;
    y = 0;
    dx = 1;
    dy = 1;
    for (;;) {
        /* Draw ball. */
        picture[y] |= 1 << x;
        lol (50, picture);
        picture[y] &= ~(1 << x);

        /* Move the ball. */
        x += dx;
        if (x < 0 || x >= 14) {
            dx = -dx;
            x += dx + dx;
        }
        y += dy;
        if (y < 0 || y >= 9) {
            dy = -dy;
            y += dy + dy;
        }
    }
}

int main()
{
    /* Disable JTAG and Trace ports, to make more pins available. */
    DDPCONCLR = 3 << 2;

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

    for (;;) {
        //demo1();
        demo2();
    }
}
