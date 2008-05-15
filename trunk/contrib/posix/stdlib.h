#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <runtime/lib.h>
#include <mem/mem.h>

#ifndef NULL
#define NULL 0
#endif

extern mem_pool_t *uos_memory;
#define malloc(size)		mem_alloc (uos_memory, size)
#define calloc(n,size)		mem_alloc (uos_memory, (n)*(size))
#define realloc(ptr,size)	mem_realloc (ptr, size)
#define free(ptr)		mem_free (ptr)
#define strdup(str)		mem_strdup (uos_memory, str)

#ifdef __cplusplus
inline void operator delete (void *ptr)
	{ mem_free (ptr); }

inline void operator delete[] (void *ptr)
	{ mem_free (ptr); }

inline void *operator new (unsigned size)
	{ return mem_alloc (uos_memory, size); }

inline void *operator new[] (unsigned size)
	{ return mem_alloc (uos_memory, size); }
#endif

static inline int atexit (void (*function)(void))
	{ return 0; }

static inline char *getenv (const char *name)
	{ return 0; }

static inline int putenv (char *name)
	{ return -1; }

static inline void exit(int status)
	{ abort(); }

#endif
