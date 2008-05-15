#include <runtime/lib.h>
#include <crc/crc-rmon.h>

/* LY: циклическое сложение с переносом. */
#if defined (__arm__) && defined (__thumb__)
#	define cyclic_add(sum, val)		\
	do {					\
		__asm__ (			\
		"add %0, %1 \n"			\
		"adc %0, %2 \n"			\
		: "+r" (sum) : "r" ((unsigned long) val), "r" (0) : "cc"); \
	} while (0)
#elif defined (__arm__)
#	define cyclic_add(sum, val)		\
	do {					\
		__asm__ (			\
		"adds %0, %0, %1 \n"		\
		"adc %0, %0, #0 \n"		\
		: "+r" (sum) : "r" ((unsigned long) val) : "cc"); \
	} while (0)
#elif ! defined (__AVR__)
#	define cyclic_add(sum, val)		\
	do {					\
		sum += val;			\
		if (sum < val)			\
			sum++;			\
	} while (0)
#endif

unsigned long crc32_rmon_byte (unsigned long sum, unsigned char byte)
{
#ifdef __AVR__
	__asm__ (
	"add %1, %A0 \n\t"
	"adc %B0, __zero_reg__ \n\t"
	"adc %C0, __zero_reg__ \n\t"
	"adc %D0, __zero_reg__ \n\t"
	"adc %1, __zero_reg__ \n\t"
	"mov %A0, %B0 \n\t"
	"mov %B0, %C0 \n\t"
	"mov %C0, %D0 \n\t"
	"mov %D0, %1"
	: "+r" (sum), "+r" (byte) : : "cc");
	return sum;
#else
	cyclic_add (sum, byte);
	return (sum >> 8) | (sum << 24);
#endif
}

unsigned long crc32_rmon (unsigned const char *buf, unsigned short len)
{
	unsigned long sum = 0;
	while (len >= 4) {
#ifdef __AVR__
		 __asm__ (
		"ld __tmp_reg__, %a1+ \n\t"
		"add %A0, __tmp_reg__ \n\t"
		"ld __tmp_reg__, %a1+ \n\t"
		"adc %B0, __tmp_reg__ \n\t"
		"ld __tmp_reg__, %a1+ \n\t"
		"adc %C0, __tmp_reg__ \n\t"
		"ld __tmp_reg__, %a1+ \n\t"
		"adc %D0, __tmp_reg__ \n\t"
		"adc %A0, __zero_reg__ \n\t"
		"adc %B0, __zero_reg__ \n\t"
		"adc %C0, __zero_reg__ \n\t"
		"adc %D0, __zero_reg__"
		: "+r" (sum), "+e" (buf) : : "cc", __tmp_reg__);
#else
		BUILD_ONLY_LITTLE_ENDIAN;
		cyclic_add (sum, (*(unsigned long*) buf));
		buf += 4;
#endif
		len -= 4;
	}
	while (len--)
		sum = crc32_rmon_byte (sum, *buf++);

	return ~sum;
}

unsigned char crc8_rmon (unsigned const char *buf, unsigned short len)
{
	unsigned short sum = 0;
	while (len-- > 1)
		sum += *buf++;
	return crc8_rmon_final (sum, *buf);
}

unsigned char crc8_rmon_final (unsigned short sum, unsigned char last)
{
	return ~(((unsigned char) sum) + ((unsigned char) (sum >> 8)) + last);
}
