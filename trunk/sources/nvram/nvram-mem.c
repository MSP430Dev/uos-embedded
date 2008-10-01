#include "runtime/lib.h"
#include "kernel/uos.h"
#include "nvram/nvram.h"

/*
 * Write an array of bytes to NVRAM.
 */
void
nvram_write_mem (nvram_t *v,
	unsigned char *str, small_uint_t len)
{
	while (len-- > 0)
		nvram_write_byte (v, *str++);
}

/*
 * Read a byte from NVRAM.
 */
void
nvram_read_mem (nvram_t *v,
	unsigned char *str, small_uint_t len)
{
	while (len-- > 0)
		*str++ = nvram_read_byte (v);
}
