#include <runtime/lib.h>
#include <mem/mem.h>

void *mem_alloc_must (mem_pool_t *region, size_t bytes, const char *item)
{
	void *p = mem_alloc (region, bytes);
	if (! p) {
		debug_printf ("No memory for %S at @%p",
			item,  __builtin_return_address (0));
		uos_halt (1);
	}
	return p;
}