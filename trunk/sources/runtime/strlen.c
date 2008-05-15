/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

mem_size_t
strlen(const unsigned char *s)
{
	const unsigned char *s1 = s;

	while (*s1++ != '\0')
		;
	return s1 - s - 1;
}
