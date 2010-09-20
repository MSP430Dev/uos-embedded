#include <runtime/lib.h>
#include <buf/buf.h>
#include <buf/buf-queue.h>

buf_t *
buf_queue_get (buf_queue_t *q)
{
	buf_t *p;

	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + q->size);
	if (q->count == 0) {
		/*debug_printf (CONST("buf_queue_get: returned 0\n"));*/
		return 0;
	}
	assert (*q->tail != 0);

	/* Get the first packet from queue. */
	p = *q->tail;

	/* Advance head pointer. */
	if (--q->tail < q->queue)
		q->tail += q->size;
	--q->count;

	/*debug_printf (CONST("buf_queue_get: returned 0x%04x\n"), p);*/
	return p;
}

void
buf_queue_put (buf_queue_t *q, buf_t *p)
{
	buf_t **head;

	/*debug_printf (CONST("buf_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n"), p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < q->size);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += q->size;

	/* Put the packet in. */
	*head = p;
	++q->count;
	/*debug_printf (CONST("    on return count = %d, head = 0x%04x\n"), q->count, q->head);*/
}

void
buf_queue_clean (buf_queue_t *q)
{
	for (; q->count > 0; --q->count) {
		buf_free (*q->tail);

		if (--q->tail < q->queue)
			q->tail += q->size;
	}
}

void
buf_queue_init (buf_queue_t *q, buf_t **buf, int bytes)
{
	q->tail = buf;
	q->queue = buf;
	q->size = bytes / sizeof (buf_t*);
	q->count = 0;
}
