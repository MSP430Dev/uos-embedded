#ifndef __SYS_LIB_H_
#define	__SYS_LIB_H_ 1
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Reading a byte from flash memory.
 * By default - handle it like a simple pointer.
 */
#define FETCH_BYTE(p)	(*(unsigned char*)(p))
#define FETCH_WORD(p)	(*(unsigned short*)(p))
#define FETCH_LONG(p)	(*(unsigned long*)(p))
#define FETCH_PTR(p)	({ void *x = (void*)(p); *(void**)x; })

#define ARRAY_LENGTH(array)	(sizeof (array) / sizeof ((array)[0]))
#define ARRAY_END(array)	((array) + ARRAY_LENGTH (array))

#include <runtime/byteorder.h>
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#include <runtime/arch.h>
#include <runtime/assert.h>
#include <runtime/list.h>

#ifndef __LINUX__
void qsort (void *a, size_t n, size_t es,
	int (*cmp)(const void*, const void*));
void *bsearch (const void *key, const void *base, size_t nmemb, size_t size,
	int (*compar) (const void *, const void *));
unsigned char *strstr (const char *haystack, const unsigned char *needle);
int strspn (const unsigned char *s, const unsigned char *accept);
int strcspn (const unsigned char *s, const unsigned char *reject);
#endif /* __LINUX__ */

#ifdef __GNUC__
#	define alloca(size)	__builtin_alloca (size)
#endif

extern int setjmp (jmp_buf);
extern void longjmp (jmp_buf, int);

/*
 * Debugging console interface.
 */
void debug_putchar (void *arg, short c);
void debug_putc (char c);
unsigned short debug_getchar (void);
int debug_peekchar (void);
void debug_puts (const char *str);
int debug_printf (const char *fmt, ...);
int debug_vprintf (const char *fmt, va_list args);
void debug_dump (const char *caption, void* data, unsigned len);
void debug_dump_stack (const char *caption, void *sp, void* frame, void *callee);
void debug_redirect (void (*func) (void*, short), void *arg);

/*
 * Call global C++ constructors.
 * This must be done from user code after initializing a memory allocation pool.
 */
void uos_call_global_initializers (void);

/*
 * Call global C++ destructors.
 * Should be called from user code when the system is halted.
 */
void uos_call_global_destructors (void);

/*
 * Check memory address.
 */
bool_t uos_valid_memory_address (void*);

/*
 * Halt the system.
 */
void uos_halt (int);

#ifndef __AVR__
static inline unsigned
strlen_flash (const char *str)
{
	return (unsigned) strlen ((const unsigned char*) str);
}

static inline void
memcpy_flash (void *dest, const char *src, unsigned char len)
{
	memcpy (dest, src, len);
}

static inline void
strcpy_flash (unsigned char *dest, const char *str)
{
	strcpy (dest, (const unsigned char*) str);
}

static inline void
strncpy_flash (unsigned char *dest, const char *str, unsigned char maxlen)
{
	strncpy (dest, (const unsigned char*) str, maxlen);
}
#endif /* __AVR__ */

static inline unsigned char
flash_fetch (const char *p)
{
	return FETCH_BYTE (p);
}

#ifdef __cplusplus
}
#endif
#endif /* !__SYS_LIB_H_ */
