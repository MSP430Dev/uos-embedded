/*
 * Computing 8-bit checksum.
 * Polynome is x^8 + x^5 + x^4 + 1 - used in
 * Dallas Semiconductor iButton devices.
 */
unsigned char crc8_dallas (unsigned const char *buf, unsigned char len);
