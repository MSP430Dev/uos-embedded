/*
 * Testing timer.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include <kernel/internal.h>

#define TIMER_IRQ	29	/* Прерывание от интервального таймера */
#define MSEC_PER_TICK	100	/* Период таймера в миллисекундах */
#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task, 0x400);
mutex_t timer_lock;

unsigned nirqs;
unsigned latency_sum;
unsigned latency_min = ~0;
unsigned latency_max;

/*
 * Печать рационального числа a/b с двумя знаками после запятой.
 */
void print_rational (char *title, unsigned a, unsigned b)
{
	unsigned val = a * 100 / b;

	debug_printf ("%s%u.%02u usec\n", title, val/100, val%100);
}

/*
 * Задача выдачи статистики на консоль.
 */
void console (void *arg)
{
	for (;;) {
//		mdelay (200);
		mutex_wait (&timer_lock);
		debug_puts ("\33[H");
		debug_puts ("Testing interrupt latency.\n\n");

		debug_printf ("CPU frequency: %u MHz\n\n", KHZ / 1000);

		debug_printf ("   Interrupts: %u  \n\n", nirqs);

		print_rational (" Latency, min: ", latency_min * 1000, KHZ);
		print_rational ("      average: ", latency_sum / nirqs * 1000, KHZ);
		print_rational ("          max: ", latency_max * 1000, KHZ);
	}
}

/*
 * Быстрый обработчик прерывания от интервального таймера.
 * Аргумент вызова не используется.
 */
static bool_t timer_handler (void *arg)
{
	/* Вычисляем количество тактов, затраченных на вход в прерывание. */
	unsigned latency = MC_ITPERIOD - MC_ITCOUNT;

	/* Снимаем бит прерывания. */
	MC_ITCSR &= ~MC_ITCSR_INT;

	/* Заново открываем маску прерывания. */
	arch_intr_allow (TIMER_IRQ);

	/*debug_printf ("<%u> ", latency);*/
	++nirqs;
	latency_sum += latency;
	if (latency_min > latency)
		latency_min = latency;
	if (latency_max < latency)
		latency_max = latency;

	/* Возвращаем 0: прерывание полностью обработано. */
	return 0;
}

void uos_init (void)
{
	/* Задаём количество wait states для внешней flash-памяти на nCS3. */
	MC_CSCON3 = MC_CSCON_WS (CS3_WAIT_STATES);

	/* Стираем экран. */
	debug_puts ("\33[H\33[2J");
#if 1
	/* Включение кэша команд для сегмента kseg0. */
	mips32_write_c0_register (C0_CONFIG, 3);
#endif
	/* Устанавливаем быстрый обработчик прерывания. */
	mutex_attach_irq (&timer_lock, TIMER_IRQ, timer_handler, 0);

	/* Используем интервальный таймер в масштабе 1:1. */
	MC_ITCSR = 0;
	MC_ITSCALE = 0;
	MC_ITPERIOD = KHZ * MSEC_PER_TICK - 1;
	MC_ITCSR = MC_ITCSR_EN;

	/* Запускаем задачу выдачи статистики на консоль. */
	task_create (console, "task", "console", 1, task, sizeof (task));
}
