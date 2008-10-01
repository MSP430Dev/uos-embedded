#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

/*
 * Write a zero-limited string to NVRAM.
 */
void
nvram_write_str (nvram_t *v,
	unsigned char *str, small_uint_t maxlen)
{
	while (maxlen-- > 0) {
		assert (*str == 0 || ((signed char) *str) >= ' ');
		nvram_write_byte (v, *str);
		if (*str)
			++str;
	}
}

/*
 * Read a zero-limited string from NVRAM.
 */
void
nvram_read_str (nvram_t *v,
	unsigned char *str, small_uint_t maxlen)
{
	while (maxlen-- > 0) {
		*str = nvram_read_byte (v);
		if (*str && (signed char) *str < ' ') {
			*str = 0;
			v->crc ^= 0xDEAD;
		}
		++str;
	}
}
