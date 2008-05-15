/*
FUNCTION
	<<strchr>>---search for character in string

INDEX
	strchr

ANSI_SYNOPSIS
	#include <string.h>
	char * strchr(const char *<[string]>, int <[c]>);

TRAD_SYNOPSIS
	#include <string.h>
	char * strchr(<[string]>, <[c]>);
	const char *<[string]>;
	int <[c]>;

DESCRIPTION
	This function finds the first occurence of <[c]> (converted to
	a char) in the string pointed to by <[string]> (including the
	terminating null character).

RETURNS
	Returns a pointer to the located character, or a null pointer
	if <[c]> does not occur in <[string]>.

PORTABILITY
<<strchr>> is ANSI C.

<<strchr>> requires no supporting OS subroutines.

QUICKREF
	strchr ansi pure
*/
#include <runtime/lib.h>

/* Nonzero if X is not aligned on a "long" boundary.  */
#define UNALIGNED(X) ((long)X & (sizeof (long) - 1))

/* How many bytes are loaded each iteration of the word copy loop.  */
#define LBLOCKSIZE (sizeof (long))

#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)

/* DETECTCHAR returns nonzero if (long)X contains the byte used
   to fill (long)MASK. */
#define DETECTCHAR(X,MASK) (DETECTNULL(X ^ MASK))

unsigned char *
strchr(const unsigned char *s, unsigned char i)
{
  unsigned char c = (unsigned char)i;
  unsigned long mask,j;
  unsigned long *aligned_addr;

  if (!UNALIGNED (s))
    {
      mask = 0;
      for (j = 0; j < LBLOCKSIZE; j++)
        mask = (mask << 8) | c;

      aligned_addr = (unsigned long*)s;
      while (!DETECTNULL (*aligned_addr) && !DETECTCHAR (*aligned_addr, mask))
        aligned_addr++;

      /* The block of bytes currently pointed to by aligned_addr
         contains either a null or the target char, or both.  We
         catch it using the bytewise search.  */

      s = (unsigned char*)aligned_addr;
    }

  while (*s && *s != c)
      s++;
  if (*s == c)
    return (unsigned char*) s;
  return 0;
}
