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

typedef long long int64_t;
typedef unsigned long long u_int64_t;

typedef int small_int_t;
typedef unsigned int small_uint_t;

typedef int bool_t;

/*
 * An integer type, large enough to keep a memory address.
 * On ARM, data pointers have 4-byte size.
 */
typedef unsigned long size_t;

typedef long jmp_buf [10];

#endif /* __MACHINE_TYPES_H_ */
