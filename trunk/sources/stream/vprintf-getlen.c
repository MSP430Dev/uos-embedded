#include <runtime/lib.h>
#include <stream/stream.h>

static void null_putchar (stream_t *t, short c)
{
}

stream_interface_t null_stream_interface = {
	.putc = (void (*) (stream_t*, short)) null_putchar
};

stream_t null_stream = {&null_stream_interface};

int vprintf_getlen (const char *fmt, va_list args)
{
	return vprintf (&null_stream, fmt, args);
}
