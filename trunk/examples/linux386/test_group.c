/*
 * Testing groups.
 */
#include "runtime/lib.h"
#include "kernel/uos.h"
#include "timer/timer.h"
#include "random/rand15.h"

#include <time.h>

timer_t timer;
lock_t A, B, C;
ARRAY (waiter, 6000);
ARRAY (sender1, 6000);
ARRAY (sender3, 6000);
ARRAY (group, sizeof(lock_group_t) + 4 * sizeof(lock_slot_t));

/*
 * Listener task. Get a lock group as an argument.
 * Waits for signals on the group, and prints them.
 */
void main_waiter (void *arg)
{
	lock_group_t *g = arg;
	lock_t *m;
	void *msg;

	for (;;) {
		lock_group_wait (g, &m, &msg);
		debug_printf ("Time %d, message %s\n",
			timer_milliseconds (&timer), msg ? msg : "Timer");
	}
}

/*
 * Sender task. Argument is a priority value.
 * Randomly sends signals (text messages) to locks A, B and C.
 */
void main_sender (void *arg)
{
	int prio = (int) arg;
	unsigned char msgA [8], msgB [8], msgC [8];

	strcpy (msgA, (unsigned char*) "A-0"); msgA [2] += prio;
	strcpy (msgB, (unsigned char*) "B-0"); msgB [2] += prio;
	strcpy (msgC, (unsigned char*) "C-0"); msgC [2] += prio;
	for (;;) {
		switch (rand15() % 3) {
		case 0:
			/*debug_printf ("signal %s\n", msgA);*/
			lock_signal (&A, msgA);
			break;
		case 1:
			/*debug_printf ("signal %s\n", msgB);*/
			lock_signal (&B, msgB);
			break;
		case 2:
			/*debug_printf ("signal %s\n", msgC);*/
			lock_signal (&C, msgC);
			break;
		}
		timer_delay (&timer, rand15() % 10 * 100);
	}
}

void uos_init (void)
{
	lock_group_t *g;

	srand15 (time(0));
	timer_init (&timer, 100, KHZ, 10);

	/*
	 * Create a group of three locks A, B and C.
	 */
	g = lock_group_init (group, sizeof(group));
	lock_group_add (g, &A);
	lock_group_add (g, &B);
	lock_group_add (g, &C);
	lock_group_add (g, &timer.decisec);
	lock_group_listen (g);

	/*
	 * Create three tasks: one listener and two senders.
	 * One sender has higher priority, and another - lower priority
	 * than listener.
	 */
	task_create (main_waiter, (void*) g, "waiter",   2, waiter, sizeof (waiter));
	task_create (main_sender, (void*) 1, "sender10", 1, sender1, sizeof (sender1));
	task_create (main_sender, (void*) 3, "sender30", 3, sender3, sizeof (sender3));
}
