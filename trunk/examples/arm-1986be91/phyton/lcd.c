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
 *  Файл lcd.c: Утилиты нижнего уровня для работы с LCD-экраном МЭЛТ MT-12864J
 *
 *============================================================================================*/

#include <runtime/lib.h>

#include "lcd.h"

/* Контекст подсистемы отрисовки */
LCD_Crystal CurrentCrystal = LCD_CRYSTAL1;  /* Текущий выбранный кристал */
LCD_Method CurrentMethod;                   /* Текущий метод отрисовки */

/* Порты LCD-кристаллов */
const LCD_Ports CrystalPorts[NUM_LCD_CRYSTALS] = {
    /*Кристалл #1*/ { /*LCD_DATA1*/ 0x18100000, /*LCD_CMD1*/ 0x10100000 },
    /*Кристалл #2*/ { /*LCD_DATA2*/ 0x18200000, /*LCD_CMD2*/ 0x10200000 },
};

/* Служебные прототипы и макросы */

static unsigned GetStatus(void);
static void WaitStatus(LCD_Status status);

/* Задержка на 8 микросекунд */
static inline void Pause(void) {
    udelay (8);
}

#define WAIT_BUSY   WaitStatus(BUSY)
#define WAIT_RESET  WaitStatus(RESET)
#define WAIT_ON     WaitStatus(ONOFF)

/* Утилиты работы с LCD */

void SetCrystal(LCD_Crystal num) {
    ARM_GPIOE->DATA = ((num + 1) << 4);
    ARM_GPIOE->OE = 0x30;
    Pause();
    CurrentCrystal = num;
}

void WriteLCD_Cmd(unsigned val) {
    LCD_CMD(CurrentCrystal) = val;
    Pause();
}

void WriteLCD_Data(unsigned val) {
    LCD_DATA(CurrentCrystal) = val;
    Pause();
}

unsigned ReadLCD_Cmd(void) {
    unsigned ret = LCD_CMD(CurrentCrystal);
    Pause();
    return ret;
}

unsigned ReadLCD_Data() {
    unsigned ret;
    LCD_DATA(CurrentCrystal); /* Первое чтение - необходимо для получения корректных данных */
    Pause();
    ret = LCD_DATA(CurrentCrystal);
    Pause();
    return ret;
}

void LCD_INIT(void) {
    unsigned crystal;

    ARM_RSTCLK->PER_CLOCK = 0xFFFFFFFF;

    /* Инициализация портов внешней шины и выводов для работы с экраном */
    ARM_GPIOA->FUNC = 0x00005555;   /* Main Function для DATA[7:0] */
    ARM_GPIOA->ANALOG = 0xFFFF;     /* Digital */
    ARM_GPIOA->PWR = 0x00005555;    /* Fast */

    ARM_GPIOE->FUNC = 0x00400500;   /* Main Function для ADDR[20,21,27] */
    ARM_GPIOE->ANALOG = 0xFFFF;     /* Digital */
    ARM_GPIOE->PWR = 0x00400500;    /* Fast */

    ARM_GPIOC->FUNC = 0x15504010;   /* Main Function для RESET WE & CLOCK & KEYS*/
    ARM_GPIOC->ANALOG = 0xFFFF;     /* Digital */
    ARM_GPIOC->PWR = 0x0008C010;    /* Fast */

    /* Инициализация внешней шины */
    ARM_EXTBUS->CONTROL = 0x0000F001;

    /* Программный сброс экрана */
    ARM_GPIOC->DATA = 0x00000200;
    ARM_GPIOC->OE = 0x00000200;
    unsigned i;
    for (i = 0; i < 255; i++)
        ARM_GPIOC->DATA = 0x00000000;
    ARM_GPIOC->DATA = 0x00000200;

    /* Инициализация всех кристаллов */
    for (crystal = LCD_CRYSTAL1; crystal < NUM_LCD_CRYSTALS; crystal++) {
        SetCrystal((LCD_Crystal)crystal);
        WAIT_BUSY;
        LCD_ON;
        WAIT_ON;
        LCD_START_LINE(0);
    }
}

void LCD_CLS(void) {
    unsigned i, j, crystal;

    /* Очистка данных для всех кристаллов */
    for (crystal = LCD_CRYSTAL1; crystal < NUM_LCD_CRYSTALS; crystal++) {
        SetCrystal((LCD_Crystal)crystal);
        WAIT_BUSY;
        LCD_OFF;
        LCD_SET_ADDRESS(0);
        for (i = 0; i < 8; i++) {
            LCD_SET_PAGE(i);
            for (j = 0; j < 64; j++)
                WriteLCD_Data(0x00);
        }
        LCD_ON;
    }
}

/* Служебные функции */

static unsigned GetStatus(void) {
    unsigned ret;

    Pause();
    ret = ReadLCD_Cmd();
    Pause();
    return ret;
}

static void WaitStatus(LCD_Status status) {
    unsigned stat;
    for (stat = GetStatus(); stat == (1 << status); stat = GetStatus())
        continue;
}

/*============================================================================================
 * Конец файла lcd.c
 *============================================================================================*/
