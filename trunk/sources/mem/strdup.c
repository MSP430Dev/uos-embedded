#include <runtime/lib.h>
#include <mem/mem.h>

unsigned char *mem_strdup (mem_pool_t *m, const unsigned char *str)
{
	mem_size_t len;
	unsigned char *p;

	if (! str)
		return 0;

	len = strlen (str);
	p = mem_alloc_dirty (m, len + 1);
	if (! p)
		return 0;

	memcpy (p, str, len + 1);

	return p;
}
