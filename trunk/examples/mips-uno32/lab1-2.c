//
// Пример для платы Uno32 и среды програмирования MPIDE.
// Copyright (C) 2012 Сергей Вакуленко
//
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

// Память для стека задачи.
ARRAY (task, 1000);

// Драйвер таймера.
timer_t timer;

// Кнопки на контактах 11 и 12.
#define MASKG_BUTTON1   (1 << 8)    // 11 - RG8
#define MASKG_BUTTON2   (1 << 7)    // 12 - RG7

// 7-сегментный индикатор на контактах 2-9.
#define MASKD_SEGM_A    (1 << 8)    // Контакт 2 - сигнал RD8
#define MASKD_SEGM_B    (1 << 0)    // Контакт 3 - сигнал RD0
#define MASKF_SEGM_C    (1 << 1)    // Контакт 4 - сигнал RF1
#define MASKD_SEGM_D    (1 << 1)    // Контакт 5 - сигнал RD1
#define MASKD_SEGM_E    (1 << 2)    // Контакт 6 - сигнал RD2
#define MASKD_SEGM_F    (1 << 9)    // Контакт 7 - сигнал RD9
#define MASKD_SEGM_G    (1 << 10)   // Контакт 8 - сигнал RD10
#define MASKD_SEGM_H    (1 << 3)    // Контакт 9 - сигнал RD3

// Отображение одной цифры на дисплее.
void display (unsigned digit)
{
    static const unsigned pattern[10] = {
        1 + 2 + 4 + 8 + 16 + 32,        // Цифра 0
            2 + 4,                      // Цифра 1
        1 + 2     + 8 + 16      + 64,   // Цифра 2
        1 + 2 + 4 + 8           + 64,   // Цифра 3
            2 + 4          + 32 + 64,   // Цифра 4
        1     + 4 + 8      + 32 + 64,   // Цифра 5
        1     + 4 + 8 + 16 + 32 + 64,   // Цифра 6
        1 + 2 + 4,                      // Цифра 7
        1 + 2 + 4 + 8 + 16 + 32 + 64,   // Цифра 8
        1 + 2 + 4 + 8      + 32 + 64,   // Цифра 9
    };

    if (digit > 9)
        digit = 9;
    unsigned mask = pattern[digit];

    if (mask & 1)   LATDSET = MASKD_SEGM_A;
    else            LATDCLR = MASKD_SEGM_A;
    if (mask & 2)   LATDSET = MASKD_SEGM_B;
    else            LATDCLR = MASKD_SEGM_B;
    if (mask & 4)   LATFSET = MASKF_SEGM_C;
    else            LATFCLR = MASKF_SEGM_C;
    if (mask & 8)   LATDSET = MASKD_SEGM_D;
    else            LATDCLR = MASKD_SEGM_D;
    if (mask & 16)  LATDSET = MASKD_SEGM_E;
    else            LATDCLR = MASKD_SEGM_E;
    if (mask & 32)  LATDSET = MASKD_SEGM_F;
    else            LATDCLR = MASKD_SEGM_F;
    if (mask & 64)  LATDSET = MASKD_SEGM_G;
    else            LATDCLR = MASKD_SEGM_G;
    if (mask & 128) LATDSET = MASKD_SEGM_H;
    else            LATDCLR = MASKD_SEGM_H;
}

// Задача опроса кнопок.
void loop (void *arg)
{
    // Состояние кнопок.
    int button1_was_pressed = 0, button2_was_pressed = 0;

    // Моменты нажатия кнопок.
    unsigned time1 = 0, time2 = 0;

    for (;;) {
        // Время в миллисекундах.
        unsigned msec = timer_milliseconds (&timer);

        // Опрашиваем кнопки.
        int button1_pressed = ! (PORTG & MASKG_BUTTON1);
        int button2_pressed = ! (PORTG & MASKG_BUTTON2);

        // Если кнопки были нажаты - запоминаем время.
        if (button1_pressed && ! button1_was_pressed)
            time1 = msec;
        if (button2_pressed && ! button2_was_pressed)
            time2 = msec;
        button1_was_pressed = button1_pressed;
        button2_was_pressed = button2_pressed;

        if (button1_pressed && button2_pressed) {
            // Обе кнопки нажаты: показываем разность.
            if (time1 > time2) {
                display (time1 - time2);
            } else {
                display (time2 - time1);
            }
        } else {
            // Отображаем текущее время, десятые доли секунды.
            display (msec / 100 % 10);
        }
        timer_delay (&timer, 1);
    }
}

// Начальная инициализация системы.
void uos_init()
{
    // Сигналы от кнопок используем как входы.
    TRISGSET = MASKG_BUTTON1 | MASKG_BUTTON2;

    // Сигналы управления 7-сегментным индикатором - выходы.
    TRISDCLR = MASKD_SEGM_A | MASKD_SEGM_B | MASKD_SEGM_D | MASKD_SEGM_E |
               MASKD_SEGM_F | MASKD_SEGM_G | MASKD_SEGM_H;
    TRISFCLR = MASKF_SEGM_C;

    // Устанавливаем прерывание от таймера с периодом 1 мсек.
    timer_init (&timer, KHZ, 1);

    // Запускаем одну задачу.
    task_create (loop, "task", 0, 1, task, sizeof (task));
}
