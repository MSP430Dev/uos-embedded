#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

bool_t __weak nvram_is_compatible (unsigned nvram, unsigned soft)
{
	if ((nvram ^ soft) > 0x0F)
		return 0;
	return 1;
}
