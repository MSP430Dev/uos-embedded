#include <runtime/lib.h>
#include <stream/stream.h>

int
vsnprintf (unsigned char *buf, int size, const char *fmt, va_list args)
{
	stream_buf_t u;
	int err;

	stropen (&u, buf, size);
	err = vprintf (&u, fmt, args);
	strclose (&u);
	return err;
}
