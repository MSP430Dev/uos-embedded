#include <runtime/lib.h>
#include <buf/buf.h>

/*
 * Count number of bufs in a chain.
 */
unsigned char
buf_chain_len (buf_t *p)
{
	unsigned char len;

	for (len = 0; p; p = p->next)
		++len;

	return len;
}
