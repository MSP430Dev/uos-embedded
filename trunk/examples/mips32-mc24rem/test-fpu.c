/*
 * Testing FPU.
 */
#include <runtime/lib.h>
#include <runtime/math.h>
#include <kernel/uos.h>
#include <kernel/internal.h>

#define TIMER_IRQ	29	/* Прерывание от интервального таймера */
#define MSEC_PER_TICK	1	/* Период таймера в миллисекундах */
#define CS3_WAIT_STATES	3	/* Такты ожидания для flash-памяти на CS3 */

ARRAY (task_console, 0x800);
ARRAY (task_fpu1, 0x800);
ARRAY (task_fpu2, 0x800);
ARRAY (task_fpu3, 0x800);
ARRAY (task_fpu4, 0x800);

task_t *t1, *t2, *t3, *t4;
volatile double e, pi_machin, pi_gauss, pi_brent;
volatile unsigned count_e_loops, count_e_errors;
volatile unsigned count_machin_loops, count_machin_errors;
volatile unsigned count_gauss_loops, count_gauss_errors;
volatile unsigned count_brent_loops, count_brent_errors;

mutex_t timer_lock;
mutex_t timer_half_sec;
unsigned timer_msec, timer_hsec;
volatile unsigned timer_sec, timer_min, timer_hour;

/*
 * Вычисление константы E = 1 + 1 + 1/2 + 1/2/3 + 1/2/3/4 + ...
 * Вызов compute_e(x) возвращает результат e*x.
 */
double compute_e (double r1)
{
	double r2 = r1 / 2;
	double r3 = r2 / 3;
	double r4 = r3 / 4;
	double r5 = r4 / 5;
	double r6 = r5 / 6;
	double r7 = r6 / 7;
	double r8 = r7 / 8;
	double r9 = r8 / 9;
	double r10 = r9 / 10;
	double r11 = r10 / 11;
	double r12 = r11 / 12;
	double r13 = r12 / 13;
	double r14 = r13 / 14;
	double r15 = r14 / 15;
	double r16 = r15 / 16;
	double r17 = r16 / 17;
	double r18 = r17 / 18;
	double r19 = r18 / 19;
	double r20 = r19 / 20;
	double e = r20 + r19;
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
double compute_pi_machin (double x, double y)
{
	double x2 = x * x;
	double x3 = x2 * x;
	double x5 = x3 * x2;
	double x7 = x5 * x2;
	double x9 = x7 * x2;
	double x11 = x9 * x2;
	double x13 = x11 * x2;
	double x15 = x13 * x2;
	double x17 = x15 * x2;
	double x19 = x17 * x2;
	double atx = x17/17 - x19/19;
	atx += x13/13 - x15/15;
	atx += x9/9 - x11/11;
	atx += x5/5 - x7/7;
	atx += x - x3/3;

	double y2 = y * y;
	double y3 = y2 * y;
	double y5 = y3 * y2;
	double y7 = y5 * y2;
	double y9 = y7 * y2;
	double y11 = y9 * y2;
	double y13 = y11 * y2;
	double y15 = y13 * y2;
	double aty = y13/13 - y15/15;
	aty += y9/9 - y11/11;
	aty += y5/5 - y7/7;
	aty += y - y3/3;
	return 4 * (4*atx - aty);
}

/*
 * Вычисление константы пи по формуле Гаусса:
 * pi/4 = 12*arctan(1/18) + 8*arctan(1/57) - 5*arctan(1/239)
 * Вызов compute_pi_gauss(1./18, 1./57, 1./239) возвращает значение пи.
 */
double compute_pi_gauss (double x, double y, double z)
{
	double x2 = x * x;
	double x3 = x2 * x;
	double x5 = x3 * x2;
	double x7 = x5 * x2;
	double x9 = x7 * x2;
	double x11 = x9 * x2;
	double x13 = x11 * x2;
	double x15 = x13 * x2;
	double atx = x13/13 - x15/15;
	atx += x9/9 - x11/11;
	atx += x5/5 - x7/7;
	atx += x - x3/3;

	double y2 = y * y;
	double y3 = y2 * y;
	double y5 = y3 * y2;
	double y7 = y5 * y2;
	double y9 = y7 * y2;
	double y11 = y9 * y2;
	double y13 = y11 * y2;
	double y15 = y13 * y2;
	double aty = y13/13 - y15/15;
	aty += y9/9 - y11/11;
	aty += y5/5 - y7/7;
	aty += y - y3/3;

	double z2 = z * z;
	double z3 = z2 * z;
	double z5 = z3 * z2;
	double z7 = z5 * z2;
	double z9 = z7 * z2;
	double z11 = z9 * z2;
	double z13 = z11 * z2;
	double z15 = z13 * z2;
	double atz = z13/13 - z15/15;
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
 * Вызов compute_pi_brent(1.0, M_SQRT1_2l, 0.25, 1.0) возвращает значение пи.
 */
double compute_pi_brent (double a0, double b0, double t0, double p0)
{
	double a1 = (a0 + b0) / 2;
	double b1 = sqrt (a0 * b0);
	double t1 = t0 - p0 * (a0 - a1)*(a0 - a1);
	double p1 = p0 + p0;

	double a2 = (a1 + b1) / 2;
	double b2 = sqrt (a1 * b1);
	double t2 = t1 - p1 * (a1 - a2)*(a1 - a2);
	double p2 = p1 + p1;

	double a3 = (a2 + b2) / 2;
	double b3 = sqrt (a2 * b2);
	double t3 = t2 - p2 * (a2 - a3)*(a2 - a3);
	return (a3 + b3)*(a3 + b3) / (4 * t3);
}

/*
 * Задача выдачи статистики на консоль.
 */
void console (void *arg)
{
	/* Разрешаем FPU для текущей задачи.
	 * Округление к ближайшему целому, исключения отключены. */
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);

	for (;;) {
		debug_puts ("\33[H");
		debug_puts ("Testing FPU.\n\n");

		debug_printf ("     Duration: %02u:%02u:%02u \n", timer_hour, timer_min, timer_sec);
		debug_printf ("CPU frequency: %u MHz\n", KHZ / 1000);
		debug_printf ("   Reg.CSCON3: %08x  \n", MC_CSCON3);
#ifdef ENABLE_ICACHE
		debug_printf ("  Instr.cache: enabled  \n\n");
#else
		debug_printf ("  Instr.cache: disabled  \n\n");
#endif
		debug_printf ("---------Value-----------------Loops-------Errors----\n");
		debug_printf ("     e:  %-20.15f  %-10u  %u \n", e, count_e_loops, count_e_errors);
		debug_printf ("Machin:  %-20.15f  %-10u  %u \n", pi_machin, count_machin_loops, count_machin_errors);
		debug_printf (" Gauss:  %-20.15f  %-10u  %u \n", pi_gauss, count_gauss_loops, count_gauss_errors);
		debug_printf (" Brent:  %-20.15f  %-10u  %u \n", pi_brent, count_brent_loops, count_brent_errors);

		/* Выдача каждые полсекунды. */
		mutex_wait (&timer_half_sec);
	}
}

/*
 * Счётные задачи в бесконечном цикле вычисляют некоторое значение и
 * сравнивают его с эталоном. Подсчитывается количество циклов и
 * количество ошибок.
 */
void test_fpu1 (void *arg)
{
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);
	for (;;) {
		/* Вычисляем E - основание натурального логарифма. */
		e = compute_e (1);
		count_e_loops++;
		if (e != 0x1.5bf0a8b145769p+1)
			count_e_errors++;
	}
}

void test_fpu2 (void *arg)
{
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);
	for (;;) {
		/* Вычисление константы пи по формуле Джона Мэчина. */
		pi_machin = compute_pi_machin (1.0/5, 1.0/239);
		count_machin_loops++;
		if (pi_machin != 0x1.921fb54442d15p+1)
			count_machin_errors++;
	}
}

void test_fpu3 (void *arg)
{
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);
	for (;;) {
		/* Вычисление константы пи по формуле Гаусса. */
		pi_gauss = compute_pi_gauss (1.0/18, 1.0/57, 1.0/239);
		count_gauss_loops++;
		if (pi_gauss != 0x1.921fb54442d17p+1)
			count_gauss_errors++;
	}
}

void test_fpu4 (void *arg)
{
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);
	for (;;) {
		/* Вычисление константы пи по алгоритму Брента-Саламина. */
		pi_brent = compute_pi_brent (1.0, 1/sqrt(2), 0.25, 1.0);
		count_brent_loops++;
		if (pi_brent != 0x1.921fb54442d1ap+1)
			count_brent_errors++;
	}
}

/*
 * Быстрый обработчик прерывания от интервального таймера.
 * Аргумент вызова не используется.
 */
static bool_t timer_handler (void *arg)
{
	/* Снимаем бит прерывания. */
	MC_ITCSR &= ~MC_ITCSR_INT;

	/* Заново открываем маску прерывания. */
	arch_intr_allow (TIMER_IRQ);

	/* Вычисляем время. */
	timer_msec += MSEC_PER_TICK;
	if (timer_msec >= 500) {
//debug_printf ("%%");
		timer_msec = 0;
		mutex_activate (&timer_half_sec, 0);
		if (++timer_hsec >= 2) {
			timer_hsec = 0;
			if (++timer_sec >= 60) {
				timer_sec = 0;
				if (++timer_min >= 60) {
					timer_min = 0;
					timer_hour++;
				}
			}
		}

	}
#if 1
	/* Меняем приоритеты вычислительных задач.
	 * Одна из задач (по циклу) получает приоритет 2, остальные - 1. */
	if (t1->prio == 2) {
		t1->prio = 1;
		t2->prio = 2;
	} else if (t2->prio == 2) {
		t2->prio = 1;
		t3->prio = 2;
	} else if (t3->prio == 2) {
		t3->prio = 1;
		t4->prio = 2;
	} else /* очевидно t4->prio == 2 */ {
		t4->prio = 1;
		t1->prio = 2;
	}
#endif
	/* Устанавливаем флаг: необходимо сменить текущую задачу. */
	task_need_schedule = 1;

	/* Возвращаем 1: прерывание полностью обработано. */
	return 1;
}

void uos_init (void)
{
	/* Задаём количество wait states для внешней flash-памяти на nCS3. */
	MC_CSCON3 = MC_CSCON_WS (CS3_WAIT_STATES);

	/* Стираем экран. */
	debug_puts ("\33[H\33[2J");

	/* Используем интервальный таймер в масштабе 1:1. */
	MC_ITCSR = 0;
	MC_ITSCALE = 0;
	MC_ITPERIOD = KHZ * MSEC_PER_TICK - 1;
	MC_ITCSR = MC_ITCSR_EN;
	mutex_attach_irq (&timer_lock, TIMER_IRQ, timer_handler, 0);

	/* Запускаем задачу выдачи статистики на консоль. */
	task_create (console, 0, "console", 10, task_console, sizeof (task_console));

	/* Запускаем задачи вычислений.
	 * По таймеру будем менять их приоритет, чтобы добиться
	 * большого количества переключений контекста. */
	t1 = task_create (test_fpu1, 0, "fpu", 2, task_fpu1, sizeof (task_fpu1));
	t2 = task_create (test_fpu2, 0, "fpu", 1, task_fpu2, sizeof (task_fpu2));
	t3 = task_create (test_fpu3, 0, "fpu", 1, task_fpu3, sizeof (task_fpu3));
	t4 = task_create (test_fpu4, 0, "fpu", 1, task_fpu4, sizeof (task_fpu4));
}
