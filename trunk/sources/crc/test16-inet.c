#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Calculate a new sum given the current sum and the new data.
 * Use 0 as the initial sum value.
 * Do not forget to invert the final checksum value.
 */
unsigned short
crc16_inet (unsigned short sum, unsigned const char *buf, unsigned short len)
{
	unsigned long longsum;
	unsigned swap_flag;

	longsum = sum;
	swap_flag = (int) buf & 1;
	if (swap_flag) {
		/* get first non-aligned byte */
		longsum += *buf++ << 8;
		--len;
	}
	for (; len>1; len-=2, buf+=2)
		longsum += *(unsigned short*) buf;

	if (len & 1) {
		/* add up any odd byte */
		longsum += *buf;
	}
	/* Build cyclic sum. */
	longsum = (longsum >> 16) + (unsigned short) longsum;
	sum = longsum;
	if (longsum & 0x10000)
		++sum;
	if (swap_flag) {
		sum = sum << 8 | sum >> 8;
	}
	/*debug_printf (CONST(" -> 0x%04x\n"), sum);*/
	return sum;
}

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

	for (i=1; i<N; ++i) {
		sum = crc16_inet (0, array, i);
		sum1 = crc16_inet (0, array1+1, i);
		sum1 = sum1 >> 8 | sum1 << 8;
		if (sum != sum1)
			printf ("[%d] - %02x - %02x\n", i, sum, sum1);
	}
	return 0;
}
