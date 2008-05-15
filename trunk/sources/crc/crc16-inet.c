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
	"	addw %w2, %0 \n"
	"	adcw %1, %0 \n"
	"	adcw $0, %0 \n"
	: "=q" (sum), "=q" (tmp1), "=q" (tmp2), "=q" (tmp3)
	: "0" ((unsigned short) proto), "1" (proto_len),
		"2" (*(unsigned long*) src), "3" (*(unsigned long*) dest)
	: "cc");
#else
	proto_len = HTONS (proto_len);
	sum = crc16_inet (HTONS (proto), (unsigned char*) &proto_len, 2);
	sum = crc16_inet (sum, src, 4);
	sum = crc16_inet (sum, dest, 4);
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

#elif __i386__
	asm volatile (
	"	addw %2, %0 \n"
	"	adcw $0, %0 \n"
	"	rolw $8, %w0 \n"
	: "=g" (sum)
	: "0" (sum), "g" (data)
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
#include <time.h>

/*
 * Compile with:
 * make crc16-inet CFLAGS=-DDEBUG_CRC16 -I.. -Dmem_size_t=int -U__i386__
 */
int
main ()
{
#define N 1024
	unsigned char array [N];
	unsigned char array1 [N+1];
	unsigned short i, sum, sum1;

	srandom (time(0));
	for (i=0; i<N; ++i)
		array1[i+1] = array[i] = random();

	for (i=1; i<=N; ++i) {
		sum = crc16_inet (0xaa55, array, i);
		sum1 = crc16_inet (0xaa55, array1+1, i);
		if (sum != sum1)
			printf ("[%d] - %02x - %02x\n", i, sum, sum1);
	}
	return 0;
}
#endif /* DEBUG_CRC16 */
