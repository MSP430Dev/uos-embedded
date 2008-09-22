#include <runtime/lib.h>
#include <mem/mem.h>

unsigned char *mem_strndup (mem_pool_t *m, const unsigned char *str,
	size_t len)
{
	unsigned char *p;

	if (! str)
		return 0;

	len = strnlen (str, len);
	p = mem_alloc_dirty (m, len + 1);
	if (! p)
		return 0;

	memcpy (p, str, len);
	p [len] = 0;
	return p;
}
