/*
 * Copyright (C) 2009 Serge Vakulenko <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * without any conditions or restrictions.  This software is provided
 * "as is" without express or implied warranty.
 */
#include "regexp9.h"
#include "regpriv.h"

unsigned short *
_runestrchr(const unsigned short *s, unsigned short c)
{
	register unsigned short tmp;

	do {
		tmp = *s++;
		if (tmp == c)
			return (unsigned short*) (s-1);
	} while (tmp != 0);
	return 0;
}
