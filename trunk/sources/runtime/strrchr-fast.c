/*
FUNCTION
	<<strrchr>>---reverse search for character in string

INDEX
	strrchr

ANSI_SYNOPSIS
	#include <string.h>
	char * strrchr(const char *<[string]>, int <[c]>);

TRAD_SYNOPSIS
	#include <string.h>
	char * strrchr(<[string]>, <[c]>);
	char *<[string]>;
	int *<[c]>;

DESCRIPTION
	This function finds the last occurence of <[c]> (converted to
	a char) in the string pointed to by <[string]> (including the
	terminating null character).

RETURNS
	Returns a pointer to the located character, or a null pointer
	if <[c]> does not occur in <[string]>.

PORTABILITY
<<strrchr>> is ANSI C.

<<strrchr>> requires no supporting OS subroutines.

QUICKREF
	strrchr ansi pure
*/
#include <runtime/lib.h>

unsigned char *
strrchr(const unsigned char *s, unsigned char i)
{
  const unsigned char *last = 0;

  if (i)
    {
      while ((s = strchr(s, i)) != 0)
	{
	  last = s;
	  s++;
	}
    }
  else
    {
      last = strchr(s, i);
    }

  return (unsigned char *) last;
}
