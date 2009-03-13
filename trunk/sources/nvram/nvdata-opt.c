#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>

bool_t __attribute__((weak))
nvdata_is_compatible (unsigned nvram, unsigned soft)
{
	if ((nvram ^ soft) > 0x0F)
		return 0;
	return 1;
}
