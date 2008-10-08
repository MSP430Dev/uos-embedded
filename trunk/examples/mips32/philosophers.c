/*
 * Five dining pfilosophers.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <uart/uart.h>
#include <random/rand15.h>

#define N		5		/* Number of philosophers */

ARRAY (task [N], 0x400);		/* Task space */
lock_t fork [N];			/* Forks */
lock_t table;				/* Broadcast communication */
lock_t screen;				/* Access to the screen */
timer_t timer;				/* Timer driver */
uart_t uart;				/* Console driver */

typedef enum {
	THINKING, EATING, WAITING,
} state_t;

const int xpos [] = { 35, 54, 47, 23, 16 };
const int ypos [] = {  3, 10, 20, 20, 10 };
int initialized = 0;

void display (int i, state_t state)
{
	lock_take (&screen);
	if (! initialized) {
		/* Clear screen. */
		puts (&uart, "\033[2J");
		initialized = 1;
	}
	printf (&uart, "\033[%d;%dH", ypos [i-1], xpos [i-1]);
	switch (state) {
	case THINKING:				/* Light blue */
		puts (&uart, "\033[1;34m");
		printf (&uart, "%d: thinking", i);
		break;
	case EATING:				/* Light green */
		puts (&uart, "\033[1;32m");
		printf (&uart, "%d: eating  ", i);
		break;
	case WAITING:				/* Light red */
		puts (&uart, "\033[1;31m");
		printf (&uart, "%d: waiting ", i);
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

	uart_init (&uart, 0, 90, KHZ, 115200);
	timer_init (&timer, 100, KHZ, 50);

	/* Create N philosopher tasks. */
	for (i=0; i<N; ++i)
		task_create (philosopher, (void*) (i+1), "phil", 1,
			task[i], sizeof (task[i]));
}
