#include <runtime/lib.h>
#include <stream/stream.h>

void drain_input (stream_t *stream)
{
	while (peekchar (stream) >= 0)
		getchar (stream);
}
