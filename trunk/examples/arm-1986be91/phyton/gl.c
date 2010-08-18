/*============================================================================================
 *
 *  (C) 2010, Phyton
 *
 *  Демонстрационный проект для 1986BE91 testboard
 *
 *  Данное ПО предоставляется "КАК ЕСТЬ", т.е. исключительно как пример, призванный облегчить
 *  пользователям разработку их приложений для процессоров Milandr 1986BE91T1. Компания Phyton
 *  не несет никакой ответственности за возможные последствия использования данного, или
 *  разработанного пользователем на его основе, ПО.
 *
 *--------------------------------------------------------------------------------------------
 *
 *  Файл gl.c: Вывод графических примитивов на LCD-экран
 *
 *============================================================================================*/

#include "lcd.h"
#include "gl.h"
#include "joystick.h"

void LCD_PutPixel(unsigned x, unsigned y) {
    unsigned shifted, page;
    unsigned char data;

    if ((x>MAX_X)|(y>MAX_Y)) return;

    SetCrystal((LCD_Crystal)(x/64));
    x %= 64;

    page = y/8;
    shifted = (1 << (y%8));
    LCD_SET_PAGE(page);
    LCD_SET_ADDRESS(x);
    data = ReadLCD_Data();
    switch (CurrentMethod){
    case MET_OR:    data |= shifted; break;
    case MET_XOR:   data ^= shifted; break;
    default:        break;
    }
    LCD_SET_ADDRESS(x);
    WriteLCD_Data(data);
}

void LCD_Line(unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
    int dx, dy, i1, i2, i, kx, ky;
    int d;
    int x, y;
    int flag;

    dy = y2 - y1;
    dx = x2 - x1;
    if (dx == 0 && dy == 0) {
        LCD_PutPixel(x1, y1);
        return;
    }

    flag = kx = ky = 1;

    if (dx < 0) {
        dx = -dx;
        kx = -1;
    }
    else if (dx == 0)
        kx = 0;

    if (dy < 0) {
        dy = -dy;
        ky = -1;
    }

    if (dx < dy) {
        flag = 0;
        d = dx;
        dx = dy;
        dy = d;
    }

    i1 = dy + dy;
    d = i1 - dx;
    i2 = d - dx;
    x = x1;
    y = y1;

    for (i=0; ; i++){
        LCD_PutPixel(x,y);
        if (i >= dx)
            break;

        if (flag)
            x += kx;
        else
            y += ky;

        if (d < 0)
            d += i1;
        else {
            d += i2;
            if (flag)
                y += ky;
            else
                x += kx;
        }
    }
}

void LCD_Circle(unsigned xc, unsigned yc, unsigned r) {
    int d, x, y;

    for (x = 0, y = r, d = 3-2*r; ;) {
        LCD_PutPixel(x+xc, y+yc);
        LCD_PutPixel(x+xc, -y+yc);
        LCD_PutPixel(-x+xc, -y+yc);
        LCD_PutPixel(-x+xc, y+yc);
        LCD_PutPixel(y+xc, x+yc);
        LCD_PutPixel(y+xc, -x+yc);
        LCD_PutPixel(-y+xc, -x+yc);
        LCD_PutPixel(-y+xc, x+yc);

        if (x >= y)
            break;

        if (d <= 0)
            d += 4*x+6;
        else {
            d += 4*(x-y)+10;
            y--;
        }
        x++;
    }
}

/*============================================================================================
 * Конец файла gl.c
 *============================================================================================*/
