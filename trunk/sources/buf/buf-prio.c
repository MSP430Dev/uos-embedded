#include <runtime/lib.h>
#include <kernel/uos.h>
#include <buf/buf.h>
#include <buf/buf-prio.h>

/*
 * Из очереди с максимальным приоритетом
 * берем самый старый пакет.
 */
buf_t *
buf_prio_queue_get (buf_prio_queue_t *q)
{
	buf_queue_t *qq;

	if (q->count <= 0)
		return 0;

	for (qq = q->queue + BUF_PRIO_NQ-1; qq >= q->queue; --qq)
		if (qq->count) {
			--q->count;
			return buf_queue_get (qq);
		}

	/* LY: если счетчик не равен нулю - то где-то явная ошибка. */
	assert (q->count == 0);
	q->count = 0;

	return 0;
}

/*
 * Ставим в очередь пакет с указанныи приоритетом.
 * Если получилось - возвращаем 1, иначе 0.
 */
int
buf_prio_queue_put (buf_prio_queue_t *q, buf_t *p, int prio)
{
	buf_queue_t *qq;

	if (q->count < q->size) {
		/* Есть место. */
		buf_queue_put (&q->queue[prio], p);
		++q->count;
		return 1;
	}

	for (qq = q->queue; qq <= q->queue + prio; ++qq) {
		if (qq->count) {
			/* Удаляем один пакет с более низким или
			 * тем же приоритетом. */
			buf_free (buf_queue_get (qq));
			buf_queue_put (&q->queue[prio], p);
			return 2;
		}
	}
	/* Нет места. */
	return 0;
}

/*
 * Обмениваем пакет на другой с большим приоритетом.
 * Или на более старый с тем же приоритетом.
 * Всегда возвращаем непустой указатель.
 */
buf_t *
buf_prio_queue_exchange (buf_prio_queue_t *q, buf_t *p, int prio)
{
	buf_t *r;
	buf_queue_t *qq;

	if (q->count <= 0) {
		/* Очередь пустая - возвращаем пакет обратно. */
		return p;
	}

	/* Из очереди с большим или тем же приоритетом
	 * берем самый старый пакет. */
	for (qq = q->queue + BUF_PRIO_NQ-1; qq >= q->queue + prio; --qq) {
		if (qq->count) {
			r = buf_queue_get (qq);
			buf_queue_put (&q->queue[prio], p);
			return r;
		}
	}
	/* Вся очередь низкоприоритетная - возвращаем пакет обратно. */
	return p;
}

void buf_prio_queue_init (buf_prio_queue_t *q, buf_t **buf, int bytes)
{
	int k, bytes_per_queue;

	bytes_per_queue = bytes / BUF_PRIO_NQ;
	q->size = bytes_per_queue / sizeof (buf_t*);
	for (k = 0; k < BUF_PRIO_NQ; ++k) {
		buf_queue_init (&q->queue[k], buf, bytes_per_queue);
		buf += q->size;
	}
	q->count = 0;
}
