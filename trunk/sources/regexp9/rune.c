/*
 * The authors of this software are Rob Pike and Howard Trickey.
 * 		Copyright (c) 1992 by AT&T.
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */

/* Copyright (c) 1992 AT&T - All rights reserved. */
#include <string.h>
#include "regexp9.h"

int
_chartorune(Rune *rune, const char *str)
{
	int c, c1, c2;
	long l;

	/*
	 * one character sequence
	 *	00000-0007F => T1
	 */
	c = *(unsigned char*)str;
	if(c < 0x80) {
		*rune = c;
		return 1;
	}

	/*
	 * two character sequence
	 *	0080-07FF => T2 Tx
	 */
	c1 = *(unsigned char*)(str+1) ^ 0x80;
	if(c1 & 0xc0)
		goto bad;
	if(c < 0xe0) {
		if(c < 0xc0)
			goto bad;
		l = ((c << 6) | c1) & 0x07ff;
		if(l <= 0x007f)
			goto bad;
		*rune = l;
		return 2;
	}

	/*
	 * three character sequence
	 *	0800-FFFF => T3 Tx Tx
	 */
	c2 = *(unsigned char*)(str+2) ^ 0x80;
	if(c2 & 0xc0)
		goto bad;
	if(c < 0xf0) {
		l = ((((c << 6) | c1) << 6) | c2) & 0xffff;
		if(l <= 0x07ff)
			goto bad;
		*rune = l;
		return 3;
	}

	/*
	 * bad decoding
	 */
bad:
	*rune = Runeerror;
	return 1;
}

char*
_utfrune(char *s, long c)
{
	long c1;
	Rune r;
	int n;

	if(c < Runesync)		/* not part of utf sequence */
		return strchr(s, c);

	for(;;) {
		c1 = *(unsigned char*)s;
		if(c1 < Runeself) {	/* one byte rune */
			if(c1 == 0)
				return 0;
			if(c1 == c)
				return s;
			s++;
			continue;
		}
		n = _chartorune(&r, s);
		if(r == c)
			return s;
		s += n;
	}
	return 0;
}
