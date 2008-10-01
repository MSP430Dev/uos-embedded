/** \namespace timer \brief \~english
 * Module "timer" - system time management.
 *
 * Full description of file timer.c.
 */

/** \namespace timer \brief \~russian
 * Модуль "timer" - управление системным временем.
 *
 * Полное описание файла timer.c.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

#if __MSDOS__
#   define TIMER_IRQ		0	/* IRQ0 */
#endif

#if I386
#   include <runtime/i386/i8253.h>
#   define TIMER_IRQ		0	/* IRQ0 */
#endif

#if __AVR__
#ifdef __AVR_ATmega2561__
#   define TIMER_IRQ		16	/* Timer 1 compare A */
#else
#   define TIMER_IRQ		11	/* Timer 1 compare A */
#endif  /*__AVR_ATmega2561__*/
#endif  /*__AVR__*/

#if ARM_S3C4530
#   define TIMER_IRQ		10	/* Timer 0 interrupt */
#endif

#if LINUX386
#   include <sys/time.h>
#   include <signal.h>
#   define TIMER_IRQ		SIGALRM
#endif

/**
 * Check that `msec' milliseconds have passed.
 * `Interval' is the time interval, probably rolled over the day.
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
 * System timer task.
 */
static void
main_timer (void *arg)
{
	timer_t *t = arg;

/*debug_printf ("main_timer\n");*/
	/* Request the IRQ. */
	lock_take_irq (&t->lock, TIMER_IRQ, 0, 0);

	/* Initialize the hardware. */
#if __AVR__
	outb (0, TCCR1A);
	outb (0, TCCR1B);
	outw ((t->khz * t->msec_per_tick) / 8 - 2, OCR1A);
	outw (0, TCNT1);
	outb (0x0A, TCCR1B);	/* clock source CK/8, clear on match A */
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
#if LINUX386
	{
	struct itimerval itv;

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = t->msec_per_tick * 1000L;
	itv.it_value = itv.it_interval;
	setitimer (ITIMER_REAL, &itv, 0);
	}
#endif
	for (;;) {
/*extern task_t *task_current; debug_printf ("t = %p, task_current = %p\n", t, task_current);*/
/*debug_printf ("milliseconds = %ld\n", t->milliseconds);*/
		lock_wait (&t->lock);
		t->milliseconds += t->msec_per_tick;
		if (t->milliseconds >= t->last_decisec + 100) {
			t->last_decisec = t->milliseconds;
/*debug_putchar (0, '~');*/
			lock_signal (&t->decisec, 0);
		}
		if (t->milliseconds >= TIMER_MSEC_PER_DAY) {
			++t->days;
			t->milliseconds -= TIMER_MSEC_PER_DAY;
			t->last_decisec -= TIMER_MSEC_PER_DAY;
		}
	}
}

/**
 * Return the (real) time in milliseconds since uOS start.
 */
unsigned long
timer_milliseconds (timer_t *t)
{
	unsigned long val;

	lock_take (&t->lock);
	val = t->milliseconds;
	lock_release (&t->lock);
	return val;
}

/**
 * Return the (real) time in milliseconds since uOS start.
 */
unsigned int
timer_days (timer_t *t)
{
	unsigned short val;

	lock_take (&t->lock);
	val = t->days;
	lock_release (&t->lock);
	return val;
}

/**
 * Return a valid snap of both days and milliseconds.
 */
void timer_snap (timer_t *t, timer_snap_t *v)
{
	lock_take (&t->lock);
	v->milliseconds = t->milliseconds;
	v->days = t->days;
	lock_release (&t->lock);
}

/**
 * Delay the current task by the given time in milliseconds.
 */
void
timer_delay (timer_t *t, unsigned long msec)
{
	unsigned long t0;

	lock_take (&t->lock);
	t0 = t->milliseconds;
	while (! interval_greater_or_equal (t->milliseconds - t0, msec)) {
		lock_wait (&t->lock);
	}
	lock_release (&t->lock);
}

/**
 * Check that `msec' milliseconds passed since the `t0' moment.
 */
bool_t
timer_passed (timer_t *t, unsigned long t0, unsigned int msec)
{
	unsigned long now;

	lock_take (&t->lock);
	now = t->milliseconds;
	lock_release (&t->lock);

	return interval_greater_or_equal (now - t0, msec);
}

/**
 * Create timer task.
 */
void
timer_init (timer_t *t, int prio, unsigned long khz,
	small_uint_t msec_per_tick)
{
	t->msec_per_tick = msec_per_tick;
	t->khz = khz;
	task_create (main_timer, t, "timer", prio, t->stack, sizeof (t->stack));
}
