/*
 * Testing FPU.
 */
#include "runtime/lib.h"
#include "runtime/math.h"
#include "kernel/uos.h"
#include <kernel/internal.h>

#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task_console, 0x400);
ARRAY (task_fpu, 0x400);

/*
 * Вычисление константы E = 1 + 1 + 1/2 + 1/2/3 + 1/2/3/4 + ...
 * Вызов compute_e(x) возвращает результат e*x.
 */
float compute_e (float r1)
{
	float r2 = r1 / 2;
	float r3 = r2 / 3;
	float r4 = r3 / 4;
	float r5 = r4 / 5;
	float r6 = r5 / 6;
	float r7 = r6 / 7;
	float r8 = r7 / 8;
	float r9 = r8 / 9;
	float r10 = r9 / 10;
	float r11 = r10 / 11;
	float r12 = r11 / 12;
	float r13 = r12 / 13;
	float r14 = r13 / 14;
	float r15 = r14 / 15;
	float r16 = r15 / 16;
	float r17 = r16 / 17;
	float r18 = r17 / 18;
	float r19 = r18 / 19;
	float r20 = r19 / 20;
	float e = r20 + r19;
	e += r18 + r17;
	e += r16 + r15;
	e += r14 + r13;
	e += r12 + r11;
	e += r10 + r9;
	e += r8 + r7;
	e += r6 + r5;
	e += r4 + r3;
	e += r2 + 2;
	return e;
}

/*
 * Вычисление константы пи по формуле Джона Мэчина:
 * pi/4 = 4*arctan(1/5) - arctan(1/239)
 * Вызов compute_pi_machin(1./5, 1./239) возвращает значение пи.
 */
float compute_pi_machin (float x, float y)
{
	float x2 = x * x;
	float x3 = x2 * x;
	float x5 = x3 * x2;
	float x7 = x5 * x2;
	float x9 = x7 * x2;
	float x11 = x9 * x2;
	float x13 = x11 * x2;
	float x15 = x13 * x2;
	float atx = x13/13 - x15/15;
	atx += x9/9 - x11/11;
	atx += x5/5 - x7/7;
	atx += x - x3/3;

	float y2 = y * y;
	float y3 = y2 * y;
	float y5 = y3 * y2;
	float y7 = y5 * y2;
	float y9 = y7 * y2;
	float y11 = y9 * y2;
	float y13 = y11 * y2;
	float y15 = y13 * y2;
	float aty = y13/13 - y15/15;
	aty += y9/9 - y11/11;
	aty += y5/5 - y7/7;
	aty += y - y3/3;
	return 4 * (4*atx - aty);
}

/*
 * Вычисление константы пи по формуле Гаусса:
 * pi/4 = 12*arctan(1/18) + 8*arctan(1/57) - 5*arctan(1/239)
 * Вызов compute_pi_machin(1./18, 1./57, 1./239) возвращает значение пи.
 */
float compute_pi_gauss (float x, float y, float z)
{
	float x2 = x * x;
	float x3 = x2 * x;
	float x5 = x3 * x2;
	float x7 = x5 * x2;
	float x9 = x7 * x2;
	float x11 = x9 * x2;
	float x13 = x11 * x2;
	float x15 = x13 * x2;
	float atx = x13/13 - x15/15;
	atx += x9/9 - x11/11;
	atx += x5/5 - x7/7;
	atx += x - x3/3;

	float y2 = y * y;
	float y3 = y2 * y;
	float y5 = y3 * y2;
	float y7 = y5 * y2;
	float y9 = y7 * y2;
	float y11 = y9 * y2;
	float y13 = y11 * y2;
	float y15 = y13 * y2;
	float aty = y13/13 - y15/15;
	aty += y9/9 - y11/11;
	aty += y5/5 - y7/7;
	aty += y - y3/3;

	float z2 = z * z;
	float z3 = z2 * z;
	float z5 = z3 * z2;
	float z7 = z5 * z2;
	float z9 = z7 * z2;
	float z11 = z9 * z2;
	float z13 = z11 * z2;
	float z15 = z13 * z2;
	float atz = z13/13 - z15/15;
	atz += z9/9 - z11/11;
	atz += z5/5 - z7/7;
	atz += z - z3/3;
	return 4 * (12*atx + 8*aty - 5*atz);
}

/*
 * Вычисление константы пи по алгоритму Брента-Саламина:
 *	a[0] = 1
 *	b[0] = sqrt (2)
 *	t[0] = 1/4
 *	p[0] = 1
 *	a[n+1] = (a[n] + b[n]) / 2
 *	b[n+1] = sqrt (a[n] * b[n])
 *	t[n+1] = t[n] - p[n] * (a[n] - a[n+1])^2
 *	p[n+1] = 2 * p[n]
 * Результат:
 *	pi = (a[n] + b[n])^2 / 4 * t[n]
 *
 * Вызов compute_pi_brent(x) возвращает значение пи*x.
 */
float compute_pi_brent (float a0)
{
	float b0 = a0 / sqrtf (2.0);
	float t0 = a0 / 4;
	float p0 = a0;

	float a1 = (a0 + b0) / 2;
	float b1 = sqrtf (a0 * b0);
	float t1 = t0 - p0 * (a0 - a1)*(a0 - a1);
	float p1 = p0 + p0;

	float a2 = (a1 + b1) / 2;
	float b2 = sqrtf (a1 * b1);
	float t2 = t1 - p1 * (a1 - a2)*(a1 - a2);
	float p2 = p1 + p1;

	float a3 = (a2 + b2) / 2;
	float b3 = sqrtf (a2 * b2);
	float t3 = t2 - p2 * (a2 - a3)*(a2 - a3);
	return (a3 + b3)*(a3 + b3) / (4 * t3);
}

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
