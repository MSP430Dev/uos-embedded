#include <runtime/lib.h>
#include <random/rand15.h>

static unsigned long next = 1;

short
rand15 (void)
{
	next = next * 1103515245L + 12345;
	return (short) (next >> 16) & 0x7fff;
}

void
srand15 (unsigned short seed)
{
	next = seed;
}
