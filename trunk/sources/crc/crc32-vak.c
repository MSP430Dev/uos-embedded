#include <runtime/lib.h>
#include <crc/crc32-vak.h>

#ifdef __AVR__
	/* LY: кольцевой сдвиг влево на 13, или в право на 19. */
	static unsigned long __attribute__((always_inline, const)) rot13 (unsigned long v)
	{
		unsigned long r;

		__asm__ (
		/* LY: кольцевой сдвиг на 16 через загрузку пар крест-на-крест. */
			"movw %A0, %C1 \n\t"
			"movw %C0, %A1 \n\t"
		/* LY: осталось циклически сдвинуть право на 3 (16 - 3 = 13). */
			"mov __tmp_reg__, %A0 \n\t"
			"lsr __tmp_reg__ \n\t"	/* LY: бит #0 в carry, и сдвигаем четыре байта. */
			"ror %D0 \n\t"
			"ror %C0 \n\t"
			"ror %B0 \n\t"
			"ror %A0 \n\t"	/* LY: теперь в carry снова бит #0, он нам не нужен. */
			"lsr __tmp_reg__ \n\t"	/* LY: бит #1 в carry, и сдвигаем четыре байта. */
			"ror %D0 \n\t"
			"ror %C0 \n\t"
			"ror %B0 \n\t"
			"ror %A0 \n\t"	/* LY: теперь в carry снова бит #1, он нам не нужен. */
			"lsr __tmp_reg__ \n\t"	/* LY: бит #2 в carry, и сдвигаем четыре байта. */
			"ror %D0 \n\t"
			"ror %C0 \n\t"
			"ror %B0 \n\t"
			"ror %A0"	/* LY: теперь в carry снова бит #2, он нам не нужен. */
			: "=r" (r) : "r" (v) : "cc", __tmp_reg__
		);
		return r;
	}
#else
#	define rot13(v) (((v) << 13) | ((v) >> 19))
#endif

unsigned long crc32_vak_byte (unsigned long sum, unsigned char byte)
{
	sum += byte;
	sum -= rot13 (sum);
	return sum;
}

unsigned long crc32_vak (unsigned long sum, unsigned const char *buf, unsigned short len)
{
	if (len) do
		sum = crc32_vak_byte (sum, *buf++);
	while (--len);
	return sum;
}
