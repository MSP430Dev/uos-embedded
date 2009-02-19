/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

typedef	signed char int8_t;
typedef	unsigned char uint8_t;

typedef	short int16_t;
typedef	unsigned short uint16_t;

#define INT_SIZE 2
typedef	long int32_t;
typedef	unsigned long uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef int small_int_t;
typedef unsigned int small_uint_t;

typedef int bool_t;

/*
 * An integer type, large enough to keep a memory address.
 * On MSP430, data pointers have 2-byte size.
 */
typedef unsigned int size_t;

typedef int jmp_buf [10];

#endif /* __MACHINE_TYPES_H_ */
