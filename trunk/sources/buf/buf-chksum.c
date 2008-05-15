#include <runtime/lib.h>
#include <buf/buf.h>
#include <crc/crc16-inet.h>

unsigned short
buf_chksum (buf_t *p, unsigned short sum)
{
	for (; p; p = p->next) {
		sum = crc16_inet (sum, p->payload, p->len);
	}
	return ~sum;
}

unsigned short
buf_chksum_pseudo (buf_t *p, unsigned char *src, unsigned char *dest,
	unsigned char proto, unsigned short proto_len)
{
	unsigned short sum;
	unsigned char swapped;

	sum = crc16_inet (HTONS (proto), (unsigned char*) &proto_len, 2);
	sum = crc16_inet (sum, src, 4);
	sum = crc16_inet (sum, dest, 4);
	for (swapped = 0; p; p = p->next) {
		sum = crc16_inet (sum, p->payload, p->len);
		if (p->len & 1) {
			sum = (sum << 8 & 0xff00) | (sum >> 8);
			swapped ^= 1;
		}
	}

	if (swapped)
		sum = (sum << 8 & 0xff00) | (sum >> 8);

	return ~sum;
}
