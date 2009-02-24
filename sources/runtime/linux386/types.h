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
/* Defined in stdlib.h: typedef unsigned long uint32_t; */

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

#define FLT_RADIX	2		/* b */
/*#define FLT_ROUNDS	__flt_rounds()*/
#define	FLT_EVAL_METHOD	(-1)		/* i387 semantics are...interesting */
#define	DECIMAL_DIG	21		/* max precision in decimal digits */

#define FLT_MANT_DIG	24		/* p */
#define FLT_EPSILON	1.19209290E-07F	/* b**(1-p) */
#define FLT_DIG		6		/* floor((p-1)*log10(b))+(b == 10) */
#define FLT_MIN_EXP	(-125)		/* emin */
#define FLT_MIN		1.17549435E-38F	/* b**(emin-1) */
#define FLT_MIN_10_EXP	(-37)		/* ceil(log10(b**(emin-1))) */
#define FLT_MAX_EXP	128		/* emax */
#define FLT_MAX		3.40282347E+38F	/* (1-b**(-p))*b**emax */
#define FLT_MAX_10_EXP	38		/* floor(log10((1-b**(-p))*b**emax)) */

#define DBL_MANT_DIG	53
#define DBL_EPSILON	2.2204460492503131E-16
#define DBL_DIG		15
#define DBL_MIN_EXP	(-1021)
#define DBL_MIN		2.2250738585072014E-308
#define DBL_MIN_10_EXP	(-307)
#define DBL_MAX_EXP	1024
#define DBL_MAX		1.7976931348623157E+308
#define DBL_MAX_10_EXP	308

#define LDBL_MANT_DIG	64
#define LDBL_EPSILON	1.0842021724855044340E-19L
#define LDBL_DIG	18
#define LDBL_MIN_EXP	(-16381)
#define LDBL_MIN	3.3621031431120935063E-4932L
#define LDBL_MIN_10_EXP	(-4931)
#define LDBL_MAX_EXP	16384
#define LDBL_MAX	1.1897314953572317650E+4932L
#define LDBL_MAX_10_EXP	4932

#endif /* __MACHINE_TYPES_H_ */