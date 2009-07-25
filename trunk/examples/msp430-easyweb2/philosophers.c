/*
 * Five dining pfilosophers.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <random/rand15.h>
#include <lcd2/lcd.h>
#include "msp430-easyweb2.h"

#define NPHIL	5			/* Number of philosophers */

timer_t timer;				/* Timer driver */
lcd_t line1, line2;
ARRAY (task [NPHIL], 200);		/* Task space */
mutex_t fork [NPHIL];			/* Forks */
mutex_t table;				/* Broadcast communication */
mutex_t screen;				/* Access to the screen */
int sound_disabled;
int button_pressed;

typedef enum {
	THINKING, EATING, WAITING,
} state_t;

void display (int i, state_t state)
{
	static int initialized = 0;
	lcd_t *line;

	mutex_lock (&screen);
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
	mutex_unlock (&screen);
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

void get_forks (int i, mutex_t *left_fork, mutex_t *right_fork)
{
	for (;;) {
		mutex_lock (left_fork);
		if (mutex_trylock (right_fork))
			return;
		mutex_unlock (left_fork);
		display (i, WAITING);
		mutex_wait (&table);
	}
}

void put_forks (mutex_t *left_fork, mutex_t *right_fork)
{
	mutex_unlock (left_fork);
	mutex_unlock (right_fork);
	mutex_signal (&table, 0);

	/* Click when forks released. */
	if (! sound_disabled) {
		buzzer_control (1);
		udelay (125);
		buzzer_control (-1);
		udelay (125);
		buzzer_control (0);
	}

	/* Button 1: enable/disable sound. */
	if (button1_pressed ()) {
		if (! button_pressed) {
			button_pressed = 1;
			sound_disabled = ! sound_disabled;
			led_control (1);
		}
	} else if (button_pressed) {
		button_pressed = 0;
		led_control (0);
	}
}

void philosopher (void *data)
{
	int i;

	i = (int) data;
	for (;;) {
		think (i);
		get_forks (i, &fork [i], &fork [(i+1) % NPHIL]);
		eat (i);
		put_forks (&fork [i], &fork [(i+1) % NPHIL]);
	}
}

void uos_init ()
{
	int i;

	timer_init (&timer, KHZ, 10);
	lcd_init (&line1, &line2, 0);
	buzzer_init ();
	led_init ();

	/* Create NPHIL philosopher tasks. */
	for (i=0; i<NPHIL; ++i)
		task_create (philosopher, (void*) i, "phil", 1,
			task[i], sizeof (task[i]));
}
