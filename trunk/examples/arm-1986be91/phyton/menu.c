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
 *  Файл Menu.c: Иерархическое текстовое меню
 *
 *============================================================================================*/

#include <runtime/lib.h>

#include "menu.h"
#include "menu_items.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"

/* Прототипы служебных функций и обработчиков */
static void DisplayMenuItemString(unsigned y, const char *ptr);
static void IdleFunc(void);
static void SelFunc(void);
static void UpFunc(void);
static void DownFunc(void);
static void ReturnFunc(void);

/* Описание меню */

typedef void (* tMenuFunc)(void);
typedef struct sMenuItem *tMenuItem;
typedef struct sMenu *tMenu;

static unsigned MenuItemIndex = 0, nMenuLevel = 0;
static unsigned ItemNumb[MAX_MENU_LEVELS];

static tMenuItem psMenuItem, psCurrentMenuItem;
static tMenu psPrevMenu[MAX_MENU_LEVELS];
static tMenu psCurrentMenu;

struct sMenuItem {
  const char* psTitle;
  tMenuFunc pfMenuFunc;
  tMenu psSubMenu;
};

struct sMenu {
  const char* psTitle;
  tMenuItem psItems;
  unsigned nItems;
};

/* Меню второго уровеня */
struct sMenuItem TextMenuItems[] = {{"Шрифты", FontFunc, 0},
                                    {"Стиль", StyleFunc, 0},
                                    {"Книжка", BookFunc, 0},
                                    {"Возврат", ReturnFunc, 0}};
struct sMenu TextMenu = {"Текст", TextMenuItems, countof(TextMenuItems)};

struct sMenuItem GraphicMenuItems[] = {{"Примитивы", ElementsFunc, 0},
                                       {"Индикаторы", IndicatorsFunc, 0},
                                       {"Возврат", ReturnFunc, 0}};
struct sMenu GraphicMenu = {"Графика", GraphicMenuItems, countof(GraphicMenuItems)};

struct sMenuItem LEDsMenuItems[] = {{"Включить", LightsOnFunc, 0},
                                    {"Возврат", ReturnFunc, 0}};
struct sMenu LEDsMenu = {"Светодиоды", LEDsMenuItems, countof(LEDsMenuItems)};

/* Главное меню */
struct sMenuItem MainMenuItems[] = {
  {"Текст", IdleFunc, &TextMenu},
  {"Графика", IdleFunc, &GraphicMenu},
  {"Светодиоды", IdleFunc, &LEDsMenu},
  {"О программе", AboutFunc, 0}};
struct sMenu MainMenu = {"Главное меню", MainMenuItems, countof(MainMenuItems)};


/* Служебные функции отрисовки меню */

void DisplayMenuItemString(unsigned y, const char *ptr) {
    unsigned x;

    LCD_PUTS(0, y, "                                        ");
    x = (MAX_X - (CurrentFont->Width * strlen((unsigned char*)ptr))) / 2;
    LCD_PUTS(x, y, ptr);
}


/* Служебные обработчики */

void IdleFunc(void) {
}


/* SEL - переход на подменю и/или вызов соответствующего обработчика */
void SelFunc(void) {
    psCurrentMenuItem = psMenuItem;

    if(psMenuItem->psSubMenu != 0) {
        MenuItemIndex = 0;
        psCurrentMenu = psMenuItem->psSubMenu;
        psMenuItem = &(psCurrentMenu->psItems)[MenuItemIndex];
        DisplayMenu();
        nMenuLevel++;
        psPrevMenu[nMenuLevel] = psCurrentMenu;
    }
    psCurrentMenuItem->pfMenuFunc();
}


/* UP - переход на один пункт вверх */
void UpFunc(void) {
    /* Отображение текущего пункта меню как невыбранного */
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;
    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    /* Определение нового пункта меню (по циклу) */
    if(MenuItemIndex > 0)
        MenuItemIndex--;
    else
        MenuItemIndex = psCurrentMenu->nItems - 1;


    /* Отображение нового пункта меню как выбранного */
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");

    ItemNumb[nMenuLevel] = MenuItemIndex;
}


/* DOWN - переход на один пункт вниз */
void DownFunc(void) {
    /* Отображение текущего пункта меню как невыбранного */
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;

    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    /* Определение нового пункта меню (по циклу) */
    if(MenuItemIndex >= ((psCurrentMenu->nItems)-1))
        MenuItemIndex = 0;
    else
        MenuItemIndex++;
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);

    /* Отображение нового пункта меню как выбранного */
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");

    ItemNumb[nMenuLevel] = MenuItemIndex;
}


/* Возврат в главное меню */
void ReturnFunc(void) {
    if(nMenuLevel == 0)
        nMenuLevel++;

    psCurrentMenu = psPrevMenu[nMenuLevel-1];
    psMenuItem = &psCurrentMenu->psItems[0];
    ItemNumb[nMenuLevel] = 0;
    MenuItemIndex = 0;
    nMenuLevel--;

    DisplayMenu();
}


/* Интерфейсные функции */

void DisplayMenuTitle(const char *ptr) {
    unsigned x, y;

    x = (MAX_X - (CurrentFont->Width * strlen((unsigned char*)ptr))) / 2;
    LCD_PUTS(x, 0, ptr);

    y = CurrentFont->Height + 1;
    CurrentMethod = MET_OR;
    LCD_Line(0, y, MAX_X, y);
}


void Menu_Init(void) {
    psCurrentMenu = &MainMenu;
    psPrevMenu[nMenuLevel] = psCurrentMenu;
    psMenuItem = MainMenuItems;
    CurrentFont = &Font_6x8;
}


void DisplayMenu(void) {
    unsigned y, index;
    tMenuItem psMenuItem2;

    LCD_CLS();
    CurrentMethod = MET_AND;

    /* Отображаем заголовок меню */
    DisplayMenuTitle(psCurrentMenu->psTitle);
    /* Отображаем пункты меню */
    for (index = 0, y = CurrentFont->Height + 4;
                index < psCurrentMenu->nItems;
                index++, y += CurrentFont->Height + 2) {
        psMenuItem2 = &(psCurrentMenu->psItems[index]);
        DisplayMenuItemString(y, psMenuItem2->psTitle);
    }

    /* Определяем текущий пункт */
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/* Ждет нажатия SEL и возвращаемся в главное меню */
void BackToMenuOnSel(void) {
  WAIT_UNTIL_KEY_PRESSED(SEL);
  DisplayMenu();
}


/* Функция-диспетчер */
void ReadKey(void) {
    unsigned key;

    while (1) {
        key = GetKey();
        switch (key) {
            case SEL:   SelFunc();  break;
            case UP:    UpFunc();   break;
            case DOWN:  DownFunc(); break;
        }
        WAIT_UNTIL_KEY_RELEASED(key);
    }
}

/*============================================================================================
 * Конец файла Menu.c
 *============================================================================================*/
