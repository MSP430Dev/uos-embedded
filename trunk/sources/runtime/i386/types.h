/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

#define INT_SIZE	4
typedef long int32_t;
typedef unsigned long uint32_t;

#if __GLIBC_HAVE_LONG_LONG
typedef long long int64_t;
typedef unsigned long long uint64_t;
#endif

typedef int small_int_t;
typedef unsigned int small_uint_t;

typedef int bool_t;

/*
 * An integer type, large enough to keep a memory address.
 */
typedef unsigned int size_t;

typedef long jmp_buf [6];

#if ARCH_HAVE_FPU
#include <float.h>
#endif

#endif /* __MACHINE_TYPES_H_ */
