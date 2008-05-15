#include <runtime/lib.h>
#include <buf/buf.h>
#include <crc/crc32-vak.h>

unsigned long
buf_chksum32 (buf_t *p, unsigned long sum)
{
	for (; p; p = p->next)
		sum = crc32_vak (sum, p->payload, p->len);
	return ~sum;
}
