//
// Пример для платы Uno32 и среды програмирования MPIDE.
// Copyright (C) 2012 Сергей Вакуленко
//
#include <runtime/lib.h>

// Кнопки на контактах 11 и 12.
#define MASKG_BUTTON1   (1 << 8)    // 11 - RG8
#define MASKG_BUTTON2   (1 << 7)    // 12 - RG7

// Светодиоды на контактах 13 и 43.
#define MASKG_LED1      (1 << 6)   // 13 - RG6
#define MASKF_LED2      (1 << 0)   // 43 - RF0

void setup ()
{
    // Сигналы от кнопок используем как входы.
    TRISGSET = MASKG_BUTTON1 | MASKG_BUTTON2;

    // Сигналы управления светодиодами - выходы.
    TRISGCLR = MASKG_LED1;
    TRISFCLR = MASKF_LED2;
}

void loop ()
{
    int need_wait = 0;

    // Опрашиваем первую кнопку.
    int button1_pressed = ! (PORTG & MASKG_BUTTON1);
    if (! button1_pressed) {
        // Не нажата - гасим первый светодиод.
        LATGCLR = MASKG_LED1;
        need_wait = 1;
    }

    // Опрашиваем вторую кнопку.
    int button2_pressed = ! (PORTG & MASKG_BUTTON2);
    if (! button2_pressed) {
        // Не нажата - гасим второй светодиод.
        LATFCLR = MASKF_LED2;
        need_wait = 1;
    }

    // Если надо, подождём.
    if (need_wait)
        mdelay (150);

    // Зажигаем оба светодиода.
    LATGSET = MASKG_LED1;
    LATFSET = MASKF_LED2;
    mdelay (50);
}

int main (void)
{
    setup();

    for (;;)
        loop();
}
