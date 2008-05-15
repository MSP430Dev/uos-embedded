#include <runtime/lib.h>
#include <buf/buf.h>
#include <mem/mem.h>

/*
 * Make a copy of buffer.
 * Return 0 when not enough memory.
 * Do not free an initial buffer.
 */
buf_t *
buf_copy (buf_t *p)
{
	buf_t *x, *q;
	int header_size;

	if (! p)
		return 0;

	/* Make a single chunk, big enough. */
	header_size = (p->payload - (unsigned char*) p);
	x = mem_alloc (mem_pool (p), p->tot_len + header_size);
	if (! x)
		return 0;

	/* Set up internal structure of the buf. */
	x->next = 0;
	x->payload = (unsigned char*) x + header_size;
	x->tot_len = p->tot_len;
	x->len = 0;

	/* Copy all chunks. */
	for (q = p; q; q = q->next) {
		memcpy (x->payload + x->len, q->payload, q->len);
		x->len += q->len;
	}
	return x;
}
