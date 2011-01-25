#include <runtime/lib.h>
#include <buf/buf.h>

/*
 * Count number of bufs in a chain.
 */
small_int_t
buf_chain_len (buf_t *p)
{
	small_int_t len;

	for (len = 0; p; p = p->next)
		++len;

	return len;
}
