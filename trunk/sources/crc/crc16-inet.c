#include <runtime/lib.h>
#include <crc/crc16-inet.h>

/*
 * Calculate a new sum given the current sum and the new data.
 * Use 0 as the initial sum value.
 * Do not forget to invert the final checksum value.
 */
unsigned short
crc16_inet (unsigned short sum, unsigned const char *buf, unsigned short len)
{
#if __AVR__
	/* Taken from Liquorice.
	 * Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk> */
	unsigned short words;

	words = len >> 1;
	if (words != 0) do {
		/* This cycle is manually optimized - do not touch! */
		asm volatile (
		"	ld __tmp_reg__, %a1+ \n"		/* 2 clocks */
		"	add %A0, __tmp_reg__ \n"		/* 1 clock */
		"	ld __tmp_reg__, %a1+ \n"		/* 2 clocks */
		"	adc %B0, __tmp_reg__ \n"		/* 1 clock */
		"	adc %A0, __zero_reg__ \n"		/* 1 clock */
		"	adc %B0, __zero_reg__ \n"		/* 1 clock */
		: "+r" (sum), "+e" (buf) : : "cc", __tmp_reg__);
		/* Additional 2+2 clocks for increment and branch.
		 * Total 7 clocks per byte. */
	} while (--words != 0);

	/* Did we have an odd number of bytes to do? */
	if (len & 1) {
		asm volatile (
		"	ld __tmp_reg__, %a1+ \n"
		"	add __tmp_reg__, %A0 \n"
		"	adc %B0, __zero_reg__ \n"
		"	adc __tmp_reg__, __zero_reg__ \n"
		"	mov %A0, %B0 \n"
		"	mov %B0, __tmp_reg__ \n"
		: "+r" (sum), "+e" (buf) : : "cc", __tmp_reg__);
	}

#elif defined (MSP430)
	if ((int) buf & 1) {
		/* get first non-aligned byte */
		asm volatile (
		"	add %1, %0 \n"
		"	addc #0, %0 \n"
		"	swpb %0 \n"
		: "+r" (sum) : "r" ((unsigned short) *buf++) : "cc");
		--len;
	}
	if (len >= 2) do {
		asm volatile (
		"	add @%1+, %0 \n"
		"	addc #0, %0 \n"
		: "+r" (sum), "+r" (buf) : : "cc");
	} while ((len -= 2) > 0);

	/* Did we have an odd number of bytes to do? */
	if (len == 1) {
		asm volatile (
		"	add %1, %0 \n"
		"	addc #0, %0 \n"
		"	swpb %0 \n"
		: "+r" (sum) : "r" ((unsigned short) *buf) : "cc");
	}

#elif __i386__
	/* Taken from Liquorice.
	 * Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk> */
	unsigned const char *tmp1 = buf;
	unsigned long words, tmp2;

        words = len >> 1;
	if (words != 0) {
		/* Note that for a 32 bit CPU this is "really bad" (TM) code.
		 * It needs reworking into something a lot faster! */
		asm volatile (
		"1:	lodsw \n"
		"	addw %%ax, %0 \n"
		"	adcw $0, %0 \n"
		"	loop 1b \n"
		: "=g" (sum), "=S" (tmp1), "=c" (tmp2)
		: "0" (sum), "1" (tmp1), "2" (words)
		: "ax");
	}

	/* Did we have an odd number of bytes to do? */
	if (len & 1) {
		asm volatile (
		"	movb (%%esi), %%al \n"
		"	xorb %%ah, %%ah \n"
		"	addw %%ax, %0 \n"
		"	adcw $0, %0 \n"
		"	rolw $8, %w0 \n"
		: "=g" (sum), "=S" (tmp1)
		: "0" (sum), "1" (tmp1)
		: "ax");
	}
#elif defined (__arm__) || defined (__thumb__)
	/*
	 * Optimized for ARM/Thumb, little endian.
	 */
	unsigned long longsum, longlen;

	longsum = sum;
	longlen = len;
	if ((int) buf & 1) {
		/* get first non-aligned byte */
		longsum += *buf++;
		longsum = (longsum >> 8) + ((unsigned char) longsum << 8);
		--longlen;
	}

	for (; longlen>1; longlen-=2, buf+=2)
		longsum += *(unsigned short*) buf;

	if (longlen & 1) {
		/* add up any odd byte */
		longsum += *buf;
		longsum = (longsum >> 8) + ((unsigned char) longsum << 8);
	}
	/* Build cyclic sum. */
	longsum = (longsum >> 16) + (unsigned short) longsum;
	sum = longsum;
	if (longsum & 0x10000)
		++sum;
#else
	unsigned long longsum;

	longsum = sum;
	if ((int) buf & 1) {
		/* get first non-aligned byte */
#if HTONS(1) == 1
		longsum += *buf++ << 8;
#else
		longsum += *buf++;
#endif
		longsum = (longsum >> 8) + ((unsigned char) longsum << 8);
		--len;
	}

	for (; len>1; len-=2, buf+=2)
		longsum += *(unsigned short*) buf;

	if (len & 1) {
		/* add up any odd byte */
#if HTONS(1) == 1
		longsum += *buf << 8;
#else
		longsum += *buf;
#endif
		longsum = (longsum >> 8) + ((unsigned char) longsum << 8);
	}
	/* Build cyclic sum. */
	longsum = (longsum >> 16) + (unsigned short) longsum;
	sum = longsum;
	if (longsum & 0x10000)
		++sum;
#endif
	/*debug_printf (" -> 0x%04x\n", sum);*/
	return sum;
}

unsigned short
crc16_inet_header (unsigned char *src, unsigned char *dest,
	unsigned char proto, unsigned short proto_len)
{
	unsigned short sum;

#if __AVR__
	/* Sum `proto' and `proto_len'. */
	sum = proto_len;
	asm volatile (
	"	add %1, %A0 \n"
	"	adc %B0, __zero_reg__ \n"
	"	adc %1, __zero_reg__ \n"
	"	mov %A0, %B0 \n"
	"	mov %B0, %1 \n"
	: "+r" (sum) : "r" (proto) : "cc");

	/* Add `src'. */
	asm volatile (
	"	ld __tmp_reg__, %a1+ \n"
	"	add %A0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1+ \n"
	"	adc %B0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1+ \n"
	"	adc %A0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1 \n"
	"	adc %B0, __tmp_reg__ \n"
	: "+r" (sum), "+e" (src) : : "cc", __tmp_reg__);

	/* Add `dest'. */
	asm volatile (
	"	ld __tmp_reg__, %a1+ \n"
	"	adc %A0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1+ \n"
	"	adc %B0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1+ \n"
	"	adc %A0, __tmp_reg__ \n"
	"	ld __tmp_reg__, %a1 \n"
	"	adc %B0, __tmp_reg__ \n"
	"	adc %A0, __zero_reg__ \n"
	"	adc %B0, __zero_reg__ \n"
	: "+r" (sum), "+e" (dest) : : "cc", __tmp_reg__);

#elif defined (MSP430)
	sum = (unsigned short) proto + src[1] + src[3] + dest[1] + dest[3];
	asm volatile (
	"	add %1, %0 \n"
	"	addc #0, %0 \n"
	"	swpb %0 \n"
	: "+r" (sum)
	: "r" (proto_len)
	: "cc");

	asm volatile (
	"	add %1, %0 \n"
	"	addc #0, %0 \n"
	: "+r" (sum)
	: "r" (src[0] + src[2] + dest[0] + dest[2])
	: "cc");

#elif __i386__
	unsigned short tmp1;
	unsigned long tmp2, tmp3;

	/* Sum `proto' and `proto_len'. */
	asm volatile (
	"	rolw $8, %w0 \n"
	"	rolw $8, %w1 \n"
	"	addl %3, %2 \n"
	"	adcl $0, %2 \n"
	"	movw %w2, %w3 \n"
	"	roll $16, %2 \n"
	"	addw %w3, %0 \n"
	"	adcw %w2, %0 \n"
	"	adcw %1, %0 \n"
	"	adcw $0, %0 \n"
	: "=r" (sum), "=r" (tmp1), "=r" (tmp2), "=r" (tmp3)
	: "0" ((unsigned short) proto), "1" (proto_len),
		"2" (*(unsigned long*) src), "3" (*(unsigned long*) dest)
	: "cc");
#else
	unsigned long longsum = proto_len + proto + src[1] + src[3] + dest[1] + dest[3];
	longsum = (longsum >> 8) + ((unsigned char) longsum << 8);
	longsum += src[0] + src[2] + dest[0] + dest[2];

	/* Build cyclic sum. */
	longsum = (longsum >> 16) + (unsigned short) longsum;
	sum = longsum;
	if (longsum & 0x10000)
		++sum;
#endif
	return sum;
}

/*
 * Calculate a new sum given the current sum and the new data.
 * Use 0 as the initial sum value.
 * Do not forget to invert the final checksum value.
 */
unsigned short
crc16_inet_byte (unsigned short sum, unsigned char data)
{
#if __AVR__
	asm volatile (
	"	add %1, %A0 \n"
	"	adc %B0, __zero_reg__ \n"
	"	adc %1, __zero_reg__ \n"
	"	mov %A0, %B0 \n"
	"	mov %B0, %1 \n"
	: "+r" (sum)
	: "r" (data)
	: "cc");

#elif defined (MSP430)
	asm volatile (
	"	add %1, %0 \n"
	"	addc #0, %0 \n"
	"	swpb %0 \n"
	: "+r" (sum)
	: "r" ((unsigned short) data)
	: "cc");

#elif __i386__
	asm volatile (
	"	addw %1, %0 \n"
	"	adcw $0, %0 \n"
	"	rolw $8, %w0 \n"
	: "+r" (sum)
	: "r" ((unsigned short) data)
	: "ax", "cc");
#else
	unsigned long longsum;

	longsum = sum;

	/* add up any odd byte */
#if HTONS(1) == 1
	longsum += data << 8;
#else
	longsum += data;
#endif
	/* Build cyclic sum. */
	longsum = (longsum >> 16) + (unsigned short) longsum;
	sum = longsum;
	if (longsum & 0x10000)
		++sum;

	sum = (sum << 8) | (unsigned char) (sum >> 8);
#endif
	/*debug_printf (" -> 0x%04x\n", sum);*/
	return sum;
}

#ifdef DEBUG_CRC16
#include <stdio.h>
/*#include <time.h>*/

/*
 * Compile with:
 * make crc16-inet CFLAGS='-DDEBUG_CRC16 -I.. -DLINUX386 -I__i386__'
 */
int test_sum (unsigned expected, unsigned initial, unsigned bytes, unsigned char *data)
{
	unsigned sum, i;

	printf ("\tinitial %04x, length %d bytes, data '", initial, bytes);
	for (i=0; i<bytes; ++i) {
		if (i)
			printf ("-");
		printf ("%02x", data[i]);
	}
	printf ("'\n");
	sum = crc16_inet (initial, data, bytes);
	if (sum == expected)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected %04x\n", sum, expected);
}

int test_header (unsigned expected, unsigned proto, unsigned proto_len,
	unsigned char *src, unsigned char *dest)
{
	unsigned sum, i;

	printf ("\tsrc '");
	for (i=0; i<4; ++i) {
		if (i)
			printf ("-");
		printf ("%02x", src[i]);
	}
	printf ("' dest '");
	for (i=0; i<4; ++i) {
		if (i)
			printf ("-");
		printf ("%02x", dest[i]);
	}
	printf ("', proto %02x, proto_len %04x\n", proto, proto_len);
	sum = crc16_inet_header (src, dest, proto, proto_len);
	if (sum == expected)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected %04x\n", sum, expected);
}

int main ()
{
	unsigned short sum, proto_len;
	unsigned char proto;

	printf ("Test 1: crc16_inet()\n");
	test_sum (0x1234, 0x1234, 0, "\x12\x34\x56\x78");

	printf ("Test 2: crc16_inet()\n");
	test_sum (0x4612, 0x1234, 1, "\x12\x34\x56\x78");

	printf ("Test 3: crc16_inet()\n");
	test_sum (0xbe9c, 0x1234, 4, "\x12\x34\x56\x78");

	sum = 0x1234;
	printf ("Test 4: crc16_inet_byte()\n");
	printf ("\tdata '12-34-56-78' length 4 bytes, initial %04x\n", sum);
	sum = crc16_inet_byte (sum, '\x12');
	if (sum == 0x4612)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected 4612\n", sum);
	sum = crc16_inet_byte (sum, '\x34');
	if (sum == 0x4646)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected 4646\n", sum);
	sum = crc16_inet_byte (sum, '\x56');
	if (sum == 0x9c46)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected 9c46\n", sum);
	sum = crc16_inet_byte (sum, '\x78');
	if (sum == 0xbe9c)
		printf ("OK:\tsum = %04x\n", sum);
	else
		printf ("ERROR:\tsum = %04x, expected be9c\n", sum);

	printf ("Test 5: crc16_inet_header()\n");
	test_header (0x787a, 0x77, 0xabcd, "\x12\x34\x56\x78", "\xa1\xb2\xc3\xd4");

	printf ("Test 6: crc16_inet_header()\n");
	test_header (0x1100, 0x11, 0x0000, "\x00\x00\x00\x00", "\x00\x00\x00\x00");

	printf ("Test 7: crc16_inet_header()\n");
	test_header (0x3322, 0x00, 0x2233, "\x00\x00\x00\x00", "\x00\x00\x00\x00");

	printf ("Test 8: crc16_inet_header()\n");
	test_header (0x4231, 0x00, 0x0000, "\x01\x02\x30\x40", "\x00\x00\x00\x00");

	printf ("Test 9: crc16_inet_header()\n");
	test_header (0x2413, 0x00, 0x0000, "\x00\x00\x00\x00", "\x10\x20\x03\x04");
	return 0;
}
#endif /* DEBUG_CRC16 */
