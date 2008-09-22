/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

typedef	signed char int8_t;
typedef	unsigned char u_int8_t;

typedef	short int16_t;
typedef	unsigned short u_int16_t;

#define INT_SIZE 4
typedef	long int32_t;
typedef	unsigned long u_int32_t;

typedef int bool_t;
typedef int sign_t;
typedef int int_t;
typedef unsigned uint_t;

typedef long long int64_t;
typedef unsigned long long u_int64_t;

/*
 * An integer type, large enough to keep a memory address.
 * On ARM, data pointers have 4-byte size.
 */
typedef unsigned long size_t;

typedef long jmp_buf [10];

/*
 * Stop a program and call debugger.
 */
static inline void
breakpoint ()
{
	for (;;)
		continue;
}

#define abort() breakpoint()

#endif /* __MACHINE_TYPES_H_ */
