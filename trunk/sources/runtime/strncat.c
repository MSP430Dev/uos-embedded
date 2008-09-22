/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

unsigned char *
strncat(unsigned char *dest, const unsigned char *src, size_t n)
{
	register unsigned char *tmp = dest;

	if (n) {
		while (*tmp++ != '\0')
			;
		tmp--;
		while ((*tmp++ = *src++) != '\0' && --n)
			;
	}
	return dest;
}
