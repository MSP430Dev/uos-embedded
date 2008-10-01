#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

/*
 * Update a zero-limited string in NVRAM, rewrite CRC.
 */
small_int_t nvram_update_str (nvram_t *v, unsigned addr,
	unsigned char *str, small_uint_t maxlen)
{
	small_int_t reason;

	reason = nvram_begin_update (v, addr);
	if (reason == NVRAM_OK) {
		nvram_write_str (v, str, maxlen);
		reason = nvram_finalize_update (v);
	}
	return reason;
}
