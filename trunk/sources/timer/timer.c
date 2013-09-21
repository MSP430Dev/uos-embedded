/**\namespace timer \brief \~english
 * Module "timer" - system time management.
 *
 * Full description of file timer.c.
 */

/**\namespace timer \brief \~russian
 * Модуль "timer" - управление системным временем.
 *
 * Полное описание файла timer.c.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <timer/timer.h>

#if I386
#   include <runtime/i386/i8253.h>
#   define TIMER_IRQ		0	/* IRQ0 */
#endif

#if __AVR__				/* Timer 1 compare A */
#   ifdef __AVR_ATmega2561__
#      define TIMER_IRQ		16
#   endif
#   if defined (__AVR_ATmega103__) || defined (__AVR_ATmega128__)
#      define TIMER_IRQ		11
#   endif
#   ifdef __AVR_ATmega161__
#      define TIMER_IRQ		6
#   endif
#   ifdef __AVR_ATmega168__
#      define TIMER_IRQ		10
#   endif
#endif  /*__AVR__*/

#if ARM_S3C4530
#   define TIMER_IRQ		10	/* Timer 0 interrupt */
#endif

#if ARM_AT91SAM
#   define TIMER_IRQ	AT91C_ID_SYS
#endif

#if ARM_OMAP44XX
#   define TIMER_IRQ		29	/* Private Timer interrupt */
#endif

#if ELVEES_MC24
#   define TIMER_IRQ		29	/* Interval Timer interrupt */
#endif

#if ELVEES_MC0226
#   define TIMER_IRQ		29	/* Interval Timer interrupt */
#endif

#if ELVEES_MC24R2
#   define TIMER_IRQ		22	/* Interval Timer interrupt */
#endif

#if ELVEES_NVCOM01
#   define TIMER_IRQ		22	/* Interval Timer interrupt */
#endif

#if ELVEES_NVCOM02
#   define TIMER_IRQ		22	/* TODO: Interval Timer interrupt */
#endif

#if ELVEES_MCT02
#   define TIMER_IRQ		22	/* Interval Timer interrupt */
#endif

#if PIC32MX
#   define TIMER_IRQ	PIC32_VECT_CT	/* Core Timer Interrupt */
#endif

#if ARM_STM32F4
#   define TIMER_IRQ		82	/* Systick */
#endif

#if ARM_1986BE9
#   define TIMER_IRQ		32	/* Systick */
#endif

#if ARM_1986BE1
#	if (ARM_SYS_TIMER==1)
#		define TIMER_IRQ	TIMER1_IRQn
#		define SYS_TIMER	ARM_TIMER1
#		define PER_CLOCK_EN	ARM_PER_CLOCK_TIMER1
#		define TIM_CLK_EN	ARM_TIM_CLOCK_EN1
#	elif (ARM_SYS_TIMER==2)
#		define TIMER_IRQ	TIMER2_IRQn
#		define SYS_TIMER	ARM_TIMER2
#		define PER_CLOCK_EN	ARM_PER_CLOCK_TIMER2
#		define TIM_CLK_EN	ARM_TIM_CLOCK_EN2
#	elif (ARM_SYS_TIMER==3)
#		define TIMER_IRQ	TIMER3_IRQn
#		define SYS_TIMER	ARM_TIMER3
#		define PER_CLOCK_EN	ARM_PER_CLOCK_TIMER3
#		define TIM_CLK_EN	ARM_TIM_CLOCK_EN3
#	elif (ARM_SYS_TIMER==4)
#		define TIMER_IRQ	TIMER4_IRQn
#		define SYS_TIMER	ARM_TIMER4
#		define PER_CLOCK_EN	ARM_PER_CLOCK_TIMER4
#		define TIM_CLK_EN	ARM_UART_CLOCK_TIM4_EN
#	else
#		warning "ARM_SYS_TIMER is not defined in CFLAGS (target.cfg). Using TIMER1 for system timer."
#		define TIMER_IRQ	TIMER1_IRQn
#		define SYS_TIMER	ARM_TIMER1
#		define PER_CLOCK_EN	ARM_PER_CLOCK_TIMER1
#		define TIM_CLK_EN	ARM_TIM_CLOCK_EN1
#	endif
#endif

#if MSP430
#   ifdef TIMERA0_VECTOR
#      define TIMER_IRQ		(TIMERA0_VECTOR / 2)
#   else
#      define TIMER_IRQ		(TIMER0_A0_VECTOR / 2)
#      define TACTL 		TA0CTL
#      define TACCR0 		TA0CCR0
#      define TAEX0 		TA0EX0
#   endif
#endif

#if LINUX386
#   include <sys/time.h>
#   include <signal.h>
#   define TIMER_IRQ		SIGALRM
#endif

/**\~english
 * Check that `msec' milliseconds have passed.
 * `Interval' is the time interval, probably rolled over the day.
 *
 * \~russian
 * Проверка, прошло ли указанное количество миллисекунд `msec'.
 * Параметр `interval' содержит интервал времени, возможно, переходящий границу суток.
 */
small_int_t
interval_greater_or_equal (long interval, long msec)
{
	if (interval < 0)
		interval += TIMER_MSEC_PER_DAY;
	else if (interval >= TIMER_MSEC_PER_DAY)
		interval -= TIMER_MSEC_PER_DAY;
	return (interval >= msec);
}

/*
 * Timer interrupt handler.
 */
bool_t
timer_handler (timer_t *t)
{
/*debug_printf ("<ms=%ld> ", t->milliseconds);*/
#if defined (ELVEES)
	/* Clear interrupt. */
	MC_ITCSR &= ~MC_ITCSR_INT;
#endif
#if ARM_AT91SAM
	/* Clear interrupt. */
	*AT91C_PITC_PIVR;
#endif
#if ARM_OMAP44XX
	/* Clear interrupt. */
	ARM_PRT_INT_STATUS = ARM_PRT_EVENT;
#endif
#if ARM_1986BE1
	SYS_TIMER->TIM_STATUS &= ~ARM_TIM_CNT_ARR_EVENT;
#endif
#if PIC32MX
	/* Increment COMPARE register. */
	unsigned compare = mips_read_c0_register (C0_COMPARE);
	do {
		compare += t->compare_step;
		mips_write_c0_register (C0_COMPARE, compare);
	} while ((int) (compare - mips_read_c0_register (C0_COUNT)) < 0);
#endif

	/* Increment current time. */
	t->milliseconds += t->msec_per_tick;
	if (t->milliseconds >= TIMER_MSEC_PER_DAY) {
		++t->days;
		t->milliseconds -= TIMER_MSEC_PER_DAY;
		t->next_decisec -= TIMER_MSEC_PER_DAY;
	}

	/* Send signal every 100 msec. */
	if (t->msec_per_tick <= 100 &&
	    t->milliseconds >= t->next_decisec) {
		t->next_decisec += 100;
/*debug_printf ("<ms=%lu,nxt=%lu> ", t->milliseconds, t->next_decisec);*/
		if (! list_is_empty (&t->decisec.waiters) ||
		    ! list_is_empty (&t->decisec.groups)) {
			mutex_activate (&t->decisec,
				(void*) (size_t) t->milliseconds);
		}
	}
	
#ifdef USER_TIMERS
	if (! list_is_empty (&t->user_timers)) {
		user_timer_t *ut;
		list_iterate (ut, &t->user_timers) {
			ut->cur_time -= t->msec_per_tick;
			if (ut->cur_time <= 0) {
				if (! list_is_empty (&ut->lock.waiters) ||
		    		    ! list_is_empty (&t->decisec.groups)) {
					mutex_activate (&ut->lock,
						(void*) (size_t) t->milliseconds);
					ut->cur_time += ut->msec_per_tick;
				}
			}
		}
	}
#endif
	
	arch_intr_allow (TIMER_IRQ);

	/* Must signal a lock, for timer_wait().
	 * TODO: Optimize timer_delay, keeping a sorted
	 * queue of delay values. */
	return 0;
}

/**\~english
 * Return the (real) time in milliseconds since uOS start.
 *
 * \~russian
 * Запрос времени в миллисекундах.
 */
unsigned long
timer_milliseconds (timer_t *t)
{
	unsigned long val;

	mutex_lock (&t->lock);
	val = t->milliseconds;
	mutex_unlock (&t->lock);
	return val;
}

/**\~english
 * Return the (real) time in days and milliseconds since uOS start.
 *
 * \~russian
 * Запрос времени в сутках и миллисекундах.
 */
unsigned int
timer_days (timer_t *t, unsigned long *milliseconds)
{
	unsigned short val;

	mutex_lock (&t->lock);
	if (milliseconds)
		*milliseconds = t->milliseconds;
	val = t->days;
	mutex_unlock (&t->lock);
	return val;
}

/**\~english
 * Delay the current task by the given time in milliseconds.
 *
 * \~russian
 * Задержка выполнения текущей задачи.
 */
void
timer_delay (timer_t *t, unsigned long msec)
{
	unsigned long t0;

	mutex_lock (&t->lock);
	t0 = t->milliseconds;
	while (! interval_greater_or_equal (t->milliseconds - t0, msec)) {
		mutex_wait (&t->lock);
	}
	mutex_unlock (&t->lock);
}

/**\~english
 * Check that `msec' milliseconds passed since the `t0' moment.
 *
 * \~russian
 * Проверка временного события.
 */
bool_t
timer_passed (timer_t *t, unsigned long t0, unsigned int msec)
{
	unsigned long now;

	mutex_lock (&t->lock);
	now = t->milliseconds;
	mutex_unlock (&t->lock);

	return interval_greater_or_equal (now - t0, msec);
}

/**\~english
 * Create timer task.
 *
 * \~russian
 * Инициализация таймера.
 */
void
timer_init (timer_t *t, unsigned long khz, small_uint_t msec_per_tick)
{
	t->msec_per_tick = msec_per_tick;
	t->khz = khz;

	/* Attach fast handler to timer interrupt. */
	mutex_attach_irq (&t->lock, TIMER_IRQ, (handler_t) timer_handler, t);

	/* Initialize the hardware. */
#if __AVR__
	TCCR1A = 0;
	TCCR1B = 0;
	OCR1A = (t->khz * t->msec_per_tick) / 8 - 2;
	TCNT1 = 0;
	TCCR1B = 0x0A;	/* clock source CK/8, clear on match A */
#endif
#if I386
	{
	/* For I386, t->khz contains actually Hz (=1193182). */
	unsigned short count = (t->khz * t->msec_per_tick + 500) / 1000;
	outb (I8253_MODE_SEL0 | I8253_MODE_RATEGEN | I8253_MODE_16BIT,
		I8253_MODE);
	outb (count & 0xff, I8253_CNTR0);
	outb (count >> 8, I8253_CNTR0);
	}
#endif
#if ARM_S3C4530
	/* Use timer 0 in toggle mode. */
	ARM_TMOD &= ~(ARM_TMOD_TE0 | ARM_TMOD_TCLR0);
	ARM_TDATA(0) = (t->khz * t->msec_per_tick) - 1;
	ARM_TCNT(0) = 0;
	ARM_TMOD |= ARM_TMOD_TE0 | ARM_TMOD_TMD0;
#endif
#if ARM_AT91SAM
	*AT91C_PITC_PIMR = (((t->khz * t->msec_per_tick + 8) >> 4) - 1) |
		AT91C_PITC_PITEN | AT91C_PITC_PITIEN;
#endif
#if ARM_OMAP44XX
	ARM_PRT_LOAD = khz * msec_per_tick - 1;
	ARM_PRT_CONTROL = ARM_PRT_TIMER_EN | ARM_PRT_AUTO_RELOAD | 
		ARM_PRT_IRQ_EN;
#endif
#if PIC32MX
	/* Use core timer. */
	unsigned count = mips_read_c0_register (C0_COUNT);
	t->compare_step = (t->khz * t->msec_per_tick + 1) / 2;
	mips_write_c0_register (C0_COMPARE, count + t->compare_step);
#endif
#if defined (ELVEES)
	/* Use interval timer with prescale 1:1. */
	MC_ITCSR = 0;
	MC_ITSCALE = 0;
	MC_ITPERIOD = t->khz * t->msec_per_tick - 1;
	MC_ITCSR = MC_ITCSR_EN;
#endif
#if ARM_CORTEX_M3 || ARM_CORTEX_M4
	ARM_SYSTICK->CTRL = 0;
	ARM_SYSTICK->VAL = 0;
#ifdef SETUP_HCLK_HSI
	/* Max 2130 msec/tick at 8 MHz. */
	ARM_SYSTICK->LOAD = 8000 * t->msec_per_tick - 1;
#else
	/* Max 213 msec/tick at 80 MHz. */
	ARM_SYSTICK->LOAD = t->khz * t->msec_per_tick - 1;
#endif
	ARM_SYSTICK->CTRL = ARM_SYSTICK_CTRL_ENABLE |
			    ARM_SYSTICK_CTRL_TICKINT |
			    ARM_SYSTICK_CTRL_HCLK;
#endif
#if ARM_1986BE1
	ARM_RSTCLK->PER_CLOCK |= PER_CLOCK_EN;
#if (ARM_SYS_TIMER==4)
	ARM_RSTCLK->UART_CLOCK |= TIM_CLK_EN;
#else
	ARM_RSTCLK->TIM_CLOCK |= TIM_CLK_EN;
#endif
	SYS_TIMER->TIM_CNT = 0;
	SYS_TIMER->TIM_PSG = 0;
	SYS_TIMER->TIM_ARR = t->khz * t->msec_per_tick - 1;
	SYS_TIMER->TIM_IE = ARM_TIM_CNT_ARR_EVENT_IE;
	SYS_TIMER->TIM_CNTRL = ARM_TIM_CNT_EN;
#endif
#if MSP430
	{
	unsigned long divider = t->msec_per_tick * t->khz;
#ifdef TAEX0
	/* Setup divider expansion. */
	small_uint_t nx = (divider - 4) >> (16 + 3);
	assert (nx <= 7);
	TAEX0 = nx;
	if (nx)
		divider /= nx + 1;
#endif
	TACTL = TACLR;				/* Stop timer. */
	TACTL = TASSEL_SMCLK | ID_DIV8;		/* Source clock SMCLK divided by 8. */
	divider = (divider + 4) >> 3;
	assert (divider <= 65536);
	TACCR0 = divider - 1;			/* Set tick rate. */
	TACTL |= MC_1;				/* Start timer in up mode. */
	}
#endif
#if LINUX386
	{
	struct itimerval itv;

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = t->msec_per_tick * 1000L;
	itv.it_value = itv.it_interval;
	setitimer (ITIMER_REAL, &itv, 0);
	}
#endif

#ifdef USER_TIMERS
	list_init (&t->user_timers);
#endif
}

#ifdef USER_TIMERS
void user_timer_init (user_timer_t *ut, small_uint_t msec_per_tick)
{
	ut->msec_per_tick = msec_per_tick;
	ut->cur_time = msec_per_tick;
	list_init (&ut->item);
}

void user_timer_add (timer_t *t, user_timer_t *ut)
{
	mutex_lock (&t->lock);
	list_append (&t->user_timers, &ut->item);
	mutex_unlock (&t->lock);
}

void user_timer_wait (user_timer_t *ut)
{
	mutex_wait (&ut->lock);
}
#endif

