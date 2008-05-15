/*
 * Match name with pattern, using `shell' style.
 * Based on FSF fnmatch().
 *
 * Copyright (C) 2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <runtime/lib.h>

/*
 * Match name with pattern.
 * Returns the initial string on success, or 0 on failure.
 * Pattern may contain the following wild symbols:
 *
 * *       - any string
 * ?       - any symbol
 * [a-z]   - symbol in range
 * [^a-z]  - symbol out of range
 * [!a-z]  - symbol out of range
 * \       - escapes * and ? (but not inside [])
 */
const unsigned char *
strmatch (const unsigned char *string, const unsigned char *pattern)
{
	const unsigned char *p = pattern, *n = string;
	unsigned char c, c1, cstart, cend, not;

	while ((c = *p++) != '\0') {
		switch (c) {
		case '?':
			if (*n == '\0')
				return 0;
			break;

		case '*':
			for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
				if (c == '?' && *n == '\0')
					return 0;

			if (c == '\0')
				return string;

			c1 = (c == '\\') ? *p : c;
			for (--p; *n != '\0'; ++n)
				if ((c == '[' || *n == c1) &&
				    strmatch (n, p) == 0)
					return string;
			return 0;

		case '[':
			if (*n == '\0')
				return 0;

			/* `Not' is nonzero if the sense of the character
			 * class is inverted.  */
			not = (*p == '!' || *p == '^');
			if (not)
				++p;

			c = *p++;
			for (;;) {
				/* [ (unterminated) loses.  */
				if (c == '\0')
					return 0;

				cstart = cend = c;
				c = *p++;
				if (c == '-' && *p != ']')  {
					cend = *p++;
					if (cend == '\0')
						return 0;

					c = *p++;
				}

				if (*n >= cstart && *n <= cend)
					goto matched;

				if (c == ']')
					break;
			}
			if (! not)
				return 0;
			break;

matched:		/* Skip the rest of the [...] that already matched. */
			while (c != ']') {
				/* [... (unterminated) loses.  */
				if (c == '\0')
					return 0;

				c = *p++;
			}
			if (not)
				return 0;
			break;

		case '\\':
			c = *p++;
			/* fall through... */
		default:
			if (c != *n)
				return 0;
			break;
		}
		++n;
	}

	if (*n == '\0')
		return string;

	return 0;
}
