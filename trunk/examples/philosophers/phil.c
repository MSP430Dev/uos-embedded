/*
 * Five dining pfilosophers.
 */
#include "demo.h"
#include <random/rand15.h>

#ifdef LINUX386
#define STACKSZ 	6000		/* Task stack size */
#else
#define STACKSZ 	390		/* Task stack size */
#endif
#define N		5		/* Number of philosophers */

char task [N] [STACKSZ];		/* Task space */
lock_t fork [N];			/* Forks */
lock_t table;				/* Broadcast communication */
lock_t screen;				/* Access to the screen */
timer_t timer;				/* Timer driver */
char initialized;

typedef enum {
	THINKING, EATING, WAITING,
} state_t;

void display (int i, state_t state)
{
#ifdef __MSDOS__
	if (debug_peekchar() >= 0) {
		debug_getchar();
		uos_halt ();
	}
#endif
	lock_take (&screen);
	if (! initialized) {
		clrscr ();
		initialized = 1;
	}
	switch (i) {
	case 1: gotoxy (35, 3);  break;
	case 2: gotoxy (54, 10); break;
	case 3: gotoxy (47, 20); break;
	case 4: gotoxy (23, 20); break;
	case 5: gotoxy (16, 10); break;
	}
	switch (state) {
	case THINKING:
		textattr (LIGHTBLUE);
		PRINTF ("%d: thinking", i);
		break;
	case EATING:
		textattr (LIGHTGREEN);
		PRINTF ("%d: eating  ", i);
		break;
	case WAITING:
		textattr (LIGHTRED);
		PRINTF ("%d: waiting ", i);
		break;
	}
	lock_release (&screen);
}

void think (int i)
{
	display (i, THINKING);
	timer_delay (&timer, rand15() % 2000);
}

void eat (int i)
{
	display (i, EATING);
	timer_delay (&timer, rand15() % 2000);
}

void get_forks (int i, lock_t *left_fork, lock_t *right_fork)
{
	for (;;) {
		lock_take (left_fork);
		if (lock_try (right_fork))
			return;
		lock_release (left_fork);
		display (i, WAITING);
		lock_wait (&table);
	}
}

void put_forks (lock_t *left_fork, lock_t *right_fork)
{
	lock_release (left_fork);
	lock_release (right_fork);
	lock_signal (&table, 0);
}

void philosopher (void *data)
{
	int i;

	i = (int) data;
	for (;;) {
		think (i);
		get_forks (i, &fork[i-1], &fork[i%N]);
		eat (i);
		put_forks (&fork[i-1], &fork[i%N]);
	}
}

void uos_init ()
{
	int i;

#if defined (__AVR__) || defined (ARM_SAMSUNG)
	uart_init (&uart, 0, 90, KHZ, 9600);
#endif
	timer_init (&timer, 100, KHZ, TIMER_MSEC);

	/* Create N philosopher tasks. */
	for (i=0; i<N; ++i)
		task_create (philosopher, (void*) (i+1), "phil", 1,
			task[i], STACKSZ, 0);
}
