/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

sign_t
strcmp(const unsigned char *s1, const unsigned char *s2)
{
	register signed char ret;
	register unsigned char tmp;

	do {
		ret = *s1++ - (tmp = *s2++);
	} while ((ret == 0) && (tmp != '\0'));
	return ret;
}
