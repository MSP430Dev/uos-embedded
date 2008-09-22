/*
 * Processor-dependent data types.
 */
#ifndef __MACHINE_TYPES_H_
#define __MACHINE_TYPES_H_ 1

#define const /*void*/

typedef char int8_t;
typedef unsigned char u_int8_t;

#define INT_SIZE 2
typedef short int16_t;
typedef unsigned short u_int16_t;

typedef long int32_t;
typedef unsigned long u_int32_t;

typedef int bool_t;
typedef int sign_t;
typedef int int_t;
typedef unsigned uint_t;

/*
 * An integer type, large enough to keep a memory address.
 * We use small memory model of x86,
 * so pointers have 2-byte size.
 */
typedef unsigned short size_t;

typedef short jmp_buf[10]; /* not implemented yet */

/*
 * Stop a program and call debugger.
 */
/*#define breakpoint()	abort()*/
#define abort()		dos_halt()
void dos_halt ();

typedef char *va_list;
#define va_start(ap, p)		(ap = (char *) (&(p)+1))
#define va_arg(ap, type)	((type *) (ap += sizeof(type)))[-1]
#define va_end(ap)

unsigned char isdigit (int c);
unsigned char isxdigit (int c);
unsigned char isalpha (int c);
unsigned char islower (int c);
unsigned char isupper (int c);
unsigned char isalnum (int c);
unsigned char toupper (int c);
unsigned char tolower (int c);
unsigned char isspace (int c);
unsigned char ispunct (int c);
unsigned char iscntrl (int c);
unsigned char isprint (int c);
unsigned char isgraph (int c);

/*
 * Specials, built into the compiler.
 */
extern short _AX, _BX, _CX, _DX, _SP, _BP, _SI, _DI,
	_CS, _DS, _ES, _SS, _FLAGS;

#define inb(port) (_DX = (port), asm ("inb ax,dx"), )

#endif /* __MACHINE_TYPES_H_ */
