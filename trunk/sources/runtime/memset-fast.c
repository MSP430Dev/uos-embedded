/*
FUNCTION
	<<memset>>---set an area of memory

INDEX
	memset

ANSI_SYNOPSIS
	#include <string.h>
	void *memset(const void *<[dst]>, int <[c]>, size_t <[length]>);

TRAD_SYNOPSIS
	#include <string.h>
	void *memset(<[dst]>, <[c]>, <[length]>)
	void *<[dst]>;
	int <[c]>;
	size_t <[length]>;

DESCRIPTION
	This function converts the argument <[c]> into an unsigned
	char and fills the first <[length]> characters of the array
	pointed to by <[dst]> to the value.

RETURNS
	<<memset>> returns the value of <[m]>.

PORTABILITY
<<memset>> is ANSI C.

    <<memset>> requires no supporting OS subroutines.

QUICKREF
	memset ansi pure
*/
#include <runtime/lib.h>

#undef memset

#define LBLOCKSIZE	(sizeof(long))
#define UNALIGNED(X)	((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN)	((LEN) < LBLOCKSIZE)

void *
memset(void *m, unsigned char c, mem_size_t n)
{
  unsigned char *s;
  unsigned long buffer;

  if (n == 0)
    return m;

  s = (unsigned char*) m;
  while (UNALIGNED (s))
    {
      *s++ = c;
      if (--n == 0)
        return m;
    }

  if (! TOO_SMALL (n))
    {
      /* If we get this far, we know that n is large and s is word-aligned. */

      /* Store D into each char sized location in BUFFER so that
         we can set large blocks quickly.  */
      buffer = c;
      buffer |= (buffer << 8);
      buffer |= (buffer << 16);
      if (LBLOCKSIZE > 4)
        buffer |= (buffer << 16) << 16;

      while (n >= LBLOCKSIZE*4)
        {
          *((unsigned long*) s) = buffer;	s += LBLOCKSIZE;
          *((unsigned long*) s) = buffer;	s += LBLOCKSIZE;
          *((unsigned long*) s) = buffer;	s += LBLOCKSIZE;
          *((unsigned long*) s) = buffer;	s += LBLOCKSIZE;
          n -= 4*LBLOCKSIZE;
        }

      while (n >= LBLOCKSIZE)
        {
          *((unsigned long*) s) = buffer;	s += LBLOCKSIZE;
          n -= LBLOCKSIZE;
        }
    }

  /* Pick up the remainder with a bytewise loop.  */
  while (n--)
    {
      *s++ = c;
    }

  return m;
}
