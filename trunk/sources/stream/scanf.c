#include <runtime/lib.h>
#include <stream/stream.h>

int
stream_scanf (stream_t *u, const char *fmt, ...)
{
	va_list	args;
	int err;

	va_start (args, fmt);
	err = stream_vscanf (u, fmt, args);
	va_end (args);
	return err;
}
