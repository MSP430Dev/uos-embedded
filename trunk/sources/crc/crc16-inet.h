/*
 * Computing 16-bit checksum (IP-compatible).
 */
#define CRC16_INET_INIT		0x0000  /* Initial checksum value */
#define CRC16_INET_GOOD		0xffff	/* Good final checksum value */

unsigned short crc16_inet (unsigned short sum, unsigned const char *buf,
	unsigned short len);
unsigned short crc16_inet_header (unsigned char *src, unsigned char *dest,
	unsigned char proto, unsigned short proto_len);
unsigned short crc16_inet_byte (unsigned short sum, unsigned char data);
