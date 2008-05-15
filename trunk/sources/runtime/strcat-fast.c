/*
FUNCTION
	<<strcat>>---concatenate strings

INDEX
	strcat

ANSI_SYNOPSIS
	#include <string.h>
	char *strcat(char *<[dst]>, const char *<[src]>);

TRAD_SYNOPSIS
	#include <string.h>
	char *strcat(<[dst]>, <[src]>)
	char *<[dst]>;
	char *<[src]>;

DESCRIPTION
	<<strcat>> appends a copy of the string pointed to by <[src]>
	(including the terminating null character) to the end of the
	string pointed to by <[dst]>.  The initial character of
	<[src]> overwrites the null character at the end of <[dst]>.

RETURNS
	This function returns the initial value of <[dst]>

PORTABILITY
<<strcat>> is ANSI C.

<<strcat>> requires no supporting OS subroutines.

QUICKREF
	strcat ansi pure
*/
#include <runtime/lib.h>

/* Nonzero if X is aligned on a "long" boundary.  */
#define ALIGNED(X) \
  (((long)X & (sizeof (long) - 1)) == 0)

#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)

unsigned char *
strcat(unsigned char *s1, const unsigned char *s2)
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

  /* s1 now points to the its trailing null character, we can
     just use strcpy to do the work for us now.

     ?!? We might want to just include strcpy here.
     Also, this will cause many more unaligned string copies because
     s1 is much less likely to be aligned.  I don't know if its worth
     tweaking strcpy to handle this better.  */
  strcpy (s1, s2);

  return s;
}
