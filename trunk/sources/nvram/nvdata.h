/*
 * Non-volatile memory storage, protected with checksum.
 */
#ifndef _NVRAM_H_
#define _NVRAM_H_ 1

#include "crc/crc32-vak.h"

#ifndef NDEBUG
#   include "kernel/internal.h"
#endif

#ifndef NVRAM_BAD_ADDRESS
#   define NVRAM_BAD_ADDRESS	((unsigned) -1l)
#endif

#define NVRAM_HEADER_SIZE	10

typedef struct {
	lock_t lock;
	unsigned long crc;
	unsigned __addr, begin, limit, end, sign;
#ifndef NDEBUG
	task_t *owner;
#endif
} nvram_t;

struct _timer_t;

bool_t nvram_is_owned (nvram_t *nv) __attribute__ ((warn_unused_result));
inline extern bool_t nvram_is_owned (nvram_t *nv)
{
#ifdef NDEBUG
	return 1;
#else
	return nv->owner == task_current;
#endif
}

bool_t nvram_is_valid_addr (nvram_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
inline extern bool_t nvram_is_valid_addr (nvram_t *nv, unsigned relative_addr)
{
	assert (nvram_is_owned (nv));
	return relative_addr >= NVRAM_HEADER_SIZE
		&& relative_addr <= nv->end - nv->begin
		&& nvram_is_owned (nv);
}

inline extern unsigned nvram_get_addr (nvram_t *nv)
{
	assert (nvram_is_owned (nv));
	if (! nvram_is_owned (nv))
		return NVRAM_BAD_ADDRESS;
	return nv->__addr - nv->begin;
}

void nvram_init (nvram_t *nv, unsigned region_begin, unsigned region_end);
void nvram_protect (nvram_t *nv, struct _timer_t *timer);
void nvram_unprotect (nvram_t *nv, struct _timer_t *timer);
void nvram_write_byte (nvram_t *nv, unsigned char c);
unsigned char nvram_read_byte (nvram_t *nv);

static inline void nvram_write_bool (nvram_t *nv, bool_t b) {
	nvram_write_byte (nv, b ? 0xA5 : 0x5A);
}

static inline bool_t nvram_read_bool (nvram_t *nv) {
	return nvram_read_byte (nv) == 0xA5;
}

void nvram_write_word (nvram_t *nv, unsigned short w);
unsigned nvram_read_word (nvram_t *nv);
#define nvram_write_short(nv, w) nvram_write_word(nv, w)
#define nvram_read_short(nv) nvram_read_word(nv)

void nvram_write_dword (nvram_t *nv, unsigned long d);
unsigned long nvram_read_dword (nvram_t *nv);
#define nvram_write_long(nv, d) nvram_write_dword(nv, d)
#define nvram_read_long(nv) nvram_read_dword(nv)

void nvram_write_str (nvram_t *nv,
	unsigned char *str, small_uint_t maxlen);
void nvram_read_str (nvram_t *nv,
	unsigned char *str, small_uint_t maxlen);
void nvram_write_mem (nvram_t *nv,
	unsigned char *buf, small_uint_t len);
void nvram_read_mem (nvram_t *nv,
	unsigned char *buf, small_uint_t len);

#define NVRAM_OK	0
#define NVRAM_EMPTY	-1
#define NVRAM_DEAD	-2
#define NVRAM_DIFFERENT	-3
#define NVRAM_INVALID	-4
#define NVRAM_EOF	-5

small_int_t nvram_begin_read (nvram_t *nv, unsigned ver_rev) __attribute__ ((warn_unused_result));
small_int_t nvram_finalize_read (nvram_t *nv) __attribute__ ((warn_unused_result));

void nvram_begin_write (nvram_t *nv);
small_int_t nvram_finalize_write (nvram_t *nv, unsigned ver_rev) __attribute__ ((warn_unused_result));

small_int_t nvram_begin_load (nvram_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
small_int_t nvram_finalize_load (nvram_t *nv) __attribute__ ((warn_unused_result));

small_int_t nvram_begin_update (nvram_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
small_int_t nvram_finalize_update (nvram_t *nv);
small_int_t nvram_update_str (nvram_t *nv, unsigned relative_addr,
	unsigned char *str, small_uint_t maxlen);

extern bool_t nvram_is_compatible (unsigned nvram, unsigned soft);

#endif /* _NVRAM_H_ */
