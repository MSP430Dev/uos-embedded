#ifndef __UOS_LIB_ARCH_H_
#define __UOS_LIB_ARCH_H_

#if defined (__AVR__)
#	include <stdarg.h>
#	include <runtime/avr/types.h>
#	include <runtime/avr/string.h>
#	include <runtime/avr/stdlib.h>
#	include <runtime/avr/io.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined (__arm__) || defined (__thumb__)
#	include <stdarg.h>
#	include <runtime/arm/types.h>
#	include <runtime/arm/string.h>
#	include <runtime/arm/stdlib.h>
#	include <runtime/arm/io.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __BIG_ENDIAN
#elif defined (MIPS32)
#	include <stdarg.h>
#	include <runtime/mips32/types.h>
#	include <runtime/mips32/string.h>
#	include <runtime/mips32/stdlib.h>
#	include <runtime/mips32/io.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __LITTLE_ENDIAN
#elif defined (I386)
#	include <stdarg.h>
#	include <runtime/i386/types.h>
#	include <runtime/i386/string.h>
#	include <runtime/i386/stdlib.h>
#	include <runtime/i386/io.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __LITTLE_ENDIAN
#elif defined (LINUX386)
#	define __timer_t_defined 1
#	include <stdarg.h>
#	include <stdlib.h>
#	include <runtime/linux386/types.h>
#	include <runtime/i386/string.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined (__MSDOS__)
#	include <runtime/i86-dos/types.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __LITTLE_ENDIAN
#elif defined (PDP11)
#	include <stdarg.h>
#	include <runtime/pdp11/types.h>
#	include <runtime/pdp11/assert.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __LITTLE_ENDIAN
#endif

#endif /* __UOS_LIB_ARCH_H_ */
