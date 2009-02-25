/*
 * Five dining pfilosophers.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <random/rand15.h>
#include "lcd.h"
#include "avr-mt-128.h"

#define N	5			/* Number of philosophers */

timer_t timer;				/* Timer driver */
lcd_t line1, line2;
ARRAY (task [N], 280);			/* Task space */
lock_t fork [N];			/* Forks */
lock_t table;				/* Broadcast communication */
lock_t screen;				/* Access to the screen */
int sound_disabled;
int button_pressed;

typedef enum {
	THINKING, EATING, WAITING,
} state_t;

void display (int i, state_t state)
{
	static int initialized = 0;
	lcd_t *line;

	lock_take (&screen);
	if (! initialized) {
		lcd_clear_all (&line1, &line2);			/* Clear screen. */
		initialized = 1;
	}
	line = (i & 1) ? &line2 : &line1;
	lcd_move (line, i*3);
	switch (state) {
	case THINKING:
		printf (line, " %d ", i+1);
		break;
	case EATING:
		printf (line, "*%d*", i+1);
		break;
	case WAITING:
		printf (line, ".%d.", i+1);
		break;
	}
	lock_release (&screen);
}

void think (int i)
{
	display (i, THINKING);
	timer_delay (&timer, 1000 + rand15() % 2000);
}

void eat (int i)
{
	display (i, EATING);
	timer_delay (&timer, 1000 + rand15() % 2000);
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

	/* Click when forks released. */
	if (! sound_disabled) {
		buzzer_control (1);
		udelay (125);
		buzzer_control (-1);
		udelay (125);
		buzzer_control (0);
	}

	/* Down button: enable/disable sound. */
	if (button_down_pressed ()) {
		if (! button_pressed) {
			button_pressed = 1;
			sound_disabled = ! sound_disabled;
		}
	} else
		button_pressed = 0;
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

	timer_init (&timer, 100, KHZ, 50);
	lcd_init (&line1, &line2, &timer);
	buzzer_init ();

	/* Create N philosopher tasks. */
	for (i=0; i<N; ++i)
		task_create (philosopher, (void*) i, "phil", 1,
			task[i], sizeof (task[i]));
}
