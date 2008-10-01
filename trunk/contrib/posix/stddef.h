#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <runtime/lib.h>
#include <mem/mem.h>

#ifndef NULL
#define NULL 0
#endif

static inline char *strerror(int n) { static char s[] = ""; return s; }

#endif
