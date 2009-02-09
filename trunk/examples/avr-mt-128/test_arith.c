/*
 * Testing runtime arithmetics.
 */
#include <runtime/lib.h>

#if 0
#include <stdio.h>
#include <inttypes.h>
#define debug_printf printf
#endif

int errors;

void unsigned_32_div (uint32_t a, uint32_t b, uint32_t expected)
{
	uint32_t result;

	result = a / b;
	if (result != expected) {
		debug_printf ("udiv32 error: %lu / %lu -> %lu, expected %lu\n",
			(unsigned long) a, (unsigned long) b,
			(unsigned long) result, (unsigned long) expected);
		++errors;
	}
}

void unsigned_16_div (uint16_t a, uint16_t b, uint16_t expected)
{
	uint16_t result;

	result = a / b;
	if (result != expected) {
		debug_printf ("udiv16 error: %u / %u -> %u, expected %u\n",
			a, b, result, expected);
		++errors;
	}
}

void signed_32_div (int32_t a, int32_t b, int32_t expected)
{
	int32_t result;

	result = a / b;
	if (result != expected) {
		debug_printf ("div32 error: %ld / %ld -> %ld, expected %ld\n",
			(long) a, (long) b, (long) result, (long) expected);
		++errors;
	}
}

void signed_16_div (int16_t a, int16_t b, int16_t expected)
{
	int16_t result;

	result = a / b;
	if (result != expected) {
		debug_printf ("div16 error: %d / %d -> %d, expected %d\n",
			a, b, result, expected);
		++errors;
	}
}

void unsigned_32_mul ()
{
	unsigned int khz = 16000;
        unsigned long baud = 38400;

	debug_printf ("khz = %d, baud = %ld, q = %d, expected %d\n",
		khz, baud,
		((int) (khz*1000L / baud) + 8) / 16 - 1,
		((int) (16000 * 1000L / 38400) + 8) / 16 - 1);
}

int main ()
{
	/* Baud 38400. */
	UBRR1L = ((int) (KHZ * 1000L / 38400) + 8) / 16 - 1;
	debug_puts ("Testing arithmetics.\n");

	/* Unsigned integer 32. */
	unsigned_32_div (100, 10, 10);
	unsigned_32_div (0xFFFFFFFF, 10, 429496729);

	/* Signed integer 32. */
	signed_32_div (100, 10, 10);
	signed_32_div (100, -10, -10);
	signed_32_div (0x7FFFFFFF, 10, 214748364);
	signed_32_div (0x7FFFFFFF, -10, -214748364);
	signed_32_div (-100, 10, -10);
	signed_32_div (-100, -10, 10);
	signed_32_div (0x80000000, 10, -214748364);
	signed_32_div (0x80000000, -10, 214748364);

	/* Unsigned integer 16. */
	unsigned_16_div (100, 10, 10);
	unsigned_16_div (0xFFFF, 10, 6553);

	/* Signed integer 16. */
	signed_16_div (100, 10, 10);
	signed_16_div (100, -10, -10);
	signed_16_div (0x7FFF, 10, 3276);
	signed_16_div (0x7FFF, -10, -3276);
	signed_16_div (-100, 10, -10);
	signed_16_div (-100, -10, 10);
	signed_16_div (0x8000, 10, -3276);
	signed_16_div (0x8000, -10, 3276);

	unsigned_32_mul ();

	if (! errors)
		debug_puts ("All tests OK.\n");

	return 0;
}
