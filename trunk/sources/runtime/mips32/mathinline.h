/*
 * Math inline functions for MIPS architecture.
 *
 * Copyright (C) 2008-2010 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#ifdef __cplusplus
# define __MATH_INLINE	__inline __attribute__((always_inline))
#else
# define __MATH_INLINE	extern __inline __attribute__((always_inline))
#endif

__MATH_INLINE float sqrtf (float x)
{
	register float result;
	asm volatile ("sqrt.s	%0, %1" : "=f" (result) : "f" (x));
	return result;
}

__MATH_INLINE double sqrt (double x)
{
	register double result;
	asm volatile ("sqrt.d	%0, %1" : "=f" (result) : "f" (x));
	return result;
}
