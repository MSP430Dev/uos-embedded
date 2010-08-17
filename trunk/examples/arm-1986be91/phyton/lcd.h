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
 *  Файл lcd.h: Утилиты нижнего уровня для работы с LCD-экраном МЭЛТ MT-12864J
 *
 *============================================================================================*/

#ifndef __LCD_H
#define __LCD_H

/* Номера LCD-кристаллов */
typedef enum {
    LCD_CRYSTAL1    = 0,
    LCD_CRYSTAL2    = 1,
    NUM_LCD_CRYSTALS
} LCD_Crystal;

/* Флаги состояния LCD */
typedef enum {
    BUSY            = 7,
    ONOFF           = 5,
    RESET           = 4,
} LCD_Status;

/* Методы вывода битов и байтов */
typedef enum {
    MET_OR          = 0,
    MET_XOR         = 1,
    MET_NOT_OR      = 2,
    MET_NOT_XOR     = 3,
    MET_AND         = 4,
} LCD_Method;

/* Рарешение экрана LCD (в пикселях) */
#define MAX_X       127
#define MAX_Y       63

/* Порты LCD-кристаллов */
typedef struct tag_LCD_Ports {
    unsigned Data;
    unsigned Cmd;
} LCD_Ports;

extern const LCD_Ports CrystalPorts[NUM_LCD_CRYSTALS];

#define LCD_DATA(x)                 (*((volatile unsigned*)CrystalPorts[x].Data))
#define LCD_CMD(x)                  (*((volatile unsigned*)CrystalPorts[x].Cmd))

/* Контекст подсистемы отрисовки */
extern LCD_Crystal CurrentCrystal;          /* Текущий выбранный кристал */
extern LCD_Method CurrentMethod;            /* Текущий метод отрисовки */

/* Утилиты работы с LCD */
void ResetLCD(void);
void InitPortLCD(void);
void InitExtBus(void);
void SetCrystal(LCD_Crystal num);
void WriteLCD_Cmd(unsigned val);
void WriteLCD_Data(unsigned val);
unsigned ReadLCD_Cmd(void);
unsigned ReadLCD_Data(void);
void LCD_INIT(void);
void LCD_CLS(void);

/* Специфические команды управления LCD */
#define LCD_ON                      WriteLCD_Cmd(0x3F)
#define LCD_OFF                     WriteLCD_Cmd(0x3E)
#define LCD_START_LINE(x)           WriteLCD_Cmd(0xC0 | (x))
#define LCD_SET_PAGE(x)             WriteLCD_Cmd(0xB8 | (x))
#define LCD_SET_ADDRESS(x)          WriteLCD_Cmd(0x40 | (x))

#endif /* __LCD_H */

/*============================================================================================
 * Конец файла lcd.h
 *============================================================================================*/
