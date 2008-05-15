/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

void *
memchr(const void *s, unsigned char c, mem_size_t n)
{
	const unsigned char *cp = (const unsigned char*) s;

	if (n) {
		do {
			if (*cp++ == c)
				goto found;  /* ugly, but faster loop */
		} while (--n);
	}
	return 0;
found:
	return (void*) (cp - 1);
}
