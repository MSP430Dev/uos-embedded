#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** \def TIMER_STACKSZ
 * \~english
 * Size of stack for timer task in bytes.
 *
 * \~russian
 * Размер стека для задачи таймера, в байтах.
 */
#ifndef TIMER_STACKSZ
#   if __AVR__
#      define TIMER_STACKSZ	256
#   endif
#   if I386
#      define TIMER_STACKSZ	0x400
#   endif
#   if MIPS32
#      define TIMER_STACKSZ	600
#   endif
#   if defined (__arm__) || defined (__thumb__)
#      define TIMER_STACKSZ	300
#   endif
#endif
#ifndef TIMER_STACKSZ
#   define TIMER_STACKSZ	6000	/* LINUX386 */
#endif

/** \~english
 *	Number of milliseconds per day.
 * \~russian
 *	Количество миллисекунд в дне.
 */
#define TIMER_MSEC_PER_DAY	(24UL*60*60*1000)

/**
 * Data structure for holding a time value.
 */
typedef struct _timer_snap_t {
	unsigned long milliseconds;
	unsigned int days;
} timer_snap_t;

struct _timer_t {
	lock_t lock;
	lock_t decisec;			/* every 0.1 second is signalled here */
	unsigned long khz;		/* reference clock */
	small_uint_t msec_per_tick;
	unsigned long milliseconds;	/* real time counter */
	unsigned long last_decisec;	/* when decisecond was signalled */
	unsigned int days;		/* days counter */
	ARRAY (stack, TIMER_STACKSZ);	/* task stack */
};

/**
 * Data structure of a timer driver.
 */
typedef struct _timer_t timer_t;

void timer_init (timer_t *t, int prio, unsigned long khz,
	small_uint_t msec_per_tick);

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
