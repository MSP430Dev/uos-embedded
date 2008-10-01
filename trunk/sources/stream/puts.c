#include <runtime/lib.h>
#include <stream/stream.h>

int stream_puts (stream_t *stream, const char *str)
{
	int length;
	unsigned char c;

	for (length = 0; ; length++) {
		c = FETCH_BYTE (str);
		if (! c)
			return length;
		putchar (stream, c);
		++str;
	}
}
