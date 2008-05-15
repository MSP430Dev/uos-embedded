#include <runtime/lib.h>
#include <stream/stream.h>

int
sscanf (const unsigned char *buf, const char *fmt, ...)
{
	stream_buf_t u;
	va_list	args;
	int err;

	stropen (&u, (unsigned char*) buf, 0);
	va_start (args, fmt);
	err = vscanf (&u, fmt, args);
	va_end (args);
	return err;
}

#ifdef TEST
/*
 * Testing sscanf(3).
 */
#undef printf
int printf (const char *format, ...);

int
main (void)
{
	int i;
	short s;
	long l;
	char a[20], b[20];

	sscanf ("    -123 +123 123xyzwvabcdefghijk ", " %d%ho%lx%5c%s",
		&i, &s, &l, a, b);
	printf ("i=%d s=%d l=%ld a=`%.5s' b=`%s'\n",
		i, s, l, a, b);

	sscanf ("abc0123456789xyz", "abc%[0123456789]xyz", a);
	printf ("a =`%s'\n", a);

	sscanf ("abc0123456789xyz", "abc%[0-9]xyz", a);
	printf ("a =`%s'\n", a);

	sscanf ("abc0123456789xyz", "abc%[^a-z]xyz", a);
	printf ("a =`%s'\n", a);

	return 0;
}
#endif
