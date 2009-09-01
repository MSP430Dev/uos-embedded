/*
 * Copyright (C) 2009 Serge Vakulenko <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * without any conditions or restrictions.  This software is provided
 * "as is" without express or implied warranty.
 */
#include "regexp9.h"

Rune *
_runestrchr(Rune *s, Rune c)
{
	register Rune tmp;

	do {
		tmp = *s++;
		if (tmp == c)
			return s-1;
	} while (tmp != 0);
	return 0;
}
