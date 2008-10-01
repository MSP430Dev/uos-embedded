/*
 * Do not include this file directly, use runtime/lib.h instead.
 *
 * This file is part of the AVR C Library.
 * Copyright (C) 1998, 1999 by Denis Chertykov (denisc@overta.ru)
 *
 * The AVR C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <runtime/avr/io.h>

/* Copy N bytes of SRC to DEST.  */
inline extern void *
memcpy (void *__to, const void *__from, size_t __n)
{
  register void *__dummy1, *__dummy2, *__dummy3;
  if (__n)
  __asm__ __volatile__ (
			"L_hi%=:\t"
			"ld __tmp_reg__,%a0+\n\t"
			"st %a1+,__tmp_reg__\n\t"
			"sbiw %2,1\n\t"
			"brne L_hi%=\n"
			: "=&e" (__dummy1), "=&e" (__dummy2), "=&w" (__dummy3)
			: "0" (__from),"1" (__to),"2" (__n)
			: "memory", "cc");
  return (__to);
}

/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
   Return the position in DEST one byte past where C was copied,
   or NULL if C was not found in the first N bytes of SRC.  */

/* Set N bytes of S to C.  */

inline extern void *
memset (void *__s, unsigned char __c, size_t __n)
{
  register void *__dummy1;
  register size_t __dummy2;
  if (__n)
    __asm__ __volatile__ (
			  "L_hi%=:\t"
			  "st %a0+, %2\n\t"
			  "sbiw %1,1\n\t"
			  "brne L_hi%="
			  : "=&e" (__dummy1), "=&w" (__dummy2)
			  : "r" ((unsigned char)__c), "0" (__s), "1" (__n)
			  : "memory", "cc");
  return __s;
}

/* Compare N bytes of S1 and S2.  */
inline extern small_int_t
__avr_memcmp (const void *__s1, const void *__s2, size_t __n)
{
  register void *__dummy1, *__dummy2;
  register size_t __dummy3;
  register signed char __res;
  if (__n)
    __asm__ ("cmp_loop%=:\t"
			  "ld __tmp_reg__,%a2+\n\t"
			  "ld %0,%a1+\n\t"
			  "sub %0,__tmp_reg__\n\t"
			  "brne cmp_out%=\n\t"
			  "sbiw %3,1\n\t"
			  "brne cmp_loop%=\n"
			  "cmp_out%=:\tbrcc cmp_end%=\n\t"
			  "ldi %0,0xff\n"
			  "cmp_end%=:\n"
			  : "=d"(__res), "=e"(__dummy1),"=e"(__dummy2),"=w"(__dummy3)
			  : "1" (__s1), "2" (__s2), "3" (__n) : "cc");
  return __res;
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as the count is constant.
 */
inline extern small_int_t
__constant_memcmp (const void *__s1, const void *__s2, size_t n)
{
	switch (n) {
	case 0:
		break;
	case 1:
		if (*(unsigned char*)__s1 < *(unsigned char*)__s2) return -1;
		if (*(unsigned char*)__s1 > *(unsigned char*)__s2) return 1;
		break;
	case 2:
		if (*(unsigned short*)__s1 < *(unsigned short*)__s2) return -1;
		if (*(unsigned short*)__s1 > *(unsigned short*)__s2) return 1;
		break;
	case 3:
		if (*(unsigned short*)__s1 < *(unsigned short*)__s2) return -1;
		if (*(unsigned short*)__s1 > *(unsigned short*)__s2) return 1;
		if (*(2+(unsigned char*)__s1) < *(2+(unsigned char*)__s2)) return -1;
		if (*(2+(unsigned char*)__s1) > *(2+(unsigned char*)__s2)) return 1;
		break;
	case 4:
		if (*(unsigned long*)__s1 < *(unsigned long*)__s2) return -1;
		if (*(unsigned long*)__s1 > *(unsigned long*)__s2) return 1;
		break;
	case 6:
		/* for Ethernet addresses */
		if (*(unsigned long*)__s1 < *(unsigned long*)__s2) return -1;
		if (*(unsigned long*)__s1 > *(unsigned long*)__s2) return 1;
		if (*(2+(unsigned short*)__s1) < *(2+(unsigned short*)__s2)) return -1;
		if (*(2+(unsigned short*)__s1) > *(2+(unsigned short*)__s2)) return 1;
		break;
	}
	return 0;
}

extern small_int_t
memcmp (const void *__s1, const void *__s2, size_t n);

#define memcmp(t, f, n)	(__builtin_constant_p(n) && ((n)==0 || (n)==1 || \
			(n)==2 || (n)==3 || (n)==4 || (n)==6) ? \
			__constant_memcmp ((t), (f), (n)) : \
			__avr_memcmp ((t), (f), (n)))

/* Copy N bytes of SRC to DEST, guaranteeing
   correct behavior for overlapping strings.  */
extern void *memmove (void *__dest, const void * __src, size_t __n);

/* Search N bytes of S for C.  */
inline extern void *
memchr (const void *__s, unsigned char __c, size_t __n)
{
  if (!__n)
    return 0;
  {
    void *__ret;
    size_t __dummy;
    __asm__ ("L_ho%=:\t"
			  "ld __tmp_reg__,%a0+\n\t"
			  "cp __tmp_reg__,%2\n\t"
			  "breq L_hi%=\n\t"
			  "sbiw %1,1\n\t"
			  "brne L_ho%=\n\t"
			  "ldi %A0,1\n\t"
			  "ldi %B0,0\n"
			  "L_hi%=:\t"
			  "sbiw %0,1"
			  : "=&e" (__ret), "=&w" (__dummy)
			  : "r" (__c), "0" (__s), "1" (__n) : "cc");
    return __ret;
  }
}

/* Return the length of S.  */
inline extern size_t
strlen (const unsigned char *__s)
{
  unsigned char *__tmp;
  __asm__ ("L_hi%=:\t"
		"ld __tmp_reg__,%a0+\n\t"
		"tst __tmp_reg__\n\t"
		"brne L_hi%="
		: "=e" (__tmp)
		: "0" (__s) : "cc");
  return (--__tmp) - __s;
}

/* Copy SRC to DEST.  */
inline extern unsigned char *
strcpy (unsigned char *__dest, const unsigned char *__src)
{
  register void *__dummy1, *__dummy2;
  __asm__ __volatile__ ("L_ho%=:\t"
			"ld __tmp_reg__,%a1+\n\t"
			"st %a0+,__tmp_reg__\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_ho%="
			: "=e" (__dummy1), "=e" (__dummy2)
			: "0" (__dest), "1" (__src) : "memory", "cc"
			);
  return __dest;
}

/* Copy no more than N characters of SRC to DEST.  */
inline extern unsigned char *
strncpy (unsigned char *__dest, const unsigned char *__src, size_t __n)
{
  register void *__dummy1, *__dummy2;
  register size_t __dummy3;
  if (__n)
    __asm__ __volatile__ ("L_ho%=:\t"
			  "ld __tmp_reg__,%a1+\n\t"
			  "st %a0+,__tmp_reg__\n\t"
			  "sbiw %2,1\n\t"
			  "breq L_hi%=\n\t"
			  "tst __tmp_reg__\n\t"
			  "brne L_ho%=\n"
			  "L_hi%=:"
			  : "=e" (__dummy1), "=e" (__dummy2), "=w" (__dummy3)
			  : "2" (__n), "0" (__dest), "1" (__src)
			  : "memory", "cc"
			  );
  return __dest;
}

/* Append SRC onto DEST.  */
inline extern unsigned char *
strcat (unsigned char *__dest, const unsigned char *__src)
{
  register void *__dummy1, *__dummy2;
  __asm__ __volatile__ ("L_hi%=:\t"
			"ld __tmp_reg__,%a1+\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_hi%=\n\t"
			"sbiw %1,1\n"
			"L_ho%=:\t"
			"ld __tmp_reg__,%a0+\n\t"
			"st %a1+,__tmp_reg__\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_ho%="
			: "=e" (__dummy1), "=e" (__dummy2)
			: "1" (__dest), "0" (__src)
			: "memory", "cc"
			);
  return __dest;
}

/* Append no more than N characters from SRC onto DEST.  */
inline extern unsigned char *
strncat (unsigned char *__dest, const unsigned char *__src, size_t __n)
{
  register void *__dummy1, *__dummy2;
  register size_t __dummy3;
  __asm__ __volatile__ ("L_hi%=:\t"
			"ld __tmp_reg__,%a1+\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_hi%=\n\t"
			"sbiw %1,1\n"
			"L_ho%=:\t"
			"ld __tmp_reg__,%a0+\n\t"
			"st %a1+,__tmp_reg__\n\t"
			"sbiw %2,1\n\t"
			"breq L_hh%=\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_ho%=\n"
			"L_hh%=:"
			: "=e" (__dummy1), "=e" (__dummy2), "=w" (__dummy3)
			: "1" (__dest), "0" (__src), "2" (__n)
			: "memory", "cc"
			);
  return __dest;
}

/* Compare S1 and S2.  */
inline extern small_int_t
strcmp (const unsigned char *__s1, const unsigned char *__s2)
{
  register void *__dummy1, *__dummy2;
  register signed char __res;
  __asm__ ("cmp_loop%=:\t"
			"ld __tmp_reg__,%a2+\n\t"
			"ld %0,%a1+\n\t"
			"sub %0,__tmp_reg__\n\t"
			"brne cmp_out%=\n\t"
			"tst __tmp_reg__\n\t"
			"brne cmp_loop%=\n"
			"cmp_out%=:\tbrcc cmp_end%=\n\t"
			"ldi %0,0xff\n"
			"cmp_end%=:\n"
			: "=d" (__res), "=e" (__dummy1), "=e" (__dummy2)
			: "1" (__s1), "2" (__s2)
			: "cc");
  return __res;
}

/* Compare N characters of S1 and S2.  */
inline extern small_int_t
strncmp (const unsigned char *__s1, const unsigned char *__s2, size_t __n)
{
  register void *__dummy1, *__dummy2;
  register size_t __dummy3;
  register signed char __res = 0;
  if (__n)
    __asm__ ("cmp_loop%=:\t"
			  "ld __tmp_reg__,%a2+\n\t"
			  "ld %0,%a1+\n\t"
			  "sub %0,__tmp_reg__\n\t"
			  "brne cmp_end%=\n\t"
			  "sbiw %3,1\n\t"
			  "breq cmp_out%=\n\t"
			  "tst __tmp_reg__\n\t"
			  "brne cmp_loop%=\n"
			  "cmp_out%=:\tbrcc cmp_end%=\n\t"
			  "ldi %0,0xff\n"
			  "cmp_end%=:\n"
			  : "=d" (__res),"=e"(__dummy1),"=e"(__dummy2),"=w"(__dummy3)
			  : "1" (__s1), "2" (__s2), "3" (__n)
			  : "cc");
  return __res;
}

/* Find the first occurrence of C in S.  */
inline extern unsigned char *
strchr (const unsigned char * __s, unsigned char __c)
{
  __asm__ ("L_hi%=:\t"
			"ld __tmp_reg__,%a0+\n\t"
			"cp __tmp_reg__,%2\n\t"
			"breq L_ho%=\n\t"
			"tst __tmp_reg__\n\t"
			"brne L_hi%=\n\t"
			"ldi %A0,1\n\t"
			"ldi %B0,0\n"
			"L_ho%=:\t"
			"sbiw %0,1"
			: "=e" (__s)
			: "0" (__s), "r" (__c)
			: "cc");
  return (unsigned char*)__s;
}

/* Find the last occurrence of C in S.  */
inline extern unsigned char *
strrchr (const unsigned char *__src, unsigned char __c)
{
  register void *__dummy;
  register unsigned char *__ret;
  __asm__ (
			"L_hi%=:\t"
			"ld __tmp_reg__,%a0+\n\t"
			"cp __tmp_reg__,%2\n\t"
			"brne L_ho%=\n\t"
			"mov %A1,%A0\n\t"
			"mov %B1,%B0\n"
			"L_ho%=:\t"
			"tst __tmp_reg__\n\t"
			"brne L_hi%=\n\t"
			: "=e" (__dummy), "=r" (__ret)
			: "r" (__c), "1" (1L), "0" (__src)
			: "cc"
			);
  return --__ret;
}

/* Find the length of STRING, but scan at most MAXLEN characters.
   If no '\0' terminator is found in that many characters, return MAXLEN.  */
inline extern size_t
strnlen (const unsigned char *__string, size_t __maxlen)
{
  const unsigned char *__end = (const unsigned char *) memchr (__string, '\0', __maxlen);
  return __end ? __end - __string : __maxlen;
}

inline extern unsigned char
strlen_flash (const char *str)
{
	unsigned char len;

	for (len=0; readb ((int) str); ++len)
		++str;
	return len;
}

inline extern void
memcpy_flash (void *dest, const char *str, unsigned char len)
{
	unsigned char *p = dest;

	while (len--)
		*p++ = readb ((int) str++);
}

inline extern void
strcpy_flash (unsigned char *dest, const char *str)
{
	while ((*dest++ = readb ((int) str++)) != 0)
		;
}

inline extern void
strncpy_flash (unsigned char *dest, const char *str, unsigned char maxlen)
{
	while (maxlen-- && (*dest++ = readb ((int) str++)) != 0)
		;
}
