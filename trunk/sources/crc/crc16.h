/*
 * Computing 16-bit checksum (x16 + x5 + x2 + 1).
 */
#define CRC16_INIT	0x0000  /* Initial checksum value */
#define CRC16_GOOD	0x0000	/* Good final checksum value */

unsigned short crc16 (unsigned short sum, unsigned const char *buf,
	unsigned short len);

unsigned short crc16_byte (unsigned short sum, unsigned char byte);
