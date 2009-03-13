#include <runtime/lib.h>
#include <kernel/uos.h>
#include <nvram/nvram.h>

/*
 * Write a short to NVRAM (lsb first).
 */
void
nvram_write16 (unsigned addr, uint16_t val)
{
	nvram_write_byte (addr, val);
	nvram_write_byte (addr, val >> 8);
}

/*
 * Read a short from NVRAM (lsb first).
 */
uint16_t
nvram_read16 (unsigned addr)
{
	unsigned val;

	val = nvram_read_byte (addr);
	val |= nvram_read_byte (addr) << 8;
	return val;
}

/*
 * Write a long to NVRAM (lsb first).
 */
void
nvram_write32 (unsigned addr, uint32_t val)
{
	nvram_write16 (addr, val);
	nvram_write16 (addr, val >> 16);
}

/*
 * Read a long from NVRAM (lsb first).
 */
uint32_t
nvram_read32 (unsigned addr)
{
	uint32_t val;

	val = nvram_read16 (addr);
	val |= (uint32_t) nvram_read16 (addr) << 16;
	return val;
}
