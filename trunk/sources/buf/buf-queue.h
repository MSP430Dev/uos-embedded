#ifndef __BUF_QUEUE_H_
#define	__BUF_QUEUE_H_ 1

struct _buf_t;

typedef struct _buf_queue_t buf_queue_t;

struct _buf_queue_t {
	struct _buf_t **tail;
	small_uint_t count;
	struct _buf_t **queue;
	small_uint_t size;
};

void buf_queue_init (buf_queue_t *q, struct _buf_t **buf, int bytes);
void buf_queue_put (buf_queue_t *q, struct _buf_t *b);
struct _buf_t *buf_queue_get (buf_queue_t *q);

inline extern bool_t buf_queue_is_full (buf_queue_t *q)
{
	return (q->count == q->size);
}

inline extern bool_t buf_queue_is_empty (buf_queue_t *q)
{
	return (q->count == 0);
}

inline extern struct _buf_t **buf_queue_first (buf_queue_t *q)
{
	struct _buf_t **head;

	head = q->tail - q->count;
	if (head < q->queue)
		head += q->size;
	return head;
}

inline extern struct _buf_t **buf_queue_last (buf_queue_t *q)
{
	return q->tail;
}

inline extern struct _buf_t **buf_queue_next (buf_queue_t *q, struct _buf_t **ptr)
{
	if (--ptr < q->queue)
		ptr += q->size;
	return ptr;
}

void buf_queue_clean (buf_queue_t *q);

#endif /* !__BUF_QUEUE_H_ */
