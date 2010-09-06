/*
 * Пять обедающих философов.
 *
 * Пятеро человек сидят за круглым столом.
 * Перед ними - тарелки, а между тарелками - пять вилок.
 * Чтобы кушать, каждому нужно две вилки.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>
#include <random/rand15.h>

#define N	5			/* Number of philosophers */

gpanel_t display;			/* LCD driver */
timer_t timer;				/* Timer driver */
ARRAY (task [N], 1000);			/* Task space */
mutex_t fork [N];			/* Forks */
mutex_t table;				/* Broadcast communication */
mutex_t screen;				/* Access to the screen */

typedef enum {
	THINKING, EATING, WAITING,
} state_t;

void show (int i, state_t state)
{
	static const int xpos [] = { 46, 85, 70, 14,  0 };
	static const int ypos [] = {  0, 28, 56, 56, 28 };
//	static const int xpos [] = { 30, 85, 85, 30,  0 };
//	static const int ypos [] = {  0, 19, 38, 56, 28 };
	static int initialized = 0;

	mutex_lock (&screen);
	if (! initialized) {
		gpanel_clear (&display, 0);			/* Clear screen. */
		initialized = 1;
	}
	gpanel_move (&display, xpos [i], ypos [i]);
	switch (state) {
	case THINKING:
		printf (&display, "%d:Думаю", i+1);
		break;
	case EATING:
		printf (&display, "%d:Ем   ", i+1);
		break;
	case WAITING:
		printf (&display, "%d:Жду  ", i+1);
		break;
	}
	mutex_unlock (&screen);
}

void think (int i)
{
	show (i, THINKING);
	timer_delay (&timer, rand15() % 2000);
}

void eat (int i)
{
	show (i, EATING);
	timer_delay (&timer, rand15() % 2000);
}

void get_forks (int i, mutex_t *left_fork, mutex_t *right_fork)
{
	for (;;) {
		mutex_lock (left_fork);
		if (mutex_trylock (right_fork))
			return;
		mutex_unlock (left_fork);
		show (i, WAITING);
		mutex_wait (&table);
	}
}

void put_forks (mutex_t *left_fork, mutex_t *right_fork)
{
	mutex_unlock (left_fork);
	mutex_unlock (right_fork);
	mutex_signal (&table, 0);
}

void philosopher (void *data)
{
	int i;

	i = (int) data;
	for (;;) {
		think (i);
		get_forks (i, &fork [i], &fork [(i+1) % N]);
		eat (i);
		put_forks (&fork [i], &fork [(i+1) % N]);
	}
}

void uos_init ()
{
	int i;
	extern gpanel_font_t font_fixed6x8;

	gpanel_init (&display, &font_fixed6x8);

	timer_init (&timer, KHZ, 50);

	/* Create N philosopher tasks. */
	for (i=0; i<N; ++i)
		task_create (philosopher, (void*) i, "phil", 1,
			task[i], sizeof (task[i]));
}
