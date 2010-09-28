#ifndef __CAN_QUEUE_H_
#define	__CAN_QUEUE_H_ 1

#define CAN_QUEUE_SIZE	16	/* max number of packets in queue */

/*
 * Queue of CAN packets.
 */
typedef struct _can_queue_t {
	unsigned count;
	can_frame_t *tail;
	can_frame_t queue [CAN_QUEUE_SIZE];
} can_queue_t;

/*
 * Initialize queue.
 */
static inline __attribute__((always_inline))
void can_queue_init (can_queue_t *q)
{
	q->tail = q->queue;
	q->count = 0;
}

/*
 * Add a packet to queue.
 * Before call, a user should check that the queue is not full.
 */
static inline __attribute__((always_inline))
void can_queue_put (can_queue_t *q, can_frame_t p)
{
	can_frame_t *head;

	/*debug_printf ("can_queue_put: p = 0x%04x, count = %d, head = 0x%04x\n", p, q->count, q->head);*/

	/* Must be called ONLY when queue is not full. */
	assert (q->count < CAN_QUEUE_SIZE);

	/* Compute the last place in the queue. */
	head = q->tail - q->count;
	if (head < q->queue)
		head += CAN_QUEUE_SIZE;

	/* Put the packet in. */
	*head = p;
	++q->count;
	/*debug_printf ("    on return count = %d, head = 0x%04x\n", q->count, q->head);*/
}

/*
 * Get a packet from queue.
 * When empty, returns {0,0,0,0};
 */
static inline __attribute__((always_inline))
can_frame_t can_queue_get (can_queue_t *q)
{
	can_frame_t p;

	assert (q->tail >= q->queue);
	assert (q->tail < q->queue + CAN_QUEUE_SIZE);
	if (q->count == 0) {
		/*debug_printf ("can_queue_get: returned 0\n");*/
		return {0};
	}

	/* Get the first packet from queue. */
	p = *q->tail;

	/* Advance head pointer. */
	if (--q->tail < q->queue)
		q->tail += CAN_QUEUE_SIZE;
	--q->count;

	/*debug_printf ("can_queue_get: returned 0x%04x\n", p);*/
	return p;
}

/*
 * Check that queue is full.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_full (can_queue_t *q)
{
	return (q->count == CAN_QUEUE_SIZE);
}

/*
 * Check that queue is empty.
 */
static inline __attribute__((always_inline))
bool_t can_queue_is_empty (can_queue_t *q)
{
	return (q->count == 0);
}

/*
 * Get a pointer to first packet.
 * Before call, a user should check that the queue is not empty.
 */
static inline __attribute__((always_inline))
can_frame_t *can_queue_first (can_queue_t *q)
{
	can_frame_t *head;

	head = q->tail - q->count;
	if (head < q->queue)
		head += CAN_QUEUE_SIZE;
	return head;
}

/*
 * Get a pointer to last packet.
 * Before call, a user should check that the queue is not empty.
 */
static inline __attribute__((always_inline))
can_frame_t *can_queue_last (can_queue_t *q)
{
	return q->tail;
}

/*
 * Get a pointer to next packet.
 */
static inline __attribute__((always_inline))
can_frame_t *can_queue_next (can_queue_t *q, can_frame_t *ptr)
{
	if (--ptr < q->queue)
		ptr += CAN_QUEUE_SIZE;
	return ptr;
}
#endif /* !__CAN_QUEUE_H_ */
