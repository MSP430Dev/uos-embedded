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
 *  Файл text.c: Вывод символов и текста на экран
 *
 *============================================================================================*/

#include <runtime/lib.h>

#include "lcd.h"
#include "text.h"
#include "joystick.h"
#include "systick.h"
#include "menu.h"

/* Выбранный шрифт для отрисовки текста */
FONT *CurrentFont;

/* Вывод байта на экран */
void LCD_PUT_BYTE(unsigned char x, unsigned char y, unsigned char data) {
    unsigned tmp_data, page, shift_num, shift_num_ex;

    if ((x>MAX_X)|(y>MAX_Y)) return;

    /* Выбор кристалла и смещение по х */
    SetCrystal((LCD_Crystal)(x/64));
    x %= 64;
    /* Определяем первую страницу и смещение по y */
    page = y/8;
    shift_num = y%8;
    shift_num_ex = 8 - shift_num;

    /* Первая страница */

    /* Читаем текущее значение*/
    LCD_SET_PAGE(page);
    LCD_SET_ADDRESS(x);
    tmp_data = ReadLCD_Data();
    /* Записываем модифицированное значение*/
    LCD_SET_PAGE(page);
    LCD_SET_ADDRESS(x);
    switch (CurrentMethod){
        case MET_OR:
                WriteLCD_Data(tmp_data | (data << shift_num));
                break;
        case MET_XOR:
                WriteLCD_Data(tmp_data ^ (data << shift_num));
                break;
        case MET_NOT_OR:
                WriteLCD_Data(tmp_data | ((data ^ 0xFF) << shift_num));
                break;
        case MET_NOT_XOR:
                WriteLCD_Data(tmp_data ^ ((data ^ 0xFF) << shift_num));
                break;
        case MET_AND:
                WriteLCD_Data((tmp_data & (0xFF >> shift_num_ex)) | (data << shift_num));
                break;
    }

    /* Вторая страница (если есть) */
    if (shift_num > 0) {
        /* Читаем текущее значение*/
        LCD_SET_PAGE(page+1);
        LCD_SET_ADDRESS(x);
        tmp_data = ReadLCD_Data();
        /* Записываем модифицированное значение*/
        LCD_SET_PAGE(page+1);
        LCD_SET_ADDRESS(x);
        switch(CurrentMethod){
            case MET_OR:
                WriteLCD_Data(tmp_data | (data >> shift_num_ex));
                break;
            case MET_XOR:
                WriteLCD_Data(tmp_data ^ (data >> shift_num_ex));
                break;
            case MET_NOT_OR:
                WriteLCD_Data(tmp_data | ((data ^ 0xFF) >> shift_num_ex));
                break;
            case MET_NOT_XOR:
                WriteLCD_Data(tmp_data ^ ((data ^ 0xFF)>> shift_num_ex));
                break;
            case MET_AND:
                WriteLCD_Data((tmp_data & (0xFF << shift_num)) | (data >> shift_num_ex));
                break;
        }
    }
}

/* Вывод символов и строк текущим шрифтом */

void LCD_PUTC(unsigned char x, unsigned char y, unsigned char ch) {
    unsigned i, j, line;
    const unsigned char *sym;

    sym = Get_Char_Data_Addr(ch);
    line = CurrentFont->Height / 8;
    if (CurrentFont->Height % 8)
        line++;

    for (j = 0; j < line; j++)
        for( i = 0; i < CurrentFont->Width; i++)
            LCD_PUT_BYTE(x + i, y + j*8, sym[i + CurrentFont->Width*j]);
}


void LCD_PUTS(unsigned char x, unsigned char y, const char* str) {
    unsigned i;
    for (i=0; str[i]; i++)
        LCD_PUTC(x + i*CurrentFont->Width, y, str[i]);
}


void LCD_PUTS_Ex(unsigned char x, unsigned char y, const char* str, unsigned char style) {
    unsigned i;
    LCD_Method OldMethod = CurrentMethod;

    switch (style) {
        /* Простая строка */
        case StyleSimple:
            CurrentMethod = MET_AND;
            LCD_PUTS(x, y, str);
            break;
        /* Мерцающая строка */
        case StyleBlink:
            CurrentMethod = MET_AND;
            LCD_PUTS(x, y, str);
            udelay(600);
            for (i = 0; i < strlen((unsigned char*)str); i++)
                LCD_PUTC(x + ((CurrentFont->Width) * i), y, 0x20);
            break;
        /* Строка с изменением фона */
        case StyleFlipFlop:
            CurrentMethod = MET_AND;
            LCD_PUTS(x, y, str);
            CurrentMethod = MET_XOR;
            LCD_PUTS(x, y, str);
            CurrentMethod = MET_NOT_XOR;
            LCD_PUTS(x, y, str);
            udelay(1200);
            CurrentMethod = MET_AND;
            LCD_PUTS(x, y, str);
            udelay(150);
            break;
        /* Дрожащая строка */
        case StyleVibratory:
            CurrentMethod = MET_AND;
            LCD_PUTS(x, y, str);
            udelay(600);
            LCD_PUTS(x+1, y+1, str);
            break;
    }
    CurrentMethod = OldMethod;
}

/*============================================================================================
 * Конец файла text.c
 *============================================================================================*/
