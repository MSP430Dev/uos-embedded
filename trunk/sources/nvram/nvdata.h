/*
 * Non-volatile memory storage, protected with checksum.
 */
#ifndef _NVDATA_H_
#define _NVDATA_H_ 1

#include <crc/crc32-vak.h>

#ifndef NDEBUG
#   include <kernel/internal.h>
#endif

#ifndef NVDATA_BAD_ADDRESS
#   define NVDATA_BAD_ADDRESS	((unsigned) -1l)
#endif

#define NVDATA_HEADER_SIZE	10

typedef struct {
	lock_t lock;
	unsigned long crc;
	unsigned __addr, begin, limit, end, sign;
#ifndef NDEBUG
	task_t *owner;
#endif
} nvdata_t;

struct _timer_t;

bool_t nvdata_is_owned (nvdata_t *nv) __attribute__ ((warn_unused_result));
inline extern bool_t nvdata_is_owned (nvdata_t *nv)
{
#ifdef NDEBUG
	return 1;
#else
	return nv->owner == task_current;
#endif
}

bool_t nvdata_is_valid_addr (nvdata_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
inline extern bool_t nvdata_is_valid_addr (nvdata_t *nv, unsigned relative_addr)
{
	assert (nvdata_is_owned (nv));
	return relative_addr >= NVDATA_HEADER_SIZE
		&& relative_addr <= nv->end - nv->begin
		&& nvdata_is_owned (nv);
}

inline extern unsigned nvdata_get_addr (nvdata_t *nv)
{
	assert (nvdata_is_owned (nv));
	if (! nvdata_is_owned (nv))
		return NVDATA_BAD_ADDRESS;
	return nv->__addr - nv->begin;
}

void nvdata_init (nvdata_t *nv, unsigned region_begin, unsigned region_end);
void nvdata_protect (nvdata_t *nv, struct _timer_t *timer);
void nvdata_unprotect (nvdata_t *nv, struct _timer_t *timer);
void nvdata_write_byte (nvdata_t *nv, unsigned char c);
unsigned char nvdata_read_byte (nvdata_t *nv);

static inline void nvdata_write_bool (nvdata_t *nv, bool_t b) {
	nvdata_write_byte (nv, b ? 0xA5 : 0x5A);
}

static inline bool_t nvdata_read_bool (nvdata_t *nv) {
	return nvdata_read_byte (nv) == 0xA5;
}

void nvdata_write_word (nvdata_t *nv, unsigned short w);
unsigned nvdata_read_word (nvdata_t *nv);
#define nvdata_write_short(nv, w) nvdata_write_word(nv, w)
#define nvdata_read_short(nv) nvdata_read_word(nv)

void nvdata_write_dword (nvdata_t *nv, unsigned long d);
unsigned long nvdata_read_dword (nvdata_t *nv);
#define nvdata_write_long(nv, d) nvdata_write_dword(nv, d)
#define nvdata_read_long(nv) nvdata_read_dword(nv)

void nvdata_write_str (nvdata_t *nv,
	unsigned char *str, small_uint_t maxlen);
void nvdata_read_str (nvdata_t *nv,
	unsigned char *str, small_uint_t maxlen);
void nvdata_write_mem (nvdata_t *nv,
	unsigned char *buf, small_uint_t len);
void nvdata_read_mem (nvdata_t *nv,
	unsigned char *buf, small_uint_t len);

#define NVDATA_OK	0
#define NVDATA_EMPTY	-1
#define NVDATA_DEAD	-2
#define NVDATA_DIFFERENT	-3
#define NVDATA_INVALID	-4
#define NVDATA_EOF	-5

small_int_t nvdata_begin_read (nvdata_t *nv, unsigned ver_rev) __attribute__ ((warn_unused_result));
small_int_t nvdata_finalize_read (nvdata_t *nv) __attribute__ ((warn_unused_result));

void nvdata_begin_write (nvdata_t *nv);
small_int_t nvdata_finalize_write (nvdata_t *nv, unsigned ver_rev) __attribute__ ((warn_unused_result));

small_int_t nvdata_begin_load (nvdata_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
small_int_t nvdata_finalize_load (nvdata_t *nv) __attribute__ ((warn_unused_result));

small_int_t nvdata_begin_update (nvdata_t *nv, unsigned relative_addr) __attribute__ ((warn_unused_result));
small_int_t nvdata_finalize_update (nvdata_t *nv);
small_int_t nvdata_update_str (nvdata_t *nv, unsigned relative_addr,
	unsigned char *str, small_uint_t maxlen);

extern bool_t nvdata_is_compatible (unsigned nvram, unsigned soft);

#endif /* _NVDATA_H_ */
