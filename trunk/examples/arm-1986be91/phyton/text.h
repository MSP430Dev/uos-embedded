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
 *  Файл text.h: Вывод символов и текста на экран
 *
 *============================================================================================*/

#ifndef __TEXT_H
#define __TEXT_H

#include "font_defs.h"

/* Стили */
typedef enum {
    StyleSimple,
    StyleBlink,
    StyleFlipFlop,
    StyleVibratory
} TextStyle;

/* Выбранный шрифт для отрисовки текста */
extern FONT *CurrentFont;

/* Вывод байта на экран */
void LCD_PUT_BYTE(unsigned x, unsigned y, unsigned data);
/* Вывод символов и строк текущим шрифтом */
void LCD_PUTC(unsigned x, unsigned y, unsigned ch);
void LCD_PUTS(unsigned x, unsigned y, const char *str);
void LCD_PUTS_Ex(unsigned x, unsigned y, const char *str, unsigned style);

#endif /*__TEXT_H*/

/*============================================================================================
 * Конец файла text.h
 *============================================================================================*/
