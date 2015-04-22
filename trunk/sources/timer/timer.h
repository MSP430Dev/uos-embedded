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
#define TIMER_MSEC_PER_DAY  (24UL*60*60*1000)
#define TIMER_USEC_PER_MSEC (1000UL)

struct _timer_t {
    mutex_t lock;
    mutex_t decisec;        /* every 0.1 second is signalled here */
    unsigned long khz;      /* reference clock */
    
#ifdef USEC_TIMER
    unsigned long usec_per_tick;
    unsigned long usec_in_msec;
#else
    small_uint_t msec_per_tick;
#endif

#if PIC32MX
    unsigned compare_step;
#endif
    unsigned long milliseconds; /* real time counter */
    unsigned long next_decisec; /* when next decisecond must be signalled */
    unsigned int days;      /* days counter */

#ifdef USER_TIMERS
    list_t user_timers;
#endif
};

#ifdef USER_TIMERS
struct _user_timer_t {
    list_t item;
    mutex_t lock;
#ifdef USEC_TIMER
	unsigned long usec_per_tick;
#else
    small_uint_t msec_per_tick;
#endif
    long int cur_time;
};
#endif

/**\~english
 * Data structure of a timer driver.
 *
 * \~russian
 * Структура данных для драйвера таймера.
 */
typedef struct _timer_t timer_t;

typedef struct _user_timer_t user_timer_t;


#ifdef USEC_TIMER
// Микросекудный таймер. Поддерживается пока только для миландровских контроллеров
void timer_init_us (timer_t *t, unsigned long khz, unsigned long usec_per_tick);
#else
void timer_init (timer_t *t, unsigned long khz, small_uint_t msec_per_tick);
#endif

#ifdef SW_TIMER
void timer_update (timer_t *t);
#endif

/* Delay current task. */
void timer_delay (timer_t *t, unsigned long msec);

/* Query real time. */
unsigned long timer_milliseconds (timer_t *t);
unsigned int timer_days (timer_t *t, unsigned long *milliseconds);
bool_t timer_passed (timer_t *t, unsigned long t0, unsigned int msec);
bool_t interval_greater_or_equal (long interval, long msec);

#ifdef USER_TIMERS
#ifdef USEC_TIMER
void user_timer_init_us (user_timer_t *ut, unsigned long usec_per_tick);
#else
void user_timer_init (user_timer_t *ut, small_uint_t msec_per_tick);
#endif
void user_timer_add (timer_t *t, user_timer_t *ut);
void user_timer_wait (user_timer_t *ut);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
