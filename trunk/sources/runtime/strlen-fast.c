/*
FUNCTION
	<<strlen>>---character string length

INDEX
	strlen

ANSI_SYNOPSIS
	#include <string.h>
	size_t strlen(const char *<[str]>);

TRAD_SYNOPSIS
	#include <string.h>
	size_t strlen(<[str]>)
	char *<[src]>;

DESCRIPTION
	The <<strlen>> function works out the length of the string
	starting at <<*<[str]>>> by counting chararacters until it
	reaches a <<NULL>> character.

RETURNS
	<<strlen>> returns the character count.

PORTABILITY
<<strlen>> is ANSI C.

<<strlen>> requires no supporting OS subroutines.

QUICKREF
	strlen ansi pure
*/
#include <runtime/lib.h>

#define LBLOCKSIZE   (sizeof (long))
#define UNALIGNED(X) ((long)X & (LBLOCKSIZE - 1))

#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)

size_t
strlen(const unsigned char *str)
{
  const unsigned char *start = str;
  unsigned long *aligned_addr;

  if (!UNALIGNED (str))
    {
      /* If the string is word-aligned, we can check for the presence of
         a null in each word-sized block.  */
      aligned_addr = (unsigned long*)str;
      while (!DETECTNULL (*aligned_addr))
        aligned_addr++;

      /* Once a null is detected, we check each byte in that block for a
         precise position of the null.  */
      str = (unsigned char*)aligned_addr;
    }

  while (*str)
    str++;
  return str - start;
}
