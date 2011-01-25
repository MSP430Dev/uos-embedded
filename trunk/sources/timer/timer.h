#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**\~english
 * Number of milliseconds per day.
 *
 * \~russian
 * Количество миллисекунд в дне.
 */
#define TIMER_MSEC_PER_DAY	(24UL*60*60*1000)

struct _timer_t {
	mutex_t lock;
	mutex_t decisec;		/* every 0.1 second is signalled here */
	unsigned long khz;		/* reference clock */
	small_uint_t msec_per_tick;
#if PIC32MX
	unsigned compare_step;
#endif
	unsigned long milliseconds;	/* real time counter */
	unsigned long next_decisec;	/* when next decisecond must be signalled */
	unsigned int days;		/* days counter */
};

/**\~english
 * Data structure of a timer driver.
 *
 * \~russian
 * Структура данных для драйвера таймера.
 */
typedef struct _timer_t timer_t;

void timer_init (timer_t *t, unsigned long khz, small_uint_t msec_per_tick);

/* Delay current task. */
void timer_delay (timer_t *t, unsigned long msec);

/* Query real time. */
unsigned long timer_milliseconds (timer_t *t);
unsigned int timer_days (timer_t *t, unsigned long *milliseconds);
bool_t timer_passed (timer_t *t, unsigned long t0, unsigned int msec);
bool_t interval_greater_or_equal (long interval, long msec);

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
