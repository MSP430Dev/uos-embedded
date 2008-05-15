/*
FUNCTION
	<<strncat>>---concatenate strings

INDEX
	strncat

ANSI_SYNOPSIS
	#include <string.h>
	char *strncat(char *<[dst]>, const char *<[src]>, size_t <[length]>);

TRAD_SYNOPSIS
	#include <string.h>
	char *strncat(<[dst]>, <[src]>, <[length]>)
	char *<[dst]>;
	char *<[src]>;
	size_t <[length]>;

DESCRIPTION
	<<strncat>> appends not more than <[length]> characters from
	the string pointed to by <[src]> (including the	terminating
	null character) to the end of the string pointed to by
	<[dst]>.  The initial character of <[src]> overwrites the null
	character at the end of <[dst]>.  A terminating null character
	is always appended to the result

WARNINGS
	Note that a null is always appended, so that if the copy is
	limited by the <[length]> argument, the number of characters
	appended to <[dst]> is <<n + 1>>.


RETURNS
	This function returns the initial value of <[dst]>

PORTABILITY
<<strncat>> is ANSI C.

<<strncat>> requires no supporting OS subroutines.

QUICKREF
	strncat ansi pure
*/
#include <runtime/lib.h>

/* Nonzero if X is aligned on a "long" boundary.  */
#define ALIGNED(X) \
  (((long)X & (sizeof (long) - 1)) == 0)

#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)

unsigned char *
strncat(unsigned char *s1, const unsigned char *s2, mem_size_t n)
{
  unsigned char *s = s1;

  /* Skip over the data in s1 as quickly as possible.  */
  if (ALIGNED (s1))
    {
      unsigned long *aligned_s1 = (unsigned long *)s1;
      while (!DETECTNULL (*aligned_s1))
	aligned_s1++;

      s1 = (unsigned char *)aligned_s1;
    }

  while (*s1)
    s1++;

  /* s1 now points to the its trailing null character, now copy
     up to N bytes from S2 into S1 stopping if a NULL is encountered
     in S2.

     It is not safe to use strncpy here since it copies EXACTLY N
     characters, NULL padding if necessary.  */
  while (n-- != 0 && (*s1++ = *s2++))
    {
      if (n == 0)
	*s1 = '\0';
    }

  return s;
}
