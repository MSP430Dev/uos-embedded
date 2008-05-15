#include <runtime/lib.h>
#include <crc/crc8-atm.h>

/*
 * x^8 + x^2 + x + 1 - used in ATM
 */
#define POLY_CONST	0x07	/* x^8 + x^2 + x + 1 */

static const unsigned char poly_tab [256] = {
	  0,  7, 14,  9, 28, 27, 18, 21, 56, 63, 54, 49, 36, 35, 42, 45,
	112,119,126,121,108,107, 98,101, 72, 79, 70, 65, 84, 83, 90, 93,
	224,231,238,233,252,251,242,245,216,223,214,209,196,195,202,205,
	144,151,158,153,140,139,130,133,168,175,166,161,180,179,186,189,
	199,192,201,206,219,220,213,210,255,248,241,246,227,228,237,234,
	183,176,185,190,171,172,165,162,143,136,129,134,147,148,157,154,
	 39, 32, 41, 46, 59, 60, 53, 50, 31, 24, 17, 22,  3,  4, 13, 10,
	 87, 80, 89, 94, 75, 76, 69, 66,111,104, 97,102,115,116,125,122,
	137,142,135,128,149,146,155,156,177,182,191,184,173,170,163,164,
	249,254,247,240,229,226,235,236,193,198,207,200,221,218,211,212,
	105,110,103, 96,117,114,123,124, 81, 86, 95, 88, 77, 74, 67, 68,
	 25, 30, 23, 16,  5,  2, 11, 12, 33, 38, 47, 40, 61, 58, 51, 52,
	 78, 73, 64, 71, 82, 85, 92, 91,118,113,120,127,106,109,100, 99,
	 62, 57, 48, 55, 34, 37, 44, 43,  6,  1,  8, 15, 26, 29, 20, 19,
	174,169,160,167,178,181,188,187,150,145,152,159,138,141,132,131,
	222,217,208,215,194,197,204,203,230,225,232,239,250,253,244,243,
};

/*
 * Table-based computing of 8-bit checksum.
 */
unsigned char
crc8_atm (unsigned const char *buf, unsigned char len)
{
	unsigned char sum = 0;
#if __AVR__
	const unsigned char *ptr = poly_tab;

	if (len) asm volatile (
	"1:	movw r30, %3 \n"	/* 1 clock */
	"	add r30, %1 \n"		/* 1 clock */
	"	adc r31, r1 \n"		/* 1 clocks */
	"	lpm __tmp_reg__, Z \n"	/* 3 clocks */
	"	ld %1, %a0+ \n"		/* 2 clocks */
	"	eor %1, __tmp_reg__ \n"	/* 1 clock */
	"	subi %2, 1 \n"		/* 1 clock */
	"	brne 1b \n"		/* 2 clocks */
					/* total 9 clocks per byte */
	: "+e" (buf), "+r" (sum), "+r" (len), "+r" (ptr)
	: :  "cc", "r30", "r31", __tmp_reg__);
#else
	if (len) do
		sum = *buf++ ^ poly_tab [sum];
	while (--len);
#endif
	return ~sum;
}

#ifdef DEBUG_CRC8
/*
 * Direct computing of 8-bit checksum.
 */
unsigned char
crc8_atm_slow (unsigned const char *buf, unsigned char len)
{
#if __AVR__
	unsigned char sum, poly;

	asm volatile (
	"SUM = %1 \n"
	"LEN = %2 \n"
	"	clr SUM \n"
	"1:	ld __tmp_reg__, %a0+ \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %3 \n"
	"	subi LEN, 1 \n"
	"	brne 1b \n"
	: "=&e" (buf), "=r" (sum), "=&r" (len), "=&r" (poly)
	: "0" (buf), "2" (len), "3" ((char)POLY_CONST));

	return ~sum;
#else
	unsigned char i, n;
	unsigned short sum;

	sum = 0;
	for (i=0; i<len; ++i) {
		sum |= *buf++;
		for (n=0; n<8; ++n) {
			if (sum & 0x8000)
				sum = (sum << 1) ^ (POLY_CONST << 8);
			else
				sum <<= 1;
		}
	}
	return ~(unsigned char) (sum >> 8);
#endif
}

#include <stdio.h>

void
test (unsigned const char *buf, unsigned char len)
{
	unsigned char i;

	for (i=0; i<len-1; ++i)
		printf ("%02x ", buf[i]);
	printf ("(%02x) - %02x\n", buf[len-1], crc8_atm (buf, len));
}

int
main ()
{
	unsigned char array [] = "\x10\x20\x00";

	test ("\x00", 1);
	test ("\x07", 1);
	test ("\x01", 2);
	test ("\x02", 2);
	test ("\x04", 2);
	test ("\x08", 2);
	test ("\x10", 2);
	test ("\x20", 2);
	test ("\x40", 2);
	test ("\x80", 2);

	test ("\x00\x01", 3);
	test ("\x00\x02", 3);
	test ("\x00\x04", 3);
	test ("\x00\x08", 3);
	test ("\x00\x10", 3);
	test ("\x00\x20", 3);
	test ("\x00\x40", 3);
	test ("\x00\x80", 3);

	test ("\x01\x00", 3);
	test ("\x02\x00", 3);
	test ("\x04\x00", 3);
	test ("\x08\x00", 3);
	test ("\x10\x00", 3);
	test ("\x20\x00", 3);
	test ("\x40\x00", 3);
	test ("\x80\x00", 3);

	test (array, 3);
	array[2] = crc8_atm (array, 3);
	test (array, 3);

	return 0;
}
#endif /* DEBUG_CRC8 */
