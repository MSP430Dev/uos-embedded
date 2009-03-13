#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvdata.h>

/*
 * Write an array of bytes to NVRAM.
 */
void
nvdata_write_mem (nvdata_t *v,
	unsigned char *str, small_uint_t len)
{
	while (len-- > 0)
		nvdata_write_byte (v, *str++);
}

/*
 * Read a byte from NVRAM.
 */
void
nvdata_read_mem (nvdata_t *v,
	unsigned char *str, small_uint_t len)
{
	while (len-- > 0)
		*str++ = nvdata_read_byte (v);
}
