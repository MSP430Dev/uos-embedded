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

/**\~english
 * Data structure for holding a time value.
 *
 * \~russian
 * Структура для хранения значения времени.
 */
typedef struct _timer_snap_t {
	unsigned long milliseconds;
	unsigned int days;
} timer_snap_t;

struct _timer_t {
	mutex_t lock;
	mutex_t decisec;		/* every 0.1 second is signalled here */
	unsigned long khz;		/* reference clock */
	small_uint_t msec_per_tick;
	unsigned long milliseconds;	/* real time counter */
	unsigned long last_decisec;	/* when decisecond was signalled */
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
bool_t timer_passed (timer_t *t, unsigned long t0, unsigned int msec);
unsigned int timer_days (timer_t *t);
bool_t interval_greater_or_equal (long interval, long msec);
void timer_snap (timer_t *timer, timer_snap_t *snap);

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
