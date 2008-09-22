#include <runtime/lib.h>
#include <stream/stream.h>

stream_interface_t debug_stream_interface = {
	.putc = (void (*) (stream_t*, short)) debug_putchar,
	.getc = (unsigned short (*) (stream_t*)) debug_getchar,
	.peekc = (int (*) (stream_t*)) debug_peekchar,
};

struct {
	stream_interface_t *interface;
} debug_stream = { &debug_stream_interface };

int
debug_printf (const char *fmt, ...)
{
	va_list	args;
	int_t err;

	va_start (args, fmt);
	err = vprintf (&debug_stream, fmt, args);
	va_end (args);
	return err;
}

int
debug_vprintf (const char *fmt, va_list args)
{
	int_t err;

	err = vprintf (&debug_stream, fmt, args);
	return err;
}

void debug_putc (char c)
{
	debug_putchar (0, c);
}
