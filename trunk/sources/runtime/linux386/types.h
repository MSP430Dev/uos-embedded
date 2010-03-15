/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

#include <stdlib.h>
#define __timer_t_defined 1

/* Defined in stdlib.h: typedef char int8_t; */
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

#define INT_SIZE 4

/* Defined in stdlib.h: typedef long int32_t; */
typedef unsigned long uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int small_int_t;
typedef unsigned int small_uint_t;

typedef int bool_t;

#include <setjmp.h>

/*
 * Stop a program and call debugger.
 */
void abort (void);
void exit (int);

#if ARCH_HAVE_FPU
#include <float.h>
#endif

#endif /* __MACHINE_TYPES_H_ */
