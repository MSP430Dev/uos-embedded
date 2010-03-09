/*
 * Measuring task switch time.
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t mailbox;
struct timespec t0;

volatile unsigned nmessages;
volatile unsigned long long latency_sum;
volatile unsigned long long latency_min = ~0;
volatile unsigned long long latency_max;

/*
 * Задача приёма сообщений.
 */
void *receiver (void *arg)
{
	struct timespec t1;
	unsigned long long latency;

	for (;;) {
		pthread_cond_wait (&mailbox, &mutex);
		if (clock_gettime (CLOCK_MONOTONIC, &t1) < 0) {
			perror ("clock_gettime");
			exit (-1);
		}

		/* Вычисляем количество тактов, затраченных на вход в прерывание. */
		latency = (t1.tv_sec - t0.tv_sec) * 1000000000ULL +
			(t1.tv_nsec - t0.tv_nsec);
		pthread_mutex_unlock (&mutex);

/*		printf ("<%llu> ", latency);*/
		++nmessages;
		latency_sum += latency;
		if (latency_min > latency)
			latency_min = latency;
		if (latency_max < latency)
			latency_max = latency;
	}
}

int main (int argc, char *argv[])
{
	struct sched_param sched;
	pthread_t tid;

	/* Стираем экран. */
	setlinebuf (stdout);
	printf ("\33[H\33[2J");

	pthread_cond_init (&mailbox, 0);

	/* Запускаем задачу приёма пакетов. */
	if (sched_getparam (0, &sched) != 0) {
		perror ("sched_getparam");
		exit (1);
	}
	if (0 != pthread_create (&tid, 0, receiver, 0)) {
		perror ("pthread_start");
		exit (1);
	}
	if (geteuid() == 0) {
		/* Increase receiver priority. */
		sched.sched_priority += 10;
		if (pthread_setschedparam (tid, SCHED_FIFO, &sched) != 0) {
			perror ("pthread_setschedparam");
			exit (1);
		}
	}

	for (;;) {
		if (clock_gettime (CLOCK_MONOTONIC, &t0) < 0) {
			perror ("clock_gettime");
			exit (-1);
		}
		pthread_cond_signal (&mailbox);
		usleep (10000);

		printf ("\33[H");
		printf ("Measuring task switch time.\n\n");

		printf ("Task switches: %u  \n\n", nmessages);

		printf (" Latency, min: %.2f usec\33[K\n", (double) latency_min / 1000.0);
		printf ("      average: %.2f usec\33[K\n", (double) latency_sum / nmessages / 1000.0);
		printf ("          max: %.2f usec\33[K\n", (double) latency_max / 1000.0);
	}
}
