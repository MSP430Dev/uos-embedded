#include <runtime/lib.h>
#include <stream/stream.h>

/*
 * Read a newline-terminated string from stream.
 */
unsigned char *
stream_gets (stream_t *stream, unsigned char *buf, int len)
{
	int c;
	unsigned char *s;

	s = buf;
        while (--len > 0) {
		if (feof (stream)) {
			if (s == buf)
				return 0;
			break;
		}
		c = getchar (stream);
//debug_printf ("c = `%c'\n", c);
		*s++ = c;
		if (c == /*'\n'*/ 13)
			break;
	}
	*s = '\0';
	return buf;
}
