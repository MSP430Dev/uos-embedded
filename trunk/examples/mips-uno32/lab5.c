//
// Пример для платы Uno32.
// Copyright (C) 2012 Сергей Вакуленко
//
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"

//
// Размер стека для задач: примерно килобайт.
//
#define STACK_NBYTES    1000

//
// Память для стеков задач.
//
ARRAY (task1_stack, STACK_NBYTES);
ARRAY (task2_stack, STACK_NBYTES);

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

//
// Отображение одного сегмента на дисплее
//
void display (int segment, int on)
{
    switch (segment) {
    case 'a':
        if (on) LATDSET = MASKD_SEGM_A;
        else    LATDCLR = MASKD_SEGM_A;
        break;
    case 'b':
        if (on) LATDSET = MASKD_SEGM_B;
        else    LATDCLR = MASKD_SEGM_B;
        break;
    case 'c':
        if (on) LATFSET = MASKF_SEGM_C;
        else    LATFCLR = MASKF_SEGM_C;
        break;
    case 'd':
        if (on) LATDSET = MASKD_SEGM_D;
        else    LATDCLR = MASKD_SEGM_D;
        break;
    case 'e':
        if (on) LATDSET = MASKD_SEGM_E;
        else    LATDCLR = MASKD_SEGM_E;
        break;
    case 'f':
        if (on) LATDSET = MASKD_SEGM_F;
        else    LATDCLR = MASKD_SEGM_F;
        break;
    case 'g':
        if (on) LATDSET = MASKD_SEGM_G;
        else    LATDCLR = MASKD_SEGM_G;
        break;
    case 'h':
        if (on) LATDSET = MASKD_SEGM_H;
        else    LATDCLR = MASKD_SEGM_H;
        break;
    }
}

//
// Опрос состояния кнопки.
// Возвращает ненулевое значение, если кнопка нажата.
//
int button_pressed (int button)
{
    switch (button) {
    case '1':
        return ! (PORTG & MASKG_BUTTON1);
    case '2':
        return ! (PORTG & MASKG_BUTTON2);
    }
    return 0;
}

//
// Функция ожидания, с остановом при нажатой кнопке.
//
void wait (unsigned msec, int button)
{
    unsigned t0 = timer_milliseconds (&timer);

    while (button_pressed (button) ||
           timer_milliseconds (&timer) - t0 < msec)
    {
        // Если нажата указанная кнопка - ждём,
        // пока она не освободится.
        timer_delay (&timer, 10);
    }
}

//
// Первая задача: вращаем нижнее кольцо восьмёрки, сегменты D-E-G-C.
// Функция не должна возвращать управление.
//
void task1 (void *arg)
{
    for (;;) {
        display ('d', 1); wait (100, '1'); display ('d', 0);
        display ('e', 1); wait (100, '1'); display ('e', 0);
        display ('g', 1); wait (100, '1'); display ('g', 0);
        display ('c', 1); wait (100, '1'); display ('c', 0);
    }
}

//
// Вторая задача: вращаем верхнее кольцо восьмёрки, сегменты A-B-G-F.
// Функция не должна возвращать управление.
//
void task2 (void *arg)
{
    for (;;) {
        display ('a', 1); wait (75, '2'); display ('a', 0);
        display ('b', 1); wait (75, '2'); display ('b', 0);
        display ('g', 1); wait (75, '2'); display ('g', 0);
        display ('f', 1); wait (75, '2'); display ('f', 0);
    }
}

//
// Начальная инициализация.
//
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

    // Создаём две задачи.
    task_create (task1, "task1", 0, 1, task1_stack, sizeof (task1_stack));
    task_create (task2, "task2", 0, 1, task2_stack, sizeof (task2_stack));
}
