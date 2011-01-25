#include <runtime/lib.h>
#include <mem/mem.h>

void *mem_xalloc (mem_pool_t *region, size_t bytes, const char *title)
{
	void *p = mem_alloc (region, bytes);
	if (! p) {
		debug_printf ("No memory for %S at @%p",
			title,  __builtin_return_address (0));
		uos_halt (1);
	}
	return p;
}
