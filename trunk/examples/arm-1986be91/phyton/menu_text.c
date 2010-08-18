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
 *  Файл Menu_text.c: Функции-обработчики меню - вывод текста
 *
 *============================================================================================*/

#include "lcd.h"
#include "menu.h"
#include "menu_items.h"
#include "text.h"
#include "joystick.h"

void FontFunc(void) {
    FONT *OldFont = CurrentFont;

    /* Выводим заголовок и ждем key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("Font Examples");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* Примеры шрифтов */
    LCD_PUTS(0, 12, "Font6X8");

    CurrentFont = &Font_12x16;
    LCD_PUTS(0, 22, "Font12X16");

    CurrentFont = &Font_7x10_thin;
    LCD_PUTS(0, 40, "Font7X10");

    CurrentFont = &Font_7x10_bold;
    LCD_PUTS(0, 52, "Font7X10 bold");

    /* Ждем нажатия SEL и возвращаемся в главное меню */
    CurrentFont = OldFont;
    BackToMenuOnSel();
}


void StyleFunc(void) {
    FONT *OldFont = CurrentFont;

    /* Выводим заголовок и ждем key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("Style Examples");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* Примеры стилей */
    do {
        LCD_PUTS_Ex(0, 12, "Blinking", StyleBlink);
        if (KEY_PRESSED(SEL)) break;
        LCD_PUTS_Ex(0, 32, "Flip-Flop", StyleFlipFlop);
        if (KEY_PRESSED(SEL)) break;
        LCD_PUTS_Ex(0, 52, "Vibrating", StyleVibratory);
    } while (!KEY_PRESSED(SEL));

    /* Нажата SEL - возвращаемся в главное меню */
    CurrentFont = OldFont;
    DisplayMenu();
}


void ShiftFunc(void) {
    FONT *OldFont = CurrentFont;

    /* Выводим заголовок и ждем key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("Running message");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* TODO - бегущая строка */
    do {

    } while (!KEY_PRESSED(SEL));

    /* Нажата SEL - возвращаемся в главное меню */
    CurrentFont = OldFont;
    DisplayMenu();
}

/* Текст для демонстрации "электронной книги" */
static const char *Book[16] = {
    "The Cortex-M3 proces-",
    "sor specifically tar-",
    "gets the low cost re-",
    "quirements of a broad",
    "range of markets and ",
    "applications, where  ",
    "memory and processor ",
    "size significantly   ",
    "impact device costs. ",
    "With multiple techno-",
    "logies to reduce me- ",
    "mory use, including  ",
    "the Thumb-2 instruc- ",
    "tion set, the proces-",
    "sor delivers an ideal",
    "32-bit platform.     ",
};

void BookFunc(void){
    unsigned top_ind, key, i;

    /* Очищаем экран и ждем key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* Цикл обработки нажатий кнопок */
    for (top_ind = 0, key = NOKEY; key != SEL; ) {
        /* Выводим текст и ждем нажатия кнопки */
        for (i = 0; i < 8; i++)
            LCD_PUTS(0, (CurrentFont->Height) * i, Book[top_ind + i]);
        WAIT_UNTIL_ANY_KEY;

        /* Обработка нажатия кнопки */
        key = GetKey();
        switch (key) {
            /* Скроллирование вверх */
            case UP:
                if (top_ind > 0)
                    top_ind--;
                break;
            /* Скроллирование вниз */
            case DOWN:
                if (top_ind < 8)
                    top_ind++;
                break;
        }
    }

    /* Нажата SEL - возвращаемся в главное меню */
    WAIT_UNTIL_KEY_RELEASED(key);
    DisplayMenu();
}

void AboutFunc(void) {
    /* Выводим текст About */
    LCD_CLS();
    CurrentMethod = MET_AND;

    LCD_PUTS(0, 0, " Milandr 1986BE91T1 ");
    LCD_PUTS(0,  CurrentFont->Height      + 1, "   Testing-board    ");
    LCD_PUTS(0, (CurrentFont->Height) * 2 + 2, "  and uOS runtime   ");
    LCD_PUTS(0, (CurrentFont->Height) * 3 + 2, "                    ");
    LCD_PUTS(0, (CurrentFont->Height) * 4 + 3, "Appl. example v.1.0 ");
    LCD_PUTS(0, (CurrentFont->Height) * 5 + 4, "    Phyton 2010     ");
    LCD_PUTS(0, (CurrentFont->Height) * 6 + 5, " http://uos.vak.ru  ");

    /* Ждем нажатия SEL и возвращаемся в главное меню */
    BackToMenuOnSel();
}

/*============================================================================================
 * Конец файла Menu_text.c
 *============================================================================================*/
