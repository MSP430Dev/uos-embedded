/*
 * Five dining philosophers: a classical problem of concurrent synchronization.
 * http://en.wikipedia.org/wiki/Dining_philosophers
 *
 * Five silent philosophers sit at a table around a bowl of spaghetti.
 * A fork is placed between each pair of adjacent philosophers.
 * Each philosopher must alternately think and eat. However, a philosopher
 * can only eat when he has both left and right forks.
 * Each fork can be held by only one philosopher and so a philosopher can
 * use the fork only if it's not being used by another philosopher.
 * After he finishes eating, he needs to put down both forks so they become
 * available to others. A philosopher can grab the fork on his right or
 * the one on his left as they become available, but can't start eating
 * before getting both of them.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <gpanel/gpanel.h>
#include <timer/timer.h>
#include <random/rand15.h>
#include "shield-lcd4884.h"
#include "devcfg.h"

#define N    5                  /* Number of philosophers */

gpanel_t display;               /* LCD driver */
timer_t timer;                  /* Timer driver */
ARRAY (task [N], 1000);         /* Task space */
mutex_t fork [N];               /* Forks */
mutex_t table;                  /* Broadcast communication */
mutex_t screen;                 /* Access to the screen */

typedef enum {
    THINKING, EATING, WAITING,
} state_t;

/*
 * Display the state of a philosopher.
 */
void show (int i, state_t state)
{
    static const int xpos [] = { 25, 48, 48,  0,  0 };
    static const int ypos [] = {  0, 20, 40, 40, 20 };
    static int initialized = 0;

    mutex_lock (&screen);
    if (! initialized) {
        gpanel_clear (&display, 0);     /* Clear screen. */
        initialized = 1;
    }
    gpanel_move (&display, xpos [i], ypos [i]);
    switch (state) {
    case THINKING:
        printf (&display, "%d:Thnk", i+1);
        break;
    case EATING:
        printf (&display, "%d:Eat ", i+1);
        break;
    case WAITING:
        printf (&display, "%d:Wait", i+1);
        break;
    }
    mutex_unlock (&screen);
}

/*
 * 'Think' for a random amount of time.
 */
void think (int i)
{
    show (i, THINKING);
    timer_delay (&timer, rand15() % 2000);
}

/*
 * 'Eat' for a random amount of time.
 */
void eat (int i)
{
    show (i, EATING);
    timer_delay (&timer, rand15() % 2000);
}

/*
 * Get left and right forks.
 */
void get_forks (int i, mutex_t *left_fork, mutex_t *right_fork)
{
    for (;;) {
        mutex_lock (left_fork);         /* Get left fork */
        if (mutex_trylock (right_fork)) /* Try to get right fork... */
            return;                     /* ...succeeded */
        mutex_unlock (left_fork);       /* Release left fork */
        show (i, WAITING);
        mutex_wait (&table);            /* Wait for a signal from others */
    }
}

/*
 * Put forks on a table.
 */
void put_forks (mutex_t *left_fork, mutex_t *right_fork)
{
    mutex_unlock (left_fork);           /* Release left fork */
    mutex_unlock (right_fork);          /* Release right fork */
    mutex_signal (&table, 0);           /* Send a signal to others */
}

/*
 * Behavior of a philisopher: think and eat.
 */
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

    led_init();
    joystick_init ();
    gpanel_init (&display, &font_fixed6x8);
    gpanel_clear (&display, 0);
    gpanel_backlight (&display, 1);

    timer_init (&timer, KHZ, 50);

    /* Create N philosopher tasks. */
    for (i=0; i<N; ++i)
        task_create (philosopher, (void*) i, "phil", 1,
            task[i], sizeof (task[i]));
}
