/*
 * Measuring interrupt latency.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include <kernel/internal.h>

#define TIMER_IRQ	22	/* Прерывание от интервального таймера */
#define MSEC_PER_TICK	100	/* Период таймера в миллисекундах */
#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task, 0x400);
mutex_t timer_lock;

volatile unsigned nirqs;
volatile unsigned latency_min = ~0;
volatile unsigned latency_max;

/*
 * Печать рационального числа a/b с двумя знаками после запятой.
 */
void print_rational (char *title, unsigned a, unsigned b)
{
	unsigned val = a * 100 / b;

	debug_printf ("%s%u.%02u usec  \n", title, val/100, val%100);
}

/*
 * Задача выдачи статистики на консоль.
 */
void console (void *arg)
{
	for (;;) {
		mutex_wait (&timer_lock);
		debug_puts ("\33[H");
		debug_puts ("Measuring interrupt latency.\n\n");

		debug_printf ("CPU frequency: %u MHz\n", KHZ / 1000);
		debug_printf ("  SDRAM clock: %u MHz\n", MPORT_KHZ / 1000);
		debug_printf ("   Reg.CSCON3: %08x  \n", MC_CSCON3);
#ifdef ENABLE_ICACHE
		debug_printf ("  Instr.cache: enabled  \n");
#else
		debug_printf ("  Instr.cache: disabled  \n");
#endif
#ifdef ENABLE_DCACHE
		debug_printf ("   Data cache: enabled  \n");
#else
		debug_printf ("   Data cache: disabled  \n");
#endif
		debug_printf ("\n");
		debug_printf ("   Interrupts: %u  \n\n", nirqs);

		print_rational (" Latency, min: ", latency_min * 1000, KHZ);
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
	if (++nirqs > 10) {
		if (latency_min > latency)
			latency_min = latency;
		if (latency_max < latency)
			latency_max = latency;
	}

	/* Возвращаем 0, чтобы послать сигнал для mutex_wait(). */
	return 0;
}

void uos_init (void)
{
	debug_printf ("\n\nTesting latency\n");
	/* Configure 16 Mbyte of external Flash memory at nCS3. */
	MC_CSCON3 = MC_CSCON_WS (5);		/* Wait states  */

	/* Стираем экран. */
	debug_puts ("\33[H\33[2J");

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
