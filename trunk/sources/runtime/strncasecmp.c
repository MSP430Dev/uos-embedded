#include <runtime/lib.h>

/* Compare no more than N characters of S1 and S2,
   ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less
   than, equal to or greater than S2.  */

sign_t
strncasecmp (const unsigned char *s1, const unsigned char *s2, size_t n)
{
	unsigned char c1, c2;
	signed char result;

	if (s1 == s2 || n == 0)
		return 0;

	do {
		c1 = *s1++;
		c2 = *s2++;
		if (isupper (c1))
			c1 = tolower (c1);
		if (isupper (c2))
			c2 = tolower (c2);

		result = c1 - c2;
		if (result != 0)
			return result;

	} while (c1 != '\0' && --n > 0);

	return 0;
}
