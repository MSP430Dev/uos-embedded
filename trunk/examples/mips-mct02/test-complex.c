/*
 * Testing FPU.
 */
#include <runtime/lib.h>
#include <runtime/math.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <uart/uart.h>
#include <elvees/mct-02.h>

volatile unsigned num_uart __attribute__ ((section (".data")))=UART0;
volatile unsigned num_ram __attribute__ ((section (".data")))=UART1;
volatile unsigned num_tswitch __attribute__ ((section (".data")))=UART2;
volatile unsigned num_fpu __attribute__ ((section (".data")))=UART3;

uart_t uart,uart_ram,uart_fpu,uart_tswitch;

#define TIMER_IRQ	22		/* Прерывание от интервального таймера */
#define MSEC_PER_TICK	1	/* Период таймера в миллисекундах */

ARRAY (task_uart, 0x400);
ARRAY (task_uart2, 0x400);
ARRAY (stack_console_ram, 2000);
ARRAY (stack_console_fpu, 0x800);
ARRAY (task_fpu1, 0x800);
ARRAY (task_fpu2, 0x800);
ARRAY (task_fpu3, 0x800);
ARRAY (task_fpu4, 0x800);

ARRAY (stack_console_tswitch, 0x400);
ARRAY (stack_receiver_tswitch, 0x400);
mutex_t mailbox;

volatile unsigned nmessages;
volatile unsigned latency_min = ~0;
volatile unsigned latency_max;

task_t *t1, *t2, *t3, *t4, *t5, *t6, *t7, *t8;
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
	/* Do not allow optimizer to treat this function as const. */
	asm ("");

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
	/* Do not allow optimizer to treat this function as const. */
	asm ("");

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
	/* Do not allow optimizer to treat this function as const. */
	asm ("");

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
	/* Do not allow optimizer to treat this function as const. */
	asm ("");

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
void console_fpu (void *arg)
{
	/* Разрешаем FPU для текущей задачи.
	 * Округление к ближайшему целому, исключения отключены. */
	task_fpu_control (task_current, FCSR_ROUND_N, FCSR_ROUND | FCSR_ENABLES);

	for (;;) {
		printf (&uart_fpu, "\33[H");
		printf (&uart_fpu, "Testing FPU. UART%d\n\n", num_fpu);

		printf (&uart_fpu, "     Duration: %02u:%02u:%02u \n", timer_hour, timer_min, timer_sec);
		printf (&uart_fpu, "CPU frequency: %u MHz\n", KHZ / 1000);
		printf (&uart_fpu, "  SDRAM clock: %u MHz\n", MPORT_KHZ / 1000);
		printf (&uart_fpu, "   Reg.CSCON3: %08x  \n", MC_CSCON3);
#ifdef ENABLE_ICACHE
		printf (&uart_fpu, "  Instr.cache: enabled  \n");
#else
		printf (&uart_fpu, "  Instr.cache: disabled  \n");
#endif
#ifdef ENABLE_DCACHE
		printf (&uart_fpu, "   Data cache: enabled  \n");
#else
		printf (&uart_fpu, "   Data cache: disabled  \n");
#endif
		printf (&uart_fpu, "\n");
		printf (&uart_fpu, "---------Value-----------------Loops-------Errors----\n");
		printf (&uart_fpu, "     e:  %-20.15f  %-10u  %u \n", e, count_e_loops, count_e_errors);
		printf (&uart_fpu, "Machin:  %-20.15f  %-10u  %u \n", pi_machin, count_machin_loops, count_machin_errors);
		printf (&uart_fpu, " Gauss:  %-20.15f  %-10u  %u \n", pi_gauss, count_gauss_loops, count_gauss_errors);
		printf (&uart_fpu, " Brent:  %-20.15f  %-10u  %u \n", pi_brent, count_brent_loops, count_brent_errors);

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
		if (e != 0x2.b7e151628aed2p0)
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
		if (pi_machin != 0x3.243f6a8885a2ap0)
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
		if (pi_gauss != 0x3.243f6a8885a2ep0)
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
		if (pi_brent != 0x3.243f6a8885a34p0)
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
	} else if (t4->prio == 2) {
		t4->prio = 1;
		t5->prio = 2;
	} else if (t5->prio == 2) {
		t5->prio = 1;
		t6->prio = 2;
	} else if (t6->prio == 2) {
		t6->prio = 1;
		t7->prio = 2;
	} else if (t7->prio == 2) {
		t7->prio = 1;
		t8->prio = 2;
	} else {
		t8->prio = 1;
		t1->prio = 2;
	}

	/* Устанавливаем флаг: необходимо сменить текущую задачу. */
	task_need_schedule = 1;

	/* Возвращаем 1: прерывание полностью обработано. */
	return 1;
}

void hello (void *data)
{
	int i;
	for (i=0;;i++) {
		printf (&uart, "\n[%d] Hello, world! >> UART%d", i, num_uart);
		if ((i%10)==0) getchar (&uart);
		//udelay(DELAY_TASK);
	}
}

void hello2 (void *data)
{
	int i;
	for (i=0;;i++) {
		printf (&uart_tswitch, "\n[%d] Hello, world! >> UART%d", i, num_tswitch);
		if ((i%10)==0) getchar (&uart_tswitch);
		//udelay(DELAY_TASK);
	}
}

#ifdef ENABLE_DCACHE
#   define SDRAM_START	0x80000000
#   define CRAMS_START	0x98400000
#else
#   define SDRAM_START	0xA0000000
#   define CRAMS_START	0xb8400000
#endif

/* 64 Мб SDRAM */
#define SDRAM_SIZE	(64*1024*1024)

/* 2 Мб внутренней памяти (CRAMS) */
#define CRAMS_SIZE	(2*1024*1024)

extern void _etext();

MEM test_mem[]={
	{ 0x00000000, 0x55555555 },
	{ 0x00000004, 0x00000001 },
	{ 0x00000008, 0x00000002 },
	{ 0x00000010, 0x00000004 },
	{ 0x00000020, 0x00000008 },
	{ 0x00000040, 0x00000010 },
	{ 0x00000080, 0x00000020 },
	{ 0x00000100, 0x00000040 },
	{ 0x00000200, 0x00000080 },
	{ 0x00000400, 0x00000100 },
	{ 0x00000800, 0x00000200 },
	{ 0x00001000, 0x00000400 },
	{ 0x00002000, 0x00000800 },
	{ 0x00004000, 0x00001000 },
	{ 0x00008000, 0x00002000 },
	{ 0x00010000, 0x00004000 },
	{ 0x00020000, 0x00008000 },
	{ 0x00040000, 0x00010000 },
	{ 0x00080000, 0x00020000 },
	{ 0x00100000, 0x00040000 },
	{ 0x00200000, 0x00080000 },
	{ 0x00400000, 0x00100000 },
	{ 0x00800000, 0x00200000 },
	{ 0x01000000, 0x00400000 },
	{ 0x02000000, 0x00800000 },
	{ 0x00000000, 0x00000000 },
};

MEM test_mem_crams[]={
	{ 0x00000000, 0x55555555 },
	{ 0x00000004, 0x00000001 },
	{ 0x00000008, 0x00000002 },
	{ 0x00000010, 0x00000004 },
	{ 0x00000020, 0x00000008 },
	{ 0x00000040, 0x00000010 },
	{ 0x00000080, 0x00000020 },
	{ 0x00000100, 0x00000040 },
	{ 0x00000200, 0x00000080 },
	{ 0x00000400, 0x00000100 },
	{ 0x00000800, 0x00000200 },
	{ 0x00001000, 0x00000400 },
	{ 0x00002000, 0x00000800 },
	{ 0x00004000, 0x00001000 },
	{ 0x00008000, 0x00002000 },
	{ 0x00010000, 0x00004000 },
	{ 0x00020000, 0x00008000 },
	{ 0x00040000, 0x00010000 },
	{ 0x00080000, 0x00020000 },
	{ 0x00100000, 0x00040000 },
	{ 0x00000000, 0x00000000 },
};

unsigned word_check (unsigned addr, unsigned val)
{
	volatile unsigned *p = (unsigned*) addr;
	unsigned rval;

	*p = val;
	rval = *p;
	if (rval == val)
		return 0;
	printf (&uart_ram, "\nAddress %08X written %08X read %08X ",
		addr, val, rval);
	return 1;
}

void word_test (unsigned addr)
{
	unsigned i, n;

	printf (&uart_ram, "\nTesting address %08X... ", addr);
	for (i=0; ; ++i) {
		for (n=1; n; n<<=1) {
			word_check (addr, n);
			word_check (addr, ~n);
		}
		putchar (&uart_ram, "|/-\\" [i&3]);
		putchar (&uart_ram, '\b');
		if (peekchar (&uart_ram) >= 0) {
			getchar (&uart_ram);
			break;
		}
	}
	puts (&uart_ram, "done.\n");
}

void sdram_test (void)
{
	unsigned addr, nerrors;

	puts (&uart_ram, "\nTesting all SDRAM ");
	puts (&uart_ram, "--------------------------------");
	puts (&uart_ram, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=SDRAM_START; addr<SDRAM_START+SDRAM_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			puts (&uart_ram, "\nToo many errors, test stopped.\n");
			return;
		}
		/* Заменяем '-' на '.' каждые 2 Мб */
		if ((addr & 0x1ffffc) == 0x1ffffc)
			putchar (&uart_ram, '.');
	}
	puts (&uart_ram, " done.\n");
}

void crams_test (void)
{
	unsigned addr, nerrors;

	puts (&uart_ram, "\nTesting all CRAMS ");
	puts (&uart_ram, "--------------------------------");
	puts (&uart_ram, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	nerrors = 0;
	for (addr=CRAMS_START; addr<CRAMS_START+CRAMS_SIZE; addr+=4) {
		nerrors += word_check (addr, 0x55555555);
		nerrors += word_check (addr, 0xAAAAAAAA);
		if (nerrors > 20) {
			puts (&uart_ram, "\nToo many errors, test stopped.\n");
			return;
		}
		/* Заменяем '-' на '.' каждые 64 Кб */
		if ((addr & 0xfffc) == 0xfffc)
			putchar (&uart_ram, '.');
	}
	puts (&uart_ram, " done.\n");
}


void inline __attribute__ ((always_inline))
sdram_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (SDRAM_START + addr) = val;
}

void inline __attribute__ ((always_inline))
crams_write (unsigned addr, unsigned val)
{
	*(volatile unsigned*) (CRAMS_START + addr) = val;
}

static void
sdram_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (SDRAM_START + addr);
	if (rval != val)
		printf(&uart_ram, "\nAddress %08X written %08X read %08X ",
			SDRAM_START + addr, val, rval);
}

static void
crams_check (unsigned addr, unsigned val)
{
	unsigned rval;

	rval = *(volatile unsigned*) (CRAMS_START + addr);
	if (rval != val)
		printf(&uart_ram, "\nAddress %08X written %08X read %08X ",
			CRAMS_START + addr, val, rval);
}

void sdram_test_address_bus ()
{
	int i,j;

	printf (&uart_ram, "\nTesting SDRAM address signals ");
	puts (&uart_ram, "----------------");
	puts (&uart_ram, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		for (j=0;test_mem[j].addr|test_mem[j].val;j++) {
			sdram_write (test_mem[j].addr, test_mem[j].val);		
		};

		for (j=0;test_mem[j].addr|test_mem[j].val;j++) {
			sdram_check (test_mem[j].addr, test_mem[j].val);		
		};

		putchar (&uart_ram, '.');
	}
	puts (&uart_ram, " done.\n");
}

void crams_test_address_bus ()
{
	int i,j;

	printf (&uart_ram, "\nTesting CRAMS address signals ");
	puts (&uart_ram, "----------------");
	puts (&uart_ram, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	for (i=0; i<16; ++i) {
		for (j=0;test_mem_crams[j].addr|test_mem_crams[j].val;j++) {
			crams_write (test_mem_crams[j].addr, test_mem_crams[j].val);		
		};

		for (j=0;test_mem_crams[j].addr|test_mem_crams[j].val;j++) {
			crams_check (test_mem_crams[j].addr, test_mem_crams[j].val);		
		};

		putchar (&uart_ram, '.');
	}
	puts (&uart_ram, " done.\n");
}

void menu ()
{
	int cmd;

	for (;;) {
		printf (&uart_ram, "\33[H\33[2J");
		printf (&uart_ram, "\nTesting memory on LDE-Vega board\n");
		printf (&uart_ram, "Generator %d.%d MHz, CPU clock %d.%d MHz, bus clock %d.%d MHz\n",
			ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10,
			MPORT_KHZ/1000, MPORT_KHZ/100%10);

#ifdef ENABLE_ICACHE
		puts (&uart_ram, "Instruction cache enabled\n");
#else
		puts (&uart_ram, "Instruction cache disabled\n");
#endif
#ifdef ENABLE_DCACHE
		puts (&uart_ram, "Data cache enabled\n");
#else
		puts (&uart_ram, "Data cache disabled\n");
#endif
		printf (&uart_ram, "  CRPLL  = %08X\n", MC_CRPLL);
		printf (&uart_ram, "  CSCON0 = %08X\n", MC_CSCON0);
		printf (&uart_ram, "  CSCON3 = %08X\n", MC_CSCON3);
		printf (&uart_ram, "  SDRCON = %08X\n", MC_SDRCON);
		printf (&uart_ram, "  SDRTMR = %08X\n", MC_SDRTMR);
		printf (&uart_ram, "  SRTMR  = %08X\n", MC_SRTMR);

		printf(&uart_ram, "\n  1. Test SDRAM address signals");
		printf(&uart_ram, "\n  2. Test SDRAM address %08X", SDRAM_START + 0x02aaaaa8);
		printf(&uart_ram, "\n  3. Test SDRAM address %08X", SDRAM_START + 0x02555554);
		printf(&uart_ram, "\n  4. Test all SDRAM (%d Mbytes)", SDRAM_SIZE/1024/1024);
		printf(&uart_ram, "\n  5. Test CRAMS address signals");
		printf(&uart_ram, "\n  6. Test CRAMS address %08X", CRAMS_START + 0x001aaaa8);
		printf(&uart_ram, "\n  7. Test CRAMS address %08X", CRAMS_START + 0x00155554);
		printf(&uart_ram, "\n  8. Test all CRAMS (%d Mbytes)", CRAMS_SIZE/1024/1024);
		puts (&uart_ram, "\n\n");

		/* Ввод команды. */
		puts (&uart_ram, "Command: ");
		cmd = getchar (&uart_ram);
		putchar (&uart_ram, '\n');

		if (cmd == '\n' || cmd == '\r')
			break;

		if (cmd == '1') {
			sdram_test_address_bus ();
		}
		if (cmd == '2') {
			word_test (SDRAM_START + 0x02aaaaa8);
		}
		if (cmd == '3') {
			word_test (SDRAM_START + 0x02555554);
		}
		if (cmd == '4') {
			sdram_test ();
		}
		if (cmd == '5') {
			crams_test_address_bus ();
		}
		if (cmd == '6') {
			word_test (CRAMS_START + 0x001aaaa8);
		}
		if (cmd == '7') {
			word_test (CRAMS_START + 0x00155554);
		}
		if (cmd == '8') {
			crams_test ();
		}
		puts (&uart_ram, "Press any key");
		getchar (&uart_ram);
	}
}

void console_ram (void *data)
{
	for (;;)
		menu ();
}

/*
 * Печать рационального числа a/b с двумя знаками после запятой.
 */
void print_rational (char *title, unsigned a, unsigned b)
{
	unsigned val = a * 100 / b;

	printf (&uart_tswitch, "%s%u.%02u usec  \n", title, val/100, val%100);
}

/*
 * Задача выдачи статистики на консоль.
 */
int flag=0;
void console_tswitch (void *arg)
{
	unsigned t0;

	for (;;) {
		t0 = mips_read_c0_register (C0_COUNT);
		mutex_signal (&mailbox, (void*) t0);

		if (flag) return;
		flag=1;
		printf (&uart_tswitch, "\33[H");

		printf (&uart_tswitch, "Measuring task switch time. UART%d\n\n", num_tswitch);

		printf (&uart_tswitch, "CPU frequency: %u MHz\n", KHZ / 1000);
		printf (&uart_tswitch, "  SDRAM clock: %u MHz\n", MPORT_KHZ / 1000);
		printf (&uart_tswitch, "   Reg.CSCON3: %08x  \n", MC_CSCON3);
#ifdef ENABLE_ICACHE
		printf (&uart_tswitch, "  Instr.cache: enabled  \n");
#else
		printf (&uart_tswitch, "  Instr.cache: disabled  \n");
#endif
#ifdef ENABLE_DCACHE
		printf (&uart_tswitch, "   Data cache: enabled  \n");
#else
		printf (&uart_tswitch, "   Data cache: disabled  \n");
#endif
		printf (&uart_tswitch, "\n");
		printf (&uart_tswitch, "Task switches: %u  \n\n", nmessages);

		print_rational (" Latency, min: ", latency_min * 1000, KHZ);
		print_rational ("          max: ", latency_max * 1000, KHZ);
		flag=0;
	}
}

/*
 * Задача приёма сообщений.
 */
void receiver_tswitch (void *arg)
{
	unsigned t0, t1, latency;

	for (;;) {
		t0 = (unsigned) mutex_wait (&mailbox);
		t1 = mips_read_c0_register (C0_COUNT);

		/* Вычисляем количество тактов, затраченных на вход в прерывание. */
		latency = t1 - t0;

		/*printf (&uart, "<%u> ", latency);*/
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
	uart_init (&uart, num_uart, 90, KHZ, 115200);
	uart_init (&uart_ram, num_ram, 91, KHZ, 115200);
	uart_init (&uart_tswitch, num_tswitch, 92, KHZ, 115200);
	uart_init (&uart_fpu, num_fpu, 93, KHZ, 115200);

	/* Стираем экран. */
	printf (&uart, "\33[H\33[2J");
	printf (&uart_ram, "\33[H\33[2J");
	printf (&uart_tswitch, "\33[H\33[2J");
	printf (&uart_fpu, "\33[H\33[2J");

	printf (&uart_ram, "\nTesting memory on LDE-Vega board\n");
	printf (&uart_ram, "Generator %d.%d MHz, CPU clock %d.%d MHz, bus clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10,
		MPORT_KHZ/1000, MPORT_KHZ/100%10);

#ifdef ENABLE_ICACHE
	puts (&uart_ram, "Instruction cache enabled\n");
#else
	puts (&uart_ram, "Instruction cache disabled\n");
#endif
#ifdef ENABLE_DCACHE
	puts (&uart_ram, "Data cache enabled\n");
#else
	puts (&uart_ram, "Data cache disabled\n");
#endif

	/* Используем интервальный таймер в масштабе 1:1. */
	MC_ITCSR = 0;
	MC_ITSCALE = 0;
	MC_ITPERIOD = KHZ * MSEC_PER_TICK - 1;
	MC_ITCSR = MC_ITCSR_EN;
	mutex_attach_irq (&timer_lock, TIMER_IRQ, timer_handler, 0);

	/* Запускаем задачу выдачи статистики на консоль. */
	task_create (console_fpu, 0, "console_fpu", 10, stack_console_fpu, sizeof (stack_console_fpu));

	/* Запускаем задачи вычислений.
	 * По таймеру будем менять их приоритет, чтобы добиться
	 * большого количества переключений контекста. */
	t1 = task_create (test_fpu1, 0, "fpu", 2, task_fpu1, sizeof (task_fpu1));
	t2 = task_create (test_fpu2, 0, "fpu", 1, task_fpu2, sizeof (task_fpu2));
	t3 = task_create (test_fpu3, 0, "fpu", 1, task_fpu3, sizeof (task_fpu3));
	t4 = task_create (test_fpu4, 0, "fpu", 1, task_fpu4, sizeof (task_fpu4));
	t5 = task_create (hello, 0, "hello", 1, task_uart, sizeof (task_uart));
	t7 = task_create (hello2, 0, "hello2", 1, task_uart2, sizeof (task_uart2));
	t6 = task_create (console_ram, 0, "console_ram", 1, stack_console_ram, sizeof (stack_console_ram));
	/* Запускаем задачу выдачи статистики на консоль. */
	//t7 = task_create (console_tswitch, 0, "console_tswitch", 1, stack_console_tswitch, sizeof (stack_console_tswitch));
	/* Запускаем задачу приёма сообщений. */
	t8 = task_create (receiver_tswitch, 0, "receiver_tswitch", 1, stack_receiver_tswitch, sizeof (stack_receiver_tswitch));
}

