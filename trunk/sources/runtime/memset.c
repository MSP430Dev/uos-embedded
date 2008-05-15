/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

#undef memset

void *
memset(void *s, unsigned char c, mem_size_t n)
{
	if (n) {
		unsigned char *s1 = (unsigned char *) s;
		do {
			*s1++ = c;
		} while (--n);
	}
	return s;
}
