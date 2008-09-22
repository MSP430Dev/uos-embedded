/*
FUNCTION
	<<strncpy>>---counted copy string

INDEX
	strncpy

ANSI_SYNOPSIS
	#include <string.h>
	char *strncpy(char *<[dst]>, const char *<[src]>, size_t <[length]>);

TRAD_SYNOPSIS
	#include <string.h>
	char *strncpy(<[dst]>, <[src]>, <[length]>)
	char *<[dst]>;
	char *<[src]>;
	size_t <[length]>;

DESCRIPTION
	<<strncpy>> copies not more than <[length]> characters from the
	the string pointed to by <[src]> (including the terminating
	null character) to the array pointed to by <[dst]>.  If the
	string pointed to by <[src]> is shorter than <[length]>
	characters, null characters are appended to the destination
	array until a total of <[length]> characters have been
	written.

RETURNS
	This function returns the initial value of <[dst]>.

PORTABILITY
<<strncpy>> is ANSI C.

<<strncpy>> requires no supporting OS subroutines.

QUICKREF
	strncpy ansi pure
*/
#include <runtime/lib.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)

#define TOO_SMALL(LEN) ((LEN) < sizeof (long))

unsigned char *
strncpy(unsigned char *dst0, const unsigned char *src0, size_t count)
{
  unsigned char *dst = dst0;
  const unsigned char *src = src0;
  long *aligned_dst;
  const long *aligned_src;

  /* If SRC and DEST is aligned and count large enough, then copy words.  */
  if (!UNALIGNED (src, dst) && !TOO_SMALL (count))
    {
      aligned_dst = (long*)dst;
      aligned_src = (long*)src;

      /* SRC and DEST are both "long int" aligned, try to do "long int"
	 sized copies.  */
      while (count >= sizeof (long int) && !DETECTNULL(*aligned_src))
	{
	  count -= sizeof (long int);
	  *aligned_dst++ = *aligned_src++;
	}

      dst = (unsigned char*)aligned_dst;
      src = (unsigned char*)aligned_src;
    }

  while (count > 0)
    {
      --count;
      if ((*dst++ = *src++) == '\0')
	break;
    }

  while (count-- > 0)
    *dst++ = '\0';

  return dst0;
}
