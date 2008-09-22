/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

size_t
strnlen(const unsigned char *s, size_t n)
{
	const unsigned char *s0 = s;
	size_t n0 = n;

	if (n) {
		do {
			if (*((const unsigned char *) s)++ == '\0')
				/* return --s - s0; */
				goto found;  /* ugly, but faster loop */
		} while (--n);
	}
	return n0;
found:
	return s - s0 - 1;
}
