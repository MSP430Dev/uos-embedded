#include <runtime/lib.h>

/* Compare S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */

sign_t
strcasecmp (const unsigned char *s1, const unsigned char *s2)
{
	unsigned char c1, c2;
	signed char result;

	if (p1 == p2)
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

	} while (c1 != '\0');

	return 0;
}
