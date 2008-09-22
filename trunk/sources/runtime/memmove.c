/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

void *
memmove(void *dest, const void *src, size_t n)
{
	const unsigned char *csrc = (const unsigned char*) src;

	if (n) {
		register unsigned char *tmp = (unsigned char *) dest;

		if (csrc < tmp) {
			tmp += n;
			csrc += n;
			do {
				*--tmp = *--csrc;
			} while (--n);
		} else {
			do {
				*tmp++ = *csrc++;
			} while (--n);
		}
	}
	return dest;
}
