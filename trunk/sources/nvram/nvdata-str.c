#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>

/*
 * Write a zero-limited string to NVRAM.
 */
void
nvdata_write_str (nvdata_t *v,
	unsigned char *str, small_uint_t maxlen)
{
	while (maxlen-- > 0) {
		assert (*str == 0 || ((signed char) *str) >= ' ');
		nvdata_write_byte (v, *str);
		if (*str)
			++str;
	}
}

/*
 * Read a zero-limited string from NVRAM.
 */
void
nvdata_read_str (nvdata_t *v,
	unsigned char *str, small_uint_t maxlen)
{
	while (maxlen-- > 0) {
		*str = nvdata_read_byte (v);
		if (*str && (signed char) *str < ' ') {
			*str = 0;
			v->crc ^= 0xDEAD;
		}
		++str;
	}
}
