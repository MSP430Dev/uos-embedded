/*
 * Computing 16-bit checksum (CCITT).
 */
#define CRC16_CCITT_INIT	0xffff  /* Initial checksum value */
#define CRC16_CCITT_GOOD	0xf0b8	/* Good final checksum value */

unsigned short crc16_ccitt (unsigned short sum, unsigned const char *buf,
	unsigned short len);

unsigned short crc16_ccitt_byte (unsigned short sum, unsigned char byte);
