/*
 * Computing 8-bit checksum.
 * Polynome is x^8 + x^2 + x + 1 - used in ATM.
 */
unsigned char crc8_atm (unsigned const char *buf, unsigned char len);
