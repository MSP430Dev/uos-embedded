/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

typedef signed char int8_t;
typedef unsigned char u_int8_t;

#define INT_SIZE 2
typedef short int16_t;
typedef unsigned short u_int16_t;

typedef long int32_t;
typedef unsigned long u_int32_t;

typedef char __attribute__((mode(__byte__))) bool_t;
typedef signed char __attribute__((mode(__byte__))) sign_t;
typedef signed char __attribute__((mode(__byte__))) int_t;
typedef unsigned char __attribute__((mode(__byte__))) uint_t;

typedef long long int64_t;
typedef unsigned long long u_int64_t;

/*
 * An integer type, large enough to keep a memory address.
 * On AVR, data pointers have 2-byte size.
 */
typedef unsigned short size_t;

typedef char jmp_buf [30]; /* not implemented yet */

/*
 * Reading from flash memory.
 */
#undef FETCH_BYTE
#undef FETCH_WORD
#undef FETCH_LONG
#undef FETCH_PTR
#define FETCH_BYTE(p) readb ((int) (p))
#define FETCH_WORD(p) readw ((int) (p))
#define FETCH_LONG(p) readl ((int) (p))
#define FETCH_PTR(p) ((void*)readw ((int) (p)))

#define __tmp_reg__	"r0"
#define __zero_reg__	"r1"

/*
 * Stop a program and call debugger.
 */
void breakpoint ();

#define abort() breakpoint()

#define OPACITY_ALIGN 1

#endif /* __MACHINE_TYPES_H_ */
