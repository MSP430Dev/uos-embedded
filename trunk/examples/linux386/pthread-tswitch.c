/*
 * Measuring task switch time.
 * Compile by command:
 *	gcc pthread-tswitch.c -lrt
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t mailbox;
struct timespec t0;

volatile unsigned nmessages;
volatile unsigned long long latency_sum;
volatile unsigned long latency_min = ~0;
volatile unsigned long latency_max;
unsigned long delta;

/*
 * Вычитание двух близких значений времени.
 * Возвращает разницу в наносекундах.
 * В случае переполнения возвращает 0.
 */
unsigned long delta_t (struct timespec *t0, struct timespec *t1)
{
	long nsec;

	if (t1->tv_sec < t0->tv_sec || t1->tv_sec > t0->tv_sec + 2)
		return 0;

	return (t1->tv_sec - t0->tv_sec) * 1000000000 +
		(t1->tv_nsec - t0->tv_nsec);
}

/*
 * Определение времени работы вызова clock_gettime(),
 * для внесения поправки в измерения.
 */
unsigned long compute_delta ()
{
	struct timespec t1, t2, t3, t4;
	unsigned long d1, d2, d3;

	/* Делаем четыре замера подряд. */
	if (clock_gettime (CLOCK_MONOTONIC, &t1) < 0) {
		perror ("clock_gettime");
		exit (-1);
	}
	if (clock_gettime (CLOCK_MONOTONIC, &t2) < 0) {
		perror ("clock_gettime");
		exit (-1);
	}
	if (clock_gettime (CLOCK_MONOTONIC, &t3) < 0) {
		perror ("clock_gettime");
		exit (-1);
	}
	if (clock_gettime (CLOCK_MONOTONIC, &t4) < 0) {
		perror ("clock_gettime");
		exit (-1);
	}

	/* Вычисляем три значения времени. */
	d1 = delta_t (&t1, &t2);
	d2 = delta_t (&t2, &t3);
	d3 = delta_t (&t3, &t4);

	/* Берём среднее значение. */
	if (d1 <= d2) {
		if (d2 <= d3)
			return d2;
		else if (d1 <= d3)
			return d3;
		else
			return d1;
	} else {
		if (d1 <= d3)
			return d1;
		else if (d2 <= d3)
			return d3;
		else
			return d2;
	}
}

/*
 * Задача приёма сообщений.
 */
void *receiver (void *arg)
{
	struct timespec t1, t2;
	unsigned long latency;

	for (;;) {
		pthread_cond_wait (&mailbox, &mutex);
		if (clock_gettime (CLOCK_MONOTONIC, &t1) < 0) {
			perror ("clock_gettime");
			exit (-1);
		}
		if (clock_gettime (CLOCK_MONOTONIC, &t2) < 0) {
			perror ("clock_gettime");
			exit (-1);
		}

		/* Вычисляем количество тактов, затраченных на вход в прерывание. */
		latency = delta_t (&t0, &t1);
		pthread_mutex_unlock (&mutex);

		/*printf ("<%lu-%lu> ", latency, delta);*/
		if (latency > 10000000 || latency <= delta)
			continue;
		latency -= delta;

		if (++nmessages > 10) {
			latency_sum += latency;
			if (latency_min > latency)
				latency_min = latency;
			if (latency_max < latency)
				latency_max = latency;
		}
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

	delta = compute_delta ();

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
		printf ("      average: %.2f usec\33[K\n", (double) latency_sum / (nmessages-10) / 1000.0);
		printf ("          max: %.2f usec\33[K\n", (double) latency_max / 1000.0);
	}
}
