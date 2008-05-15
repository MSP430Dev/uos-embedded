#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <runtime/lib.h>
#include <mem/mem.h>

#ifndef NULL
#define NULL 0
#endif

#define size_t         mem_size_t

static inline char *strerror(int n) { return ""; }

#endif
