/*
 * Measuring task switch time.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include <kernel/internal.h>

#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task_console, 0x400);
ARRAY (task_receiver, 0x400);
mutex_t mailbox;

volatile unsigned nmessages;
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
	unsigned t0;

	for (;;) {
		t0 = mips_read_c0_register (C0_COUNT);
		mutex_signal (&mailbox, (void*) t0);

		debug_puts ("\33[H");
		debug_puts ("Measuring task switch time.\n\n");

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
		debug_printf ("Task switches: %u  \n\n", nmessages);

		print_rational (" Latency, min: ", latency_min * 1000, KHZ);
		print_rational ("          max: ", latency_max * 1000, KHZ);
	}
}

/*
 * Задача приёма сообщений.
 */
void receiver (void *arg)
{
	unsigned t0, t1, latency;

	for (;;) {
		t0 = (unsigned) mutex_wait (&mailbox);
		t1 = mips_read_c0_register (C0_COUNT);

		/* Вычисляем количество тактов, затраченных на вход в прерывание. */
		latency = t1 - t0;

		/*debug_printf ("<%u> ", latency);*/
		if (++nmessages > 10) {
			if (latency_min > latency)
				latency_min = latency;
			if (latency_max < latency)
				latency_max = latency;
		}
	}
}

void uos_init (void)
{
	/* Задаём количество wait states для внешней flash-памяти на nCS3. */
	MC_CSCON3 = MC_CSCON_WS (CS3_WAIT_STATES);

	/* Стираем экран. */
	debug_puts ("\33[H\33[2J");

	/* Запускаем задачу выдачи статистики на консоль. */
	task_create (console, 0, "console", 1, task_console, sizeof (task_console));

	/* Запускаем задачу приёма сообщений. */
	task_create (receiver, 0, "rcv", 2, task_receiver, sizeof (task_receiver));
}
