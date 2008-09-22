#ifndef __SYS_LIB_H_
#define	__SYS_LIB_H_ 1
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Reading a byte from flash memory.
 * By default - handle it like a simple pointer.
 */
#define FETCH_BYTE(p) (*(unsigned char*)(p))
#define FETCH_WORD(p) (*(unsigned short*)(p))
#define FETCH_LONG(p) (*(unsigned long*)(p))
#define FETCH_PTR(p) (*((void**)((void*)(p))))

#define likely(x) __builtin_expect (x,1)
#define unlikely(x) __builtin_expect (x,0)
#define __noinline __attribute__ ((noinline))
#define __alwaysinline inline __attribute__ ((always_inline))
#define __forceinline extern __alwaysinline
#define __weak __attribute__ ((weak))
#define __noop do{}while(0)

#define ARRAY_LENGTH(array)	(sizeof (array) / sizeof ((array)[0]))
#define ARRAY_END(array)	((array) + ARRAY_LENGTH (array))

#include <runtime/byteorder.h>
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#include <runtime/arch.h>
#include <runtime/assert.h>

#define unreachable() assert(0)
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define BUILD_BUG_ON_ZERO(expression) (sizeof(char[1 - 2 * !!(expression)]) - 1)
#define BUILD_ONLY_LITTLE_ENDIAN BIULD_BUG_ON (_BYTE_ORDER != __LITTLE_ENDIAN)

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

struct _task_t;
struct _stream_t;
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
void debug_wait_enter (void);
void debug_task_print (struct _task_t *t);
void task_print (struct _stream_t *stream, struct _task_t *t);
void debug_dump_stack_current (void);
void debug_dump_stack_task (struct _task_t *task);
void debug_dump_stack (const char *caption, void *sp, void* frame, void *callee);
bool_t __debug_stack_is_codir (void *up, void *deep);
const char* __debug_task_name (struct _task_t *task);
const char* __debug_ptr_name (void *ptr);
void debug_redirect (void (*func) (void*, short), void *arg);

/*
 * Call global C++ constructors and destructors.
 */
void uos_call_global_initializers (void);
void uos_call_global_destructors (void);

/* Check memory address. */
bool_t uos_valid_memory_address (void*);

#ifndef __AVR__
inline extern unsigned
strlen_flash (const char *str)
{
	return (unsigned) strlen ((const unsigned char*) str);
}

inline extern void
memcpy_flash (void *dest, const char *src, unsigned char len)
{
	memcpy (dest, src, len);
}

inline extern void
strcpy_flash (unsigned char *dest, const char *str)
{
	strcpy (dest, (const unsigned char*) str);
}

inline extern void
strncpy_flash (unsigned char *dest, const char *str, unsigned char maxlen)
{
	strncpy (dest, (const unsigned char*) str, maxlen);
}
#endif /* __AVR__ */

inline extern unsigned char
flash_fetch (const char *p)
{
	return FETCH_BYTE (p);
}

#define type_of(item)							\
	__typeof__ (item)

#define offset_of(type, member)						\
	__builtin_offsetof (type, member)

#define container_of(ptr, type, member) ({				\
		const type_of (((type *)0)->member) *__mptr = (ptr);	\
		(type *)((char *)__mptr - offset_of (type,member));	\
	})

#define DEFINE_DEVICE_ADDR(name, val)			\
        	__asm __volatile (			\
		".globl " #name	"\n\t"			\
		".set " #name ", %0"			\
                :: "n" (val)				\
	)

#define DEFINE_DEVICE_CONST(name, val)			\
	DEFINE_DEVICE_ADDR (__dcp_##name, val);

#define GET_DEVICE_CONST(name) ({			\
		extern const void *__dcp_##name;	\
		(unsigned) (&__dcp_##name);		\
	})


#include <runtime/list.h>

#ifndef bitfield_set
#	define bitfield_orone(field, bool_value) do {	\
		if (bool_value)				\
			(field) = 1;			\
	} while (0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* !__SYS_LIB_H_ */
