#include <runtime/lib.h>
#include <stream/stream.h>

uint_t stream_puts (stream_t *stream, const char *str)
{
	uint_t length;
	unsigned char c;

	for (length = 0; ; length++) {
		c = FETCH_BYTE (str);
		if (! c)
			return length;
		putchar (stream, c);
		++str;
	}
}
