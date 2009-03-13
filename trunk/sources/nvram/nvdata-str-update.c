#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>

/*
 * Update a zero-limited string in NVRAM, rewrite CRC.
 */
small_int_t nvdata_update_str (nvdata_t *v, unsigned addr,
	unsigned char *str, small_uint_t maxlen)
{
	small_int_t reason;

	reason = nvdata_begin_update (v, addr);
	if (reason == NVDATA_OK) {
		nvdata_write_str (v, str, maxlen);
		reason = nvdata_finalize_update (v);
	}
	return reason;
}
