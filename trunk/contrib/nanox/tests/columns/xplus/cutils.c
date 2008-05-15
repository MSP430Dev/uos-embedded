#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "extern.h"

void xFatal (char *fmt, ...)
{
	va_list args;

	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	va_end (args);
	exit (-1);
}

void xError (char *fmt, ...)
{
	va_list args;

	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	va_end (args);
}
