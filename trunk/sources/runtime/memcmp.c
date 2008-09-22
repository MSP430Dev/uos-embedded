/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

#undef memcmp

sign_t
memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *cs1 = (const unsigned char*) s1;
	const unsigned char *cs2 = (const unsigned char*) s2;

	if (n) {
		signed char ret;
		do
			ret = *cs1++ - *cs2++;
		while (ret == 0 && --n);
		return ret;
	}
	return 0;
}
