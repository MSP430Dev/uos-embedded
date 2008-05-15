#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>

static stream_interface_t debug_interface = {
	debug_putchar,
	0, 0,
};

static struct {
	stream_interface_t *interface;
} debug = { &debug_interface };

int
debug_printf (char *fmt, ...)
{
	va_list	args;
	int err;

	va_start (args, fmt);
	err = vprintf (&debug, fmt, args);
	va_end (args);
	return err;
}

int
debug_vprintf (const char *fmt, va_list args)
{
	return vprintf (&debug, fmt, args);
}
