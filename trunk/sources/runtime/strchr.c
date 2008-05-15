/*
   Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>

   Permission to use, copy, modify, and distribute this software and
   its documentation for any purpose and without fee is hereby granted,
   without any conditions or restrictions.  This software is provided
   "as is" without express or implied warranty.
 */

#include <runtime/lib.h>

unsigned char *
strchr(const unsigned char *s, unsigned char c)
{
	register unsigned char tmp;

	do {
		tmp = *s++;
		if (tmp == c)
			return (unsigned char *)(s - 1);
	} while (tmp != '\0');
	return 0;
}
