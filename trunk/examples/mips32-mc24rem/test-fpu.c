/*
 * Testing FPU.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include <kernel/internal.h>

#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task_console, 0x400);
ARRAY (task_fpu, 0x400);

/*
 * Задача выдачи статистики на консоль.
 */
void console (void *arg)
{
	for (;;) {
		debug_puts ("\33[H");
		debug_puts ("Testing FPU.\n\n");

		debug_printf ("CPU frequency: %u MHz\n", KHZ / 1000);
		debug_printf ("   Reg.CSCON3: %08x  \n", MC_CSCON3);
#ifdef ENABLE_ICACHE
		debug_printf ("  Instr.cache: enabled  \n\n");
#else
		debug_printf ("  Instr.cache: disabled  \n\n");
#endif
/*TODO*/
	}
}

/*
 * Задача приёма сообщений.
 */
void test_fpu (void *arg)
{
	/* Разрешаем FPU для текущей задачи.
	 * Округление к ближайшему целому, исключения отключены. */
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);

	for (;;) {
/*TODO*/
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
	task_create (test_fpu, 0, "fpu", 2, task_fpu, sizeof (task_fpu));
}
