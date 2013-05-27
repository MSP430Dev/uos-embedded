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
#       if defined (ARM_CORTEX_M1) || defined (ARM_CORTEX_M3) || \
	   defined (ARM_CORTEX_M4)
#		include <runtime/cortex-m/io.h>
#       else
#		include <runtime/arm/io.h>
#       endif
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __BIG_ENDIAN
#elif defined (MIPS32)
#	include <stdarg.h>
#	include <runtime/mips/types.h>
#	include <runtime/mips/string.h>
#	include <runtime/mips/stdlib.h>
#	include <runtime/mips/io.h>
#	include <runtime/ctype.h>
#	define __BYTE_ORDER __LITTLE_ENDIAN
#	define __FLOAT_WORD_ORDER __LITTLE_ENDIAN
#elif defined (MSP430)
#	include <stdarg.h>
#	include <runtime/msp430/types.h>
#	include <runtime/msp430/string.h>
#	include <runtime/msp430/stdlib.h>
#	include <runtime/msp430/io.h>
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
#endif

#endif /* __UOS_LIB_ARCH_H_ */
